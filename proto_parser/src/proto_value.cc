/*
*   Copyright 2020 Google LLC
*
*   Licensed under the Apache License, Version 2.0 (the "License");
*   you may not use this file except in compliance with the License.
*   You may obtain a copy of the License at
*
*       https://www.apache.org/licenses/LICENSE-2.0
*
*   Unless required by applicable law or agreed to in writing, software
*   distributed under the License is distributed on an "AS IS" BASIS,
*   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*   See the License for the specific language governing permissions and
*   limitations under the License.
*/

#include "proto_value.h"

namespace wireless_android_play_analytics {

std::string ProtoValue::PrintToTextProto() {
  return PrintToTextProtoHelper(0);
}

std::string ProtoValue::PrintToTextProtoHelper(int indent_count) {
  // TODO(alexanderlin): Implement.
  return std::string();
}

// std::unique_ptr<ProtoValue> ProtoValue::Create(absl::string_view text_proto, 
//   google::protobuf::Message* message) {
//   const google::protobuf::Descriptor* descriptor = message->GetDescriptor();
//   google::protobuf::TextFormat::Parser parser;
//   google::protobuf::TextFormat::ParseInfoTree tree;
//   parser.WriteLocationsTo(&tree);
//   parser.ParseFromString(static_cast<std::string>(text_proto), message);
//   ProtoParser proto_parser;
//   proto_parser.DelimiteTextProto(text_proto);
  
//   int last_field_loc = 0;
//   // Root Message has no field_name;
//   std::unique_ptr<MessageValue> message_val = absl::make_unique<MessageValue>(""); 
//   std::vector<std::unique_ptr<ProtoValue>>& message_field = 
//     message_val->GetFieldsMutable();
//   proto_parser.PopulateFields(last_field_loc, descriptor, &tree, message,
//     message_field);
//   return message_val;
// }

} // namespace wireless_android_play_analytics