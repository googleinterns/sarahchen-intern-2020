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

HologramDataSourceAvailabilityFetcher::HologramDataSourceAvailabilityFetcher(){
    // Assert(FLAGS_config_file_path is not empty)
    // Populate data_source_availability_
    // AcquireConfig(FLAGS_config_file_path)
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

void HologramDataSourceAvailabilityFetcher::GetHologramDataAvailability
    (absl::CivilMinute date) {
    // TODO(alexanderlin): Implement.
    // for(const std::pair<std::string, std::string>& it : system_to_cell_map_) {
    //     for(int i = 0; i < hologram_configs_.data_source_config_size(); ++i) {
    //     UpdateProto(it.first, date, hologram_configs_.data_source_config(i), status);
    // }
}

void HologramDataSourceAvailabilityFetcher::UpdateProto(std::string system, 
    absl::CivilMinute date, DataSource data_source, StatusType status){
    // TODO(alexanderlin): Implement.
    // Call UpdateHistory for updating proto's history
}

void HologramDataSourceAvailabilityFetcher::UpdateHistory(
    HologramDataAvailability* availability_proto, absl::CivilMinute time, 
    StatusType status) {
    // TODO(alexanderlin): Implement.    
}

} // namespace wireless_android_play_analytics