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

#ifndef WIRELESS_ANDROID_PLAY_ANALYTICS_PROTO_PARSER_SRC_PROTO_VALUE_H
#define WIRELESS_ANDROID_PLAY_ANALYTICS_PROTO_PARSER_SRC_PROTO_VALUE_H

#include <gtest/gtest.h>
#include <assert.h>
#include <google/protobuf/text_format.h>
#include <vector>
#include <memory>
#include <sstream>

#include "absl/types/variant.h"
#include "absl/strings/string_view.h"
#include "absl/container/flat_hash_map.h"

namespace wireless_android_play_analytics {

class ProtoValue {

 public:
  // The only place where name can be set publicly since once set it should
  // not be changed.
  explicit ProtoValue(absl::string_view field_name, int indent_count) 
   : field_name_(field_name), indent_count_(indent_count) {}

  virtual ~ProtoValue() = default;

  static std::unique_ptr<ProtoValue> Create(absl::string_view text_proto, 
    google::protobuf::Message& message);

  // Gets the name of current variable.
  const std::string& GetName() const {
    return field_name_;
  }

  // Gets the comment above current field.
  const std::string& GetCommentAboveField() const {
    return comment_above_field_;
  }

  // Gets the comment behind current field.
  const std::string& GetCommentBehindField() const {
    return comment_behind_field_;
  }

  // Sets the comment behind current field.
  void SetCommentBehindField(absl::string_view val) {
    comment_behind_field_ = std::string(val);
  }

  // Sets the comment behind current field.
  void SetCommentAboveField(absl::string_view val) {
    comment_above_field_ = std::string(val);
  }

  int GetIndentCount() {
    return indent_count_;
  }

  // Outputs content of this ProtoValue and all its nested types as a string 
  // including any comments.
  std::string PrintToTextProto();
  
 private:

  std::string comment_above_field_;
  std::string comment_behind_field_;
  std::string field_name_;
  int indent_count_;

  // Helper function to print text proto in order to abstract indent_count
  // from the user.
  virtual std::string PrintToTextProtoHelper() = 0;
};

} // namespace wireless_android_play_analytics

#endif // WIRELESS_ANDROID_PLAY_ANALYTICS_PROTO_PARSER_SRC_PROTO_VALUE_H