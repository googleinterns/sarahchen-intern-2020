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
#include "proto_parser.h"
#include "message_value.h"

namespace wireless_android_play_analytics {

std::string ProtoValue::PrintToTextProto() {
  return PrintToTextProtoHelper();
}

std::unique_ptr<ProtoValue> ProtoValue::Create(absl::string_view text_proto, 
    google::protobuf::Message& message) {
  google::protobuf::TextFormat::Parser parser;
  google::protobuf::TextFormat::ParseInfoTree tree;
  parser.WriteLocationsTo(&tree);
  parser.ParseFromString(std::string(text_proto), &message);
  ProtoParser proto_parser(text_proto);
  
  // Root Message has no field_name.
  std::unique_ptr<ProtoValue> message_val = absl::make_unique<MessageValue>("", 
      0, 0); 

  proto_parser.PopulateFields(message, tree, 0, message_val.get());
  return message_val;
}

void ProtoValue::SetCommentAboveField(absl::string_view val) {
  comments_above_field_ = absl::StrSplit(val, '\n');

  // Add pound before comment if user forgets.
  for (std::string& comment : comments_above_field_) {
    if (!comment.empty() && comment[0] != '#') {
      comment = "# " + comment;
    }
  }
}

} // namespace wireless_android_play_analytics