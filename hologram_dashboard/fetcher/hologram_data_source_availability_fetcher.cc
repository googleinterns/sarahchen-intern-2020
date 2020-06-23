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
    assert(absl::LoadTimeZone("America/Los_Angeles", &google_time));
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
    // Hardcode database location since there's no access to real Hologram
    // server.
    std::filesystem::path database_loc = std::filesystem::current_path();
    database_loc += "/fetcher/testdata/Database/";
    std::string date = absl::FormatTime("%Y/%m/%d/", time, google_time);
    
    for(const auto& [system, borg_cell] : system_to_cell_) {
        for(int i = 0; i < hologram_configs_.data_source_config_size(); ++i) {
            HologramConfig config = hologram_configs_.data_source_config(i);
            int64_t latest_job = -1;
            std::filesystem::path update_coordinator_path = database_loc; 
            update_coordinator_path += borg_cell + "/" ;
            update_coordinator_path += "update_coordinator_done/" + date;
            std::filesystem::path update_lookup_server_path = database_loc;
            update_lookup_server_path +=  borg_cell + "/";
            update_lookup_server_path += "update_lookup_server_done/" + date;
            // If corpus has not succeeded for the day yet no data is injested.
            
            // Prevent checking last update multiple times within a run.
            if (system_to_last_update_.find(system) == 
                system_to_last_update_.end()) {
                latest_job = GetKvickServerUpdateTimestampMicros(
                    update_lookup_server_path, update_coordinator_path);   
            }
            
            if(latest_job == -1) {
                UpdateDataAvailability(system, time, config.source_type(), 
                    StatusType::DATA_NOT_INGESTED_ON_TIME);
            }
            else {
                system_to_last_update_[system] = latest_job;
                std::filesystem::path data_source_path = database_loc;
                std::string corpus_source = Corpus_Name(config.kvick_corpus());
                data_source_path += borg_cell + "/" + corpus_source + "/" + date;
                if (CheckDoneFileExists(data_source_path)) {
                    // Acquire time timestamp for earliest DONE file.
                    std::vector<std::string> corpus_done_files;
                    for (const std::filesystem::directory_entry& entry : 
                        std::filesystem::directory_iterator(data_source_path)) {
                        corpus_done_files.push_back(entry.path().filename());
                    }
                    std::sort(corpus_done_files.begin(), corpus_done_files.end());
                    // Need to adjust for difference in precision.
                    if (corpus_done_files[0] <= std::to_string(latest_job)) {
                        UpdateDataAvailability(system, time, config.source_type(), 
                            StatusType::SUCCESS);
                        continue;
                    }
                }
                UpdateDataAvailability(system, time, config.source_type(), 
                    StatusType::DATA_NOT_INGESTED_ON_TIME);
            }
        }
    }
}

int64_t HologramDataSourceAvailabilityFetcher::GetKvickServerUpdateTimestampMicros(
    const std::filesystem::path update_lookup_server_path, 
    const std::filesystem::path update_coordinator_path) {       
    // If one of the folders doesn't even exist or is empty, then the corpus
    // has not finished running today yet.
    if (!CheckDoneFileExists(update_coordinator_path) || 
        !CheckDoneFileExists(update_lookup_server_path)) {
        return -1;
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
        return -1;
    }
    return std::stoll(corpus_done_time[corpus_done_time.size() - 1]);
}

bool HologramDataSourceAvailabilityFetcher::CheckDoneFileExists(
    std::filesystem::path path){
    return std::filesystem::exists(path) && !std::filesystem::is_empty(path);
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