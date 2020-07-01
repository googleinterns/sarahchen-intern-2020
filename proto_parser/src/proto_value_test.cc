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

class ProtoValueTest : public ::testing::Test {
 protected:
  
  virtual void SetUp() {
    message_ = ProtoValue::Create(text_proto, test_);
  }

  // virtual void TearDown() {}

  TestProto test_;
  std::unique_ptr<ProtoValue> message_;
};

TEST_F(ProtoValueTest, CreateTest) {
  MessageValue* message_val = dynamic_cast<MessageValue*>(message_.get());
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

} // namespace wireless_android_play_analytics

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}