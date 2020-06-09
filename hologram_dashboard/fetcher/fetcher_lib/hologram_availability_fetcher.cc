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
#include "hologram_availability_fetcher.h"


HologramException::HologramException() 
    :message("Undefined Error") {}

HologramException::HologramException(std::string message_in) 
    :message(message_in) {}

HologramException::~HologramException() {}

const char* HologramException::what() {
    return message.c_str();
}

HologramAvailabilityFetcher::HologramAvailabilityFetcher(){}

HologramAvailabilityFetcher::HologramAvailabilityFetcher
    (std::unordered_map<std::string, std::string> kvick_folder){}

HologramAvailabilityFetcher::HologramAvailabilityFetcher
    (std::string hologram_config_path){}

HologramAvailabilityFetcher::HologramAvailabilityFetcher
    (std::string hologram_config_path, std::unordered_map<std::string, std::string> kvick_folder){}

void HologramAvailabilityFetcher::parseCommand() {}

void HologramAvailabilityFetcher::populateConfigs(std::string path) {}

wireless_android_play_analytics::StatusType HologramAvailabilityFetcher::fetchStatus
    (std::string date, std::string source, std::string location) {
        return wireless_android_play_analytics::StatusType();
    }

void HologramAvailabilityFetcher::populateSource
    (wireless_android_play_analytics::HologramDataAvailability& data) {}