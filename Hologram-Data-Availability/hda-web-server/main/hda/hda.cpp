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
#include "hda.h"
#include <vector>
#include <sstream>
using namespace Pistache;
using namespace std;

HDAHandler::HDAHandler()
    :server(Address(Ipv4::any(), Port(8000))) {
        init();
}

HDAHandler::HDAHandler(Address Addr)
    :server(Addr) {
        init();
}

void HDAHandler::init() {
    using namespace Rest;
    server.init();
    
    Routes::Get(router, "/:system", 
        Routes::bind(&HDAHandler::getDashboard, this));
    Routes::Get(router, "/:system/:source/:date", 
        Routes::bind(&HDAHandler::sendCommand, this));
    Routes::Get(router, "/", 
        Routes::bind(&HDAHandler::getLastRefreshed, this));

    server.setHandler(router.handler());
}

void HDAHandler::start() {
    server.serve();
}

void HDAHandler::getLastRefreshed(const Rest::Request& request, 
    Http::ResponseWriter response) {
    UNUSED(request);
    response.send(Http::Code::Ok, "Wed May 19 15:46:11 2020");
}

void HDAHandler::getDashboard(const Rest::Request& request, 
    Http::ResponseWriter response) {
    vector<string> dataSources {"SPAM", "PLAY_COUNTRY", 
        "APP_COUNTRY_PUBLISH_TIME", "QUERY_CATEGORY_SOURCE", 
            "UNIFIED_USER_DATA_SOURCE"};
    vector<string> dates {"5/19/2020", "5/18/2020", "5/17/2020", "5/16/2020", 
        "5/15/2020", "5/14/2020", "5/13/2020"};
    string system_in = request.param(":system").as<string>();
    auto stream = response.stream(Http::Code::Ok);
    const char* system = system_in.c_str();

    stream << "<div id=\"" << system << "_TEXT\">\n";
    stream << "\t<br/>\n\t<p>" << system << "</p>\n";

    for(const string &data : dataSources) {
        stream << "\t<div class=\"inline\">\n";
        stream << "\t\t<span>" << data.c_str() << "&nbsp&nbsp</span>\n";
        for(const string& date : dates) {
            stream << "\t\t<div class=";
            if (rand() % 2) {
                stream << "\"online\"";
            }
            else {
                stream << "\"offline\"";
            }
            stream << " title=\"" << date.c_str() << "\" id=\"" << data.c_str() << "%" << date.c_str() 
                << "%" << system << "\"></div>\n";
        }
        stream << "\t</div>\n\n";
    }
    stream.ends();
}

void HDAHandler::sendCommand(const Rest::Request& request, 
    Http::ResponseWriter response) {
        string date = request.param(":date").as<string>();
        string system = request.param(":system").as<string>();
        string source = request.param(":source").as<string>();
        string command = "sample command to run backfill for " + source 
            + " on " + system + " for " + date;
        response.send(Http::Code::Ok, command);
}