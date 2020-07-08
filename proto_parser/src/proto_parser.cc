/*
*   Copyright 2020 Google LLC
*
*   Licensed under the Apache License, Version 2.0 (the "License");
*   you may not use this file except in compliance with the License.
*   You may obtain a copy of the License at
*
*    https://www.apache.org/licenses/LICENSE-2.0
*
*   Unless required by applicable law or agreed to in writing, software
*   distributed under the License is distributed on an "AS IS" BASIS,
*   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*   See the License for the specific language governing permissions and
*   limitations under the License.
*/

#include "proto_parser.h"
#include "message_value.h"
#include "primitive_value.h"

namespace wireless_android_play_analytics {
  
void ProtoParser::PopulateFields(const google::protobuf::Message& message,
    const google::protobuf::TextFormat::ParseInfoTree& tree, int indent_count,
    ProtoValue* proto_value) {
  assert(proto_value != nullptr);
  const google::protobuf::Reflection* reflection = message.GetReflection();
  const google::protobuf::Descriptor* descriptor = message.GetDescriptor();
  MessageValue* message_tmp = 
      dynamic_cast<MessageValue*>(proto_value);
  assert(message_tmp != nullptr);
  int field_count = descriptor->field_count();

  // Populate field_info_list with all the fields in the text proto. 
  for (int i = 0; i < field_count; ++i) {
    const google::protobuf::FieldDescriptor* field_descriptor = 
        descriptor->field(i);
    if (field_descriptor->is_repeated()) {
      int field_size = reflection->FieldSize(message, field_descriptor);
      for (int index = 0; index < field_size; ++index) {
        int field_loc = GetLocation(tree, field_descriptor, index);
        if (field_descriptor->cpp_type() == 
            google::protobuf::FieldDescriptor::CppType::CPPTYPE_MESSAGE) {
          const google::protobuf::Message& nested_message = 
              reflection->GetRepeatedMessage(message, field_descriptor, index);
          google::protobuf::TextFormat::ParseInfoTree* nested_tree = 
              tree.GetTreeForNested(field_descriptor, 
              /*repeated_field_index=*/index);
          message_tmp->AddField(CreateMessage(nested_message, *nested_tree,
              indent_count, field_loc, field_descriptor->name()));
        } else {
          message_tmp->AddField(CreatePrimitive(message, field_loc, 
              /*repeated_field_index=*/index, field_descriptor, indent_count));
        }
      }
    } else if(reflection->HasField(message, field_descriptor)) {
      int field_loc = GetLocation(tree, field_descriptor, -1);
        if (field_descriptor->cpp_type() == 
            google::protobuf::FieldDescriptor::CppType::CPPTYPE_MESSAGE) {
          const google::protobuf::Message& nested_message = 
              reflection->GetMessage(message, field_descriptor);
          google::protobuf::TextFormat::ParseInfoTree* nested_tree = 
              tree.GetTreeForNested(field_descriptor, 
              /*repeated_field_index=*/-1);
          message_tmp->AddField(CreateMessage(nested_message, *nested_tree,
              indent_count, field_loc, field_descriptor->name()));
        } else {
          message_tmp->AddField(CreatePrimitive(message, field_loc, 
              /*repeated_field_index=*/-1, field_descriptor, indent_count));
        }
    }
  }

  message_tmp->SortFields();
}

std::unique_ptr<ProtoValue> ProtoParser::CreateMessage(
    const google::protobuf::Message& message, 
    const google::protobuf::TextFormat::ParseInfoTree& tree, int indent_count,
    int field_loc, absl::string_view name){
  std::unique_ptr<ProtoValue> message_val = 
      absl::make_unique<MessageValue>(name, indent_count, field_loc);

  PopulateComments(field_loc, message_val.get());
  PopulateFields(message, tree, indent_count + 1, message_val.get());
  
  return message_val;
}

void ProtoParser::PopulateComments(int field_loc, ProtoValue* message) {
  // TODO(alexanderlin): Implement.
}

std::unique_ptr<ProtoValue> ProtoParser::CreatePrimitive(
    const google::protobuf::Message& message, int field_loc, 
    int repeated_field_index, 
    const google::protobuf::FieldDescriptor* field_descriptor, 
    int indent_count) {
  // TODO(alexanderlin): Implement.
  return nullptr;
}

int ProtoParser::GetLocation(
    const google::protobuf::TextFormat::ParseInfoTree& tree,
    const google::protobuf::FieldDescriptor* field_descriptor, 
    int repeated_field_index) {
  return tree.GetLocation(field_descriptor, repeated_field_index).line;
}

} // namespace wireless_android_play_analytics