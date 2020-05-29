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
#include <pistache/http.h>
#include <pistache/router.h>
#include <pistache/endpoint.h>

using namespace Pistache;

class HDAHandler {
    public:
        HDAHandler();

        HDAHandler(Address addr);

        void start();
    private:
        Rest::Router router;
        Http::Endpoint server;

        void getDashboard(const Rest::Request& request, Http::ResponseWriter response);

        void sendCommand(const Rest::Request& request, Http::ResponseWriter response);

        void getLastRefreshed(const Rest::Request& request, Http::ResponseWriter response);

        void init();
};