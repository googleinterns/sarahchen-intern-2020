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
  field_nested_message {  # comment 3
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

TEST(ProtoValueTest, CreateTest) {
  ProtoParser parser;
  TestProto test;
  std::shared_ptr<MessageValue> message = 
    std::dynamic_pointer_cast<MessageValue>(ProtoValue::Create(text_proto, 
    &test));
  ASSERT_NE(nullptr, message);
  const std::vector<std::shared_ptr<ProtoValue>>& fields = 
    message->GetFields();
  ASSERT_EQ("", message->GetName());
  ASSERT_EQ("", message->GetCommentAboveField());
  ASSERT_EQ("", message->GetCommentBehindField());
  ASSERT_EQ(5, fields.size());
}

TEST(ProtoParserTest, DelimiteTextProtoTest) {
  ProtoParser parser;
  parser.DelimiteTextProto(text_proto);
  int idx = 0;
  std::istringstream iss(text_proto);
  while (iss.good()) {
    std::string tmp;
    getline(iss,tmp);
    EXPECT_EQ(parser.lines_[idx++], tmp);
  }
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
  ASSERT_EQ(2, parser.GetLocation(&tree, field_descriptor, -1));
  field_descriptor = descriptor->FindFieldByLowercaseName("bool_field");
  ASSERT_EQ(16, parser.GetLocation(&tree, field_descriptor, 0));
  ASSERT_EQ(17, parser.GetLocation(&tree, field_descriptor, 1));
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
  field_descriptor = descriptor->FindFieldByLowercaseName("field_nested_message");
  // Acquire the field descriptor and tree for the nested type
  descriptor = field_descriptor->message_type();
  nested_tree = tree.GetTreeForNested(field_descriptor, 0);
  field_descriptor = descriptor->FindFieldByLowercaseName("int64_field");
  ASSERT_EQ(6, parser.GetLocation(nested_tree, field_descriptor, -1));  
}

TEST(ProtoParserTest, PopulateCommentsTest) {
  std::shared_ptr<ProtoValue> message = std::make_shared<PrimitiveValue>("");
  ProtoParser parser;
  parser.lines_.push_back("#hi");
  parser.lines_.push_back("#bye");
  parser.lines_.push_back("test: 15 #more comment");
  parser.PopulateComments(0, 2, message);
  ASSERT_EQ("#hi\n#bye\n", message->GetCommentAboveField());
  ASSERT_EQ("#more comment", message->GetCommentBehindField());
}

TEST(ProtoParserTest, PopulateCommentsClosingBracket) {
  std::shared_ptr<ProtoValue> message = std::make_shared<MessageValue>("");
  ProtoParser parser;
  parser.lines_.push_back("}");
  parser.lines_.push_back("  }");
  parser.lines_.push_back("\t}");
  parser.lines_.push_back("#comment");
  parser.lines_.push_back("test: 1");
  parser.PopulateComments(0, 4, message);
  ASSERT_EQ("#comment\n", message->GetCommentAboveField());
}

TEST(ProtoParserTest, PopulateCommentsStringVariable) {
  std::shared_ptr<ProtoValue> message = std::make_shared<MessageValue>("");
  ProtoParser parser;
  parser.lines_.push_back("");
  parser.lines_.push_back("test: 4 #\"#\"#\"#\"\"");
  parser.PopulateComments(0, 1, message);
  ASSERT_EQ("#\"#\"#\"#\"\"", message->GetCommentBehindField());
  parser.lines_[1] = "test: \"string#not a comment\"";
  parser.PopulateComments(0, 1, message);
  ASSERT_EQ("", message->GetCommentBehindField());
  parser.lines_[1] = "test: \"string\" #some comment";
  parser.PopulateComments(0, 1, message);
  ASSERT_EQ("#some comment", message->GetCommentBehindField());
}

TEST(ProtoParserTest, CreatePrimitiveTest) {
  ProtoParser parser;
  TestProto test;
  google::protobuf::TextFormat::Parser google_parser;
  google_parser.ParseFromString(text_proto, &test);
  const google::protobuf::Descriptor* descriptor = test.GetDescriptor();
  const google::protobuf::FieldDescriptor* field_descriptor = nullptr;
  std::shared_ptr<PrimitiveValue> primitive = nullptr;
  field_descriptor = descriptor->FindFieldByLowercaseName("int32_field");
  parser.DelimiteTextProto(text_proto);
  ProtoParser::FieldInformation field(2, -1, field_descriptor);
  std::shared_ptr<ProtoValue>message = parser.CreatePrimitive(
    &test, field, 0);
  
  primitive = std::dynamic_pointer_cast<PrimitiveValue>(message);
  ASSERT_NE(primitive, nullptr);
  absl::variant<double, float, int, unsigned int, int64_t, uint64_t, bool, 
    google::protobuf::EnumValueDescriptor *, std::string> val = 1;
  ASSERT_TRUE(absl::holds_alternative<int>(primitive->GetVal()));
  ASSERT_EQ(val, primitive->GetVal());
  ASSERT_EQ("  # comment 1\n", primitive->GetCommentAboveField());
  ASSERT_EQ("", primitive->GetCommentBehindField());
  ASSERT_EQ("int32_field", primitive->GetName());
}

TEST(ProtoParserTest, CreatePrimitiveNestedTest) {
  ProtoParser parser;
  TestProto test;
  google::protobuf::TextFormat::Parser google_parser;
  google_parser.ParseFromString(text_proto, &test);
  const google::protobuf::Descriptor* descriptor = test.GetDescriptor();
  const google::protobuf::FieldDescriptor* field_descriptor = nullptr;
  std::shared_ptr<PrimitiveValue> primitive = nullptr;
  field_descriptor = descriptor->FindFieldByLowercaseName("bool_field");
  parser.DelimiteTextProto(text_proto);
  ProtoParser::FieldInformation field(17, 1, field_descriptor);
  primitive = std::dynamic_pointer_cast<PrimitiveValue>(parser.CreatePrimitive(
    &test, field, 17));
  ASSERT_NE(primitive, nullptr);
  absl::variant<double, float, int, unsigned int, int64_t, uint64_t, bool, 
    google::protobuf::EnumValueDescriptor *, std::string> val = false;
  ASSERT_TRUE(absl::holds_alternative<bool>(primitive->GetVal()));
  ASSERT_EQ(val, primitive->GetVal());
  ASSERT_EQ("", primitive->GetCommentAboveField());
  ASSERT_EQ("# comment 6", primitive->GetCommentBehindField());
  ASSERT_EQ("bool_field", primitive->GetName());
}

TEST(ProtoParserTest, PopulateFields) {
  ProtoParser parser;
  TestProto test;
  google::protobuf::TextFormat::ParseInfoTree tree;
  google::protobuf::TextFormat::Parser google_parser;
  google_parser.WriteLocationsTo(&tree);
  google_parser.ParseFromString(text_proto, &test);
  std::shared_ptr<ProtoValue> message = std::make_shared<MessageValue>("");
  int last_field_loc = 0;
  parser.DelimiteTextProto(text_proto);
  parser.PopulateFields(last_field_loc, &tree, &test, message);
  std::shared_ptr<MessageValue> message_value = 
    std::dynamic_pointer_cast<MessageValue>(message);
  const std::vector<std::shared_ptr<ProtoValue>>& fields = 
    message_value->GetFields();
  // Ensure last_field_loc is updated accordingly.
  ASSERT_EQ(18, last_field_loc);
  // Ensure the types are correct.
  ASSERT_NE(nullptr, std::dynamic_pointer_cast<PrimitiveValue>(fields[0]));
  ASSERT_NE(nullptr, std::dynamic_pointer_cast<MessageValue>(fields[1]));
  ASSERT_NE(nullptr, std::dynamic_pointer_cast<MessageValue>(fields[2]));
  ASSERT_NE(nullptr, std::dynamic_pointer_cast<PrimitiveValue>(fields[3]));
  ASSERT_NE(nullptr, std::dynamic_pointer_cast<PrimitiveValue>(fields[4]));
}

TEST(ProtoParserTest, CreateMessageTest) {
  ProtoParser parser;
  TestProto test;
  google::protobuf::TextFormat::Parser google_parser;
  google::protobuf::TextFormat::ParseInfoTree tree;
  const google::protobuf::Descriptor* descriptor = test.GetDescriptor();
  const google::protobuf::Reflection* reflection = test.GetReflection();
  const google::protobuf::FieldDescriptor* field_descriptor = nullptr;
  const google::protobuf::TextFormat::ParseInfoTree* nested_tree = nullptr;
  field_descriptor = descriptor->FindFieldByLowercaseName("field_nested_message");
  google_parser.WriteLocationsTo(&tree);
  google_parser.ParseFromString(text_proto, &test);
  parser.DelimiteTextProto(text_proto);
  const google::protobuf::Message& nested_message = 
    reflection->GetRepeatedMessage(test, field_descriptor, 0);
  nested_tree = tree.GetTreeForNested(field_descriptor, 0);
  int last_field_loc = 3;
  std::shared_ptr<MessageValue> message = 
    std::dynamic_pointer_cast<MessageValue>(
    parser.CreateMessage(&nested_message, nested_tree, last_field_loc, 5, 
    field_descriptor->name()));
  ASSERT_NE(nullptr, message);
  ASSERT_EQ(7, last_field_loc);
  ASSERT_EQ("  # comment 2\n", message->GetCommentAboveField());
  ASSERT_EQ("# comment 3", message->GetCommentBehindField());
  ASSERT_EQ("field_nested_message", message->GetName());
  const std::vector<std::shared_ptr<ProtoValue>>& fields = message->GetFields();
  ASSERT_EQ(1, fields.size());
  std::shared_ptr<PrimitiveValue> primitive = 
    std::dynamic_pointer_cast<PrimitiveValue>(fields[0]);
  ASSERT_NE(nullptr, primitive);
  ASSERT_EQ("int64_field", primitive->GetName());
  int64_t primitive_val = 100;
  absl::variant<double, float, int, unsigned int, int64_t, uint64_t, bool, 
    google::protobuf::EnumValueDescriptor *, std::string> val = primitive_val;
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