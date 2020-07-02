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
  // TODO(alexanderlin): Implement.
}

std::unique_ptr<ProtoValue> ProtoParser::CreateMessage(
    const google::protobuf::Message& message, 
    const google::protobuf::TextFormat::ParseInfoTree& tree, int indent_count,
    int field_loc, absl::string_view name){
  // TODO(alexanderlin): Implement.
  return nullptr;
}

void ProtoParser::PopulateComments(int field_loc, ProtoValue* message) {
  // TODO(alexanderlin): Implement.
}

std::unique_ptr<ProtoValue> ProtoParser::CreatePrimitive(
    const google::protobuf::Message& message, int field_loc, int index, 
    const google::protobuf::FieldDescriptor* field_descriptor, 
    int indent_count) {
  // TODO(alexanderlin): Implement.
  return nullptr;
}

int ProtoParser::GetLocation(
    const google::protobuf::TextFormat::ParseInfoTree& tree,
    const google::protobuf::FieldDescriptor* field_descriptor, int index) {
  google::protobuf::TextFormat::ParseLocation location = 
      tree.GetLocation(field_descriptor, index);
  return location.line;
}

} // namespace wireless_android_play_analytics