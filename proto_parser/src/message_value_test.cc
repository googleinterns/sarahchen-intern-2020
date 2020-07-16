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
    field_0_copy = absl::make_unique<MessageValue>("temp", 0, 0);
    field_1_copy = absl::make_unique<MessageValue>("temp1", 0, 1);
    field_2_copy = absl::make_unique<MessageValue>("temp2", 0, 2);
  }

  MessageValue message_;
  std::unique_ptr<ProtoValue> field_0_;
  std::unique_ptr<ProtoValue> field_1_;
  std::unique_ptr<ProtoValue> field_2_;
  std::unique_ptr<ProtoValue> field_0_copy;
  std::unique_ptr<ProtoValue> field_1_copy;
  std::unique_ptr<ProtoValue> field_2_copy;
};

TEST_F(MessageValueTest, ProtoValueGetterTests) {
  EXPECT_EQ(message_.GetName(), "test");
  EXPECT_EQ(message_.GetCommentAboveField().size(), 0);
  EXPECT_EQ(message_.GetCommentBehindField(), "");
  EXPECT_EQ(message_.GetLineNumber(), 0);
  EXPECT_EQ(message_.GetIndentCount(), 0);
}

MATCHER_P(ProtoValuePtrIsEqual, element, "") {
  return arg.GetName() == element->GetName() && 
      arg.GetIndentCount() == element->GetIndentCount() &&
      arg.GetLineNumber() == element->GetLineNumber();
}

TEST_F(MessageValueTest, AddFieldsTest) {
  message_.AddField(std::move(field_2_));
  message_.AddField(std::move(field_0_)); 
  message_.AddField(std::move(field_1_)); 
  EXPECT_THAT(message_.GetFieldsMutable(), testing::ElementsAre(
    testing::Pointee(ProtoValuePtrIsEqual(field_2_copy.get())), 
    testing::Pointee(ProtoValuePtrIsEqual(field_0_copy.get())), 
    testing::Pointee(ProtoValuePtrIsEqual(field_1_copy.get()))));
  EXPECT_THAT(message_.GetFields(), testing::ElementsAre(
    testing::Pointee(ProtoValuePtrIsEqual(field_2_copy.get())), 
    testing::Pointee(ProtoValuePtrIsEqual(field_0_copy.get())), 
    testing::Pointee(ProtoValuePtrIsEqual(field_1_copy.get()))));
}

TEST_F(MessageValueTest, SortFieldsTest) {
  message_.AddField(std::move(field_2_));
  message_.AddField(std::move(field_0_)); 
  message_.AddField(std::move(field_1_)); 
  message_.SortFields();
  EXPECT_THAT(message_.GetFieldsMutable(), testing::ElementsAre(
    testing::Pointee(ProtoValuePtrIsEqual(field_0_copy.get())), 
    testing::Pointee(ProtoValuePtrIsEqual(field_1_copy.get())), 
    testing::Pointee(ProtoValuePtrIsEqual(field_2_copy.get()))));
  EXPECT_THAT(message_.GetFields(), testing::ElementsAre(
    testing::Pointee(ProtoValuePtrIsEqual(field_0_copy.get())), 
    testing::Pointee(ProtoValuePtrIsEqual(field_1_copy.get())), 
    testing::Pointee(ProtoValuePtrIsEqual(field_2_copy.get()))));
}

TEST_F(MessageValueTest, DeleteFieldTest) {
  message_.AddField(std::move(field_2_));
  message_.AddField(std::move(field_0_)); 
  message_.AddField(std::move(field_1_)); 
  EXPECT_TRUE(message_.DeleteField(0));
  EXPECT_THAT(message_.GetFieldsMutable(), testing::ElementsAre( 
    testing::Pointee(ProtoValuePtrIsEqual(field_0_copy.get())), 
    testing::Pointee(ProtoValuePtrIsEqual(field_1_copy.get()))));
  EXPECT_THAT(message_.GetFields(), testing::ElementsAre( 
    testing::Pointee(ProtoValuePtrIsEqual(field_0_copy.get())), 
    testing::Pointee(ProtoValuePtrIsEqual(field_1_copy.get()))));
  EXPECT_FALSE(message_.DeleteField(2));
  EXPECT_THAT(message_.GetFieldsMutable(), testing::ElementsAre( 
    testing::Pointee(ProtoValuePtrIsEqual(field_0_copy.get())), 
    testing::Pointee(ProtoValuePtrIsEqual(field_1_copy.get()))));
  EXPECT_THAT(message_.GetFields(), testing::ElementsAre( 
    testing::Pointee(ProtoValuePtrIsEqual(field_0_copy.get())), 
    testing::Pointee(ProtoValuePtrIsEqual(field_1_copy.get()))));
  EXPECT_TRUE(message_.DeleteField(0));
  EXPECT_TRUE(message_.DeleteField(0));
  EXPECT_EQ(message_.GetFields().size(), 0);
  EXPECT_EQ(message_.GetFieldsMutable().size(), 0);
}

} // namespace wireless_android_play_analytics

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}