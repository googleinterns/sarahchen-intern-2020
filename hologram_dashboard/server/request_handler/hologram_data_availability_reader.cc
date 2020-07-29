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

#include "hologram_data_availability_reader.h"

namespace wireless_android_play_analytics {

  HologramDataAvailabilityReader::HologramDataAvailabilityReader
      (absl::string_view path)
      : root_path(std::string(path)) {
    std::string config_path = absl::StrCat(path, "hologram_config.ascii");
    Parse(config_path, &configs);
  }

  void HologramDataAvailabilityReader::Parse
      (absl::string_view path, google::protobuf::Message* message) {
    std::ifstream ins;
    ins.open(std::string(path));
    google::protobuf::io::IstreamInputStream istream_input_stream(&ins);
    assert(google::protobuf::TextFormat::Parse(&istream_input_stream, message));
  }

  nlohmann::json HologramDataAvailabilityReader::GetDashboardJSON
      (absl::string_view system_dir) {
    nlohmann::json message;
    std::string protos_path = absl::StrCat(root_path, system_dir);

    for (const HologramConfig& config : configs.data_source_config()) {
      // This simulates the second key (source type).
      std::string data_set_path = 
          absl::StrCat(protos_path, SourceType_Name(config.source_type()), 
          ".textproto");
      HologramDataAvailability hologram_data_availability;
      Parse(data_set_path, &hologram_data_availability);
      std::string data_set = 
          SourceType_Name(hologram_data_availability.source_type());
      message[data_set] = nlohmann::json::array();
      for (const HologramDataAvailability_AvailabilityStatus& 
          availability_status : hologram_data_availability.availability_status()
          ) {
        message[data_set].push_back({availability_status.date(), 
            availability_status.source_ingestion_status() == 
            HologramDataAvailability::INGESTED});   
      }
    }

    return message;
  }

} // namespace wireless_android_play_analytics