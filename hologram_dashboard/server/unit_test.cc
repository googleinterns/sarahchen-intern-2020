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

#include "request_handler/request_handler.h"
#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/peer.h>
#include <pistache/router.h>
#include <iostream>
#include <pistache/tests/httplib.h>
#include <gtest/gtest.h>
using namespace Pistache;
using namespace Pistache::Rest;

TEST(test_get_last_refreshed, last_refreshed_valid_input) {
    // Test acquiring the date
    httplib::Client client("localhost", Port(8000));
    auto res = client.Get("/");
    ASSERT_EQ(res->status, 200);
    ASSERT_EQ(res->body, "Wed May 19 15:46:11 2020");
}

TEST(test_get_dashboard, dashboard_valid_input) {
    httplib::Client client("localhost", Port(8000));
    auto res = client.Get("/CHIPPER");
    std::ifstream ifs("test.html");
    std::string correct_html;
    while(ifs.good()) {
        getline(ifs, correct_html);
        correct_html += "\n";
    }
    ASSERT_EQ(res->status, 200);
    ASSERT_EQ(res->body, correct_html);
}

TEST(test_get_command, command_valid_input) {
    httplib::Client client("localhost", Port(8000));
    auto res = client.Get("/CHIPPER/SPAM/5-2-2020");
    std::string correct_html;
    correct_html = "sample command to run backfill or SPAM on CHIPPER for 5-2-2020");
    ASSERT_EQ(res->status, 200);
    ASSERT_EQ(res->body, correct_html);
}