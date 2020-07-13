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

std::unique_ptr<ProtoValue> ValueFactory(const ProtoValue* message);

int main() {
  std::ifstream ifs;
  std::string text_proto;
  EventPredicate event_predicate;
  MessageValue custom_predicate("and", 0, 0);

  ifs.open("demo/proto_texts/original_proto_text.txt");

  assert(ifs.good());

  while (ifs.good()) {
    std::string tmp;
    std::getline(ifs, tmp);
    text_proto += tmp + "\n";
  }

  ProtoParser parser(text_proto);
  std::unique_ptr<ProtoValue> message = parser.Create(event_predicate);
  MessageValue* event_predicate_instance = 
      dynamic_cast<MessageValue*>(message.get());
  std::vector<ProtoValue*> fields = 
      event_predicate_instance->GetFieldsMutable();
  for (ProtoValue* field : fields) {
    std::unique_ptr<ProtoValue> construct = ValueFactory(field);
  }

  return 0;
}

void CopyVal(ProtoValue* copy, const ProtoValue* original) {
  copy->SetCommentBehindField(original->GetCommentBehindField());
  copy->SetCommentAboveField(original->GetCommentAboveField());
  PrimitiveValue* copy_val = dynamic_cast<PrimitiveValue*>(copy);
  const PrimitiveValue* original_val = dynamic_cast<const PrimitiveValue*>(
      original);
  if (copy_val != nullptr && original_val != nullptr) {
    // Copy their values only if they are both primitive values.
    copy_val->SetVal(original_val->GetVal());
  }
}

std::unique_ptr<ProtoValue> MakeMessageMatcher(const ProtoValue* message) {
  const MessageValue* original = dynamic_cast<const MessageValue*>(message);
  const std::vector<std::unique_ptr<ProtoValue>>& fields = 
      original->GetFields();
  std::unique_ptr<ProtoValue> constructed_val = nullptr;
  if (fields.size() == 2) {
    // event_matcher, contains int_comparator -> field_value.
    constructed_val = absl::make_unique<MessageValue>(
        "field_value", original->GetIndentCount(), original->GetLineNumber());
    CopyVal(constructed_val.get(), message);
    std::unique_ptr<ProtoValue> path = absl::make_unique<PrimitiveValue>(
        "path", original->GetIndentCount() + 1, 0);
    CopyVal(path.get(), fields[0].get());
    std::unique_ptr<ProtoValue> equals_int = 
        absl::make_unique<PrimitiveValue>("equals_int", 
        original->GetIndentCount() + 1, 1);
    CopyVal(equals_int.get(), fields[1].get());
    MessageValue* constructed_val_message = 
        dynamic_cast<MessageValue*>(constructed_val.get());
    constructed_val_message->AddField(std::move(path));
    constructed_val_message->AddField(std::move(equals_int));
  } else if (field.size() == 1){
    // event_matcher, only contains input_field -> field_exists.
    constructed_val = absl::make_unique<PrimitiveValue>("field_exists", 
        original->GetIndentCount(), original->GetLineNumber());
    CopyVal(constructed_val.get(), original);
    PrimitiveValue* field_exists = dynamic_cast<PrimitiveValue*>(
        constructed_val.get());
    PrimitiveValue* input_field = dynamic_cast<PrimitiveValue*>(
        fields[0].get());
    field_exists->SetVal(input_field->GetVal());
  }

  return constructed_val;
}

std::unique_ptr<ProtoValue> MakePredicate(const ProtoValue* message) {
  
}

std::unique_ptr<ProtoValue> ValueFactory(const ProtoValue* message) {
  const MessageValue* original = dynamic_cast<const MessageValue*>(message);
  const std::vector<std::unique_ptr<ProtoValue>>& fields = 
      original->GetFields();
  std::unique_ptr<ProtoValue> constructed_val = nullptr;
  if (original->GetName() == "event_matcher") {
    return MakeMessageMatcher(message);
  } else if (original->GetName() == "ui_element_path_predicate") {
    // ui_element_path_predicate -> ui_element.
    constructed_val = absl::make_unique<MessageValue>("ui_element",
        original->GetIndentCount(), original->GetLineNumber());
    CopyVal(constructed_val.get(), original);
    std::unique_ptr<ProtoValue> type = absl::make_unique<PrimitiveValue>("type",
        original->GetIndentCount() + 1, 0);
    CopyVal(type.get(), fields[0].get());
    MessageValue* constructed_val_message = 
        dynamic_cast<MessageValue*>(constructed_val.get());
    constructed_val_message->AddField(std::move(type));
    if (fields.size() > 1) {
      std::unique_ptr<ProtoValue> and_val = absl::make_unique<MessageValue>(
          "and", original->GetIndentCount() + 1, 1);
      for (int i = 1; i < fields.size(); ++i) {
        MessageValue* nested_predicate = 
            dynamic_cast<MessageValue*>(fields[i].get());
        const std::vector<std::unique_ptr<ProtoValue>>& nested_fields = 
            nested_predicate->GetFields();
        if (fields[i].get()->GetName() == "leaf_ui_predicate") {
          std::unique_ptr<ProtoValue> leaf_ui = absl::make_unique<MessageValue>(
              "leaf_ui", original->GetIndentCount() + 2, 0);
          CopyVal(leaf_ui.get(), fields[i].get());
          MessageValue* leaf_ui_val = 
              dynamic_cast<MessageValue*>(leaf_ui.get());
          std::unique_ptr<ProtoValue>
          leaf_ui_val->AddField(MakeMessageMatcher(nested_fields[1].get()));
        } else {

        }
      }
      constructed_val_message->AddField(std::move(and_val));
    }
  }
  return constructed_val;
}