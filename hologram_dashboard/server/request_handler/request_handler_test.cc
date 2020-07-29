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

#include <gtest/gtest.h>
#include <pistache/client.h>
#include <pistache/async.h>
#include "external/pistache/tests/httplib.h"
#include <chrono>

#include "request_handler.h"

namespace wireless_android_play_analytics {

const std::string expected_mock_time = "Wed May 19 15:46:11 2020";
const std::string expected_mock_injection_command = 
    "sample command to run backfill for SPAM on Chipper for 08-07-2020";
const std::string expected_message_string = 
    R"json({"SPAM":[["5/19/2020",true],["5/18/2020",true]]})json";

class RequestHandlerTest : public ::testing::Test {
 protected:
  RequestHandlerTest()
    : addr_(Pistache::Address(Pistache::Ipv4::any(), Pistache::Port(0))),
      end_point_(Pistache::Http::Endpoint(addr_)) { }

  virtual void SetUp() {
    Pistache::Rest::Routes::Get(router_, "/", 
        Pistache::Rest::Routes::bind (&RequestHandler::GetLastRefreshed, 
        &handler_));
    Pistache::Rest::Routes::Get(router_, "/:system/:source/:date", 
        Pistache::Rest::Routes::bind(&RequestHandler::PopulateBackfillCommand, 
        &handler_));
    Pistache::Rest::Routes::Get(router_, "/:system", 
        Pistache::Rest::Routes::bind(&RequestHandler::GetDashboard, &handler_));
    end_point_.init();
    end_point_.setHandler(router_.handler());
    end_point_.serveThreaded();
  }

  void TearDown() override {
    end_point_.shutdown();
  }

  Pistache::Address addr_;
  Pistache::Rest::Router router_;
  RequestHandler handler_;
  Pistache::Http::Endpoint end_point_;
};

TEST_F(RequestHandlerTest, GetTimeTest) {  
  httplib::Client client("localhost", end_point_.getPort());

  std::shared_ptr<httplib::Response> res = client.Get("/");

  EXPECT_EQ(res->body, expected_mock_time);
}

TEST_F(RequestHandlerTest, GetInjectionCommandTest) {  
  httplib::Client client("localhost", end_point_.getPort());

  std::shared_ptr<httplib::Response> res = client.Get("/Chipper/SPAM/08-07-2020");

  EXPECT_EQ(res->body, expected_mock_injection_command);
}

} // namespace wireless_android_play_analytics

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}