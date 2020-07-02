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
  
void ProtoParser::PopulateFields(const google::protobuf::Message& message,
    const google::protobuf::TextFormat::ParseInfoTree& tree, int indent_count,
    ProtoValue* proto_value) {
  assert(proto_value != nullptr);
  const google::protobuf::Reflection* reflection = message.GetReflection();
  const google::protobuf::Descriptor* descriptor = message.GetDescriptor();
  MessageValue* message_tmp = 
      dynamic_cast<MessageValue*>(proto_value);
  assert(message_tmp);
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
              tree.GetTreeForNested(field_descriptor, index);
          message_tmp->AddField(CreateMessage(nested_message, *nested_tree,
              indent_count, field_loc, field_descriptor->name()));
        } else {
          message_tmp->AddField(CreatePrimitive(message, field_loc, index,
              field_descriptor, indent_count));
        }
      }
    } else if(reflection->HasField(message, field_descriptor)) {
      int field_loc = GetLocation(tree, field_descriptor, -1);
        if (field_descriptor->cpp_type() == 
            google::protobuf::FieldDescriptor::CppType::CPPTYPE_MESSAGE) {
          const google::protobuf::Message& nested_message = 
              reflection->GetMessage(message, field_descriptor);
          google::protobuf::TextFormat::ParseInfoTree* nested_tree = 
              tree.GetTreeForNested(field_descriptor, -1);
          message_tmp->AddField(CreateMessage(nested_message, *nested_tree,
              indent_count, field_loc, field_descriptor->name()));
        } else {
          message_tmp->AddField(CreatePrimitive(message, field_loc, -1,
              field_descriptor, indent_count));
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
      absl::make_unique<MessageValue>(std::string(name), indent_count, 
      field_loc);
  PopulateComments(field_loc, message_val.get());
  PopulateFields(message, tree, indent_count + 1, message_val.get());
  return message_val;
}

void ProtoParser::PopulateComments(int field_loc, ProtoValue* message) {
  int comment_above_idx = field_loc - 1;
  std::vector<std::string> comment_above_tmp;

  // Acquire all comments.
  while(comment_above_idx > 0) {
    size_t first_non_space = 
        lines_[comment_above_idx].find_first_not_of(" \t");
    if (first_non_space == std::string::npos) {
      comment_above_idx--;
    } else if (lines_[comment_above_idx][first_non_space] == '#') {
      comment_above_tmp.push_back(
          lines_[comment_above_idx].substr(first_non_space));
      comment_above_idx--;
    } else {
      break;
    }
  }

  // Add comments into the message.
  std::vector<std::string>& comments_above_field = 
      message->GetCommentAboveFieldMutable();
  for(int i = comment_above_tmp.size() - 1; i >= 0; --i) {
    comments_above_field.push_back(comment_above_tmp[i]);
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
}

std::unique_ptr<ProtoValue> ProtoParser::CreatePrimitive(
    const google::protobuf::Message& message, int field_loc, int index, 
    const google::protobuf::FieldDescriptor* field_descriptor, 
    int indent_count) {
  const google::protobuf::Reflection* reflection = message.GetReflection();
  // String used for GetRepeatedStringReference.
  std::string str;
  std::unique_ptr<ProtoValue> message_val = absl::make_unique<PrimitiveValue>(
      field_descriptor->name(), indent_count, field_loc);
  PopulateComments(field_loc, message_val.get());
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