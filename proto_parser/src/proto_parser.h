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

  // Populates the message with all the fields of the message.
  void PopulateFields(int& last_field_loc, 
    const google::protobuf::TextFormat::ParseInfoTree* tree,
    const google::protobuf::Message* message,
    std::shared_ptr<ProtoValue>& message_val);

  // Splits the text proto using endline character into an array of string.
  void DelimiteTextProto(const std::string& text_proto);

  private:

  FRIEND_TEST(ProtoParserTest, DelimiteTextProtoTest);
  FRIEND_TEST(ProtoParserTest, GetLocationTest);
  FRIEND_TEST(ProtoParserTest, GetNestedLocationTest);
  FRIEND_TEST(ProtoParserTest, PopulateCommentsTest);
  FRIEND_TEST(ProtoParserTest, PopulateCommentsClosingBracket);
  FRIEND_TEST(ProtoParserTest, PopulateCommentsStringVariable);
  FRIEND_TEST(ProtoParserTest, CreatePrimitiveTest);
  FRIEND_TEST(ProtoParserTest, CreatePrimitiveNestedTest);
  FRIEND_TEST(ProtoParserTest, PopulateFields);
  FRIEND_TEST(ProtoParserTest, CreateMessageTest);

  // Stores all the information for a field necessary for create functions.
  // TODO FieldInformation -> FieldInfo
  struct FieldInformation {

    FieldInformation(int line, int index, 
      const google::protobuf::FieldDescriptor* field_descriptor)
      : line_(line), index_(index), field_descriptor_(field_descriptor) {}

    // Overrides the comparison operator of FieldInformation.
    bool operator< (const FieldInformation& rhs) {
      return this->line_ < rhs.line_;
    }

    int line_;
    // Index of the repeated field, -1 if not repeated.
    int index_;
    const google::protobuf::FieldDescriptor* field_descriptor_;
  };

  // Acquires and populates the comments of a specific field.
  void PopulateComments(int last_field_loc, int field_loc, 
    std::shared_ptr<ProtoValue>& message);

  // Creates a MessageValue field.
  std::shared_ptr<ProtoValue> CreateMessage(
    const google::protobuf::Message* message, const google::protobuf::TextFormat::ParseInfoTree* tree,
    int& last_field_loc, int field_loc, const std::string& name);

  // Creates a PrimitiveValue field.
  std::shared_ptr<ProtoValue> CreatePrimitive(
    const google::protobuf::Message* message, const FieldInformation& field, 
    int last_field_loc);

  // Acquires the location of a field.
  int GetLocation(const google::protobuf::TextFormat::ParseInfoTree* tree, 
    const google::protobuf::FieldDescriptor* field_descriptor, int index);

  std::vector<std::string> lines_;
};

} // namespace wireless_android_play_analytics

#endif // WIRELESS_ANDROID_PLAY_ANALYTICS_PROTO_PARSER_SRC_PROTO_PARSER_H