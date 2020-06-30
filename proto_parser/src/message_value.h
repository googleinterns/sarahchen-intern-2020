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

#ifndef WIRELESS_ANDROID_PLAY_ANALYTICS_PROTO_PARSER_SRC_MESSAGE_VALUE_H
#define WIRELESS_ANDROID_PLAY_ANALYTICS_PROTO_PARSER_SRC_MESSAGE_VALUE_H

#include "proto_value.h"

namespace wireless_android_play_analytics {

class MessageValue : public ProtoValue {

 public:

  explicit MessageValue(absl::string_view field_name, int indent_count) 
   : ProtoValue(field_name, indent_count) {}

  // Gets all the fields of the current Message
  const std::vector<std::unique_ptr<ProtoValue>>& GetFields() const {
    return fields_;
  }

  // Gets all the fields of the current Message with intention to modify it.
  std::vector<ProtoValue*> GetFieldsMutable();

  // Appends a field to the back of fields and fields mutable
  void AddField(std::unique_ptr<ProtoValue> field);

 private:

  std::string PrintToTextProtoHelper();

  std::vector<ProtoValue*> fields_mutable_;

  std::vector<std::unique_ptr<ProtoValue>> fields_;
};

} // namespace wireless_android_play_analytics

#endif // WIRELESS_ANDROID_PLAY_ANALYTICS_PROTO_PARSER_SRC_MESSAGE_VALUE_H