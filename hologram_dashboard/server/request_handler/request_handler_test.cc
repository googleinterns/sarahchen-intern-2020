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

namespace wireless_android_play_analytics {

// TEST(RequestHandlerTest, GetDashboardTest) {
//   Pistache::Http::Client client;
//   client.init();
//   Pistache::Http::Client::RequestBuilder request_builder = 
//       client.get("/Chipper");
//   Pistache::Http::Client::Async::Promise<Response> response = 
//       resquest_builder.send();
//   response.then(
//     (Pistache::Http::Response resp) {
//       std::cout << resp.body();
//     }
//   )
// }

} // namespace wireless_android_play_analytics

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}