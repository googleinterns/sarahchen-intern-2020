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

using ::google::protobuf::util::MessageDifferencer;

namespace wireless_android_play_analytics{

TEST(FetcherTest, ConstructorMissingFlags) {
    // Missing one flag.
    absl::SetFlag(&FLAGS_chipper_batch_job_cell, "cv");
    absl::SetFlag(&FLAGS_hologram_source_config_file_path, "path");
    ASSERT_DEATH(HologramDataSourceAvailabilityFetcher(), "");
}

TEST(FetcherTest, ConstructorValidFlags) {
    absl::SetFlag(&FLAGS_chipper_batch_job_cell, "cv");
    absl::SetFlag(&FLAGS_hologram_source_config_file_path, 
        "fetcher/testdata/hologram_config_valid.ascii");
    absl::SetFlag(&FLAGS_chipper_gdpr_batch_job_cell, "ef");
    HologramDataSourceAvailabilityFetcher hologram_fetcher;
    EXPECT_THAT(hologram_fetcher.system_to_cell_, 
        testing::UnorderedElementsAre(testing::Pair(System::CHIPPER, "cv"), 
            testing::Pair(System::CHIPPER_GDPR, "ef")));
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
    hologram_fetcher.AcquireConfig(
        "fetcher/testdata/hologram_config_valid.ascii");
    HologramConfigSet expected_hologram_config;
    // Create a proto with expected values
    HologramConfig* config = expected_hologram_config.add_data_source_config();
    config->set_source_type(DataSource::APP_TYPE_SOURCE);
    config->set_kvick_corpus(Corpus::HOLOGRAM_DEFAULT);
    config = expected_hologram_config.add_data_source_config();
    config->set_source_type(DataSource::USER_ATTRIBUTE);
    config->set_kvick_corpus(Corpus::HOLOGRAM_USER_ATTRIBUTE);
    
    EXPECT_TRUE(MessageDifferencer::Equals(expected_hologram_config, 
        hologram_fetcher.hologram_configs_));
}

TEST(FetcherTest, UpdateHistoryNewProto) {
    HologramDataSourceAvailabilityFetcher hologram_fetcher;
    HologramDataAvailability data_availability;
    absl::Time time = absl::Now();
    absl::TimeZone google_time;
    ASSERT_TRUE(absl::LoadTimeZone("America/Los_Angeles", &google_time));
    StatusType status = StatusType::SUCCESS;
    DataSourceDetail detail;
    detail.set_timestamp_millis(absl::ToUnixSeconds(time));
    detail.set_status(status);
    detail.set_date(absl::FormatTime(time, google_time));
    hologram_fetcher.UpdateHistory(&data_availability, time, status);
    EXPECT_EQ(data_availability.history_size(), 1);
    EXPECT_TRUE(MessageDifferencer::Equals(data_availability.history(0), 
        detail));
}

TEST(FetcherTest, UpdateHistorySameDay) {
    HologramDataSourceAvailabilityFetcher hologram_fetcher;
    HologramDataAvailability data_availability;
    absl::Time time = absl::Now();
    absl::TimeZone google_time;
    ASSERT_TRUE(absl::LoadTimeZone("America/Los_Angeles", &google_time));
    StatusType status = StatusType::SUCCESS;
    DataSourceDetail detail;
    detail.set_timestamp_millis(absl::ToUnixSeconds(time));
    detail.set_date(absl::FormatTime(time, google_time));
    detail.set_status(status);
    DataSourceDetail* history = data_availability.add_history();
    history->set_timestamp_millis(absl::ToUnixSeconds(time));
    history->set_date(absl::FormatTime(time, google_time));
    history->set_status(status);
    hologram_fetcher.UpdateHistory(&data_availability, time, status);
    ASSERT_EQ(data_availability.history_size(), 1);
    EXPECT_TRUE(MessageDifferencer::Equals(data_availability.history(0), 
        detail));
}

TEST(FetcherTest, UpdateHistoryIncrementHistory) {
    HologramDataSourceAvailabilityFetcher hologram_fetcher;
    HologramDataAvailability data_availability;
    StatusType status = StatusType::SUCCESS;
    DataSourceDetail detail;
    absl::CivilDay civil_time(2020, 5, 4);
    absl::TimeZone google_time;
    ASSERT_TRUE(absl::LoadTimeZone("America/Los_Angeles", &google_time));
    absl::Time time = absl::FromCivil(civil_time, google_time);
    DataSourceDetail* history = data_availability.add_history();
    history->set_timestamp_millis(absl::ToUnixSeconds(time));
    history->set_date(absl::FormatTime(time, google_time));
    history->set_status(status);
    // One day later
    absl::CivilDay civil_time_day_late(2020, 5, 5);;
    time = absl::FromCivil(civil_time_day_late, google_time);
    detail.set_timestamp_millis(absl::ToUnixSeconds(time));
    detail.set_date(absl::FormatTime(time, google_time));
    detail.set_status(status);
    hologram_fetcher.UpdateHistory(&data_availability, time, status);
    ASSERT_EQ(data_availability.history_size(), 2);
    EXPECT_TRUE(MessageDifferencer::Equals(data_availability.history(1), 
        detail));
    // One month later
    absl::CivilDay civil_time_month_late(2020, 6, 5);;
    time = absl::FromCivil(civil_time_month_late, google_time);
    detail.set_timestamp_millis(absl::ToUnixSeconds(time));
    detail.set_date(absl::FormatTime(time, google_time));
    detail.set_status(status);
    hologram_fetcher.UpdateHistory(&data_availability, time, status);
    ASSERT_EQ(data_availability.history_size(), 3);
    EXPECT_TRUE(MessageDifferencer::Equals(data_availability.history(2), 
        detail));
    // One year later
    absl::CivilDay civil_time_year_late(2021, 6, 5);;
    time = absl::FromCivil(civil_time_year_late, google_time);
    detail.set_timestamp_millis(absl::ToUnixSeconds(time));
    detail.set_date(absl::FormatTime(time, google_time));
    detail.set_status(status);
    hologram_fetcher.UpdateHistory(&data_availability, time, status);
    ASSERT_EQ(data_availability.history_size(), 4);
    EXPECT_TRUE(MessageDifferencer::Equals(data_availability.history(3), 
        detail));
}

TEST(FetcherTest, UpdateHistoryOverflowHistory) {
    HologramDataSourceAvailabilityFetcher hologram_fetcher;
    HologramDataAvailability data_availability;
    StatusType status = StatusType::SUCCESS;
    DataSourceDetail detail;
    absl::CivilDay civil_time(2020, 5, 4);
    absl::TimeZone google_time;
    ASSERT_TRUE(absl::LoadTimeZone("America/Los_Angeles", &google_time));
    absl::Time time = absl::FromCivil(civil_time, google_time);
    for(int i = 0; i < 7; ++i) {
        // Use increments of i as unique identifiers of each history entry.
        DataSourceDetail* tmp = data_availability.add_history();
        tmp->set_timestamp_millis(absl::ToUnixSeconds(time) + i);
        tmp->set_status(status);
    }
    absl::CivilDay civil_time_day_late(2020, 5, 5);;
    absl::Time time_late = absl::FromCivil(civil_time_day_late, google_time);
    detail.set_timestamp_millis(absl::ToUnixSeconds(time_late));
    detail.set_date(absl::FormatTime(time_late, google_time));
    detail.set_status(status);
    hologram_fetcher.UpdateHistory(&data_availability, time_late, status);
    ASSERT_EQ(data_availability.history_size(), 7);
    EXPECT_TRUE(MessageDifferencer::Equals(data_availability.history(6), 
        detail));
    // The oldest history should be deleted so the new oldest history should be
    // previously second oldest history.
    detail.set_timestamp_millis(absl::ToUnixSeconds(time) + 1);
    detail.clear_date();
    EXPECT_TRUE(MessageDifferencer::Equals(data_availability.history(0), 
        detail));
}

TEST(FetcherTest, UpdateDataAvailability) {
    HologramDataSourceAvailabilityFetcher hologram_fetcher;
    System system = System::CHIPPER;
    absl::CivilDay civil_time(2020, 5, 4);
    absl::TimeZone google_time;
    ASSERT_TRUE(absl::LoadTimeZone("America/Los_Angeles", &google_time));
    absl::Time time = absl::FromCivil(civil_time, google_time);
    DataSource data_source = DataSource::APPS_DAILY_DATA_SOURCE;
    StatusType status = StatusType::SUCCESS;
    hologram_fetcher.UpdateDataAvailability(system, time, data_source, status);
    ASSERT_NE(hologram_fetcher.system_to_data_source_availability_.find(system),
        hologram_fetcher.system_to_data_source_availability_.end());
    absl::flat_hash_map<DataSource,HologramDataAvailability>& 
        data_to_availability_map = 
        hologram_fetcher.system_to_data_source_availability_[system];
    ASSERT_NE(data_to_availability_map.find(data_source), 
        data_to_availability_map.end());
    HologramDataAvailability data_availability;
    data_availability.set_data_source(data_source);
    DataSourceDetail* latest_status = data_availability.mutable_latest_status();
    latest_status->set_timestamp_millis(absl::ToUnixSeconds(time));
    latest_status->set_date(absl::FormatTime(time, google_time));
    latest_status->set_status(status);
    hologram_fetcher.UpdateHistory(&data_availability, time, status);
    EXPECT_TRUE(MessageDifferencer::Equals(data_availability, 
        hologram_fetcher.system_to_data_source_availability_[system]
        [data_source]));
}

} // namespace wireless_android_play_analytics

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}