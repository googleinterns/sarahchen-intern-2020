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
  
void ProtoParser::PopulateFields(int& prev_field_line, 
    const google::protobuf::TextFormat::ParseInfoTree& tree,
    const google::protobuf::Message& message,
    ProtoValue* proto_value, int indent_count) {
  assert(proto_value != nullptr);
  const google::protobuf::Reflection* reflection = message.GetReflection();
  const google::protobuf::Descriptor* descriptor = message.GetDescriptor();
  MessageValue* message_tmp = 
      dynamic_cast<MessageValue*>(proto_value);
  assert(message_tmp);
  int field_count = descriptor->field_count();
  std::vector<FieldInfo> field_info_list;
  // Populate field_info_list with all the fields in the text proto. 
  for (int i = 0; i < field_count; ++i) {
    const google::protobuf::FieldDescriptor* field_descriptor = 
        descriptor->field(i);
    if (field_descriptor->is_repeated()) {
      int field_size = reflection->FieldSize(message, field_descriptor);
      for (int j = 0; j < field_size; ++j) {
        field_info_list.push_back(FieldInfo(/*line=*/GetLocation(tree, 
            field_descriptor, j), /*index=*/j, field_descriptor));
      }
    } else if(reflection->HasField(message, field_descriptor)) {
      field_info_list.push_back(FieldInfo(/*line=*/GetLocation(tree, 
          field_descriptor, -1), /*index=*/-1, field_descriptor));
    }
  }
  // Sort the fields by their line number.
  std::sort(field_info_list.begin(), field_info_list.end());
  // Set message fields back into message_tmp.
  for (const FieldInfo& field : field_info_list) {
    if (field.field_descriptor->cpp_type() == 
      google::protobuf::FieldDescriptor::CppType::CPPTYPE_MESSAGE) {
      const google::protobuf::Message* nested_message = nullptr;
      if (field.field_descriptor->is_repeated()) {
        nested_message = &reflection->GetRepeatedMessage(message, 
            field.field_descriptor, field.index);
      } else {
        nested_message = &reflection->GetMessage(message, 
            field.field_descriptor);
      }
      google::protobuf::TextFormat::ParseInfoTree* nested_tree = tree.
        GetTreeForNested(field.field_descriptor, field.index);
      // CreateMessage should update prev_field_line to the line the message
      // ends.
      message_tmp->AddField(CreateMessage(*nested_message, 
          *nested_tree, indent_count, prev_field_line, field.line, 
          field.field_descriptor->name()));
    } else {
      message_tmp->AddField(CreatePrimitive(message, field, 
          prev_field_line, indent_count));
      prev_field_line = field.line + 1;
    }
  }
}

std::unique_ptr<ProtoValue> ProtoParser::CreateMessage(
    const google::protobuf::Message& message, 
    const google::protobuf::TextFormat::ParseInfoTree& tree, int indent_count,
    int& last_field_loc, int field_loc, absl::string_view name) {
  // TODO(alexanderlin): Implement.
  return nullptr;
}

void ProtoParser::PopulateComments(int last_field_loc, 
    int field_loc, ProtoValue* message) {
  // TODO(alexanderlin): Implement.
}

std::unique_ptr<ProtoValue> ProtoParser::CreatePrimitive(
    const google::protobuf::Message& message, const FieldInfo& field, 
    int last_field_loc, int indent_count) {
  // TODO(alexanderlin): Implement.
  return nullptr;
}

int ProtoParser::GetLocation(
    const google::protobuf::TextFormat::ParseInfoTree& tree,
    const google::protobuf::FieldDescriptor* field_descriptor, int index) {
  // TODO(alexanderlin): Implement.
  return 0;
}

} // namespace wireless_android_play_analytics