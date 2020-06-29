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

#include "primitive_value.h"

namespace wireless_android_play_analytics {

std::string PrimitiveValue::PrintToTextProtoHelper() {
  // Get indents.
  std::string indents;
  for(int i = 0; i < this->GetIndentCount(); ++i) {
    indents += "  ";
  }

  // Print the comments.
  std::string output;
  output += this->GetCommentAboveField();
  output += indents + this->GetName();

  // Print the value.
  output += ": " + val_as_string_;
  output += " " + this->GetCommentBehindField() + "\n";

  return output;
}

void PrimitiveValue::SetVal(const 
    absl::variant<double, float, int, unsigned int, int64_t, uint64_t, bool, 
    const google::protobuf::EnumValueDescriptor*, std::string>& val) {
  val_ = val;
  if (absl::holds_alternative<double>(val)) {
    val_as_string_ = std::to_string(absl::get<double>(val));
  } else if (absl::holds_alternative<float>(val)) {
    val_as_string_ = std::to_string(absl::get<float>(val));
  } else if (absl::holds_alternative<int>(val)) {
    val_as_string_ = std::to_string(absl::get<int>(val));
  } else if (absl::holds_alternative<unsigned int>(val)) {
    val_as_string_ = std::to_string(absl::get<unsigned int>(val));
  } else if (absl::holds_alternative<int64_t>(val)) {
    val_as_string_ = std::to_string(absl::get<int64_t>(val));
  } else if (absl::holds_alternative<uint64_t>(val)) {
    val_as_string_ = std::to_string(absl::get<uint64_t>(val));
  } else if (absl::holds_alternative<bool>(val)) {
    bool input_val = absl::get<bool>(val);
    if (input_val) {
      val_as_string_ = "true";
    } else {
      val_as_string_ = "false";
    }
  } else if (absl::holds_alternative<const google::protobuf::EnumValueDescriptor*>(
      val)) {
    const google::protobuf::EnumValueDescriptor* input_val = 
        absl::get<const google::protobuf::EnumValueDescriptor*>(val);
    val_as_string_ = input_val->name();
  }
}

} // namespace wireless_android_play_analytics