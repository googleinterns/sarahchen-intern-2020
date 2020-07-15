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
#include "proto/event_predicate.pb.h"

namespace wireless_android_play_analytics{
 
class PrimitiveValueTest : public ::testing::Test {
 protected:
  PrimitiveValueTest()
    : message_(MessageValue("test", 0, 0)) { }

  virtual void SetUp() { }

  MessageValue message_;
};

TEST_F(PrimitiveValueTest, ProtoValueGetterTests) {
  EXPECT_EQ(message_.GetName(), "test");
  EXPECT_EQ(message_.GetCommentAboveField().size(), 0);
  EXPECT_EQ(message_.GetCommentBehindField(), "");
  EXPECT_EQ(message_.GetLineNumber(), 0);
  EXPECT_EQ(message_.GetIndentCount(), 0);
}

} // namespace wireless_android_play_analytics

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}