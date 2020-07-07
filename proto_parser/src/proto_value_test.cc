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
  EXPECT_EQ(printed_text_proto, well_formatted_text_proto);
}

TEST_F(ProtoValueTest, PrintModifiedTextProtoTest) {
  const std::vector<std::unique_ptr<ProtoValue>>& fields = 
      message_val->GetFields();
  MessageValue* field_nested_message = dynamic_cast<MessageValue*>(
      fields[2].get());

  // Add comments above nested message.
  field_nested_message->SetCommentAboveField("# new comment\n#");

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
  EXPECT_EQ(printed_text_proto, modified_text_proto);
}

} // namespace wireless_android_play_analytics

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}