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
  // TODO(alexanderlin): Implement.
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