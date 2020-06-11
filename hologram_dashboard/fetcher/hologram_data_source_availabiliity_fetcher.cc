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
    AcquireConfig(const std::string& path) {
    std::ifstream ins(path);
    google::protobuf::TextFormat::Parser P;
    std::string config_content;
    
    // Path must be valid.
    assert(ins.good());

    while(ins.good()) {   
        std::string tmp; 
        getline(ins, tmp);
        config_content += tmp + "\n";
    }

    // If either the file doesn't exist this will return false.
    assert(P.ParseFromString(config_content, &hologram_configs_));
}

} // namespace wireless_android_play_analytics