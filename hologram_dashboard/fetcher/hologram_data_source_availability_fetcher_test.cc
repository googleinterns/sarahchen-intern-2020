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

TEST(FetcherTest, InvalidAcquireConfig) {
    HologramDataSourceAvailabilityFetcher hologram_fetcher;
    // Passing a path that does not lead to a file
    ASSERT_DEATH(hologram_fetcher.AcquireConfig(""), "");
    // A path that has a file, but the content of the file is invalid
    ASSERT_DEATH(hologram_fetcher.AcquireConfig(
        "fetcher/testdata/hologram_config_malformed.ascii"), "");
}

TEST(FetcherTest, ValidAcquireConfig) {
    HologramDataSourceAvailabilityFetcher hologram_fetcher;
    // Test correct path.
    hologram_fetcher.AcquireConfig("fetcher/testdata/hologram_config_valid.ascii");
    HologramConfigSet expected_hologram_config;
    // Create a proto with expected values
    HologramConfig* config = expected_hologram_config.add_data_source_config();
    config->set_source_type(DataSource::APP_TYPE_SOURCE);
    config->set_kvick_corpus(Corpus::HOLOGRAM_DEFAULT);
    config = expected_hologram_config.add_data_source_config();
    config->set_source_type(DataSource::USER_ATTRIBUTE);
    config->set_kvick_corpus(Corpus::HOLOGRAM_USER_ATTRIBUTE);
    
    EXPECT_TRUE(google::protobuf::util::MessageDifferencer::Equals
        (expected_hologram_config, hologram_fetcher.hologram_configs_));
}

TEST(FetcherTest, MissingFlags) {
    HologramDataSourceAvailabilityFetcher hologram_fetcher;
    // Missing one flag.
    int argc = 3;
    // Allocate space for each of the arguments.
    char arg1[35];
    char arg2[35];
    char arg3[35];
    char* argv[3] = {arg1, arg2, arg3};
    sprintf(arg1, "fetcher_main");
    sprintf(arg2, "-chipper_batch_job_cell=cv");
    sprintf(arg3, "-config_file_path=path");
    ASSERT_DEATH(hologram_fetcher.ParseFlags(argc, argv), "");
    // Missing different flag.
    sprintf(arg3, "-chipper_gdpr_batch_job_cell=ef");
    ASSERT_DEATH(hologram_fetcher.ParseFlags(argc, argv), "");
}

TEST(FetcherTest, ValidFlags) {
    HologramDataSourceAvailabilityFetcher hologram_fetcher;
    int argc = 4;
    char arg1[35];
    char arg2[35];
    char arg3[35];
    char arg4[35];
    char* argv[4] = {arg1, arg2, arg3, arg4};
    sprintf(arg1, "fetcher_main");
    sprintf(arg2, "-chipper_batch_job_cell=cv");
    sprintf(arg3, "-config_file_path=path");
    sprintf(arg4, "-chipper_gdpr_batch_job_cell=ef");
    EXPECT_EQ("path", hologram_fetcher.ParseFlags(argc, argv));
    for (auto it = hologram_fetcher.system_to_cell_map_.begin();
        it != hologram_fetcher.system_to_cell_map_.end(); it++) {
        EXPECT_TRUE(it->first == "CHIPPER" || it->first == "CHIPPER_GDPR");
        if (it->first == "CHIPPER") {
            EXPECT_EQ(it->second, "cv");
        }
        else {
            EXPECT_EQ(it->second, "ef");
        }
    }
}

} // namespace wireless_android_play_analytics

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}