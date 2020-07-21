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

TEST(MessageValueTest, ProtoValueGetterTests) {
  MessageValue message(/*field_name=*/"test", /*indent_count=*/0, 
      /*line_number=*/0);
  EXPECT_EQ(message.GetName(), "test");
  EXPECT_EQ(message.GetCommentAboveField().size(), 0);
  EXPECT_EQ(message.GetCommentBehindField(), "");
  EXPECT_EQ(message.GetLineNumber(), 0);
  EXPECT_EQ(message.GetIndentCount(), 0);
}

MATCHER_P(ProtoValuePtrIsEqual, element, 
    "Matches whether two ProtoValues are equal") {
  return arg.GetName() == element->GetName() && 
      arg.GetIndentCount() == element->GetIndentCount() &&
      arg.GetLineNumber() == element->GetLineNumber();
}

TEST(MessageValueTest, AddFieldsTest) {
  MessageValue message(/*field_name=*/"test", /*indent_count=*/0, 
      /*line_number=*/0);
  std::unique_ptr<ProtoValue> field_0 = 
      absl::make_unique<MessageValue>("temp", 0, 0);
  std::unique_ptr<ProtoValue> field_1 = 
      absl::make_unique<MessageValue>("temp1", 0, 1);
  std::unique_ptr<ProtoValue> field_2 = 
      absl::make_unique<MessageValue>("temp2", 0, 2);
  std::unique_ptr<ProtoValue> field_0_expect = 
      absl::make_unique<MessageValue>("temp", 0, 0);
  std::unique_ptr<ProtoValue> field_1_expect = 
      absl::make_unique<MessageValue>("temp1", 0, 1);
  std::unique_ptr<ProtoValue> field_2_expect = 
      absl::make_unique<MessageValue>("temp2", 0, 2);
  message.AddField(std::move(field_2));
  message.AddField(std::move(field_0)); 
  message.AddField(std::move(field_1)); 
  EXPECT_THAT(message.GetFieldsMutable(), testing::ElementsAre(
    testing::Pointee(ProtoValuePtrIsEqual(field_2_expect.get())), 
    testing::Pointee(ProtoValuePtrIsEqual(field_0_expect.get())), 
    testing::Pointee(ProtoValuePtrIsEqual(field_1_expect.get()))));
  EXPECT_THAT(message.GetFields(), testing::ElementsAre(
    testing::Pointee(ProtoValuePtrIsEqual(field_2_expect.get())), 
    testing::Pointee(ProtoValuePtrIsEqual(field_0_expect.get())), 
    testing::Pointee(ProtoValuePtrIsEqual(field_1_expect.get()))));
}

TEST(MessageValueTest, SortFieldsTest) {
  MessageValue message(/*field_name=*/"test", /*indent_count=*/0, 
      /*line_number=*/0);
  std::unique_ptr<ProtoValue> field_0 = 
      absl::make_unique<MessageValue>("temp", 0, 0);
  std::unique_ptr<ProtoValue> field_1 = 
      absl::make_unique<MessageValue>("temp1", 0, 1);
  std::unique_ptr<ProtoValue> field_2 = 
      absl::make_unique<MessageValue>("temp2", 0, 2);
  std::unique_ptr<ProtoValue> field_0_expect = 
      absl::make_unique<MessageValue>("temp", 0, 0);
  std::unique_ptr<ProtoValue> field_1_expect = 
      absl::make_unique<MessageValue>("temp1", 0, 1);
  std::unique_ptr<ProtoValue> field_2_expect = 
      absl::make_unique<MessageValue>("temp2", 0, 2);
  message.AddField(std::move(field_2));
  message.AddField(std::move(field_0)); 
  message.AddField(std::move(field_1)); 
  message.SortFields();
  EXPECT_THAT(message.GetFieldsMutable(), testing::ElementsAre(
    testing::Pointee(ProtoValuePtrIsEqual(field_0_expect.get())), 
    testing::Pointee(ProtoValuePtrIsEqual(field_1_expect.get())), 
    testing::Pointee(ProtoValuePtrIsEqual(field_2_expect.get()))));
  EXPECT_THAT(message.GetFields(), testing::ElementsAre(
    testing::Pointee(ProtoValuePtrIsEqual(field_0_expect.get())), 
    testing::Pointee(ProtoValuePtrIsEqual(field_1_expect.get())), 
    testing::Pointee(ProtoValuePtrIsEqual(field_2_expect.get()))));
}

TEST(MessageValueTest, DeleteFieldTest) {
  MessageValue message(/*field_name=*/"test", /*indent_count=*/0, 
      /*line_number=*/0);
  std::unique_ptr<ProtoValue> field_0 = 
      absl::make_unique<MessageValue>("temp", 0, 0);
  std::unique_ptr<ProtoValue> field_1 = 
      absl::make_unique<MessageValue>("temp1", 0, 1);
  std::unique_ptr<ProtoValue> field_2 = 
      absl::make_unique<MessageValue>("temp2", 0, 2);
  std::unique_ptr<ProtoValue> field_0_expect = 
      absl::make_unique<MessageValue>("temp", 0, 0);
  std::unique_ptr<ProtoValue> field_1_expect = 
      absl::make_unique<MessageValue>("temp1", 0, 1);
  std::unique_ptr<ProtoValue> field_2_expect = 
      absl::make_unique<MessageValue>("temp2", 0, 2);
  message.AddField(std::move(field_2));
  message.AddField(std::move(field_0)); 
  message.AddField(std::move(field_1)); 
  EXPECT_TRUE(message.DeleteField(0));
  EXPECT_THAT(message.GetFieldsMutable(), testing::ElementsAre( 
    testing::Pointee(ProtoValuePtrIsEqual(field_0_expect.get())), 
    testing::Pointee(ProtoValuePtrIsEqual(field_1_expect.get()))));
  EXPECT_THAT(message.GetFields(), testing::ElementsAre( 
    testing::Pointee(ProtoValuePtrIsEqual(field_0_expect.get())), 
    testing::Pointee(ProtoValuePtrIsEqual(field_1_expect.get()))));
  EXPECT_FALSE(message.DeleteField(2));
  EXPECT_THAT(message.GetFieldsMutable(), testing::ElementsAre( 
    testing::Pointee(ProtoValuePtrIsEqual(field_0_expect.get())), 
    testing::Pointee(ProtoValuePtrIsEqual(field_1_expect.get()))));
  EXPECT_THAT(message.GetFields(), testing::ElementsAre( 
    testing::Pointee(ProtoValuePtrIsEqual(field_0_expect.get())), 
    testing::Pointee(ProtoValuePtrIsEqual(field_1_expect.get()))));
  EXPECT_TRUE(message.DeleteField(0));
  EXPECT_TRUE(message.DeleteField(0));
  EXPECT_EQ(message.GetFields().size(), 0);
  EXPECT_EQ(message.GetFieldsMutable().size(), 0);
}

} // namespace wireless_android_play_analytics

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}