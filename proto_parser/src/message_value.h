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

  MessageValue()
    : ProtoValue("", 0, -1) {}

  explicit MessageValue(absl::string_view field_name, int indent_count, 
      int field_line) 
   : ProtoValue(field_name, indent_count, field_line) {}

  // Gets all the fields of the current Message
  const std::vector<std::unique_ptr<ProtoValue>>& GetFields() const {
    return fields_;
  }

  // Gets all the fields of the current Message with intention to modify it.
  std::vector<ProtoValue*> GetFieldsMutable();

  // Appends a field to the back of fields and fields mutable.
  void AddField(std::unique_ptr<ProtoValue> field);

  // Deletes the field at a specified index.
  bool DeleteField(int idx) {
    if (idx >= fields_.size()) {
      return false;
    }
    fields_.erase(fields_.begin() + idx);
    return true;
  }

  // Sorts the fields_ vector.
  void SortFields() {
    std::sort(fields_.begin(), fields_.end(), [](
        const std::unique_ptr<ProtoValue>& lhs,
        const std::unique_ptr<ProtoValue>& rhs){
          return lhs->GetLineNumber() < rhs->GetLineNumber();
        });
  }

 private:

  std::string PrintToTextProtoHelper();

  std::vector<std::unique_ptr<ProtoValue>> fields_;
};

} // namespace wireless_android_play_analytics

#endif // WIRELESS_ANDROID_PLAY_ANALYTICS_PROTO_PARSER_SRC_MESSAGE_VALUE_H