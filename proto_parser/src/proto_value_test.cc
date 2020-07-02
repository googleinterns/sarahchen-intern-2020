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
    message_val = dynamic_cast<MessageValue*>(message.get());
  }

  // virtual void TearDown() {}

  TestProto test;
  std::unique_ptr<ProtoValue> message;
  MessageValue* message_val;
};

TEST_F(ProtoValueTest, PrintToTextProtoTest) {
  std::string printed_text_proto = message_val->PrintToTextProto();
  ASSERT_EQ(printed_text_proto, well_formatted_text_proto);
}

TEST_F(ProtoValueTest, PrintModifiedTextProtoTest) {
  const std::vector<std::unique_ptr<ProtoValue>>& fields = 
      message_val->GetFields();
  MessageValue* field_nested_message = dynamic_cast<MessageValue*>(
      fields[2].get());

  // Add comments above nested message.
  std::vector<std::string>& field_nested_message_comments_above_field = 
    field_nested_message->GetCommentAboveFieldMutable();
  field_nested_message_comments_above_field.push_back("# new comment");
  field_nested_message_comments_above_field.push_back("#");
  const std::vector<std::unique_ptr<ProtoValue>>& 
      first_field_nested_message_fields = field_nested_message->GetFields();
  
  // Modify value and comments of a nested primitive.
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

TEST_F(ProtoValueTest, OuterProtoTest) {
  const std::vector<std::unique_ptr<ProtoValue>>& fields = 
      message_val->GetFields();
  EXPECT_EQ("", message_val->GetName());
  EXPECT_EQ(0, message_val->GetCommentAboveField().size());
  EXPECT_EQ("", message_val->GetCommentBehindField());
  EXPECT_EQ(0, message_val->GetIndentCount());
  EXPECT_EQ(5, fields.size());
}

TEST_F(ProtoValueTest, FirstFieldTest) {
  const std::vector<std::unique_ptr<ProtoValue>>& fields = 
      message_val->GetFields();
  PrimitiveValue* int32_field = 
      dynamic_cast<PrimitiveValue*>(fields[0].get());
  ASSERT_NE(nullptr, int32_field);
  EXPECT_EQ("int32_field", int32_field->GetName());
  ASSERT_THAT(int32_field->GetCommentAboveField(),
      testing::ElementsAreArray({"# comment 1",}));
  EXPECT_EQ("", int32_field->GetCommentBehindField());
  int int32_field_val = 1;
  absl::variant<double, float, int, unsigned int, int64_t, uint64_t, bool, 
      const google::protobuf::EnumValueDescriptor *, std::string> val = int32_field_val;
  EXPECT_TRUE(absl::holds_alternative<int>(int32_field->GetVal()));
  EXPECT_EQ(val, int32_field->GetVal());
}

TEST_F(ProtoValueTest, SecondFieldTest) {
  const std::vector<std::unique_ptr<ProtoValue>>& fields = 
      message_val->GetFields();
  MessageValue* field_nested_message = 
      dynamic_cast<MessageValue*>(fields[1].get());
  int64_t int64_field_val = 100;
  absl::variant<double, float, int, unsigned int, int64_t, uint64_t, bool, 
      const google::protobuf::EnumValueDescriptor *, std::string> val = 
      int64_field_val;
  ASSERT_NE(nullptr, field_nested_message);
  EXPECT_EQ("field_nested_message", field_nested_message->GetName());
  EXPECT_THAT(field_nested_message->GetCommentAboveField(),
      testing::ElementsAreArray({"# comment 2"}));
  EXPECT_EQ("# comment 3", field_nested_message->GetCommentBehindField());
  EXPECT_EQ(0, field_nested_message->GetIndentCount());
  const std::vector<std::unique_ptr<ProtoValue>>& field_nested_message_fields =
      field_nested_message->GetFields();

  // Nested Primitive values check.
  PrimitiveValue* int64_field = 
      dynamic_cast<PrimitiveValue*>(field_nested_message_fields[0].get());
  EXPECT_EQ(1, field_nested_message_fields.size());
  ASSERT_NE(nullptr, int64_field);
  EXPECT_EQ("int64_field", int64_field->GetName());
  EXPECT_EQ(0, int64_field->GetCommentAboveField().size());
  EXPECT_EQ("", int64_field->GetCommentBehindField());
  EXPECT_EQ(1, int64_field->GetIndentCount());
  EXPECT_TRUE(absl::holds_alternative<int64_t>(int64_field->GetVal()));
  EXPECT_EQ(val, int64_field->GetVal());
}
  
TEST_F(ProtoValueTest, ThirdFieldTest) {
  const std::vector<std::unique_ptr<ProtoValue>>& fields = 
      message_val->GetFields();
  MessageValue* field_nested_message = 
      dynamic_cast<MessageValue*>(fields[2].get());
  int64_t int64_field_val = 100;
  absl::variant<double, float, int, unsigned int, int64_t, uint64_t, bool, 
      const google::protobuf::EnumValueDescriptor *, std::string> val = 
      int64_field_val;
  ASSERT_NE(nullptr, field_nested_message);
  EXPECT_EQ("field_nested_message", field_nested_message->GetName());
  EXPECT_EQ(0, field_nested_message->GetCommentAboveField().size());
  EXPECT_EQ("", field_nested_message->GetCommentBehindField());
  EXPECT_EQ(0, field_nested_message->GetIndentCount());
  const std::vector<std::unique_ptr<ProtoValue>>& field_nested_message_fields =
      field_nested_message->GetFields();

  // Nested Primitive values check.
  PrimitiveValue* int64_field = 
      dynamic_cast<PrimitiveValue*>(field_nested_message_fields[0].get());
  EXPECT_EQ(1, field_nested_message_fields.size());
  ASSERT_NE(nullptr, int64_field);
  EXPECT_EQ("int64_field", int64_field->GetName());
  EXPECT_THAT(int64_field->GetCommentAboveField(),
      testing::ElementsAreArray({"# comment 4", "# comment 5"}));
  EXPECT_EQ("", int64_field->GetCommentBehindField());
  EXPECT_EQ(1, int64_field->GetIndentCount());
  EXPECT_TRUE(absl::holds_alternative<int64_t>(int64_field->GetVal()));
  EXPECT_EQ(val, int64_field->GetVal());
}
  
TEST_F(ProtoValueTest, FourthFieldTest) {
  const std::vector<std::unique_ptr<ProtoValue>>& fields = 
      message_val->GetFields();
  PrimitiveValue* bool_field = 
      dynamic_cast<PrimitiveValue*>(fields[3].get());
  ASSERT_NE(nullptr, bool_field);
  EXPECT_EQ("bool_field", bool_field->GetName());
  EXPECT_EQ(0, bool_field->GetCommentAboveField().size());
  EXPECT_EQ("", bool_field->GetCommentBehindField());
  EXPECT_EQ(0, bool_field->GetIndentCount());
  bool bool_field_val = true;
  absl::variant<double, float, int, unsigned int, int64_t, uint64_t, bool, 
      const google::protobuf::EnumValueDescriptor *, std::string> val = 
      bool_field_val;
  EXPECT_TRUE(absl::holds_alternative<bool>(bool_field->GetVal()));
  EXPECT_EQ(val, bool_field->GetVal());
}

TEST_F(ProtoValueTest, FifthFieldTest) {
  const std::vector<std::unique_ptr<ProtoValue>>& fields = 
      message_val->GetFields();
  PrimitiveValue* bool_field = 
      dynamic_cast<PrimitiveValue*>(fields[4].get());
  ASSERT_NE(nullptr, bool_field);
  EXPECT_EQ("bool_field", bool_field->GetName());
  EXPECT_EQ(0, bool_field->GetCommentAboveField().size());
  EXPECT_EQ("# comment 6", bool_field->GetCommentBehindField());
  EXPECT_EQ(0, bool_field->GetIndentCount());
  bool bool_field_val = false;
  absl::variant<double, float, int, unsigned int, int64_t, uint64_t, bool, 
      const google::protobuf::EnumValueDescriptor *, std::string> val = 
      bool_field_val;
  EXPECT_TRUE(absl::holds_alternative<bool>(bool_field->GetVal()));
  EXPECT_EQ(val, bool_field->GetVal());
}

} // namespace wireless_android_play_analytics

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}