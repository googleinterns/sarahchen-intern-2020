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

#include "hologram_data_source_availability_fetcher.h"
#include <filesystem>

namespace wireless_android_play_analytics{

TEST(FetcherTest, AcquireConfigTest) {
    // Test malformed/invalid path.
    HologramDataSourceAvailabilityFetcher h;
    ASSERT_DEATH(h.AcquireConfig("/abc"), "");
    ASSERT_DEATH(
        h.AcquireConfig("fetcher/hologram_config_malformed.config"), "");

    // Test correct path.
    h.AcquireConfig("fetcher/hologram_config.config");
    std::ifstream ins("fetcher/hologram_config.config");
    std::string compare;
    while(ins.good()) {
        std::string tmp;
        ins >> tmp;
        compare += tmp;
    }
    std::string hologram_config_debug = h.hologram_configs_.DebugString();
    // Remove all spaces and endline characters.
    hologram_config_debug.erase(
        remove(hologram_config_debug.begin(), hologram_config_debug.end(), ' '), 
            hologram_config_debug.end());
    hologram_config_debug.erase(
        remove(hologram_config_debug.begin(), hologram_config_debug.end(), '\n'), 
            hologram_config_debug.end());
    ASSERT_EQ(hologram_config_debug, compare);
}

} // namespace wireless_android_play_analytics

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}