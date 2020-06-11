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

#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <gflags/gflags.h>
#include <assert.h>
#include "fetcher/proto/hologram_availability.pb.h"
#include "fetcher/proto/hologram_config.pb.h"
#include "hologram_data_fetcher.h"

DEFINE_string(chipper_batch_job_cell, "", 
    "The job running cell of Chipper Batch.");
DEFINE_string(chipper_gdpr_batch_job_cell, "", 
    "The job running cell of Chipper GDPR pipeline.");
DEFINE_string(config_file_path, "", 
    "Specifies where the config file can be located.");

namespace wireless_android_play_analytics {

class HologramDataSourceAvailabilityFetcher: public HologramDataFetcher {

public:
    void Process() override;

private:
    std::unordered_map<std::string, std::string> system_to_cell_map_;
    HologramConfigSet hologram_configs_;
    std::unordered_map<std::string, HologramDataAvailability> 
        data_sources_availability_map_;
};

} // namespace wireless_android_play_analytics