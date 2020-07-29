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

#include "hologram_data_availability_reader.h"
#include <gtest/gtest.h>

namespace wireless_android_play_analytics {

const std::string expected_message_string = 
    R"json({"SPAM":[["5/19/2020",true],["5/18/2020",true]]})json";
const std::string test_root = "server/request_handler/sample_proto/Test/";

TEST(HologramDataAvailabilityReaderTest, GetDashboardJSONTest) {
  HologramDataAvailabilityReader reader(test_root);
  std::vector<HologramDataAvailability>& protos = reader.GetDashboardJSON("Chipper/");
  EXPECT_EQ(message.dump(), expected_message_string);
}

} // namespace wireless_android_play_analytics

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}