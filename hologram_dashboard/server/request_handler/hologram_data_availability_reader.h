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

#include <vector>
#include <filesystem>
#include <sstream>
#include <fstream>
#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include "fetcher/proto/hologram_availability.pb.h"
#include "fetcher/proto/hologram_config.pb.h"
#include "nlohmann/json.hpp"
#include "absl/strings/str_cat.h"

namespace wireless_android_play_analytics {

class HologramDataAvailabilityReader {

 public:
  // Non-default constructor with the root path of files to simulate reading
  // from database.
  HologramDataAvailabilityReader(absl::string_view path);

  nlohmann::json GetDashboardJSON(absl::string_view system_dir);

 private:
  std::string root_path;
  HologramConfigSet configs;

  // Parses the proto given by the path
  void Parse(absl::string_view path, google::protobuf::Message* message);
};

} // namespace wireless_android_play_analytics