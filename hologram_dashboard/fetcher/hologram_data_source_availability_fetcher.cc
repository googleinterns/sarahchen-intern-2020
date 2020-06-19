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
}

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

void HologramDataSourceAvailabilityFetcher::
GetStatus(absl::Time time) {
    absl::TimeZone google_time;
    assert(absl::LoadTimeZone("America/Los_Angeles", &google_time));
    absl::CivilSecond civil_time = absl::ToCivilSecond(time, google_time);
    // Need to hardcode current directory since bazel is built at a different
    // directory and can't include an entire folder for bazel.
    const std::string current_dir = "/usr/local/google/home/alexanderlin"
        + "/sarahchen-intern-2020/hologram_dashboard/fetcher/testdata/Database";

    std::string month_folder = std::to_string(civil_time.month());
    // If the month is 1 digit, prepend it with a 0.
    if (month_folder.size() == 1) {
        month_folder = "0" + month;
    }
    month += "/";
    std::string day_folder = std::to_string(civil_time.day());
    // If the day is 1 digit, prepend it with a 0.
    if (day_folder.size() == 1) {
        day_folder = "0" + day;
    }
    day_folder += "/";
    // tm_month represents years after 1900, so need to append 1900 to it to
    // reflect calendar year.
    std::string year_folder = std::to_string(civil_time.year());
    year_folder += "/"
    
    for(const std::pair<System, std::string>& it : system_to_cell_) {
        for(int i = 0; i < hologram_configs_.data_source_config_size(); ++i) {
            HologramConfig config = hologram_configs_.data_source_config(i);
            // Get the latest update time for each of the corpusfor each of the
            // systems.
            const absl::flat_hash_map<Corpus, std::string>& corpus_to_update =
                system_to_corpus_to_last_update[it.first];
            if (corpus_to_update.find(config.kvick_corpus()) ==
                corpus_to_update.end()) {
                std::filesystem::path update_coordinator_path = current_dir 
                    + it->second + "update_coordinator_done/" + year_folder 
                    + month_folder + day_folder;
                std::filesystem::path update_lookup_server_path = current_dir 
                    + it->second + "update_lookup_server_done/" + year_folder 
                    + month_folder + day_folder;
                // If one of the folders doesn't even exist then this job is not
                // ran yet and definitely did not include the data source.
                if (!std::filesystem::exists(path) || 
                    !std::filesystem::exists(path)) {
                    UpdateProto(it->first, time, config.source_type(), 
                    Status::DATA_NOT_INGESTED_ON_TIME);
                }
            }
        }
    }
}

} // namespace wireless_android_play_analytics