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
#include <vector>
#include <sstream>

using namespace Pistache;
using namespace std;

class HandlerUnitTest: public RequestHandler {
    public:
        void run() {
            cout << "testing getDashboard\n";
            test_getDashboard();
            cout << "testing sendCommand\n";
            test_sendCommand();
            cout << "testing getLastRefreshed\n";
            test_getLastRefreshed();
        }
        void test_getDashboard() {

        }
        void test_sendCommand() {

        }
        void test_getLastRefreshed() {
            
        }
};

int main() {
    return 0;
}