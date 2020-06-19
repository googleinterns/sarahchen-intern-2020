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

#include <pistache/http.h>
#include <pistache/router.h>
#include <pistache/endpoint.h>

namespace wireless_android_play_analytics {

class RequestHandler {

    public:
        // Constructs the html code for the dashboard of the specific system.
        void GetDashboard(const Pistache::Rest::Request& request, 
            Pistache::Http::ResponseWriter response);

        // Constructs a command to backfill a specified pipe and send said 
        // command to the user.
        void PopulateBackfillCommand(const Pistache::Rest::Request& request, 
            Pistache::Http::ResponseWriter response);

        // Acquires the latest refresh time and send it to the user.
        void GetLastRefreshed(const Pistache::Rest::Request& request, 
            Pistache::Http::ResponseWriter response);
};

} // namespace wireless_android_play_analytics