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
  HologramDataAvailabilityReader reader("server/request_handler/sample_proto/");
  nlohmann::json message;

  if (system == "Chipper") {
    // This simulates the first key (system).
    message = reader.GetDashboardJSON("Chipper/");
  } else {
    message = reader.GetDashboardJSON("Chipper_GDPR/");
  }

  response.send(Pistache::Http::Code::Ok, message.dump());
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