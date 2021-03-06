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

namespace{

  std::string Trim(absl::string_view s) {
    const absl::string_view prefix = " \t";
    size_t first_non_space = s.find_first_not_of(prefix);
    if (first_non_space == absl::string_view::npos) {
      return std::string();
    } else {
      return std::string(s.substr(first_non_space));
    }
  }
} // namespace

std::unique_ptr<ProtoValue> ProtoParser::Create(
    google::protobuf::Message& message) {
  google::protobuf::TextFormat::Parser google_parser;
  google::protobuf::TextFormat::ParseInfoTree tree;
  google_parser.WriteLocationsTo(&tree);
  google_parser.ParseFromString(text_proto_, &message);
  
  // Root Message has no field_name and no indentation.
  std::unique_ptr<ProtoValue> message_val = absl::make_unique<MessageValue>(
      /*field_name=*/"", /*indent_count=*/0, /*field_line=*/0); 

  PopulateFields(message, tree, /*indent_count=*/0, 
      message_val.get());

  return message_val;
}
  
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
  int comment_above_loc = field_loc - 1;

  std::vector<std::string> comments_above;

  // Acquire all comments.
  while(comment_above_loc >= 0) {
    std::string line_content = Trim(lines_[comment_above_loc]);

    if (line_content.empty() || absl::StartsWith(line_content, "#")) {
      comments_above.push_back(line_content);
    } else {
      break;
    }

    --comment_above_loc;
  }
  
  // Revert the vector to make sure they're in the correct order.
  std::reverse(comments_above.begin(), comments_above.end());
  message->SetCommentAboveField(comments_above);

  size_t comment_behind_starting_position = 0;
  // If field is a string need to modify when the search starts.
  PrimitiveValue* primitive = static_cast<PrimitiveValue*>(message);
  // If field is a message, casting to primitive value will result in returning
  // a nullptr.
  if (primitive != nullptr) {
    if (absl::holds_alternative<std::string>(primitive->GetVal())) {
      std::string val = absl::get<std::string>(primitive->GetVal());
      comment_behind_starting_position = lines_[field_loc].find(val) + 
          val.size();
    }
  }

  size_t comment_start = lines_[field_loc].find('#', 
      comment_behind_starting_position);
  
  if (comment_start != std::string::npos) {
    message->SetCommentBehindField(lines_[field_loc].substr(comment_start));
  }
}

std::unique_ptr<ProtoValue> ProtoParser::CreatePrimitive(
    const google::protobuf::Message& message, int field_loc, 
    int repeated_field_index, 
    const google::protobuf::FieldDescriptor* field_descriptor, 
    int indent_count) {
  const google::protobuf::Reflection* reflection = message.GetReflection();

  std::unique_ptr<ProtoValue> message_val = absl::make_unique<PrimitiveValue>(
      field_descriptor->name(), indent_count, field_loc);
  PopulateComments(field_loc, message_val.get());
  PrimitiveValue* primitive = dynamic_cast<PrimitiveValue*>(message_val.get());
  if (field_descriptor->is_repeated()) {
    switch(field_descriptor->cpp_type()){
      case google::protobuf::FieldDescriptor::CppType::CPPTYPE_INT32:
        primitive->SetVal(reflection->GetRepeatedInt32(message, 
            field_descriptor, repeated_field_index));
        break;
      case google::protobuf::FieldDescriptor::CppType::CPPTYPE_INT64:
        primitive->SetVal(reflection->GetRepeatedInt64(message, 
            field_descriptor, repeated_field_index));
        break;
      case google::protobuf::FieldDescriptor::CppType::CPPTYPE_UINT32:
        primitive->SetVal(reflection->GetRepeatedUInt32(message, 
            field_descriptor, repeated_field_index));
        break;
      case google::protobuf::FieldDescriptor::CppType::CPPTYPE_UINT64:
        primitive->SetVal(reflection->GetRepeatedUInt64(message, 
            field_descriptor, repeated_field_index));
        break;
      case google::protobuf::FieldDescriptor::CppType::CPPTYPE_DOUBLE:
        primitive->SetVal(reflection->GetRepeatedDouble(message, 
            field_descriptor, repeated_field_index));
        break;
      case google::protobuf::FieldDescriptor::CppType::CPPTYPE_FLOAT:
        primitive->SetVal(reflection->GetRepeatedFloat(message, 
            field_descriptor, repeated_field_index));
        break;
      case google::protobuf::FieldDescriptor::CppType::CPPTYPE_BOOL:
        primitive->SetVal(reflection->GetRepeatedBool(message, 
            field_descriptor, repeated_field_index));
        break;
      case google::protobuf::FieldDescriptor::CppType::CPPTYPE_ENUM:
        primitive->SetVal(reflection->GetRepeatedEnum(message, 
            field_descriptor, repeated_field_index));
        break;
      case google::protobuf::FieldDescriptor::CppType::CPPTYPE_STRING: {
          std::string str;   
          str = reflection->GetRepeatedStringReference(message, field_descriptor, 
              repeated_field_index, &str);
          primitive->SetVal(str);
        }
        break;
      case google::protobuf::FieldDescriptor::CppType::CPPTYPE_MESSAGE:
        assert(false);
    }
  }
  else {
    switch(field_descriptor->cpp_type()){
      case google::protobuf::FieldDescriptor::CppType::CPPTYPE_INT32:
        primitive->SetVal(reflection->GetInt32(message, field_descriptor));
        break;
      case google::protobuf::FieldDescriptor::CppType::CPPTYPE_INT64:
        primitive->SetVal(reflection->GetInt64(message, field_descriptor));
        break;
      case google::protobuf::FieldDescriptor::CppType::CPPTYPE_UINT32:
        primitive->SetVal(reflection->GetUInt32(message, field_descriptor));
        break;
      case google::protobuf::FieldDescriptor::CppType::CPPTYPE_UINT64:
        primitive->SetVal(reflection->GetUInt64(message, field_descriptor));
        break;
      case google::protobuf::FieldDescriptor::CppType::CPPTYPE_DOUBLE:
        primitive->SetVal(reflection->GetDouble(message, field_descriptor));
        break;
      case google::protobuf::FieldDescriptor::CppType::CPPTYPE_FLOAT:
        primitive->SetVal(reflection->GetFloat(message, field_descriptor));
        break;
      case google::protobuf::FieldDescriptor::CppType::CPPTYPE_BOOL:
        primitive->SetVal(reflection->GetBool(message, field_descriptor));
        break;
      case google::protobuf::FieldDescriptor::CppType::CPPTYPE_ENUM:
        primitive->SetVal(reflection->GetEnum(message, field_descriptor));
        break;
      case google::protobuf::FieldDescriptor::CppType::CPPTYPE_STRING:
        primitive->SetVal(reflection->GetString(message, field_descriptor));
        break;
      case google::protobuf::FieldDescriptor::CppType::CPPTYPE_MESSAGE:
        assert(false);
    }
  }
  return message_val;
}

int ProtoParser::GetLocation(
    const google::protobuf::TextFormat::ParseInfoTree& tree,
    const google::protobuf::FieldDescriptor* field_descriptor, 
    int repeated_field_index) {
  return tree.GetLocation(field_descriptor, repeated_field_index).line;
}

} // namespace wireless_android_play_analytics