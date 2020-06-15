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

#include <vector>
#include <sstream>

namespace wireless_android_play_analytics {

RequestHandler::RequestHandler()
    :server_(Pistache::Address(Pistache::Ipv4::any(), Pistache::Port(8000))) {
        Init();
}

RequestHandler::RequestHandler(Pistache::Address Addr)
    :server_(Addr) {
        Init();
}

void RequestHandler::Init() {
    server_.init();
    
    // Binds the router with correct request methods.
    Pistache::Rest::Routes::Get(router_, "/:system", 
        Pistache::Rest::Routes::bind (&RequestHandler::GetDashboard, this));
    Pistache::Rest::Routes::Get(router_, "/:system/:source/:date", 
        Pistache::Rest::Routes::bind
            (&RequestHandler::PopulateBackfillCommand, this));
    Pistache::Rest::Routes::Get(router_, "/", 
        Pistache::Rest::Routes::bind (&RequestHandler::GetLastRefreshed, this));

    server_.setHandler(router_.handler());
}

void RequestHandler::Start() {
    server_.serve();
}

void RequestHandler::GetLastRefreshed(const Pistache::Rest::Request& request, 
    Pistache::Http::ResponseWriter response) {
    // Information within the request is unnecessary.
    UNUSED(request);
    response.send(Pistache::Http::Code::Ok, "Wed May 19 15:46:11 2020");
}

void RequestHandler::GetDashboard(const Pistache::Rest::Request& request, 
    Pistache::Http::ResponseWriter response) {
    // TODO(alexanderlin): Use real data once I get access to them.
    std::vector<std::string> dataSources {"SPAM", "PLAY_COUNTRY", 
        "APP_COUNTRY_PUBLISH_TIME", "QUERY_CATEGORY_SOURCE", 
            "UNIFIED_USER_DATA_SOURCE"};
    std::vector<std::string> dates {"5/19/2020", "5/18/2020", "5/17/2020", "5/16/2020", 
        "5/15/2020", "5/14/2020", "5/13/2020"};
    std::string system_in = request.param(":system").as<std::string>();
    std::string status = "111010111110001011100100111000100110001010000010100111010";
    auto stream = response.stream(Pistache::Http::Code::Ok);
    const char* system = system_in.c_str();
    // track the online or offline bits
    int i = 0;

    // Since the html file may be large, stream the data as we write it for
    // more efficiency
    stream << "<table id=\"" << system << "%TEXT\">\n";
    stream << "\t<br/>\n\t<p>" << system << "</p>\n";

    for(const std::string &data : dataSources) {
        stream << "\t<tr>\n";
        stream << "\t\t<td>" << data.c_str() << "&nbsp&nbsp</td>\n";
        for(const std::string& date : dates) {
            stream << "\t\t<td class=";
            if (status[i++] == '1') {
                stream << "\"online\"";
            }
            else {
                stream << "\"offline\"";
            }
            stream << " title=\"" << date.c_str() << "\" id=\"" << data.c_str() 
                << "%" << date.c_str() << "%" << system << "\"></td>\n";
        }
        stream << "\t</tr>\n\n";
    }
    stream << "</table>\n";
    stream.ends();
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