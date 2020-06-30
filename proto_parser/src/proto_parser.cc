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
    const google::protobuf::TextFormat::ParseInfoTree& tree,
    const google::protobuf::Message& message,
    ProtoValue* proto_value, int indent_count) {
  assert(proto_value != nullptr);
  const google::protobuf::Reflection* reflection = message.GetReflection();
  const google::protobuf::Descriptor* descriptor = message.GetDescriptor();
  MessageValue* message_tmp = 
      dynamic_cast<MessageValue*>(proto_value);
  assert(message_tmp);
  std::vector<std::unique_ptr<ProtoValue>>& message_field = message_tmp->
      GetFieldsMutable();
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
      message_field.push_back(CreateMessage(*nested_message, *nested_tree, 
          indent_count, prev_field_line, field.line, 
          field.field_descriptor->name()));
    } else {
      message_field.push_back(CreatePrimitive(message, field, prev_field_line, 
          indent_count));
      prev_field_line = field.line + 1;
    }
  }
}

std::unique_ptr<ProtoValue> ProtoParser::CreateMessage(
    const google::protobuf::Message& message, 
    const google::protobuf::TextFormat::ParseInfoTree& tree, int indent_count,
    int& last_field_loc, int field_loc, absl::string_view name){
  std::unique_ptr<ProtoValue> message_val = 
      absl::make_unique<MessageValue>(std::string(name), indent_count);
  PopulateComments(last_field_loc, field_loc, message_val.get());
  // Make sure the fields start from after the parent message.
  last_field_loc = field_loc + 1;
  PopulateFields(last_field_loc, tree, message, message_val.get(), indent_count + 1);
  return message_val;
}

void ProtoParser::PopulateComments(int last_field_loc, 
    int field_loc, ProtoValue* message) {
  std::string comments_above_field;
  bool first_comment = false;
  for(int i = last_field_loc; i < field_loc; ++i) {
    // Make sure this line is not the closing bracket of a nested message
    if(!first_comment) {
      for (const char& c : lines_[i]) {
        if (c != '}' && c != ' ' && c != '\t') {
          first_comment = true;
        }
      }
    }
    if (first_comment) {
      comments_above_field += lines_[i] + "\n";
    }
  }
  // Figure out if the field is a string
  std::string comment_behind_field;
  std::string field_line = lines_[field_loc];
  size_t begin_quote_idx = field_line.find('\"');
  size_t pound_idx = field_line.find('#');
  
  if (begin_quote_idx != std::string::npos) {
    size_t end_quote_idx = field_line.find("\"", begin_quote_idx + 1);
    // Only if beginning and end quote exist is this a string.
    if (end_quote_idx != std::string::npos) {
      while(pound_idx != std::string::npos) {
        if (pound_idx < begin_quote_idx) {
          // If pound sign is before the string, then the string must be part of
          // the comment eg. a: 2 #"hi"comment.
          comment_behind_field = field_line.substr(pound_idx);
          break;
        }
        else if (pound_idx > end_quote_idx) {
          // If pound sign is after the string, then it must indicate beginning 
          // of comment eg. a : "hi" #comment.
          comment_behind_field = field_line.substr(pound_idx);
          break;
        }
        pound_idx = field_line.find('#', pound_idx + 1);
      }
    }
  }
  else if (pound_idx != std::string::npos) {
    comment_behind_field = field_line.substr(pound_idx);
  }
  message->SetCommentBehindField(comment_behind_field);
  message->SetCommentAboveField(comments_above_field);
}

std::unique_ptr<ProtoValue> ProtoParser::CreatePrimitive(
    const google::protobuf::Message& message, const FieldInfo& field, 
    int last_field_loc, int indent_count) {
  const google::protobuf::Reflection* reflection = message.GetReflection();
  int field_loc = field.line;
  int index = field.index;
  const google::protobuf::FieldDescriptor* field_descriptor = 
      field.field_descriptor;
  // String used for GetRepeatedStringReference.
  std::string str;
  std::unique_ptr<ProtoValue> message_val = absl::make_unique<PrimitiveValue>(
      field_descriptor->name(), indent_count);
  PopulateComments(last_field_loc, field_loc, message_val.get());
  PrimitiveValue* primitive = dynamic_cast<PrimitiveValue*>(message_val.get());
  if (field_descriptor->is_repeated()) {
    switch(field_descriptor->cpp_type()){
      case google::protobuf::FieldDescriptor::CppType::CPPTYPE_INT32:
        primitive->SetVal(reflection->GetRepeatedInt32(message, 
            field_descriptor, index));
        break;
      case google::protobuf::FieldDescriptor::CppType::CPPTYPE_INT64:
        primitive->SetVal(reflection->GetRepeatedInt64(message, 
            field_descriptor, index));
        break;
      case google::protobuf::FieldDescriptor::CppType::CPPTYPE_UINT32:
        primitive->SetVal(reflection->GetRepeatedUInt32(message, 
            field_descriptor, index));
        break;
      case google::protobuf::FieldDescriptor::CppType::CPPTYPE_UINT64:
        primitive->SetVal(reflection->GetRepeatedUInt64(message, 
            field_descriptor, index));
        break;
      case google::protobuf::FieldDescriptor::CppType::CPPTYPE_DOUBLE:
        primitive->SetVal(reflection->GetRepeatedDouble(message, 
            field_descriptor, index));
        break;
      case google::protobuf::FieldDescriptor::CppType::CPPTYPE_FLOAT:
        primitive->SetVal(reflection->GetRepeatedFloat(message, 
            field_descriptor, index));
        break;
      case google::protobuf::FieldDescriptor::CppType::CPPTYPE_BOOL:
        primitive->SetVal(reflection->GetRepeatedBool(message, 
            field_descriptor, index));
        break;
      case google::protobuf::FieldDescriptor::CppType::CPPTYPE_ENUM:
        primitive->SetVal(reflection->GetRepeatedEnum(message, 
            field_descriptor, index));
        break;
      case google::protobuf::FieldDescriptor::CppType::CPPTYPE_STRING:
        str = reflection->GetRepeatedStringReference(message, field_descriptor, 
            index, &str);
        primitive->SetVal(str);
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
    const google::protobuf::FieldDescriptor* field_descriptor, int index) {
  google::protobuf::TextFormat::ParseLocation location = 
      tree.GetLocation(field_descriptor, index);
  return location.line;
}

} // namespace wireless_android_play_analytics