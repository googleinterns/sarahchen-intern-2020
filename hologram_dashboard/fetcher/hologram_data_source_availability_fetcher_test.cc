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
#include "gmock/gmock.h"

#include <google/protobuf/util/message_differencer.h>

namespace wireless_android_play_analytics{

TEST(FetcherTest, ConstructorMissingFlags) {
    // Missing one flag.
    absl::SetFlag(&FLAGS_chipper_batch_job_cell, "ja-d");
    absl::SetFlag(&FLAGS_hologram_source_config_file_path, "path");
    ASSERT_DEATH(HologramDataSourceAvailabilityFetcher(), "");
}

TEST(FetcherTest, ConstructorValidFlags) {
    absl::SetFlag(&FLAGS_chipper_batch_job_cell, "ja-d");
    absl::SetFlag(&FLAGS_hologram_source_config_file_path, 
        "fetcher/testdata/hologram_config_valid.ascii");
    absl::SetFlag(&FLAGS_chipper_gdpr_batch_job_cell, "oc-d");
    HologramDataSourceAvailabilityFetcher hologram_fetcher;
    EXPECT_THAT(hologram_fetcher.system_to_cell_, 
        testing::UnorderedElementsAre(testing::Pair(System::CHIPPER, "ja-d"), 
            testing::Pair(System::CHIPPER_GDPR, "oc-d")));
}

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

TEST(FetcherTest, UpdateCorpusFinishTimeOneTime) {
    HologramDataSourceAvailabilityFetcher hologram_fetcher;
    std::filesystem::path update_coordinator_path = 
        std::filesystem::current_path();
    std::filesystem::path update_lookup_server_path = 
        std::filesystem::current_path();
    update_coordinator_path += "/fetcher/testdata/Database/ja-d/";
    update_coordinator_path += "update_coordinator_done/2020/06/04/";
    update_lookup_server_path += "/fetcher/testdata/Database/ja-d/"; 
    update_lookup_server_path += "update_lookup_server_done/2020/06/04/";
    EXPECT_EQ("1591419575000000", hologram_fetcher.UpdateCorpusFinishTime(
        update_lookup_server_path, update_coordinator_path));
}

TEST(FetcherTest, UpdateCorpusFinishTimeMultipleFiles) {
    HologramDataSourceAvailabilityFetcher hologram_fetcher;
    std::filesystem::path update_coordinator_path = 
        std::filesystem::current_path();
    std::filesystem::path update_lookup_server_path = 
        std::filesystem::current_path();
    update_coordinator_path += "/fetcher/testdata/Database/ja-d/";
    update_coordinator_path += "update_coordinator_done/2020/06/03/";
    update_lookup_server_path += "/fetcher/testdata/Database/ja-d/"; 
    update_lookup_server_path += "update_lookup_server_done/2020/06/03/";
    EXPECT_EQ("1591419572000000", hologram_fetcher.UpdateCorpusFinishTime(
        update_lookup_server_path, update_coordinator_path));
}

TEST(FetcherTest, UpdateCorpusFinishTimeNoFile) {
    HologramDataSourceAvailabilityFetcher hologram_fetcher;
    std::filesystem::path update_coordinator_path = 
        std::filesystem::current_path();
    std::filesystem::path update_lookup_server_path = 
        std::filesystem::current_path();
    update_coordinator_path += "/fetcher/testdata/Database/ja-d/";
    update_coordinator_path += "update_coordinator_done/2020/06/02/";
    update_lookup_server_path += "/fetcher/testdata/Database/ja-d/"; 
    update_lookup_server_path += "update_lookup_server_done/2020/06/02/";
    EXPECT_EQ("", hologram_fetcher.UpdateCorpusFinishTime(
        update_lookup_server_path, update_coordinator_path));
}

TEST(FetcherTest, GetHologramDataAvailability) {
    HologramDataSourceAvailabilityFetcher hologram_fetcher;
    absl::CivilDay civil_time(2020, 06, 04);
    absl::TimeZone google_time;
    ASSERT_TRUE(absl::LoadTimeZone("America/Los_Angeles", &google_time));
    absl::Time time = absl::FromCivil(civil_time, google_time);
    hologram_fetcher.GetHologramDataAvailability(time);
}

} // namespace wireless_android_play_analytics

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}