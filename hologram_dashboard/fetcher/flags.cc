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

#include "flags.h"

ABSL_FLAG(std::string, chipper_batch_job_cell, "", 
    "The job running cell of Chipper Batch.");
ABSL_FLAG(std::string, chipper_gdpr_batch_job_cell, "", 
    "The job running cell of Chipper GDPR pipeline.");
ABSL_FLAG(std::string, hologram_source_config_file_path, "", 
    "Specifies where the config file can be located.");
ABSL_FLAG(absl::Time, hologram_job_time, absl::Now(), 
    "The time for which you are interested in checking the DONE file");