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
#include "../src/proto_parser.h"
#include "proto/event_predicate.pb.h"

using ::wireless_android_play_analytics::ProtoValue;
using ::wireless_android_play_analytics::MessageValue;
using ::wireless_android_play_analytics::PrimitiveValue;
using ::wireless_android_play_analytics::EventPredicate;
using ::wireless_android_play_analytics::ProtoParser;

namespace {

std::string Generate(MessageValue* message) {
  const std::vector<ProtoValue*>& message_fields = message->GetFieldsMutable();
  absl::variant<double, float, int, unsigned int, int64_t, uint64_t, bool, 
      const google::protobuf::EnumValueDescriptor*, std::string> val;

  // Generate random int_comparator.
  MessageValue* event_matcher = static_cast<MessageValue*>(message_fields[0]);
  if (rand() % 2) {
    std::unique_ptr<ProtoValue> int_comparator = 
        absl::make_unique<PrimitiveValue>("int_comparator", 1, 8);
    
    PrimitiveValue* int_comparator_ptr = 
        static_cast<PrimitiveValue*>(int_comparator.get());

    // Generate comment
    if (rand() % 2) {
      int_comparator_ptr->SetCommentBehindField("# randomly generated comment");
    }
    std::vector<std::string> comments_above_field;
    int count = 0;
    while(rand() % 2) {
      comments_above_field.push_back("# randomly generated comment " + 
          std::to_string(count++));
    }
    int_comparator_ptr->SetCommentAboveField(comments_above_field);

    int input = rand() % 1000;
    val = input;
    int_comparator_ptr->SetVal(val);

    event_matcher->AddField(std::move(int_comparator));
  }

  // Randomly erase leaf_ui_predicates.
  MessageValue* ui_element_path_predicate = static_cast<MessageValue*>(
      message_fields[2]);
  if (rand() % 2) {
    ui_element_path_predicate->DeleteField(2);
  }
  if (rand() % 2) {
    ui_element_path_predicate->DeleteField(1);
  }
  
  return message->PrintToTextProto();
}

} // namespace

int main() {
  std::ifstream ifs;
  std::string text_proto;
  EventPredicate event_predicate;

  ifs.open("demo/proto_texts/original_proto_text.txt");

  assert(ifs.good());

  while (ifs.good()) {
    std::string tmp;
    std::getline(ifs, tmp);
    text_proto += tmp + "\n";
  }

  ProtoParser parser(text_proto);

  for(int i = 0; i < 10; ++i) {
    std::unique_ptr<ProtoValue> message = parser.Create(event_predicate);
    std::ofstream outs;
    outs.open("demo/proto_texts/proto_text(" + std::to_string(i) +").txt");
    outs << Generate(static_cast<MessageValue*>(message.get()));
  }

  return 0;
}