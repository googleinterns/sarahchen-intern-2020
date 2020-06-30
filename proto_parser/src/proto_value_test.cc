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

#include "proto_parser.h"
#include "proto_value.h"
#include "gmock/gmock.h"
#include "proto/example.pb.h"

#include <google/protobuf/util/message_differencer.h>

namespace wireless_android_play_analytics{
 
const std::string text_proto = R"pb(
# comment 1
int32_field: 1

# comment 2
field_nested_message { # comment 3
  int64_field: 100
}

field_nested_message {
  # comment 4
  # comment 5
  int64_field: 100
}


bool_field: true
bool_field: false # comment 6
)pb";

const std::string well_formatted_text_proto = R"pb(# comment 1
int32_field: 1 
# comment 2
field_nested_message { # comment 3
  int64_field: 100 
}
field_nested_message { 
  # comment 4
  # comment 5
  int64_field: 100 
}
bool_field: true 
bool_field: false # comment 6
)pb";

const std::string modified_text_proto = R"pb(# comment 1
int32_field: 1 
# comment 2
field_nested_message { # comment 3
  int64_field: 100 
}
# new comment
#
field_nested_message { 
  # comment 4
  # comment 5
  int64_field: 1234567890 # another new comment
}
bool_field: true 
bool_field: false # comment 6
)pb";


class ProtoValueTest : public ::testing::Test {
 protected:
  
  virtual void SetUp() {
    message = ProtoValue::Create(text_proto, test);
  }

  // virtual void TearDown() {}

  TestProto test;
  std::unique_ptr<ProtoValue> message;
};

TEST_F(ProtoValueTest, PrintToTextProtoTest) {
  MessageValue* message_val = dynamic_cast<MessageValue*>(message.get());
  std::string printed_text_proto = message_val->PrintToTextProto();
  ASSERT_EQ(printed_text_proto, well_formatted_text_proto);
}

TEST_F(ProtoValueTest, PrintModifiedTextProtoTest) {
  MessageValue* message_val = dynamic_cast<MessageValue*>(message.get());
  const std::vector<std::unique_ptr<ProtoValue>>& fields = 
      message_val->GetFields();
  MessageValue* field_nested_message = dynamic_cast<MessageValue*>(
      fields[2].get());
  field_nested_message->SetCommentAboveField("# new comment\n#\n");
  const std::vector<std::unique_ptr<ProtoValue>>& 
      first_field_nested_message_fields = field_nested_message->GetFields();
  PrimitiveValue* primitive = dynamic_cast<PrimitiveValue*>(
      first_field_nested_message_fields[0].get());
  primitive->SetCommentBehindField("# another new comment");
  int64_t int64_field_val = 1234567890;
  absl::variant<double, float, int, unsigned int, int64_t, uint64_t, bool, 
      const google::protobuf::EnumValueDescriptor *, std::string> val =
      int64_field_val;
  primitive->SetVal(val);
  std::string printed_text_proto = message_val->PrintToTextProto();
  ASSERT_EQ(printed_text_proto, modified_text_proto);
}

TEST_F(ProtoValueTest, CreateTest) {
  MessageValue* message_val = dynamic_cast<MessageValue*>(message.get());
  ASSERT_NE(nullptr, message_val);
  const std::vector<std::unique_ptr<ProtoValue>>& fields = 
      message_val->GetFields();
  ASSERT_EQ("", message_val->GetName());
  ASSERT_EQ("", message_val->GetCommentAboveField());
  ASSERT_EQ("", message_val->GetCommentBehindField());
  ASSERT_EQ(0, message_val->GetIndentCount());
  ASSERT_EQ(5, fields.size());
  // Check the first field is correct.
  PrimitiveValue* int32_field = dynamic_cast<PrimitiveValue*>(fields[0].get());
  ASSERT_NE(nullptr, int32_field);
  ASSERT_EQ("int32_field", int32_field->GetName());
  ASSERT_EQ("# comment 1\n", int32_field->GetCommentAboveField());
  ASSERT_EQ("", int32_field->GetCommentBehindField());
  int int32_field_val = 1;
  absl::variant<double, float, int, unsigned int, int64_t, uint64_t, bool, 
      const google::protobuf::EnumValueDescriptor *, std::string> val = int32_field_val;
  ASSERT_TRUE(absl::holds_alternative<int>(int32_field->GetVal()));
  ASSERT_EQ(val, int32_field->GetVal());
  // Check the second field is correct.
  MessageValue* field_nested_message = nullptr;
  PrimitiveValue* int64_field = nullptr;
  int64_t int64_field_val = 100;
  val = int64_field_val;
  field_nested_message = dynamic_cast<MessageValue*>(fields[1].get());
  ASSERT_NE(nullptr, field_nested_message);
  ASSERT_EQ("field_nested_message", field_nested_message->GetName());
  ASSERT_EQ("# comment 2\n", field_nested_message->GetCommentAboveField());
  ASSERT_EQ("# comment 3", field_nested_message->GetCommentBehindField());
  ASSERT_EQ(0, field_nested_message->GetIndentCount());
  const std::vector<std::unique_ptr<ProtoValue>>& 
      first_field_nested_message_fields = field_nested_message->GetFields();
  ASSERT_EQ(1, first_field_nested_message_fields.size());
  int64_field = dynamic_cast<PrimitiveValue*>(
      first_field_nested_message_fields[0].get());
  ASSERT_NE(nullptr, int64_field);
  ASSERT_EQ("int64_field", int64_field->GetName());
  ASSERT_EQ("", int64_field->GetCommentAboveField());
  ASSERT_EQ("", int64_field->GetCommentBehindField());
  ASSERT_EQ(1, int64_field->GetIndentCount());
  ASSERT_TRUE(absl::holds_alternative<int64_t>(int64_field->GetVal()));
  ASSERT_EQ(val, int64_field->GetVal());
  // Check the third field is correct.
  field_nested_message = dynamic_cast<MessageValue*>(fields[2].get());
  ASSERT_NE(nullptr, field_nested_message);
  ASSERT_EQ("field_nested_message", field_nested_message->GetName());
  ASSERT_EQ("", field_nested_message->GetCommentAboveField());
  ASSERT_EQ("", field_nested_message->GetCommentBehindField());
  ASSERT_EQ(0, field_nested_message->GetIndentCount());
  const std::vector<std::unique_ptr<ProtoValue>>& 
      second_field_nested_message_fields = field_nested_message->GetFields();
  ASSERT_EQ(1, second_field_nested_message_fields.size());
  int64_field = dynamic_cast<PrimitiveValue*>(
      second_field_nested_message_fields[0].get());
  ASSERT_NE(nullptr, int64_field);
  ASSERT_EQ("int64_field", int64_field->GetName());
  ASSERT_EQ("  # comment 4\n  # comment 5\n", 
      int64_field->GetCommentAboveField());
  ASSERT_EQ("", int64_field->GetCommentBehindField());
  ASSERT_EQ(1, int64_field->GetIndentCount());
  ASSERT_TRUE(absl::holds_alternative<int64_t>(int64_field->GetVal()));
  ASSERT_EQ(val, int64_field->GetVal());
  // Check the fourth field is correct.
  PrimitiveValue* bool_field = nullptr;
  bool_field = dynamic_cast<PrimitiveValue*>(fields[3].get());
  ASSERT_NE(nullptr, bool_field);
  ASSERT_EQ("bool_field", bool_field->GetName());
  ASSERT_EQ("", bool_field->GetCommentAboveField());
  ASSERT_EQ("", bool_field->GetCommentBehindField());
  ASSERT_EQ(0, bool_field->GetIndentCount());
  bool bool_field_val = true;
  val = bool_field_val;
  ASSERT_TRUE(absl::holds_alternative<bool>(bool_field->GetVal()));
  ASSERT_EQ(val, bool_field->GetVal());
  // Check the fifth field is correct.
  bool_field = dynamic_cast<PrimitiveValue*>(fields[4].get());
  ASSERT_NE(nullptr, bool_field);
  ASSERT_EQ("bool_field", bool_field->GetName());
  ASSERT_EQ("", bool_field->GetCommentAboveField());
  ASSERT_EQ("# comment 6", bool_field->GetCommentBehindField());
  ASSERT_EQ(0, bool_field->GetIndentCount());
  bool_field_val = false;
  val = bool_field_val;
  ASSERT_TRUE(absl::holds_alternative<bool>(bool_field->GetVal()));
  ASSERT_EQ(val, bool_field->GetVal());
}

TEST(ProtoParserTest, GetLocationTest) {
  ProtoParser parser;
  TestProto test;
  google::protobuf::TextFormat::Parser google_parser;
  google::protobuf::TextFormat::ParseInfoTree tree;
  google_parser.WriteLocationsTo(&tree);
  google_parser.ParseFromString(text_proto, &test);
  const google::protobuf::Descriptor* descriptor = test.GetDescriptor();
  const google::protobuf::FieldDescriptor* field_descriptor = nullptr;
  field_descriptor = descriptor->FindFieldByLowercaseName("int32_field");
  ASSERT_EQ(2, parser.GetLocation(tree, field_descriptor, -1));
  field_descriptor = descriptor->FindFieldByLowercaseName("bool_field");
  ASSERT_EQ(16, parser.GetLocation(tree, field_descriptor, 0));
  ASSERT_EQ(17, parser.GetLocation(tree, field_descriptor, 1));
}

TEST(ProtoParserTest, GetNestedLocationTest) {
  ProtoParser parser;
  TestProto test;
  google::protobuf::TextFormat::Parser google_parser;
  google::protobuf::TextFormat::ParseInfoTree tree;
  google_parser.WriteLocationsTo(&tree);
  google_parser.ParseFromString(text_proto, &test);
  const google::protobuf::Descriptor* descriptor = test.GetDescriptor();
  const google::protobuf::FieldDescriptor* field_descriptor = nullptr;
  const google::protobuf::TextFormat::ParseInfoTree* nested_tree = nullptr;
  field_descriptor = descriptor->FindFieldByLowercaseName(
      "field_nested_message");
  // Acquire the field descriptor and tree for the nested type
  descriptor = field_descriptor->message_type();
  nested_tree = tree.GetTreeForNested(field_descriptor, 0);
  field_descriptor = descriptor->FindFieldByLowercaseName("int64_field");
  ASSERT_EQ(6, parser.GetLocation(*nested_tree, field_descriptor, -1));  
}

TEST(ProtoParserTest, PopulateCommentsTest) {
  std::unique_ptr<ProtoValue> message = absl::make_unique<PrimitiveValue>("", 0);
  ProtoParser parser;
  parser.lines_.push_back("#hi");
  parser.lines_.push_back("#bye");
  parser.lines_.push_back("test: 15 #more comment");
  parser.PopulateComments(0, 2, message.get());
  ASSERT_EQ("#hi\n#bye\n", message->GetCommentAboveField());
  ASSERT_EQ("#more comment", message->GetCommentBehindField());
}

TEST(ProtoParserTest, PopulateCommentsClosingBracket) {
  std::unique_ptr<ProtoValue> message = absl::make_unique<MessageValue>("", 0);
  ProtoParser parser;
  parser.lines_.push_back("}");
  parser.lines_.push_back("  }");
  parser.lines_.push_back("\t}");
  parser.lines_.push_back("#comment");
  parser.lines_.push_back("test: 1");
  parser.PopulateComments(0, 4, message.get());
  ASSERT_EQ("#comment\n", message->GetCommentAboveField());
}

TEST(ProtoParserTest, PopulateCommentsStringVariable) {
  std::unique_ptr<ProtoValue> message = absl::make_unique<MessageValue>("", 0);
  ProtoParser parser;
  parser.lines_.push_back("");
  parser.lines_.push_back("test: 4 #\"#\"#\"#\"\"");
  parser.PopulateComments(0, 1, message.get());
  ASSERT_EQ("#\"#\"#\"#\"\"", message->GetCommentBehindField());
  parser.lines_[1] = "test: \"string#not a comment\"";
  parser.PopulateComments(0, 1, message.get());
  ASSERT_EQ("", message->GetCommentBehindField());
  parser.lines_[1] = "test: \"string\" #some comment";
  parser.PopulateComments(0, 1, message.get());
  ASSERT_EQ("#some comment", message->GetCommentBehindField());
}

TEST(ProtoParserTest, CreatePrimitiveTest) {
  ProtoParser parser(text_proto);
  TestProto test;
  google::protobuf::TextFormat::Parser google_parser;
  google_parser.ParseFromString(text_proto, &test);
  const google::protobuf::Descriptor* descriptor = test.GetDescriptor();
  const google::protobuf::FieldDescriptor* field_descriptor = nullptr;
  std::unique_ptr<ProtoValue> message = nullptr;
  field_descriptor = descriptor->FindFieldByLowercaseName("int32_field");
  ProtoParser::FieldInfo field(2, -1, field_descriptor);
  message = parser.CreatePrimitive(test, field, 0, 0);
  PrimitiveValue* primitive = dynamic_cast<PrimitiveValue*>(message.get());
  absl::variant<double, float, int, unsigned int, int64_t, uint64_t, bool, 
      const google::protobuf::EnumValueDescriptor *, std::string> val = 1;
  ASSERT_NE(nullptr, primitive);
  ASSERT_TRUE(absl::holds_alternative<int>(primitive->GetVal()));
  ASSERT_EQ(val, primitive->GetVal());
  ASSERT_EQ("# comment 1\n", primitive->GetCommentAboveField());
  ASSERT_EQ("", primitive->GetCommentBehindField());
  ASSERT_EQ("int32_field", primitive->GetName());
}

TEST(ProtoParserTest, CreatePrimitiveNestedTest) {
  ProtoParser parser(text_proto);
  TestProto test;
  google::protobuf::TextFormat::Parser google_parser;
  google_parser.ParseFromString(text_proto, &test);
  const google::protobuf::Descriptor* descriptor = test.GetDescriptor();
  const google::protobuf::FieldDescriptor* field_descriptor = nullptr;
  std::unique_ptr<ProtoValue> message = nullptr;
  field_descriptor = descriptor->FindFieldByLowercaseName("bool_field");
  ProtoParser::FieldInfo field(17, 1, field_descriptor);
  message = parser.CreatePrimitive(test, field, 17, 0);
  PrimitiveValue* primitive = dynamic_cast<PrimitiveValue*>(message.get());
  absl::variant<double, float, int, unsigned int, int64_t, uint64_t, bool, 
      const google::protobuf::EnumValueDescriptor *, std::string> val = false;
  ASSERT_NE(primitive, nullptr);
  ASSERT_TRUE(absl::holds_alternative<bool>(primitive->GetVal()));
  ASSERT_EQ(val, primitive->GetVal());
  ASSERT_EQ("", primitive->GetCommentAboveField());
  ASSERT_EQ("# comment 6", primitive->GetCommentBehindField());
  ASSERT_EQ("bool_field", primitive->GetName());
}

TEST(ProtoParserTest, CreateMessageTest) {
  ProtoParser parser(text_proto);
  TestProto test;
  google::protobuf::TextFormat::Parser google_parser;
  google::protobuf::TextFormat::ParseInfoTree tree;
  const google::protobuf::Descriptor* descriptor = test.GetDescriptor();
  const google::protobuf::Reflection* reflection = test.GetReflection();
  const google::protobuf::FieldDescriptor* field_descriptor = nullptr;
  const google::protobuf::TextFormat::ParseInfoTree* nested_tree = nullptr;
  field_descriptor = descriptor->FindFieldByLowercaseName(
      "field_nested_message");
  google_parser.WriteLocationsTo(&tree);
  google_parser.ParseFromString(text_proto, &test);
  const google::protobuf::Message& nested_message = 
    reflection->GetRepeatedMessage(test, field_descriptor, 0);
  nested_tree = tree.GetTreeForNested(field_descriptor, 0);
  int last_field_loc = 3;
  std::unique_ptr<ProtoValue> message_val = 
      parser.CreateMessage(nested_message, *nested_tree, 0, last_field_loc, 5, 
      field_descriptor->name());
  MessageValue* message = dynamic_cast<MessageValue*>(message_val.get());
  ASSERT_NE(nullptr, message);
  ASSERT_EQ(7, last_field_loc);
  ASSERT_EQ("# comment 2\n", message->GetCommentAboveField());
  ASSERT_EQ("# comment 3", message->GetCommentBehindField());
  ASSERT_EQ("field_nested_message", message->GetName());
  const std::vector<std::unique_ptr<ProtoValue>>& fields = message->GetFields();
  ASSERT_EQ(1, fields.size());
  PrimitiveValue* primitive = dynamic_cast<PrimitiveValue*>(fields[0].get());
  ASSERT_NE(nullptr, primitive);
  ASSERT_EQ("int64_field", primitive->GetName());
  int64_t primitive_val = 100;
  absl::variant<double, float, int, unsigned int, int64_t, uint64_t, bool, 
      const google::protobuf::EnumValueDescriptor *, std::string> val = primitive_val;
  ASSERT_TRUE(absl::holds_alternative<int64_t>(primitive->GetVal()));
  ASSERT_EQ(val, primitive->GetVal());
  ASSERT_EQ("", primitive->GetCommentAboveField());
  ASSERT_EQ("", primitive->GetCommentBehindField());
}

} // namespace wireless_android_play_analytics

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}