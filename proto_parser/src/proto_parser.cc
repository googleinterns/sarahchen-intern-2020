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

void ProtoParser::PopulateFields(UpperLayerInfo* prev_field_line, 
    const google::protobuf::TextFormat::ParseInfoTree& tree,
    const google::protobuf::Message& message,
    ProtoValue* proto_value, int indent_count) {
  // TODO(alexanderlin): Implement.
}

std::unique_ptr<ProtoValue> ProtoParser::CreateMessage(
    const google::protobuf::Message& message, 
    const google::protobuf::TextFormat::ParseInfoTree& tree, int indent_count,
    UpperLayerInfo* last_field_loc, int field_loc, absl::string_view name) {
  // TODO(alexanderlin): Implement.
  return nullptr;
}

void ProtoParser::PopulateComments(const UpperLayerInfo& last_field_loc, 
    int field_loc, ProtoValue* message) {
  // TODO(alexanderlin): Implement.
}

// TODO: Change return type to primitive value
std::unique_ptr<ProtoValue> ProtoParser::CreatePrimitive(
    const google::protobuf::Message& message, const FieldInfo& field, 
    const UpperLayerInfo& last_field_loc, int indent_count) {
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