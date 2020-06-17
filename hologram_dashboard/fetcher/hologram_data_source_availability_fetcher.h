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
#include <unordered_map>
#include <gtest/gtest.h>
#include <assert.h>
#include <fstream>
#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include "fetcher/proto/hologram_availability.pb.h"
#include "fetcher/proto/hologram_config.pb.h"
#include "hologram_data_fetcher.h"
#include "flags.h"

namespace wireless_android_play_analytics {

class HologramDataSourceAvailabilityFetcher: public HologramDataFetcher {

public:
    void Process() override;

private:
    FRIEND_TEST(FetcherTest, InvalidAcquireConfig);
    FRIEND_TEST(FetcherTest, ValidAcquireConfig);
    // Poulates hologram_config_ and ends the program if the path provided leads
    // to wrong file or malformed file.
    void AcquireConfig(const std::string& config_file_path);

    FRIEND_TEST(FetcherTest, MissingFlags);
    FRIEND_TEST(FetcherTest, ValidFlags);
    // Populates system_to_cell_map_ given the arguments and returns the path
    // to the config file.
    std::string ParseFlags();
    
    std::unordered_map<std::string, std::string> system_to_cell_map_;
    HologramConfigSet hologram_configs_;
    std::unordered_map<std::string, HologramDataAvailability> 
        data_sources_availability_map_;
};

} // namespace wireless_android_play_analytics