/*
    Copyright 2020 Google LLC

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        https://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/
#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <gflags/gflags.h>
#include <assert.h>
#include "fetcher/proto/hologram_availability.pb.h"
#include "fetcher/proto/hologram_config.pb.h"

DEFINE_string(chipper_batch_job_cell, "undefined", "Specifies the server on which chipper file location is stored");
DEFINE_string(chipper_GDPR_batch_job_cell, "undefined", "Specifies the server on which chipperGDPR file location is stored");
DEFINE_string(prime_meridian_batch_job_cell, "undefined", "Specifies the server on which prime meridian file location is stored");
DEFINE_string(config_file_path, "undefined", "Specifies where the config file can be located");

namespace wireless_android_play_analytics {
class HologramAvailabilityFetcher {
public:
    /* 
    * Default constructs the fetcher and begins to parse command and 
    * populate the necessary data
    */
    HologramAvailabilityFetcher();

    /* 
    * Send the updated hologram availability information to the database
    */
    void SendToDatabase();

private:
    std::unordered_map<std::string, std::string> kvick_folder;
    HologramConfigSet hologram_configs;
    std::unordered_map<std::string, HologramDataAvailability> 
        data_sources_availability_map;

    /* 
    * Fetches the status of a specific data source given its batch job cell
    * and date
    */
    StatusType fetchStatus(std::string date, std::string source,
        std::string batch_job_cell_loc);

    /* 
    * Checks the database to see if previous protos exist, if it does load it
    * otherwise, create brand new protos 
    */
    void loadProtoFromDatabase();

    /* 
    * Reads the given config file to populate hologram_configs
    */
    void populateConfigs(std::string path);

    /* 
    * Checks the disk for today's done file and make the necessary changes to
    * the corresponding sources
    */
    void populateData();

    /* 
    * Parses gflag commands to ensure the fetcher are given all the necessary
    * information to proceed
    */
    void parseCommand();
};

} // namespace wireless_android_play_analytics