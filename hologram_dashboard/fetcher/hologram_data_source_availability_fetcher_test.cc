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

TEST(FetcherTest, UpdateHistoryNewProto) {
    HologramDataSourceAvailabilityFetcher hologram_fetcher;
    HologramDataAvailability hda;
    std::time_t time = std::time(NULL);
    StatusType status = StatusType::SUCCESS;
    DataSourceDetail detail;
    detail.set_date((int) time);
    detail.set_status(status);
    hologram_fetcher.UpdateHistory(hda, time, status);
    EXPECT_EQ(hda.history_size(), 1);
    EXPECT_TRUE(google::protobuf::util::MessageDifferencer::Equals(
        hda.history(0), detail));
}

TEST(FetcherTest, UpdateHistorySameDay) {
    HologramDataSourceAvailabilityFetcher hologram_fetcher;
    HologramDataAvailability hda;
    std::time_t time = std::time(NULL);
    StatusType status = StatusType::SUCCESS;
    DataSourceDetail detail;
    detail.set_date((int) time);
    detail.set_status(status);
    DataSourceDetail* history = hda.add_history();
    history->set_date((int) time);
    history->set_status(status);
    hologram_fetcher.UpdateHistory(hda, time, status);
    ASSERT_EQ(hda.history_size(), 1);
    EXPECT_TRUE(google::protobuf::util::MessageDifferencer::Equals(
        hda.history(0), detail));
}

TEST(FetcherTest, UpdateHistoryIncrementHistory) {
    HologramDataSourceAvailabilityFetcher hologram_fetcher;
    HologramDataAvailability hda;
    StatusType status = StatusType::SUCCESS;
    DataSourceDetail detail;
    struct tm time = {0};
    time.tm_year = 120;
    time.tm_mon = 5;
    time.tm_mday = 4;
    std::time_t base_time = mktime(&time);
    DataSourceDetail* history = hda.add_history();
    history->set_date((int) base_time);
    history->set_status(status);
    // One day later
    time.tm_mday++;
    std::time_t day_late_time = mktime(&time);
    detail.set_date((int) day_late_time);
    detail.set_status(status);
    hologram_fetcher.UpdateHistory(hda, day_late_time, status);
    ASSERT_EQ(hda.history_size(), 2);
    EXPECT_TRUE(google::protobuf::util::MessageDifferencer::Equals(
        hda.history(1), detail));
    // One month later
    time.tm_mon++;
    std::time_t mon_late_time = mktime(&time);
    detail.set_date((int) mon_late_time);
    hologram_fetcher.UpdateHistory(hda, mon_late_time, status);
    ASSERT_EQ(hda.history_size(), 3);
    EXPECT_TRUE(google::protobuf::util::MessageDifferencer::Equals(
        hda.history(2), detail));
    // One year later
    time.tm_year++;
    std::time_t year_late_time = mktime(&time);
    detail.set_date((int) year_late_time);
    hologram_fetcher.UpdateHistory(hda, year_late_time, status);
    ASSERT_EQ(hda.history_size(), 4);
    EXPECT_TRUE(google::protobuf::util::MessageDifferencer::Equals(
        hda.history(3), detail));
}

TEST(FetcherTest, UpdateHistoryOverflowHistory) {
    HologramDataSourceAvailabilityFetcher hologram_fetcher;
    HologramDataAvailability hda;
    StatusType status = StatusType::SUCCESS;
    DataSourceDetail detail;
    struct tm time = {0};
    time.tm_year = 120;
    time.tm_mon = 5;
    time.tm_mday = 4;
    std::time_t base_time = mktime(&time);
    for(int i = 0; i < 7; ++i) {
        // Use increments of i as unique identifiers of each history entry.
        DataSourceDetail* tmp = hda.add_history();
        tmp->set_date((int) base_time + i);
        tmp->set_status(status);
    }
    time.tm_mday++;
    detail.set_date((int) mktime(&time));
    detail.set_status(status);
    hologram_fetcher.UpdateHistory(hda, mktime(&time), status);
    ASSERT_EQ(hda.history_size(), 7);
    EXPECT_TRUE(google::protobuf::util::MessageDifferencer::Equals(
        hda.history(6), detail));
    // The oldest history should be deleted so the new oldest history should be
    // previously second oldest history.
    detail.set_date((int) base_time + 1);
    EXPECT_TRUE(google::protobuf::util::MessageDifferencer::Equals(
        hda.history(0), detail));
}

TEST(FetcherTest, UpdateProto) {
    HologramDataSourceAvailabilityFetcher hologram_fetcher;
    std::string system = "CHIPPER";
    struct tm time = {0};
    time.tm_year = 120;
    time.tm_mon = 5;
    time.tm_mday = 4;
    std::time_t base_time = mktime(&time);
    DataSource data_source = DataSource::APPS_DAILY_DATA_SOURCE;
    StatusType status = StatusType::SUCCESS;
    hologram_fetcher.UpdateProto(system, base_time, data_source, status);
    ASSERT_NE(hologram_fetcher.data_sources_availability_map_.find(system),
        hologram_fetcher.data_sources_availability_map_.end());
    std::unordered_map<DataSource,HologramDataAvailability>& 
        data_to_availability_map = 
        hologram_fetcher.data_sources_availability_map_[system];
    ASSERT_NE(data_to_availability_map.find(data_source), 
        data_to_availability_map.end());
    HologramDataAvailability hda;
    hda.set_data_source(data_source);
    DataSourceDetail* latest_status = hda.mutable_latest_status();
    latest_status->set_date((int) base_time);
    latest_status->set_status(status);
    hologram_fetcher.UpdateHistory(hda, base_time, status);
    EXPECT_TRUE(google::protobuf::util::MessageDifferencer::Equals(
        hda, 
        hologram_fetcher.data_sources_availability_map_[system][data_source]));
}

} // namespace wireless_android_play_analytics

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}