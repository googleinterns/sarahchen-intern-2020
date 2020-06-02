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

class RequestHandler {
    public:
        /*
        * Requires: request is a valid request of form /{system}
        * Modifies: response
        * Effect: Constructs the html code for the dashboard of the specific
        *         system
        */
        void getDashboard(const Rest::Request& request, 
            Http::ResponseWriter response);

        /*
        * Requires: request is a valid request of form /{system}/{source}/{date}
        * Modifies: response
        * Effect: Constructs a command to backfill a specified pipe and send said
        *         command to the user
        */
        void sendCommand(const Rest::Request& request, 
            Http::ResponseWriter response);

        /*
        * Requires: request is a valid request of form /
        * Modifies: response
        * Effect: Acquires the latest refresh time and send it to the user
        */
        void getLastRefreshed(const Rest::Request& request, 
            Http::ResponseWriter response);
};

class DashboardServer {
    public: 
        /*
        * Requires: Nothing
        * Modifies: router, server
        * Effect: Constructs a request handler using default port of 8000
        */
        DashboardServer();

        /*
        * Requires: Addr is a valid address
        * Modifies: router, server
        * Effect: Constructs a request handler using user defined address
        */
        DashboardServer(Address addr);

        /*
        * Requires: nothing
        * Modifies: router, server
        * Effect: Starts servicing requests from the server
        */        
        void start();
    private:
        Rest::Router router;
        Http::Endpoint server;

        /*
        * Requires: router is initialized to a port
        * Modifies: router, server
        * Effect: Binds the request methods to the router and initialize the
        *         server to be ready to start serving
        */
        void init();
};