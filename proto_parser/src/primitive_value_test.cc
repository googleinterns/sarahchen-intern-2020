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

#include "primitive_value.h"
#include "gmock/gmock.h"
#include "proto/event_predicate.pb.h"

namespace wireless_android_play_analytics{
 
class PrimitiveValueTest : public ::testing::Test {
 protected:
  PrimitiveValueTest()
    : primitive_(PrimitiveValue("test", 0, 0)) { }

  virtual void SetUp() { }

  PrimitiveValue primitive_;
};

TEST_F(PrimitiveValueTest, ProtoValueGetterTests) {
  EXPECT_EQ(primitive_.GetName(), "test");
  EXPECT_EQ(primitive_.GetCommentAboveField().size(), 0);
  EXPECT_EQ(primitive_.GetCommentBehindField(), "");
  EXPECT_EQ(primitive_.GetLineNumber(), 0);
  EXPECT_EQ(primitive_.GetIndentCount(), 0);
}

TEST_F(PrimitiveValueTest, DoubleValTest) {
  double val_in = -3.1415;
  absl::variant<double, float, int, unsigned int, int64_t, uint64_t, bool, 
      const google::protobuf::EnumValueDescriptor*, std::string> val = val_in;
  primitive_.SetVal(val);
  EXPECT_EQ(primitive_.GetVal(), val);
  EXPECT_EQ(primitive_.PrintToTextProto(), "test: -3.141500\n");
}

TEST_F(PrimitiveValueTest, FloatValTest) {
  float val_in = 3.14;
  absl::variant<double, float, int, unsigned int, int64_t, uint64_t, bool, 
      const google::protobuf::EnumValueDescriptor*, std::string> val = val_in;
  primitive_.SetVal(val);
  EXPECT_EQ(primitive_.GetVal(), val);
  EXPECT_EQ(primitive_.PrintToTextProto(), "test: 3.140000\n");
}

TEST_F(PrimitiveValueTest, IntValTest) {
  int val_in = -2147483648;
  absl::variant<double, float, int, unsigned int, int64_t, uint64_t, bool, 
      const google::protobuf::EnumValueDescriptor*, std::string> val = val_in;
  primitive_.SetVal(val);
  EXPECT_EQ(primitive_.GetVal(), val);
  EXPECT_EQ(primitive_.PrintToTextProto(), "test: -2147483648\n");
}

TEST_F(PrimitiveValueTest, UnsignedIntTest) {
  unsigned int val_in = 4294967295;
  absl::variant<double, float, int, unsigned int, int64_t, uint64_t, bool, 
      const google::protobuf::EnumValueDescriptor*, std::string> val = val_in;
  primitive_.SetVal(val);
  EXPECT_EQ(primitive_.GetVal(), val);
  EXPECT_EQ(primitive_.PrintToTextProto(), "test: 4294967295\n");
}

TEST_F(PrimitiveValueTest, Int64Test) {
  int64_t val_in = -9223372036854775807;
  absl::variant<double, float, int, unsigned int, int64_t, uint64_t, bool, 
      const google::protobuf::EnumValueDescriptor*, std::string> val = val_in;
  primitive_.SetVal(val);
  EXPECT_EQ(primitive_.GetVal(), val);
  EXPECT_EQ(primitive_.PrintToTextProto(), "test: -9223372036854775807\n");
}

TEST_F(PrimitiveValueTest, UnsignedInt64Test) {
  uint64_t val_in = 18446744073709551615;
  absl::variant<double, float, int, unsigned int, int64_t, uint64_t, bool, 
      const google::protobuf::EnumValueDescriptor*, std::string> val = val_in;
  primitive_.SetVal(val);
  EXPECT_EQ(primitive_.GetVal(), val);
  EXPECT_EQ(primitive_.PrintToTextProto(), "test: 18446744073709551615\n");
}

TEST_F(PrimitiveValueTest, BoolTest) {
  bool val_in = true;
  absl::variant<double, float, int, unsigned int, int64_t, uint64_t, bool, 
      const google::protobuf::EnumValueDescriptor*, std::string> val = val_in;
  primitive_.SetVal(val);
  EXPECT_EQ(primitive_.GetVal(), val);
  EXPECT_EQ(primitive_.PrintToTextProto(), "test: true\n");
}

TEST_F(PrimitiveValueTest, EnumTest) {
  EventPredicate event_predicate_prototype;
  const google::protobuf::EnumDescriptor* enum_descriptor = 
      event_predicate_prototype.UiElementType_descriptor();
  const google::protobuf::EnumValueDescriptor* val_in = 
      enum_descriptor->FindValueByName("STORE_UI_ELEMENT");
  absl::variant<double, float, int, unsigned int, int64_t, uint64_t, bool, 
      const google::protobuf::EnumValueDescriptor*, std::string> val = val_in;
  primitive_.SetVal(val);
  EXPECT_EQ(primitive_.GetVal(), val);
  EXPECT_EQ(primitive_.PrintToTextProto(), "test: STORE_UI_ELEMENT\n");
}

TEST_F(PrimitiveValueTest, StringTest) {
  std::string val_in = "# Hello World!";
  absl::variant<double, float, int, unsigned int, int64_t, uint64_t, bool, 
      const google::protobuf::EnumValueDescriptor*, std::string> val = val_in;
  primitive_.SetVal(val);
  EXPECT_EQ(primitive_.GetVal(), val);
  EXPECT_EQ(primitive_.PrintToTextProto(), "test: \"# Hello World!\"\n");
}

} // namespace wireless_android_play_analytics

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}