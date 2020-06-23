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
#include <string>
#include "absl/flags/flag.h"

ABSL_DECLARE_FLAG(std::string, hologram_source_config_file_path);
ABSL_DECLARE_FLAG(absl::Time, hologram_data_availability_check_time);
ABSL_DECLARE_FLAG(std::vector<std::string>, chipper_required_sources);
ABSL_DECLARE_FLAG(std::vector<std::string>, chipper_gdpr_required_sources);
ABSL_DECLARE_FLAG(std::string, chipper_job_start_root);
ABSL_DECLARE_FLAG(std::string, chipper_gdpr_job_start_root);
ABSL_DECLARE_FLAG(std::string, android_spam_done_file_root);
ABSL_DECLARE_FLAG(std::string, prime_meridian_done_file_root);
ABSL_DECLARE_FLAG(std::string, default_done_file_root);
ABSL_DECLARE_FLAG(std::string, spam_done_file_root);
ABSL_DECLARE_FLAG(std::string, user_attribute_done_file_root);
ABSL_DECLARE_FLAG(std::string, user_experiment_done_file_root);