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

#include "message_value.h"
#include "gmock/gmock.h"

namespace wireless_android_play_analytics{
 
class MessageValueTest : public ::testing::Test {
 protected:
  MessageValueTest()
    : message_(MessageValue("test", 0, 0)) { }

  virtual void SetUp() { 
    field_0_ = absl::make_unique<MessageValue>("temp", 0, 0);
    field_1_ = absl::make_unique<MessageValue>("temp1", 0, 1);
    field_2_ = absl::make_unique<MessageValue>("temp2", 0, 2);
    ProtoValue* field_0_ptr_ = field_0_.get();
    ProtoValue* field_1_ptr_ = field_1_.get();
    ProtoValue* field_2_ptr_ = field_2_.get();
  }

  MessageValue message_;
  std::unique_ptr<ProtoValue> field_0_;
  std::unique_ptr<ProtoValue> field_1_;
  std::unique_ptr<ProtoValue> field_2_;
  ProtoValue* field_0_ptr_;
  ProtoValue* field_1_ptr_;
  ProtoValue* field_2_ptr_;
};

TEST_F(MessageValueTest, ProtoValueGetterTests) {
  EXPECT_EQ(message_.GetName(), "test");
  EXPECT_EQ(message_.GetCommentAboveField().size(), 0);
  EXPECT_EQ(message_.GetCommentBehindField(), "");
  EXPECT_EQ(message_.GetLineNumber(), 0);
  EXPECT_EQ(message_.GetIndentCount(), 0);
}

TEST_F(MessageValueTest, AddFieldsTest) {
  message_.AddField(std::move(field_2_));
  message_.AddField(std::move(field_0_)); 
  message_.AddField(std::move(field_1_)); 
  EXPECT_THAT(message_.GetFieldsMutable(), 
      testing::ElementsAre(testing::Pointee(*field_2_), testing::Pointee(*field_0_), 
      testing::Pointee(*field_1_)));
}

Matcher<const ProtoValue&> Equals(const ProtoValue& expected) {
  return AllOf
}

TEST_F(MessageValueTest, SortFieldsTest) {
  message_.SortFields();
}

} // namespace wireless_android_play_analytics

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}