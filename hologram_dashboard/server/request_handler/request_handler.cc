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

#include "request_handler.h"

namespace wireless_android_play_analytics {

void RequestHandler::GetLastRefreshed(const Pistache::Rest::Request& request, 
  Pistache::Http::ResponseWriter response) {
  // Information within the request is unnecessary.
  UNUSED(request);
  response.send(Pistache::Http::Code::Ok, "Wed May 19 15:46:11 2020");
}

void RequestHandler::GetDashboard(const Pistache::Rest::Request& request, 
    Pistache::Http::ResponseWriter response) {
  response.headers().add<Pistache::Http::Header::AccessControlAllowOrigin>("*");
  std::string system = request.param(":system").as<std::string>();
  // TODO(alexanderlin): Use real data once I get access to them.
  nlohmann::json message;
  HologramConfigSet configs;
  std::string path = "server/request_handler/sample_proto";
  std::ifstream ins;
  ins.open("server/request_handler/sample_proto/hologram_config.ascii");
  google::protobuf::io::IstreamInputStream config_stream(&ins);


  assert(google::protobuf::TextFormat::Parse(&config_stream, &configs));

  if (system == "Chipper") {
    // This simulates the first key (system).
    path = absl::StrCat(path, "/Chipper/");
  } else {
    path = absl::StrCat(path, "/Chipper_GDPR/");
  }
  for (const HologramConfig& config : configs.data_source_config()) {
    // This simulates the second key (source type).
    std::string data_set_path = 
        absl::StrCat(path, SourceType_Name(config.source_type()), 
        ".textproto");
    HologramDataAvailability data_set_availability;
    std::ifstream data_set_stream_in(data_set_path);
    google::protobuf::io::IstreamInputStream 
        data_set_stream(&data_set_stream_in);
    assert(google::protobuf::TextFormat::Parse(&data_set_stream, 
        &data_set_availability));
    DataAvailabilityReader(data_set_availability, &message);
  }
  response.send(Pistache::Http::Code::Ok, message.dump());
}

void RequestHandler::DataAvailabilityReader(
    const HologramDataAvailability& hologram_data_availability, 
    nlohmann::json* message) {
  std::string data_set = 
      SourceType_Name(hologram_data_availability.source_type());
  (*message)[data_set] = nlohmann::json::array();
  for (const HologramDataAvailability_AvailabilityStatus& availability_status 
      : hologram_data_availability.availability_status() ) {
    (*message)[data_set].push_back({availability_status.date(), 
        availability_status.source_ingestion_status() == 
        HologramDataAvailability::INGESTED});   
    }
}

void RequestHandler::PopulateBackfillCommand
    (const Pistache::Rest::Request& request, 
  Pistache::Http::ResponseWriter response) {
  std::string date = request.param(":date").as<std::string>();
  std::string system = request.param(":system").as<std::string>();
  std::string source = request.param(":source").as<std::string>();
  std::string command = "sample command to run backfill for " + source 
      + " on " + system + " for " + date;
  response.send(Pistache::Http::Code::Ok, command);
}

} // namespace wireless_android_play_analytics