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
    absl::StrAppend(&indents, "  ");
  }
  // Only print name and brackets if this isn't a proto-text holder.
  std::string output;
  if (!this->GetName().empty()) {
    const std::vector<std::string>& comment_above_field = 
        this->GetCommentAboveField();
    for(const std::string& comment : comment_above_field) {
      absl::StrAppend(&output, indents, comment, "\n");
    }
    
    absl::StrAppend(&output, indents, this->GetName(), " {");
    if (!this->GetCommentBehindField().empty()) {
     absl::StrAppend(&output, " ", this->GetCommentBehindField());
    }
    absl::StrAppend(&output, "\n");
  }

  for (const std::unique_ptr<ProtoValue>& field : fields_) {
    absl::StrAppend(&output, field->PrintToTextProto());
  }

  if (!this->GetName().empty()) {
    absl::StrAppend(&output, indents, "}\n");
  }

  return output;
}

void MessageValue::AddField(std::unique_ptr<ProtoValue> field) {
  fields_.push_back(std::move(field));
}

std::vector<ProtoValue*> MessageValue::GetFieldsMutable() {
  std::vector<ProtoValue*> result;

  for (const std::unique_ptr<ProtoValue>& proto_value : fields_) {
    result.push_back(proto_value.get());
  }

  return result;
}

} // namespace wireless_android_play_analytics