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
#include <gtest/gtest.h>
#include <assert.h>
#include <fstream>
#include <time.h>
#include <chrono>
#include <filesystem>
#include <algorithm>
#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include "fetcher/proto/hologram_availability.pb.h"
#include "fetcher/proto/hologram_config.pb.h"
#include "hologram_data_fetcher.h"
#include "flags.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/btree_map.h"
#include "absl/strings/string_view.h"

namespace wireless_android_play_analytics {

class HologramDataSourceAvailabilityFetcher: public HologramDataFetcher {

public:
    // Populates the system_to_cell_map_ with flag input
    HologramDataSourceAvailabilityFetcher();
    void Process() override;

private:
    FRIEND_TEST(FetcherTest, ConstructorMissingFlags);
    FRIEND_TEST(FetcherTest, ConstructorValidFlags);
    FRIEND_TEST(FetcherTest, InvalidAcquireConfig);
    FRIEND_TEST(FetcherTest, ValidAcquireConfig);
    // Poulates hologram_config_ and ends the program if the path provided leads
    // to wrong file or malformed file.
    void AcquireConfig(const std::string& config_file_path);

    // Fetches availability info from database.
    void FetchFromDatabase();

    // Populates the private variables.
    void SetUpFetcher();

    FRIEND_TEST(FetcherTest, GetHologramDataAvailability);
    // Gets the status of all data sources for all systems at the specified time.
    void GetHologramDataAvailability(absl::Time time);

    FRIEND_TEST(FetcherTest, UpdateCorpusFinishTimeOneTime);
    FRIEND_TEST(FetcherTest, UpdateCorpusFinishTimeNoFile);
    FRIEND_TEST(FetcherTest, UpdateCorpusFinishTimeMultipleFiles);
    // Updates the finish time for a specified system and corpus, returns 
    // whether said corpus has ran at all for the day.
    int64_t GetKvickServerUpdateTimestampMicros(
        std::filesystem::path update_lookup_server_path, 
        std::filesystem::path update_coordinator_path);
    
    // Checks either if the supposed folder with done files have any
    bool CheckDoneFileExists(std::filesystem::path path);

    // Updates the proto given the type of data source, system it is in, the
    // date, and the status acquired by GetStatus.
    void UpdateDataAvailability(System system, absl::Time time, 
        DataSource data_source, StatusType status);

    // Acts as the helper function for UpdateProto specifically regarding
    // modification of a proto's history
    void UpdateHistory(HologramDataAvailability* availability_proto, 
        absl::Time time, StatusType status);

    struct HologramClient {
        System system_;

        std::string last_update_;

        std::string job_start_file_root_;

        std::vector<DataSource> required_source_types_;
    };

    // Acquires the client's info for specified system.
    HologramClient GetClientInfo(System system, 
        const std::vector<std::string>& required_sources, 
        absl::string_view job_start_file_root);

    std::vector<HologramClient> hologram_client_info_;
    HologramConfigSet hologram_configs_;
    absl::flat_hash_map<Corpus, std::string> corpus_to_done_file_root;
    absl::TimeZone google_time;
};

} // namespace wireless_android_play_analytics