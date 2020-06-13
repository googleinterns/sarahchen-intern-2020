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
#include <google/protobuf/util/message_differencer.h>

namespace wireless_android_play_analytics{

TEST(FetcherTest, AcquireConfigTest) {
    // Test malformed/invalid path.
    HologramDataSourceAvailabilityFetcher hologram_fetcher;
    ASSERT_DEATH(hologram_fetcher.AcquireConfig("/abc"), "");
    ASSERT_DEATH(hologram_fetcher.AcquireConfig(
        "fetcher/testdata/hologram_config_malformed.config"), "");

    // Test correct path.
    hologram_fetcher.AcquireConfig("fetcher/testdata/hologram_config_test.config");
    HologramConfigSet expected_hologram_config;
    // Create a proto with expected values
    HologramConfig* config = expected_hologram_config.add_data_source_config();
    config->set_source_type(DataSource::APP_TYPE_SOURCE);
    config->set_kvick_corpus(Corpus::HOLOGRAM_DEFAULT);
    config = expected_hologram_config.add_data_source_config();
    config->set_source_type(DataSource::USER_ATTRIBUTE);
    config->set_kvick_corpus(Corpus::HOLOGRAM_USER_ATTRIBUTE);
    
    ASSERT_TRUE(google::protobuf::util::MessageDifferencer::Equals(expected_hologram_config, hologram_fetcher.hologram_configs_));
}

} // namespace wireless_android_play_analytics

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}