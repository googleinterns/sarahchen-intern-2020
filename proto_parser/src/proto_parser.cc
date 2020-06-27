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

namespace wireless_android_play_analytics {

void ProtoParser::PopulateFields(int& prev_field_line, 
  const google::protobuf::TextFormat::ParseInfoTree* tree,
  const google::protobuf::Message& message,
  std::shared_ptr<ProtoValue>& proto_value) {
  const google::protobuf::Reflection* reflection = message.GetReflection();
  const google::protobuf::Descriptor* descriptor = message.GetDescriptor();
  std::shared_ptr<MessageValue> message_tmp = 
    std::dynamic_pointer_cast<MessageValue>(proto_value);
  assert(message_tmp);
  std::vector<std::shared_ptr<ProtoValue>>& message_field = message_tmp->
    GetFieldsMutable();
  int field_count = descriptor->field_count();
  std::vector<FieldInfo> field_info_list;
  // Get the order of textproto
  for (int i = 0; i < field_count; ++i) {
    const google::protobuf::FieldDescriptor* field_descriptor = 
      descriptor->field(i);
    if (field_descriptor->is_repeated()) {
      int field_size = reflection->FieldSize(message, field_descriptor);
      for (int j = 0; j < field_size; ++j) {
        field_info_list.push_back(FieldInfo(GetLocation(tree, field_descriptor, 
          j), j, field_descriptor));
      }
    } else if(reflection->HasField(message, field_descriptor)) {
      field_info_list.push_back(FieldInfo(GetLocation(tree, field_descriptor, 
      -1), -1, field_descriptor));
    }
  }
  std::sort(field_info_list.begin(), field_info_list.end());
  // Get every field of this proto.
  for (const FieldInfo& field : field_info_list) {
    if (field.field_descriptor_->cpp_type() == 
      google::protobuf::FieldDescriptor::CppType::CPPTYPE_MESSAGE) {
      const google::protobuf::Message* nested_message = nullptr;
      if (field.field_descriptor_->is_repeated()) {
        nested_message = &reflection->GetRepeatedMessage(message, 
          field.field_descriptor_, field.index_);
      }
      else {
        nested_message = &reflection->GetMessage(message, 
          field.field_descriptor_);
      }
      google::protobuf::TextFormat::ParseInfoTree* nested_tree = tree->
        GetTreeForNested(field.field_descriptor_, field.index_);
      // CreateMessage should update prev_field_line to the line the message
      // ends.
      message_field.push_back(std::dynamic_pointer_cast<MessageValue>(
        CreateMessage(*nested_message, nested_tree, prev_field_line, field.line_, 
        field.field_descriptor_->name())));
    }
    else {
      message_field.push_back(std::dynamic_pointer_cast<ProtoValue>(
        CreatePrimitive(message, field, prev_field_line)));
      prev_field_line = field.line_ + 1;
    }
  }
}

std::shared_ptr<MessageValue> ProtoParser::CreateMessage(
  const google::protobuf::Message& message, 
  const google::protobuf::TextFormat::ParseInfoTree* tree,
  int& last_field_loc, int field_loc, const std::string& name) {
  // TODO(alexanderlin): Implement.
  return nullptr;
}

void ProtoParser::PopulateComments(int last_field_loc, int field_loc, 
  std::shared_ptr<ProtoValue> message) {
  // TODO(alexanderlin): Implement.
}

// TODO: Change return type to primitive value
std::shared_ptr<PrimitiveValue> ProtoParser::CreatePrimitive(
  const google::protobuf::Message& message, const FieldInfo& field,
  int last_field_loc) {
  // TODO(alexanderlin): Implement.
  return nullptr;
}

int ProtoParser::GetLocation(
  const google::protobuf::TextFormat::ParseInfoTree* tree,
  const google::protobuf::FieldDescriptor* field_descriptor, int index) {
  // TODO(alexanderlin): Implement.
  return 0;
}

void ProtoParser::DelimiteTextProto(absl::string_view text_proto) {
  // TODO(alexanderlin): Implement.
}

} // namespace wireless_android_play_analytics