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

#include "request_handler/request_handler.h"

int main(int argc, char* argv[]) {
    // Initiates the server.
    Pistache::Http::Endpoint server(Pistache::Address(Pistache::Ipv4::any(), 
        Pistache::Port(8000)));
    Pistache::Rest::Router router;
    wireless_android_play_analytics::RequestHandler handler;
    server.init();

    // Set up the router.
    Pistache::Rest::Routes::Get(router, "/:system", 
        Pistache::Rest::Routes::bind 
        (&wireless_android_play_analytics::RequestHandler::GetDashboard, 
        &handler));
    Pistache::Rest::Routes::Get(router, "/:system/:source/:date", 
        Pistache::Rest::Routes::bind
        (&wireless_android_play_analytics::RequestHandler::PopulateBackfillCommand, 
        &handler));
    Pistache::Rest::Routes::Get(router, "/", 
        Pistache::Rest::Routes::bind 
        (&wireless_android_play_analytics::RequestHandler::GetLastRefreshed, 
        &handler));

    server.setHandler(router.handler());

    // Run the server.
    server.serve();
}