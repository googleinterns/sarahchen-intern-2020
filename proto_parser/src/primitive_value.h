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

#ifndef WIRELESS_ANDROID_PLAY_ANALYTICS_PROTO_PARSER_SRC_PRIMITIVE_VALUE_H
#define WIRELESS_ANDROID_PLAY_ANALYTICS_PROTO_PARSER_SRC_PRIMITIVE_VALUE_H

#include "proto_value.h"

namespace wireless_android_play_analytics {

class PrimitiveValue : public ProtoValue {

 public:

  explicit PrimitiveValue(absl::string_view field_name) 
   : ProtoValue(field_name) {}
   
  // Gets the value of the current Primitve.
  const absl::variant<double, float, int64_t, uint64_t, bool, 
    google::protobuf::EnumValueDescriptor *>& GetVal() const {
    return val_;
  }

  void SetVal(const 
    absl::variant<double, float, int64_t, uint64_t, bool, 
      google::protobuf::EnumValueDescriptor *>& val) {
    val_ = val;
  }

  // Gets the value of the current primitive with intention to modify it.
  absl::variant<double, float, int64_t, uint64_t, bool>& GetValMutable();
 private:

  absl::variant<double, float, int64_t, uint64_t, bool, 
    google::protobuf::EnumValueDescriptor *> val_;
};

} // namespace wireless_android_play_analytics

#endif // WIRELESS_ANDROID_PLAY_ANALYTICS_PROTO_PARSER_SRC_PRIMITIVE_VALUE_H