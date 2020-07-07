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

class ProtoValueTest : public ::testing::Test {
 protected:
  
  virtual void SetUp() {
    message_ = ProtoValue::Create(text_proto, test_);
    message_val_ = dynamic_cast<MessageValue*>(message_.get());
  }

  TestProto test_;
  std::unique_ptr<ProtoValue> message_;
  MessageValue* message_val_;
};

TEST_F(ProtoValueTest, OuterProtoTest) {
  const std::vector<std::unique_ptr<ProtoValue>>& fields = 
      message_val_->GetFields();
  EXPECT_EQ(message_val_->GetName(), "");
  EXPECT_EQ(message_val_->GetCommentAboveField().size(), 0);
  EXPECT_EQ(message_val_->GetCommentBehindField(), "");
  EXPECT_EQ(message_val_->GetIndentCount(), 0);
  EXPECT_EQ(fields.size(), 5);
}

TEST_F(ProtoValueTest, FirstFieldTest) {
  // First field is int32_field a primitive with only comment above.
  const std::vector<std::unique_ptr<ProtoValue>>& fields = 
      message_val_->GetFields();
  PrimitiveValue* int32_field = 
      dynamic_cast<PrimitiveValue*>(fields[0].get());
  ASSERT_NE(int32_field, nullptr);
  EXPECT_EQ(int32_field->GetName(), "int32_field");
  ASSERT_THAT(int32_field->GetCommentAboveField(),
      testing::ElementsAreArray({"# comment 1",}));
  EXPECT_EQ(int32_field->GetCommentBehindField(), "");

  // Inititialize the value to make sure it is held by an integer.
  int int32_field_val = 1;
  absl::variant<double, float, int, unsigned int, int64_t, uint64_t, bool, 
      const google::protobuf::EnumValueDescriptor *, std::string> val = 
      int32_field_val;
  EXPECT_TRUE(absl::holds_alternative<int>(int32_field->GetVal()));
  EXPECT_EQ(int32_field->GetVal(), val);
}

TEST_F(ProtoValueTest, SecondFieldTest) {
  // Second field is field_nested_message, a nested message with comment above
  // and behind.
  const std::vector<std::unique_ptr<ProtoValue>>& fields = 
      message_val_->GetFields();
  MessageValue* field_nested_message = 
      dynamic_cast<MessageValue*>(fields[1].get());
  ASSERT_NE(field_nested_message, nullptr);
  EXPECT_EQ(field_nested_message->GetName(), "field_nested_message");
  EXPECT_THAT(field_nested_message->GetCommentAboveField(),
      testing::ElementsAreArray({"# comment 2"}));
  EXPECT_EQ(field_nested_message->GetCommentBehindField(), "# comment 3");
  EXPECT_EQ(field_nested_message->GetIndentCount(), 0);
  const std::vector<std::unique_ptr<ProtoValue>>& field_nested_message_fields =
      field_nested_message->GetFields();

  // int64_field is the nested primitive field of field_nested message with no 
  // comments.
  PrimitiveValue* int64_field = 
      dynamic_cast<PrimitiveValue*>(field_nested_message_fields[0].get());
  // Iniitialize the value with int64_t to ensure it is held by int64_t.
  int64_t int64_field_val = 100;
  absl::variant<double, float, int, unsigned int, int64_t, uint64_t, bool, 
      const google::protobuf::EnumValueDescriptor *, std::string> val = 
      int64_field_val;
  EXPECT_EQ(field_nested_message_fields.size(), 1);
  ASSERT_NE(int64_field, nullptr);
  EXPECT_EQ(int64_field->GetName(), "int64_field");
  EXPECT_EQ(int64_field->GetCommentAboveField().size(), 0);
  EXPECT_EQ(int64_field->GetCommentBehindField(), "");
  // Since this field is nested it should have indentation of 1.
  EXPECT_EQ(int64_field->GetIndentCount(), 1);
  EXPECT_TRUE(absl::holds_alternative<int64_t>(int64_field->GetVal()));
  EXPECT_EQ(int64_field->GetVal(), val);
}
  
TEST_F(ProtoValueTest, ThirdFieldTest) {
  // Third field is also a nested message but with no comments.
  const std::vector<std::unique_ptr<ProtoValue>>& fields = 
      message_val_->GetFields();
  MessageValue* field_nested_message = 
      dynamic_cast<MessageValue*>(fields[2].get());
  ASSERT_NE(field_nested_message, nullptr);
  EXPECT_EQ(field_nested_message->GetName(), "field_nested_message");
  EXPECT_EQ(field_nested_message->GetCommentAboveField().size(), 0);
  EXPECT_EQ(field_nested_message->GetCommentBehindField(), "");
  EXPECT_EQ(field_nested_message->GetIndentCount(), 0);
  const std::vector<std::unique_ptr<ProtoValue>>& field_nested_message_fields =
      field_nested_message->GetFields();

  // This int64_field is a nested primitive with multiple comments above.
  PrimitiveValue* int64_field = 
      dynamic_cast<PrimitiveValue*>(field_nested_message_fields[0].get());
  int64_t int64_field_val = 100;
  absl::variant<double, float, int, unsigned int, int64_t, uint64_t, bool, 
      const google::protobuf::EnumValueDescriptor *, std::string> val = 
      int64_field_val;
  EXPECT_EQ(1, field_nested_message_fields.size());
  ASSERT_NE(int64_field, nullptr);
  EXPECT_EQ(int64_field->GetName(), "int64_field");
  EXPECT_THAT(int64_field->GetCommentAboveField(),
      testing::ElementsAreArray({"# comment 4", "# comment 5"}));
  EXPECT_EQ(int64_field->GetCommentBehindField(), "");
  EXPECT_EQ(int64_field->GetIndentCount(), 1);
  EXPECT_TRUE(absl::holds_alternative<int64_t>(int64_field->GetVal()));
  EXPECT_EQ(int64_field->GetVal(), val);
}
  
TEST_F(ProtoValueTest, FourthFieldTest) {
  // Fourth field is a primitive that has no comments.
  const std::vector<std::unique_ptr<ProtoValue>>& fields = 
      message_val_->GetFields();
  PrimitiveValue* bool_field = 
      dynamic_cast<PrimitiveValue*>(fields[3].get());
  ASSERT_NE(nullptr, bool_field);
  EXPECT_EQ(bool_field->GetName(), "bool_field");
  EXPECT_EQ(bool_field->GetCommentAboveField().size(), 0);
  EXPECT_EQ(bool_field->GetCommentBehindField(), "");
  EXPECT_EQ(bool_field->GetIndentCount(), 0);
  bool bool_field_val = true;
  absl::variant<double, float, int, unsigned int, int64_t, uint64_t, bool, 
      const google::protobuf::EnumValueDescriptor *, std::string> val = 
      bool_field_val;
  EXPECT_TRUE(absl::holds_alternative<bool>(bool_field->GetVal()));
  EXPECT_EQ(bool_field->GetVal(), val);
}

TEST_F(ProtoValueTest, FifthFieldTest) {
  // Fifth field is a primitive field with only comment behind the field.
  const std::vector<std::unique_ptr<ProtoValue>>& fields = 
      message_val_->GetFields();
  PrimitiveValue* bool_field = 
      dynamic_cast<PrimitiveValue*>(fields[4].get());
  ASSERT_NE(nullptr, bool_field);
  EXPECT_EQ(bool_field->GetName(), "bool_field");
  EXPECT_EQ(bool_field->GetCommentAboveField().size(), 0);
  EXPECT_EQ(bool_field->GetCommentBehindField(), "# comment 6");
  EXPECT_EQ(bool_field->GetIndentCount(), 0);
  bool bool_field_val = false;
  absl::variant<double, float, int, unsigned int, int64_t, uint64_t, bool, 
      const google::protobuf::EnumValueDescriptor *, std::string> val = 
      bool_field_val;
  EXPECT_TRUE(absl::holds_alternative<bool>(bool_field->GetVal()));
  EXPECT_EQ(bool_field->GetVal(), val);
}

} // namespace wireless_android_play_analytics

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}