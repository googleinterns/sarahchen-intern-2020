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
  return PrintToTextProtoHelper();
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