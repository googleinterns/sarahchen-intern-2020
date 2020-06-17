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

void HologramDataSourceAvailabilityFetcher::Process() {
    // TODO(alexanderlin): add implementation.
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

void HologramDataSourceAvailabilityFetcher::UpdateProto(std::string system, 
    std::time_t time, DataSource data_source, StatusType status) {
    std::unordered_map<DataSource,HologramDataAvailability>& 
        data_to_availability_map = system_to_data_source_availability_[system];
    HologramDataAvailability& availability_proto = 
        data_to_availability_map[data_source];
    DataSourceDetail* latest_status = availability_proto.mutable_latest_status();
    if (!availability_proto.has_data_source()) {
        availability_proto.set_data_source(data_source);
    }
    latest_status->set_date((int) time);
    latest_status->set_status(status);
    UpdateHistory(&availability_proto, time, status);
}

void HologramDataSourceAvailabilityFetcher::UpdateHistory(
    HologramDataAvailability* availability_proto, std::time_t time, StatusType status) {
    if (availability_proto->history_size() != 0) {
        // Acquire the latest history
        DataSourceDetail* latest_history = availability_proto->mutable_history(
            availability_proto->history_size() - 1);
        time_t history_time = latest_history->date();
        struct tm time_tm;
        struct tm history_time_tm;
        // Have to use localtime_r since localtime will overwrite each other
        localtime_r(&time, &time_tm);
        localtime_r(&history_time, &history_time_tm);
        // Note here we ignore the case in which the given time is lower than
        // the latest status because user should not be able to go back in time
        // and check details of the past.
        if (time_tm.tm_year == history_time_tm.tm_year &&
            time_tm.tm_mon == history_time_tm.tm_mon &&
            time_tm.tm_mday == history_time_tm.tm_mday) {
            // If it's the same day just update the status.
            latest_history->set_date((int) time);
            latest_history->set_status(status);
            return;
        }
    }
    // Add a history if either its empty or it is later than latest history.
    DataSourceDetail* detail = availability_proto->add_history();
    detail->set_date((int) time);
    detail->set_status(status);

    if (availability_proto->history_size() > 7) {
        // Remove the oldest history
        auto history_list = availability_proto->mutable_history();
        history_list->DeleteSubrange(0, 1);
    }
}

} // namespace wireless_android_play_analytics