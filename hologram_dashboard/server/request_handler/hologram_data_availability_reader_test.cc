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

#include <gtest/gtest.h>
#include <google/protobuf/util/message_differencer.h>

#include "hologram_data_availability_reader.h"
#include "gmock/gmock.h"

namespace wireless_android_play_analytics {

const std::string expected_message_string = 
    R"json({"SPAM":[["5/19/2020",true],["5/18/2020",true]]})json";
const std::string test_root = "server/request_handler/sample_proto/Test/";

MATCHER_P(EqualsProto, element, "matches whether two protos are equal") {
  return google::protobuf::util::MessageDifferencer::Equals(arg, element);
}

TEST(HologramDataAvailabilityReaderTest, GetDashboardJSONTest) {
  HologramDataAvailabilityReader reader(test_root);
  const std::vector<HologramDataAvailability>& protos = 
      reader.GetDashboardJSON("Chipper/");
  HologramDataAvailability expected_spam_proto;
  std::string spam_proto_path = absl::StrCat(test_root, 
      "Chipper/SPAM.textproto");
  std::ifstream ins;
  ins.open(spam_proto_path);
  google::protobuf::io::IstreamInputStream istream_input_stream(&ins);
  google::protobuf::TextFormat::Parse(&istream_input_stream, 
      &expected_spam_proto);
  EXPECT_THAT(protos, testing::ElementsAre(EqualsProto(expected_spam_proto)));
}

} // namespace wireless_android_play_analytics

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}