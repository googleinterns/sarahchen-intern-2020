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
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"

using ::wireless_android_play_analytics::ProtoValue;
using ::wireless_android_play_analytics::MessageValue;
using ::wireless_android_play_analytics::PrimitiveValue;
using ::wireless_android_play_analytics::EventPredicate;
using ::wireless_android_play_analytics::ProtoParser;

ABSL_FLAG(std::string, output_dir, "", "Output file directory");

namespace {

class EventPredicateConverter {
 public:

  // Original:
  //           # comment
  //           # to
  //           # save

  //           # Condition 1
  //           event_matcher {
  //             input_field: "PlayExtension.store.click"
  //           }
  //           # Condition 2
  //           event_matcher {
  //             input_field: "PlayExtension.log_source"
  //             int_comparator: 65  # WEB_STORE
  //           }
  //           # Condition 3
  //           ui_element_path_predicate {
  //             ui_element_type: STORE_UI_ELEMENT
  //             # Condition 3.1
  //             leaf_ui_predicate {
  //               # ...
  //               ui_element_type: STORE_UI_ELEMENT
  //               ui_element_matcher {
  //                 input_field: "type"
  //                 int_comparator: 7305  # AUTHENTICATION_OPT_OUT_CHECKED
  //               }
  //             }
  //             # Condition 3.2
  //             index_ui_matcher {
  //               ui_element_type: STORE_UI_ELEMENT
  //               index: -2
  //               ui_element_matcher {
  //                 input_field: "type"
  //                 int_comparator: 1  # SOMETHING
  //               }
  //             }
  //           }
  // Modified:
  //           and {
  //             # Condition 1
  //             field_exists: "PlayExtension.store.click"
  //             # Condition 2
  //             field_value {
  //               path: "PlayExtension.log_source"
  //               equals_int: 65  # WEB_STORE
  //             }
  //             # Condition 3
  //             ui_element {
  //               type: STORE_UI_ELEMENT
  //               and {
  //                 # Condition 3.1
  //                 leaf_ui {
  //                   path:  "type"
  //                   equals_int: 7305  # AUTHENTICATION_OPT_OUT_CHECKED
  //                 }
  //                 # Condition 3.2
  //                 index_ui {
  //                   index: -2
  //                   path: "type"
  //                   equals_int: 1  # SOMETHING
  //                 }
  //               }
  //             }
  //           }
  std::unique_ptr<ProtoValue> ConvertEventPredicateFromFile(
      absl::string_view path) {
    ProtoParser parser(ReadTextProtoFromStream(path));
    EventPredicate event_predicate_prototype;
    std::unique_ptr<ProtoValue> event_predicate = 
        parser.Create(event_predicate_prototype);
    MessageValue* event_predicate_ptr = 
        static_cast<MessageValue*>(event_predicate.get());  
    // We are converting event_matcher/ui_element_path_predicate to and here
    // event_matcher {} => and {}
    // ui_element_path_predicate {} => and {}
    std::unique_ptr<ProtoValue> and_field = 
        absl::make_unique<MessageValue>("and", 0, 0);
    MessageValue* and_field_ptr = static_cast<MessageValue*>(and_field.get());

    for (const std::unique_ptr<ProtoValue>& field : 
        event_predicate_ptr->GetFields()) {
      if (field->GetName() == "event_matcher") {
        absl::flat_hash_map<std::string, ProtoValue*> field_name_to_value_ptr = 
            GetFieldNameToValuePtrMap(*static_cast<MessageValue*>(field.get()));
        if (field_name_to_value_ptr.find("int_comparator") != 
            field_name_to_value_ptr.end()) {
          and_field_ptr->AddField(EventMatcherToFieldValue(*field));
        } else {
          and_field_ptr->AddField(EventMatcherToFieldExists(*field));
        }
        
      } else if (field->GetName() == "ui_element_path_predicate") {
        and_field_ptr->AddField(UiElementPathPredicateToUiElement(*field));
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
  void CopyVal(const ProtoValue& original, ProtoValue* copy) {
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
  absl::flat_hash_map<std::string, ProtoValue*> GetFieldNameToValuePtrMap(
      const MessageValue& original) {
    absl::flat_hash_map<std::string, ProtoValue*> field_name_to_value_ptr;
    for (const std::unique_ptr<ProtoValue>& field : original.GetFields()) {
      field_name_to_value_ptr[field->GetName()] = field.get();
    }

    return field_name_to_value_ptr;
  }

  // A universal converter for message matcher into either index_ui,
  // field_value, or field_exists.
  void MessageMatcherConverter(const ProtoValue& original, 
      MessageValue* constructed_field) {
    // Get the name of each field on the original proto.
    absl::flat_hash_map<std::string, ProtoValue*> field_name_to_value_ptr = 
        GetFieldNameToValuePtrMap(*static_cast<const MessageValue*>(&original));

    std::unique_ptr<ProtoValue> path = absl::make_unique<PrimitiveValue>(
      "path", original.GetIndentCount() + 2, 1);
    CopyVal(*field_name_to_value_ptr["input_field"], path.get());
    std::unique_ptr<ProtoValue> equals_int = 
        absl::make_unique<PrimitiveValue>("equals_int", 
        original.GetIndentCount() + 2, 2);
    CopyVal(*field_name_to_value_ptr["int_comparator"], equals_int.get());
    constructed_field->AddField(std::move(path));
    constructed_field->AddField(std::move(equals_int));
  }

  // Original: 
  //            # Condition 3.1
  //            leaf_ui_predicate {
  //              # ...
  //              ui_element_type: STORE_UI_ELEMENT
  //              ui_element_matcher {
  //                input_field: "type"
  //                int_comparator: 7305  # AUTHENTICATION_OPT_OUT_CHECKED
  //              }
  //            }
  // Modified: 
  //            # Condition 3.1
  //            leaf_ui {
  //              path:  "type"
  //              equals_int: 7305  # AUTHENTICATION_OPT_OUT_CHECKED
  //            }
  std::unique_ptr<ProtoValue> LeafUiPredicateToLeafUi(
      const ProtoValue& original) {
    absl::flat_hash_map<std::string, ProtoValue*> field_name_to_value_ptr = 
        GetFieldNameToValuePtrMap(*static_cast<const MessageValue*>(&original));
    std::unique_ptr<ProtoValue> leaf_ui = 
        absl::make_unique<MessageValue>(/*field_name=*/"leaf_ui", 
        original.GetIndentCount() + 2, /*line_number=*/0);
    CopyVal(original, leaf_ui.get());
    MessageMatcherConverter(*field_name_to_value_ptr["ui_element_matcher"],
        static_cast<MessageValue*>(leaf_ui.get()));
    return leaf_ui;
  }

  // Original: 
  //            # Condition 3.2
  //            index_ui_matcher {
  //              ui_element_type: STORE_UI_ELEMENT
  //              index: -2
  //              ui_element_matcher {
  //                input_field: "type"
  //                int_comparator: 1  # SOMETHING
  //              }
  //            }
  // Modified: 
  //            # Condition 3.2
  //            index_ui {
  //              index: -2
  //              path: "type"
  //              equals_int: 1  # SOMETHING
  //            }
  std::unique_ptr<ProtoValue> IndexUiPredicateToIndexUi(
      const ProtoValue& original) {
    absl::flat_hash_map<std::string, ProtoValue*> field_name_to_value_ptr = 
        GetFieldNameToValuePtrMap(*static_cast<const MessageValue*>(&original));
    std::unique_ptr<ProtoValue> index_ui = 
        absl::make_unique<MessageValue>("index_ui", 
        original.GetIndentCount() + 2, 0);
    CopyVal(original, index_ui.get());
    MessageValue* index_ui_ptr = static_cast<MessageValue*>(index_ui.get());

    // Add the index before adding the rest.
    std::unique_ptr<ProtoValue> index_field = 
        absl::make_unique<PrimitiveValue>("index", 
        original.GetIndentCount() + 3, 0);
    CopyVal(*field_name_to_value_ptr["index"], index_field.get());
    index_ui_ptr->AddField(std::move(index_field));

    MessageMatcherConverter(*field_name_to_value_ptr["ui_element_matcher"],
        index_ui_ptr);
    return index_ui;
  }

  // Original: 
  //            # Condition 1
  //            event_matcher {
  //              input_field: "PlayExtension.store.click"
  //            }
  // Modified:
  //            # Condition 1
  //            field_exists: "PlayExtension.store.click"
  std::unique_ptr<ProtoValue> EventMatcherToFieldExists(
      const ProtoValue& original) {
    absl::flat_hash_map<std::string, ProtoValue*> field_name_to_value_ptr = 
        GetFieldNameToValuePtrMap(*static_cast<const MessageValue*>(&original));
    std::unique_ptr<ProtoValue> field_exists = 
        absl::make_unique<PrimitiveValue>("field_exists", 
        original.GetIndentCount() + 1, 0);
    CopyVal(original, field_exists.get());
    PrimitiveValue* field_exists_ptr = static_cast<PrimitiveValue*>(
        field_exists.get());
    PrimitiveValue* input_field_ptr = static_cast<PrimitiveValue*>(
        field_name_to_value_ptr["input_field"]);
    field_exists_ptr->SetVal(input_field_ptr->GetVal());
    return field_exists;
  }

  // Original:
  //           # Condition 2
  //           event_matcher {
  //             input_field: "PlayExtension.log_source"
  //             int_comparator: 65  # WEB_STORE
  //           }
  // Modified:
  //           # Condition 2
  //           field_value {
  //             path: "PlayExtension.log_source"
  //             equals_int: 65  # WEB_STORE
  //           }
  std::unique_ptr<ProtoValue> EventMatcherToFieldValue(
      const ProtoValue& original) {
    // Need to increase indent count by one to account for the fact that this
    // primitive is wrapped in the and {} message.
    std::unique_ptr<ProtoValue> field_value = 
        absl::make_unique<MessageValue>(/*field_name=*/"field_value", 
        /*indent_count=*/original.GetIndentCount() + 1, /*field_line=*/0);
    CopyVal(original, field_value.get());
    MessageMatcherConverter(original, 
        static_cast<MessageValue*>(field_value.get()));
    return field_value;
  }

  // Original:
  //           # Condition 3
  //           ui_element_path_predicate {
  //             ui_element_type: STORE_UI_ELEMENT
  //             # Condition 3.1
  //             leaf_ui_predicate {
  //               # ...
  //               ui_element_type: STORE_UI_ELEMENT
  //               ui_element_matcher {
  //                 input_field: "type"
  //                 int_comparator: 7305  # AUTHENTICATION_OPT_OUT_CHECKED
  //               }
  //             }
  //             # Condition 3.2
  //             index_ui_matcher {
  //               ui_element_type: STORE_UI_ELEMENT
  //               index: -2
  //               ui_element_matcher {
  //                 input_field: "type"
  //                 int_comparator: 1  # SOMETHING
  //               }
  //             }
  //           }
  // Modified:
  //           ui_element {
  //           type: STORE_UI_ELEMENT
  //           and {
  //             # Condition 3.1
  //             leaf_ui {
  //               path:  "type"
  //               equals_int: 7305  # AUTHENTICATION_OPT_OUT_CHECKED
  //             }
  //             # Condition 3.2
  //             index_ui {
  //               index: -2
  //               path: "type"
  //               equals_int: 1  # SOMETHING
  //             }
  //           }
  //         }
  std::unique_ptr<ProtoValue> UiElementPathPredicateToUiElement(
      const ProtoValue& original) {
    std::unique_ptr<ProtoValue> ui_element = absl::make_unique<MessageValue>(
        "ui_element", original.GetIndentCount() + 1, 0);
    MessageValue* ui_element_ptr = static_cast<MessageValue*>(ui_element.get());
    CopyVal(original, ui_element.get());

    absl::flat_hash_map<std::string, ProtoValue*> field_name_to_value_ptr = 
        GetFieldNameToValuePtrMap(*static_cast<const MessageValue*>(&original));

    // Convert ui_element_type to type
    std::unique_ptr<ProtoValue> type = absl::make_unique<PrimitiveValue>(
        "type", 
        field_name_to_value_ptr["ui_element_type"]->GetIndentCount() + 1, 0);
    CopyVal(*field_name_to_value_ptr["ui_element_type"], type.get());
    ui_element_ptr->AddField(std::move(type));

    // Create nested predicate only if they exist.
    if (field_name_to_value_ptr.size() > 1) {
      std::unique_ptr<ProtoValue> and_field = absl::make_unique<MessageValue>(
          "and", original.GetIndentCount() + 2, 1);
      MessageValue* and_field_ptr = static_cast<MessageValue*>(and_field.get());
      if (field_name_to_value_ptr.find("leaf_ui_predicate") != 
          field_name_to_value_ptr.end()) {
        and_field_ptr->AddField(LeafUiPredicateToLeafUi(
            *field_name_to_value_ptr["leaf_ui_predicate"]));
      }
      if (field_name_to_value_ptr.find("index_ui_matcher") != 
          field_name_to_value_ptr.end()) {
        and_field_ptr->AddField(IndexUiPredicateToIndexUi(
            *field_name_to_value_ptr["index_ui_matcher"]));
      }
      ui_element_ptr->AddField(std::move(and_field));
    }

    return ui_element;    
  }
};

void UpdateSampleHelper(absl::string_view output_dir, int idx) {
  EventPredicateConverter converter;

  std::string file_path = absl::StrCat("demo/proto_texts/proto_text(", 
      std::to_string(idx), ").textproto");
  std::unique_ptr<ProtoValue> updated_sample = 
      converter.ConvertEventPredicateFromFile(file_path);
  std::string output_file_path = 
      absl::StrCat(output_dir, "updated_proto_text(", std::to_string(idx), 
      ").textproto");
  std::ofstream outs;
  outs.open(output_file_path);
  outs << updated_sample->PrintToTextProto();
}

} // namespace

int main(int argc, char* argv[]) {
  absl::ParseCommandLine(argc, argv);
  assert(!absl::GetFlag(FLAGS_output_dir).empty());
  EventPredicateConverter converter;
  std::unique_ptr<ProtoValue> output = 
      converter.ConvertEventPredicateFromFile(
      "demo/proto_texts/original_proto_text.textproto");
  std::ofstream outs;
  std::string output_dir = absl::GetFlag(FLAGS_output_dir);
  if(output_dir[output_dir.size() - 1] != '/') {
    output_dir = absl::StrCat(output_dir, "/");
  }
  outs.open(absl::StrCat(output_dir, "updated_proto_text.textproto"));
  outs << output->PrintToTextProto();
  for(int i = 0; i < 10; ++i) {
    UpdateSampleHelper(output_dir, i);
  }
  return 0;
}