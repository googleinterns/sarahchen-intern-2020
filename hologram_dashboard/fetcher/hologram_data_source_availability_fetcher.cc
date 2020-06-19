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

namespace wireless_android_play_analytics{

HologramDataSourceAvailabilityFetcher::HologramDataSourceAvailabilityFetcher() {
    // Ensure that all the flags are given.
    assert(!absl::GetFlag(FLAGS_chipper_batch_job_cell).empty());
    assert(!absl::GetFlag(FLAGS_chipper_gdpr_batch_job_cell).empty());
    assert(!absl::GetFlag(FLAGS_hologram_source_config_file_path).empty());

    system_to_cell_[System::CHIPPER] = 
        absl::GetFlag(FLAGS_chipper_batch_job_cell);
    system_to_cell_[System::CHIPPER_GDPR] = 
        absl::GetFlag(FLAGS_chipper_gdpr_batch_job_cell);
    AcquireConfig(absl::GetFlag(FLAGS_hologram_source_config_file_path));
}

void HologramDataSourceAvailabilityFetcher::Process() {
    // TODO(alexanderlin): add implementation.
    // FetchFromDatabase()
    // GetStatus(todays date)
    // SendToDataBase()
}

void HologramDataSourceAvailabilityFetcher::
    AcquireConfig(const std::string& config_file_path) {
    std::ifstream ins(config_file_path);
    // Path must be valid.
    assert(ins.good());
    google::protobuf::io::IstreamInputStream config_stream(&ins);

    // If either the file doesn't exist this will return false.
    assert(google::protobuf::TextFormat::Parse(&config_stream, 
                                                &hologram_configs_));
}

void HologramDataSourceAvailabilityFetcher::FetchFromDatabase() {
    // TODO(alexanderlin): Implement.
}

void HologramDataSourceAvailabilityFetcher::GetHologramDataAvailability(
    absl::Time date) {
    // TODO(alexanderlin): Implement.
    // for(const std::pair<std::string, std::string>& it : system_to_cell_map_) {
    //     for(int i = 0; i < hologram_configs_.data_source_config_size(); ++i) {
    //     UpdateProto(it.first, date, hologram_configs_.data_source_config(i), status);
    // }
}

void HologramDataSourceAvailabilityFetcher::UpdateDataAvailability(System system, 
    absl::Time time, DataSource data_source, StatusType status) {
    absl::flat_hash_map<DataSource, HologramDataAvailability>& 
        data_to_availability_map = system_to_data_source_availability_[system];
    HologramDataAvailability& hologram_data_availability = 
        data_to_availability_map[data_source];
    DataSourceDetail* latest_status = 
        hologram_data_availability.mutable_latest_status();
    if (!hologram_data_availability.has_data_source()) {
        hologram_data_availability.set_data_source(data_source);
    }
    latest_status->set_date(absl::ToUnixSeconds(time));
    latest_status->set_status(status);
    UpdateHistory(&hologram_data_availability, time, status);
}

void HologramDataSourceAvailabilityFetcher::UpdateHistory(
    HologramDataAvailability* hologram_data_availability, absl::Time time, 
    StatusType status) {
    absl::TimeZone google_time;
    // Load time zone could fail according to absl doc.
    assert(absl::LoadTimeZone("America/Los_Angeles", &google_time));
    if (hologram_data_availability->history_size() != 0) {
        // Acquire the latest history
        DataSourceDetail* latest_history = hologram_data_availability->
            mutable_history(hologram_data_availability->history_size() - 1);
        absl::Time history_time = absl::FromUnixSeconds(latest_history->date());
        absl::CivilSecond history_civil_time = absl::ToCivilSecond(history_time, 
            google_time);
        absl::CivilSecond time_civil = absl::ToCivilSecond(time, google_time);
        // Note here we ignore the case in which the given time is lower than
        // the latest status because user should not be able to go back in time
        // and check details of the past.
        if (time_civil.year() == history_civil_time.year() &&
            time_civil.month() == history_civil_time.month() &&
            time_civil.day() == history_civil_time.day()) {
            // If it's the same day just update the status.
            latest_history->set_date(absl::ToUnixSeconds(time));
            latest_history->set_status(status);
            return;
        }
    }
    // Add a history if either its empty or it is later than latest history.
    DataSourceDetail* detail = hologram_data_availability->add_history();
    detail->set_date(absl::ToUnixSeconds(time));
    detail->set_status(status);

    if (hologram_data_availability->history_size() > 7) {
        // Remove the oldest history
        auto history_list = hologram_data_availability->mutable_history();
        history_list->DeleteSubrange(0, 1);
    }
}

} // namespace wireless_android_play_analytics