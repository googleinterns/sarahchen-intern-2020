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
  // TODO: message-> 
  const google::protobuf::Message* message,
  // ProtoValue*
  std::shared_ptr<ProtoValue>& message_val) {
  const google::protobuf::Reflection* reflection = message->GetReflection();
  const google::protobuf::Descriptor* descriptor = message->GetDescriptor();
  std::shared_ptr<MessageValue> message_tmp = std::dynamic_pointer_cast<MessageValue>(message_val);
  assert(message_tmp);
  std::vector<std::shared_ptr<ProtoValue>>& message_field = message_tmp->
    GetFieldsMutable();
  int field_count = descriptor->field_count();
  //TODO field_list -> field_info_list
  std::vector<FieldInformation> field_list;
  // Get the order of textproto
  for (int i = 0; i < field_count; ++i) {
    // TODO: field -> field_descriptor
    const google::protobuf::FieldDescriptor* field = descriptor->field(i);
    if (field->is_repeated()) {
      int field_size = reflection->FieldSize(*message, field);
      for (int j = 0; j < field_size; ++j) {
        field_list.push_back(FieldInformation(GetLocation(tree, field, j), j, 
          field));
      }
    } else if(reflection->HasField(*message, field)) {
      field_list.push_back(FieldInformation(GetLocation(tree, field, -1), -1,
      field));
    }
  }
  std::sort(field_list.begin(), field_list.end());
  // Get every field of this proto.
  for (const FieldInformation& field : field_list) {
    if (field.field_descriptor_->cpp_type() == 
      google::protobuf::FieldDescriptor::CppType::CPPTYPE_MESSAGE) {
      const google::protobuf::Message* nested_message = nullptr;
      if (field.field_descriptor_->is_repeated()) {
        nested_message = &reflection->GetRepeatedMessage(*message, 
          field.field_descriptor_, field.index_);
      }
      else {
        nested_message = &reflection->GetMessage(*message, field.field_descriptor_);
      }
      google::protobuf::TextFormat::ParseInfoTree* nested_tree = tree->GetTreeForNested(
        field.field_descriptor_, field.index_);
      // CreateMessage should update prev_field_line to the line the message
      // ends.
      message_field.push_back(CreateMessage(nested_message, nested_tree, 
        prev_field_line, field.line_, 
        field.field_descriptor_->name()));
    }
    else {
      message_field.push_back(CreatePrimitive(message, field, prev_field_line));
      prev_field_line = field.line_ + 1;
    }
  }
}

std::shared_ptr<ProtoValue> ProtoParser::CreateMessage(
  const google::protobuf::Message* message, 
  const google::protobuf::TextFormat::ParseInfoTree* tree,
  int& last_field_loc, int field_loc, const std::string& name){
  std::shared_ptr<ProtoValue> message_val = 
    std::make_shared<MessageValue>(name);
  PopulateComments(last_field_loc, field_loc, message_val);
  // Make sure the fields start from after the parent message.
  last_field_loc = field_loc + 1;
  PopulateFields(last_field_loc, tree, message, message_val);
  return message_val;
}

void ProtoParser::PopulateComments(int last_field_loc, 
  int field_loc, std::shared_ptr<ProtoValue>& message) {
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

std::shared_ptr<ProtoValue> ProtoParser::CreatePrimitive(
  const google::protobuf::Message* message, const FieldInformation& field, 
  int last_field_loc) {
  const google::protobuf::Reflection* reflection = message->GetReflection();
  int field_loc = field.line_;
  int index = field.index_;
  const google::protobuf::FieldDescriptor* field_descriptor = 
    field.field_descriptor_;
  // String used for GetRepeatedStringReference.
  std::string str;
  std::shared_ptr<ProtoValue> message_val = std::make_shared<PrimitiveValue>(
    field_descriptor->name());
  PopulateComments(last_field_loc, field_loc, message_val);
  std::shared_ptr<PrimitiveValue> primitive = 
    std::dynamic_pointer_cast<PrimitiveValue>(message_val);
  assert(primitive);
  if (field_descriptor->is_repeated()) {
    switch(field_descriptor->cpp_type()){
      case google::protobuf::FieldDescriptor::CppType::CPPTYPE_INT32:
        primitive->SetVal(reflection->GetRepeatedInt32(*message, 
          field_descriptor, index));
        break;
      case google::protobuf::FieldDescriptor::CppType::CPPTYPE_INT64:
        primitive->SetVal(reflection->GetRepeatedInt64(*message, 
          field_descriptor, index));
        break;
      case google::protobuf::FieldDescriptor::CppType::CPPTYPE_UINT32:
        primitive->SetVal(reflection->GetRepeatedUInt32(*message, 
          field_descriptor, index));
        break;
      case google::protobuf::FieldDescriptor::CppType::CPPTYPE_UINT64:
        primitive->SetVal(reflection->GetRepeatedUInt64(*message, 
          field_descriptor, index));
        break;
      case google::protobuf::FieldDescriptor::CppType::CPPTYPE_DOUBLE:
        primitive->SetVal(reflection->GetRepeatedDouble(*message, 
          field_descriptor, index));
        break;
      case google::protobuf::FieldDescriptor::CppType::CPPTYPE_FLOAT:
      primitive->SetVal(reflection->GetRepeatedFloat(*message, field_descriptor, 
        index));
        break;
      case google::protobuf::FieldDescriptor::CppType::CPPTYPE_BOOL:
        primitive->SetVal(reflection->GetRepeatedBool(*message, 
          field_descriptor, index));
        break;
      case google::protobuf::FieldDescriptor::CppType::CPPTYPE_ENUM:
        primitive->SetVal(reflection->GetRepeatedEnum(*message, 
          field_descriptor, index));
        break;
      case google::protobuf::FieldDescriptor::CppType::CPPTYPE_STRING:
        str = reflection->GetRepeatedStringReference(*message, field_descriptor, 
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
        primitive->SetVal(reflection->GetInt32(*message, field_descriptor));
        break;
      case google::protobuf::FieldDescriptor::CppType::CPPTYPE_INT64:
        primitive->SetVal(reflection->GetInt64(*message, field_descriptor));
        break;
      case google::protobuf::FieldDescriptor::CppType::CPPTYPE_UINT32:
        primitive->SetVal(reflection->GetUInt32(*message, field_descriptor));
        break;
      case google::protobuf::FieldDescriptor::CppType::CPPTYPE_UINT64:
        primitive->SetVal(reflection->GetUInt64(*message, field_descriptor));
        break;
      case google::protobuf::FieldDescriptor::CppType::CPPTYPE_DOUBLE:
        primitive->SetVal(reflection->GetDouble(*message, field_descriptor));
        break;
      case google::protobuf::FieldDescriptor::CppType::CPPTYPE_FLOAT:
        primitive->SetVal(reflection->GetFloat(*message, field_descriptor));
        break;
      case google::protobuf::FieldDescriptor::CppType::CPPTYPE_BOOL:
        primitive->SetVal(reflection->GetBool(*message, field_descriptor));
        break;
      case google::protobuf::FieldDescriptor::CppType::CPPTYPE_ENUM:
        primitive->SetVal(reflection->GetEnum(*message, field_descriptor));
        break;
      case google::protobuf::FieldDescriptor::CppType::CPPTYPE_STRING:
        primitive->SetVal(reflection->GetString(*message, field_descriptor));
        break;
      case google::protobuf::FieldDescriptor::CppType::CPPTYPE_MESSAGE:
        assert(false);
    }
  }
  return std::dynamic_pointer_cast<ProtoValue>(primitive);
}

int ProtoParser::GetLocation(const google::protobuf::TextFormat::ParseInfoTree* tree,
  const google::protobuf::FieldDescriptor* field_descriptor, int index) {
  google::protobuf::TextFormat::ParseLocation location = 
    tree->GetLocation(field_descriptor, index);
  return location.line;
}

void ProtoParser::DelimiteTextProto(const std::string& text_proto) {
  std::istringstream iss(text_proto);
  // Split the text_proto into a vector delimited by endlines.
  while(iss.good()) {
    std::string tmp;
    getline(iss, tmp);
    lines_.push_back(tmp);
  }
}

} // namespace wireless_android_play_analytics