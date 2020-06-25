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

#ifndef WIRELESS_ANDROID_PLAY_ANALYTICS_PROTO_PARSER_SRC_PROTO_PARSER_H
#define WIRELESS_ANDROID_PLAY_ANALYTICS_PROTO_PARSER_SRC_PROTO_PARSER_H

#include "proto_value.h"
#include "primitive_value.h"
#include "message_value.h"

namespace wireless_android_play_analytics {

class ProtoParser {

  public:

  void DelimiteTextProto(const std::string& text_proto);
  int GetLocation(google::protobuf::TextFormat::ParseInfoTree* tree, 
    const google::protobuf::FieldDescriptor* field_descriptor, int index);
  void PopulateFields(int& last_field_loc, 
    google::protobuf::TextFormat::ParseInfoTree* tree,
    const google::protobuf::Message* message,
    std::shared_ptr<ProtoValue>& message_val);
  void PopulateComments(int last_field_loc, int field_loc, 
    std::shared_ptr<ProtoValue>& message);
  std::shared_ptr<ProtoValue> CreateMessage(
    const google::protobuf::Message* message, google::protobuf::TextFormat::ParseInfoTree* tree,
    int& last_field_loc, int field_loc, const std::string& name);
  std::shared_ptr<ProtoValue> CreatePrimitive(
    const google::protobuf::Message* message, 
    const google::protobuf::FieldDescriptor* field_descriptor, int index, 
    int last_field_loc, int field_loc);

  private:
  
  struct FieldInformation {

    FieldInformation(int line, int index, 
      const google::protobuf::FieldDescriptor* field_descriptor)
      : line_(line), index_(index), field_descriptor_(field_descriptor) {}
    bool operator< (const FieldInformation& rhs) {
      return this.line_ < rhs.line_;
    }
    int line_;
    int index_;
    const google::protobuf::FieldDescriptor* field_descriptor_;
  }
  std::vector<std::string> lines_;
  absl::flat_hash_map<const google::protobuf::FieldDescriptor*, int> 
    repeated_field_indices_;
};

} // namespace wireless_android_play_analytics

#endif // WIRELESS_ANDROID_PLAY_ANALYTICS_PROTO_PARSER_SRC_PROTO_PARSER_H