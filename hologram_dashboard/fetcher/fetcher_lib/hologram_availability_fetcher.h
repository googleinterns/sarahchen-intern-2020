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
#include "fetcher/fetcher_lib/hologram_availability.pb.h"
#include "fetcher/fetcher_lib/hologram_config.pb.h"


DEFINE_string(chipper_server, "undefined", "Specifies the server on which chipper file location is stored");
DEFINE_string(chipperGDPR_server, "undefined", "Specifies the server on which chipperGDPR file location is stored");
DEFINE_string(prime_meridian_server, "undefined", "Specifies the server on which prime meridian file location is stored");

class HologramAvailabilityFetcher {
    public:
        /* 
         * Requires: Nothing
         * Modifies: Nothing
         * Effects: Default constructor without any settings
         */ 
        HologramAvailabilityFetcher();

        /* 
         * Requires: kvick_folder_in includes exactly all the systems and has 
         *           valid mappings
         * Modifies: kvick_folder
         * Effects: Non-default constructor to populate the mapping between
         *          systems and their disk locations
         */ 
        HologramAvailabilityFetcher(std::unordered_map<std::string, std::string> kvick_folder);

        /* 
         * Requires: hologram_config_path is a valid path
         * Modifies: hologram_configs
         * Effects: Non-default constructor to populate configs of hologram
         */ 
        HologramAvailabilityFetcher(std::string hologram_config_path);

        /* 
         * Requires: kvick_folder_in includes exactly all the systems and has 
         *           valid mappings, hologram_config_path is a valid path
         * Modifies: kvick_folder, hologram_config
         * Effects: Non-default constructor to populate the mapping between
         *          systems and their disk locations and configs
         */ 
        HologramAvailabilityFetcher(std::string hologram_config_path, 
            std::unordered_map<std::string, std::string> kvick_folder);

        /* 
         * Requires: None
         * Modifies: kvick_folder
         * Effects: Parses gflag commands, and throws an error if the requires
         *          flags are not set
         */ 
        void parseCommand();

        /* 
         * Requires: kvick_folder and hologram_configs are populated
         * Modifies: data_sources
         * Effects: Populate the status of every data source for the past 7 days
         *          including today
         */ 
        void populateData();

        /* 
         * Requires: None
         * Modifies: None
         * Effects: Sends data_sources to database
         */ 
        void sendToDatabase();

        /* 
         * Requires: path is a valid path to a config file
         * Modifies: hologram_configs
         * Effects: populate information on configs of the hologram
         */ 
        void populateConfigs(std::string path);

    private:
        std::unordered_map<std::string, std::string> kvick_folder;
        wireless_android_play_analytics::HologramConfigSet hologram_configs;
        wireless_android_play_analytics::HologramDataAvailability data_sources;

        /* 
         * Requires: date is of correct format, source and location are valid
         * Modifies: filesystem
         * Effects: Check the file system to figure out if the data sources are
         *          included in the specified day's pipeline
         */ 
        wireless_android_play_analytics::StatusType fetchStatus(std::string date, std::string source, 
            std::string location);

        /* 
         * Requires: data is valid
         * Modifies: data
         * Effects: populates the status of a source for the current day and 
         *          past 6 days
         */ 
        void populateSource(wireless_android_play_analytics::HologramDataAvailability& data);
};

class HologramException: virtual public std::exception {
    public:
        /* 
        * Requires: None
        * Modifies: message
        * Effects: Constructs the default error message
        */ 
        HologramException();

        /* 
        * Requires: None
        * Modifies: message
        * Effects: Constructs a custom error message
        */ 
        HologramException(std::string message_in);

        /* 
        * Requires: None
        * Modifies: None
        * Effects: destructor for the exception
        */ 
        virtual ~HologramException();

        /* 
        * Requires: None
        * Modifies: None
        * Effects: Returns the error message
        */ 
        virtual const char* what();
    
    private:
        std::string message;
};