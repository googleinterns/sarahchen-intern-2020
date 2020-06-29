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
#include "message_value.h"

namespace wireless_android_play_analytics {

std::string MessageValue::PrintToTextProtoHelper() {
  // Get indents.
  std::string indents;
  for(int i = 0; i < this->GetIndentCount(); ++i) {
    indents += "  ";
  }
  // Print the comments.
  std::string output;
  if (!this->GetName().empty()) {
    output += this->GetCommentAboveField();
    output += indents + this->GetName();
    output += " { " + this->GetCommentBehindField() + "\n";
  }

  for (const std::unique_ptr<ProtoValue>& field : fields_) {
    output += field->PrintToTextProto();
  }

  if (!this->GetName().empty()) {
    output += indents + "}\n";
  }

  return output;
}

} // namespace wireless_android_play_analytics