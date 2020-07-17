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

TEST(PrimitiveValueTest, ProtoValueGetterTests) {
  PrimitiveValue primitive(/*field_name=*/"test", /*indent_count=*/0, 
      /*line_number=*/0);
  EXPECT_EQ(primitive.GetName(), "test");
  EXPECT_EQ(primitive.GetCommentAboveField().size(), 0);
  EXPECT_EQ(primitive.GetCommentBehindField(), "");
  EXPECT_EQ(primitive.GetLineNumber(), 0);
  EXPECT_EQ(primitive.GetIndentCount(), 0);
}

TEST(PrimitiveValueTest, DoubleValTest) {
  PrimitiveValue primitive(/*field_name=*/"test", /*indent_count=*/0, 
      /*line_number=*/0);
  double val_in = -3.1415;
  absl::variant<double, float, int, unsigned int, int64_t, uint64_t, bool, 
      const google::protobuf::EnumValueDescriptor*, std::string> val = val_in;
  primitive.SetVal(val);
  EXPECT_EQ(primitive.GetVal(), val);
  EXPECT_EQ(primitive.PrintToTextProto(), "test: -3.141500\n");
}

TEST(PrimitiveValueTest, FloatValTest) {
  PrimitiveValue primitive(/*field_name=*/"test", /*indent_count=*/0, 
      /*line_number=*/0);
  float val_in = 3.14;
  absl::variant<double, float, int, unsigned int, int64_t, uint64_t, bool, 
      const google::protobuf::EnumValueDescriptor*, std::string> val = val_in;
  primitive.SetVal(val);
  EXPECT_EQ(primitive.GetVal(), val);
  EXPECT_EQ(primitive.PrintToTextProto(), "test: 3.140000\n");
}

TEST(PrimitiveValueTest, IntValTest) {
  PrimitiveValue primitive(/*field_name=*/"test", /*indent_count=*/0, 
      /*line_number=*/0);
  int val_in = -2147483648;
  absl::variant<double, float, int, unsigned int, int64_t, uint64_t, bool, 
      const google::protobuf::EnumValueDescriptor*, std::string> val = val_in;
  primitive.SetVal(val);
  EXPECT_EQ(primitive.GetVal(), val);
  EXPECT_EQ(primitive.PrintToTextProto(), "test: -2147483648\n");
}

TEST(PrimitiveValueTest, UnsignedIntTest) {
  PrimitiveValue primitive(/*field_name=*/"test", /*indent_count=*/0, 
      /*line_number=*/0);
  unsigned int val_in = 4294967295;
  absl::variant<double, float, int, unsigned int, int64_t, uint64_t, bool, 
      const google::protobuf::EnumValueDescriptor*, std::string> val = val_in;
  primitive.SetVal(val);
  EXPECT_EQ(primitive.GetVal(), val);
  EXPECT_EQ(primitive.PrintToTextProto(), "test: 4294967295\n");
}

TEST(PrimitiveValueTest, Int64Test) {
  PrimitiveValue primitive(/*field_name=*/"test", /*indent_count=*/0, 
      /*line_number=*/0);
  int64_t val_in = -9223372036854775807;
  absl::variant<double, float, int, unsigned int, int64_t, uint64_t, bool, 
      const google::protobuf::EnumValueDescriptor*, std::string> val = val_in;
  primitive.SetVal(val);
  EXPECT_EQ(primitive.GetVal(), val);
  EXPECT_EQ(primitive.PrintToTextProto(), "test: -9223372036854775807\n");
}

TEST(PrimitiveValueTest, UnsignedInt64Test) {
  PrimitiveValue primitive(/*field_name=*/"test", /*indent_count=*/0, 
      /*line_number=*/0);
  uint64_t val_in = 18446744073709551615;
  absl::variant<double, float, int, unsigned int, int64_t, uint64_t, bool, 
      const google::protobuf::EnumValueDescriptor*, std::string> val = val_in;
  primitive.SetVal(val);
  EXPECT_EQ(primitive.GetVal(), val);
  EXPECT_EQ(primitive.PrintToTextProto(), "test: 18446744073709551615\n");
}

TEST(PrimitiveValueTest, BoolTest) {
  PrimitiveValue primitive(/*field_name=*/"test", /*indent_count=*/0, 
      /*line_number=*/0);
  bool val_in = true;
  absl::variant<double, float, int, unsigned int, int64_t, uint64_t, bool, 
      const google::protobuf::EnumValueDescriptor*, std::string> val = val_in;
  primitive.SetVal(val);
  EXPECT_EQ(primitive.GetVal(), val);
  EXPECT_EQ(primitive.PrintToTextProto(), "test: true\n");
}

TEST(PrimitiveValueTest, EnumTest) {
  PrimitiveValue primitive(/*field_name=*/"test", /*indent_count=*/0, 
      /*line_number=*/0);
  EventPredicate event_predicate_prototype;
  const google::protobuf::EnumDescriptor* enum_descriptor = 
      event_predicate_prototype.UiElementType_descriptor();
  const google::protobuf::EnumValueDescriptor* val_in = 
      enum_descriptor->FindValueByName("STORE_UI_ELEMENT");
  absl::variant<double, float, int, unsigned int, int64_t, uint64_t, bool, 
      const google::protobuf::EnumValueDescriptor*, std::string> val = val_in;
  primitive.SetVal(val);
  EXPECT_EQ(primitive.GetVal(), val);
  EXPECT_EQ(primitive.PrintToTextProto(), "test: STORE_UI_ELEMENT\n");
}

TEST(PrimitiveValueTest, StringTest) {
  PrimitiveValue primitive(/*field_name=*/"test", /*indent_count=*/0, 
      /*line_number=*/0);
  std::string val_in = "# Hello World!";
  absl::variant<double, float, int, unsigned int, int64_t, uint64_t, bool, 
      const google::protobuf::EnumValueDescriptor*, std::string> val = val_in;
  primitive.SetVal(val);
  EXPECT_EQ(primitive.GetVal(), val);
  EXPECT_EQ(primitive.PrintToTextProto(), "test: \"# Hello World!\"\n");
}

} // namespace wireless_android_play_analytics

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}