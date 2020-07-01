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
#include "absl/strings/str_split.h"

namespace wireless_android_play_analytics {

class ProtoParser {

 public:

  ProtoParser() {}

  ProtoParser(absl::string_view text_proto) {
    lines_ = absl::StrSplit(text_proto, '\n');
  }

  struct PrevFieldLine {

    PrevFieldLine(int line_in) 
      : line(line_in) {}

    int line;
  };

  // Populates the message with all the fields of the message.
  void PopulateFields(PrevFieldLine& last_field_loc, 
    const google::protobuf::TextFormat::ParseInfoTree& tree,
    const google::protobuf::Message& message,
    ProtoValue* proto_value, int indent_count);

 private:
  
  // Stores all the information for a field necessary for create functions.
  struct FieldInfo {

    FieldInfo(int line_in, int index_in, 
        const google::protobuf::FieldDescriptor* field_descriptor_in)
      : line(line_in), index(index_in), field_descriptor(field_descriptor_in) {}

    // Overrides the comparison operator of FieldInfo.
    bool operator< (const FieldInfo& rhs) {
      return this->line < rhs.line;
    }

    // Location of the field in the text proto.
    int line;
    // Index of the repeated field, -1 if not repeated.
    int index;
    // Field Descriptor of the current field.
    const google::protobuf::FieldDescriptor* field_descriptor;
  };

  // Acquires and populates the comments of a specific field.
  void PopulateComments(int last_field_loc, int field_loc, 
    ProtoValue* message);

  // Creates a MessageValue field.
  std::unique_ptr<ProtoValue> CreateMessage(
      const google::protobuf::Message& message, 
      const google::protobuf::TextFormat::ParseInfoTree& tree, int indent_count,
    PrevFieldLine& last_field_loc, int field_loc, absl::string_view name);

  // Creates a PrimitiveValue field.
  std::unique_ptr<ProtoValue> CreatePrimitive(
      const google::protobuf::Message& message, const FieldInfo& field, 
      int last_field_loc, int indent_count);

  // Acquires the location of a field.
  int GetLocation(const google::protobuf::TextFormat::ParseInfoTree& tree, 
      const google::protobuf::FieldDescriptor* field_descriptor, int index);

  std::vector<std::string> lines_;
};

} // namespace wireless_android_play_analytics

#endif // WIRELESS_ANDROID_PLAY_ANALYTICS_PROTO_PARSER_SRC_PROTO_PARSER_H