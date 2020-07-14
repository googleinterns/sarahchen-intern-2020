/*
*   Copyright 2020 Google LLC
*
*   Licensed under the Apache License, Version 2.0 (the "License");
*   you may not use this file except in compliance with the License.
*   You may obtain a copy of the License at
*
*       https://www.apache.org/licenses/LICENSE-2.0
*
*   Unless required by applicable law or agreed to in writing, software
*   distributed under the License is distributed on an "AS IS" BASIS,
*   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*   See the License for the specific language governing permissions and
*   limitations under the License.
*/

#include <filesystem>
#include <fstream>

#include "../src/proto_value.h"
#include "../src/proto_parser.h"
#include "proto/event_predicate.pb.h"

using ::wireless_android_play_analytics::ProtoValue;
using ::wireless_android_play_analytics::MessageValue;
using ::wireless_android_play_analytics::PrimitiveValue;
using ::wireless_android_play_analytics::EventPredicate;
using ::wireless_android_play_analytics::ProtoParser;

namespace {

class EventPredicateConverter {
 public:

  std::unique_ptr<ProtoValue> ConvertEventPredicateFromFile(
      absl::string_view path) {
    ProtoParser parser(ReadTextProtoFromStream(path));
    EventPredicate event_predicate_prototype;
    std::unique_ptr<ProtoValue> and_field = 
        absl::make_unique<MessageValue>("and", 0, 0);
    MessageValue* and_field_ptr = static_cast<MessageValue*>(and_field.get());

    std::unique_ptr<ProtoValue> event_predicate = 
        parser.Create(event_predicate_prototype);
    MessageValue* event_predicate_ptr = 
        static_cast<MessageValue*>(event_predicate.get());
    const std::vector<std::unique_ptr<ProtoValue>>& fields = 
      event_predicate_ptr->GetFields();

    for (const std::unique_ptr<ProtoValue>& field : fields) {
      if (field.get()->GetName() == "event_matcher") {
        absl::flat_hash_map<std::string, ProtoValue*> name_to_ptr_map = 
            GetFieldNameToPtrMap(*field.get());
        if (name_to_ptr_map.find("int_comparator") == name_to_ptr_map.end()) {
          and_field_ptr->AddField(EventMatcherToFieldExists(*field.get()));
        } else {
          and_field_ptr->AddField(EventMatcherToFieldValue(*field.get()));
        }
        
      } else if (field.get()->GetName() == "ui_element_path_predicate") {
        and_field_ptr->AddField(UiElementPathPredicateToUiElement(*field.get()));
      }
    }

    return and_field;
  }

 private:

  // Reads text proto into a string from a file.
  std::string ReadTextProtoFromStream(absl::string_view path) {
    std::ifstream ifs;
    std::string text_proto;

    ifs.open(std::string(path));

    assert(ifs.good());

    while (ifs.good()) {
      std::string tmp;
      std::getline(ifs, tmp);
      text_proto += tmp + "\n";
    }

    return text_proto;
  }

  // Copies the comments of an original proto value to a copy, if they're both
  // primitive types also copy the value.
  void CopyVal(ProtoValue* copy, const ProtoValue& original) {
    copy->SetCommentBehindField(original.GetCommentBehindField());
    copy->SetCommentAboveField(original.GetCommentAboveField());
    PrimitiveValue* copy_val = dynamic_cast<PrimitiveValue*>(copy);
    const PrimitiveValue* original_val = dynamic_cast<const PrimitiveValue*>(
        &original);
    if (copy_val != nullptr && original_val != nullptr) {
      // Copy their values only if they are both primitive values.
      copy_val->SetVal(original_val->GetVal());
    }
  }

  // Gets a hash table that maps field names of a message value to their
  // corresponding protovalue pointer.
  absl::flat_hash_map<std::string, ProtoValue*> GetFieldNameToPtrMap(
      const ProtoValue& original) {
    const MessageValue* original_ptr = 
        static_cast<const MessageValue*>(&original);
    absl::flat_hash_map<std::string, ProtoValue*> name_to_ptr_map;
    const std::vector<std::unique_ptr<ProtoValue>>& fields = 
        original_ptr->GetFields();
    for (const std::unique_ptr<ProtoValue>& field : fields) {
      name_to_ptr_map[field.get()->GetName()] = field.get();
    }

    return name_to_ptr_map;
  }

  // A universal converter for message matcher into either index_ui,
  // field_value, or field_exists.
  std::unique_ptr<ProtoValue> MessageMatcherUniversalConverter(
      const ProtoValue& original, absl::string_view name, 
      const ProtoValue* index = nullptr) {
    std::unique_ptr<ProtoValue> constructed_field = nullptr;
    // Get the name of each field on the original proto.
    absl::flat_hash_map<std::string, ProtoValue*> name_to_ptr_map = 
        GetFieldNameToPtrMap(original);

    if (name == "index_ui" || name == "field_value" || name == "leaf_ui") {
      constructed_field = absl::make_unique<MessageValue>(name, 
          original.GetIndentCount() + 1, 0);
      MessageValue* constructed_field_ptr = static_cast<MessageValue*>(
          constructed_field.get());
      if (name == "index_ui") {
        // Index UI has an extra field of index.
        std::unique_ptr<ProtoValue> index_field = 
            absl::make_unique<PrimitiveValue>("index", 
            original.GetIndentCount() + 2, 0);
        CopyVal(index_field.get(), *index);
        constructed_field_ptr->AddField(std::move(index_field));
      }
      std::unique_ptr<ProtoValue> path = absl::make_unique<PrimitiveValue>(
        "path", original.GetIndentCount() + 2, 1);
      CopyVal(path.get(), *name_to_ptr_map["input_field"]);
      std::unique_ptr<ProtoValue> equals_int = 
          absl::make_unique<PrimitiveValue>("equals_int", 
          original.GetIndentCount() + 2, 2);
      CopyVal(equals_int.get(), *name_to_ptr_map["int_comparator"]);
      constructed_field_ptr->AddField(std::move(path));
      constructed_field_ptr->AddField(std::move(equals_int));
    } else {
      constructed_field = absl::make_unique<PrimitiveValue>(name, 
          original.GetIndentCount() + 1, 0);
      PrimitiveValue* constructed_field_ptr = static_cast<PrimitiveValue*>(
          constructed_field.get());
      PrimitiveValue* input_field_ptr = static_cast<PrimitiveValue*>(
          name_to_ptr_map["input_field"]);
      constructed_field_ptr->SetVal(input_field_ptr->GetVal());
    }

    // Copy the comments of the original proto to the constructed field
    CopyVal(constructed_field.get(), original);

    return constructed_field;
  }

  std::unique_ptr<ProtoValue> LeafUiPredicateToLeafUi(
      const ProtoValue& original) {
    absl::flat_hash_map<std::string, ProtoValue*> name_to_ptr_map = 
        GetFieldNameToPtrMap(original);
    return MessageMatcherUniversalConverter(
        *name_to_ptr_map["ui_element_matcher"], "leaf_ui");
  }

  std::unique_ptr<ProtoValue> IndexUiPredicateToIndexUi(
      const ProtoValue& original) {
    absl::flat_hash_map<std::string, ProtoValue*> name_to_ptr_map = 
        GetFieldNameToPtrMap(original);
    return MessageMatcherUniversalConverter(
        *name_to_ptr_map["ui_element_matcher"], "index_ui", 
        name_to_ptr_map["index"]);
  }

  std::unique_ptr<ProtoValue> EventMatcherToFieldExists(
      const ProtoValue& original) {
    return MessageMatcherUniversalConverter(original, "field_exists");
  }

  std::unique_ptr<ProtoValue> EventMatcherToFieldValue(
      const ProtoValue& original) {
    return MessageMatcherUniversalConverter(original, "field_value");
  }

  std::unique_ptr<ProtoValue> UiElementPathPredicateToUiElement(
      const ProtoValue& original) {
    std::unique_ptr<ProtoValue> ui_element = absl::make_unique<MessageValue>(
        "ui_element", original.GetIndentCount() + 1, 0);
    MessageValue* ui_element_ptr = static_cast<MessageValue*>(ui_element.get());
    CopyVal(ui_element.get(), original);

    absl::flat_hash_map<std::string, ProtoValue*> name_to_ptr_map = 
        GetFieldNameToPtrMap(original);

    // Convert ui_element_type to type
    std::unique_ptr<ProtoValue> type = absl::make_unique<PrimitiveValue>(
        "type", name_to_ptr_map["ui_element_type"]->GetIndentCount() + 1, 
        0);
    CopyVal(type.get(), *name_to_ptr_map["ui_element_type"]);
    ui_element_ptr->AddField(std::move(type));

    // Create nested predicate only if they exist.
    if (name_to_ptr_map.size() > 1) {
      std::unique_ptr<ProtoValue> and_field = absl::make_unique<MessageValue>(
          "and", original.GetIndentCount() + 2, 1);
      MessageValue* and_field_ptr = static_cast<MessageValue*>(and_field.get());
      if (name_to_ptr_map.find("leaf_ui_predicate") != name_to_ptr_map.end()) {
        and_field_ptr->AddField(LeafUiPredicateToLeafUi(
            *name_to_ptr_map["leaf_ui_predicate"]));
      }
      if (name_to_ptr_map.find("index_ui_matcher") != name_to_ptr_map.end()) {
        and_field_ptr->AddField(IndexUiPredicateToIndexUi(
            *name_to_ptr_map["index_ui_matcher"]));
      }
      ui_element_ptr->AddField(std::move(and_field));
    }

    return ui_element;    
  }
};

} // namespace

int main() {
  EventPredicateConverter converter;
  std::unique_ptr<ProtoValue> output = 
      converter.ConvertEventPredicateFromFile(
      "demo/proto_texts/original_proto_text.txt");
  std::cout << output.get()->PrintToTextProto();
  return 0;
}