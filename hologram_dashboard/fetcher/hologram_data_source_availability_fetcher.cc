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
    absl::Time time) {
    absl::TimeZone google_time;
    assert(absl::LoadTimeZone("America/Los_Angeles", &google_time));
    absl::CivilSecond civil_time = absl::ToCivilSecond(time, google_time);
    // Hardcode database location since there's no access to real Hologram
    // server.
    std::filesystem::path database_loc = "/fetcher/testdata/Database/";

    std::string month_folder = std::to_string(civil_time.month());
    // If the month is 1 digit, prepend it with a 0.
    if (month_folder.size() == 1) {
        month_folder = "0" + month_folder;
    }
    month_folder += "/";
    std::string day_folder = std::to_string(civil_time.day());
    // If the day is 1 digit, prepend it with a 0.
    if (day_folder.size() == 1) {
        day_folder = "0" + day_folder;
    }
    day_folder += "/";
    std::string year_folder = std::to_string(civil_time.year());
    year_folder += "/";
    
    for(const std::pair<System, std::string>& it : system_to_cell_) {
        for(int i = 0; i < hologram_configs_.data_source_config_size(); ++i) {
            HologramConfig config = hologram_configs_.data_source_config(i);
            const absl::flat_hash_map<Corpus, std::string>& corpus_to_update =
                system_to_corpus_to_last_update_[system];
            std::filesystem::path update_coordinator_path = database_loc; 
            update_coordinator_path += it.second + "update_coordinator_done/";
            update_coordinator_path += year_folder + month_folder + day_folder;
            std::filesystem::path update_lookup_server_path = database_loc;
            update_lookup_server_path +=  it.second;
            update_lookup_server_path += "update_lookup_server_done/";
            update_lookup_server_path += year_folder + month_folder + day_folder;
            // If corpus has not succeeded for the day yet no data is injested.
            std::string latest_job = UpdateCorpusFinishTime(
                update_lookup_server_path, update_coordinator_path);
            if(latest_job.empty()) {
                UpdateDataAvailability(it->first, time, config.source_type(), 
                    Status::DATA_NOT_INGESTED_ON_TIME);
            }
            else {
                system_to_last_update_[system] = latest_job;
                std::filesystem::path data_source_path = database_loc;
                std::string data_source = DataSource_Name(config.source_type());
                data_source_path += it.second + data_source + "/";
                data_source_path += year_folder + month_folder + day_folder;
            }
        }
    }
}

std::string HologramDataSourceAvailabilityFetcher::UpdateCorpusFinishTime(
    const std::filesystem::path update_lookup_server_path, 
    const std::filesystem::path update_coordinator_path) {       
    // If one of the folders doesn't even exist or is empty, then the corpus
    // has not finished running today yet.
    if (!std::filesystem::exists(update_coordinator_path) || 
        !std::filesystem::exists(update_lookup_server_path) ||
        std::filesystem::is_empty(update_coordinator_path) ||
        std::filesystem::is_empty(update_lookup_server_path)) {
        return std::string();
    }
    // Acquire the intersection of coordinator and lookupserver files.
    std::vector<std::string> update_coordinator_files;
    for (const std::filesystem::directory_entry& entry : 
        std::filesystem::directory_iterator(update_coordinator_path)) {
        update_coordinator_files.push_back(entry.path().filename());
    }
    std::sort(update_coordinator_files.begin(), 
        update_coordinator_files.end());
    std::vector<std::string> update_lookup_server_files;
    for (const std::filesystem::directory_entry& entry : 
        std::filesystem::directory_iterator(update_lookup_server_path)) {
        update_lookup_server_files.push_back(entry.path().filename());
    }
    std::sort(update_lookup_server_files.begin(), 
        update_lookup_server_files.end());
    std::vector<std::string> corpus_done_time;
    std::set_intersection(update_lookup_server_files.begin(), 
        update_lookup_server_files.end(), update_coordinator_files.begin(), 
        update_coordinator_files.end(), back_inserter(corpus_done_time));
    // If there is no intersection, this corpus has not successfuly ran yet.
    if (corpus_done_time.empty()) {
        return std::string();
    }
    return corpus_done_time[corpus_done_time.size() - 1];
}

void HologramDataSourceAvailabilityFetcher::UpdateDataAvailability(
    System system, absl::Time date, DataSource data_source, 
    StatusType status){
    // TODO(alexanderlin): Implement.
    // Call UpdateHistory for updating proto's history
}

void HologramDataSourceAvailabilityFetcher::UpdateHistory(
    HologramDataAvailability* availability_proto, absl::Time time, 
    StatusType status) {
    // TODO(alexanderlin): Implement.    
}

} // namespace wireless_android_play_analytics