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

#include <filesystem>
#include <fstream>

#include "../src/proto_value.h"
#include "proto/event_predicate.pb.h"

using ::wireless_android_play_analytics::ProtoValue;
using ::wireless_android_play_analytics::EventPredicate;

int main() {
  std::ifstream ifs("demo/proto_texts/original_proto_text.txt");
  std::string text_proto;
  EventPredicate event_predicate;

  while (ifs.good()) {
    std::getline(ifs, text_proto);
  }

  std::unique_ptr<ProtoValue> message = ProtoValue::Create(text_proto, 
      event_predicate);
  
  std::cout << message.get()->PrintToTextProto();
  return 0;
}