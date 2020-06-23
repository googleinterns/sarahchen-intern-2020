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

#include "proto_value.h"

namespace wireless_android_play_analytics {

std::unique_ptr<ProtoValue> ProtoValue::Create(absl::string_view text_proto, 
    google::protobuf::Descriptor* descriptor) {
    // TODO(alexanderlin): Implement.
    return nullptr;
}

std::string ProtoValue::PrintToTextProto() {
    return PrintToTextProtoHelper(0);
}

std::string ProtoValue::PrintToTextProtoHelper(int indent_count) {
    // TODO(alexanderlin): Implement.
    return std::string();
}

std::unique_ptr<ProtoValue> ProtoValue::ProtoParser::ParseVal(
    const google::protobuf::Message& message, int row_start, int row_end, 
    int index) {
    // TODO(alexanderlin): Implement.
    return nullptr;
}

std::unique_ptr<ProtoValue> ProtoValue::ProtoParser::ParseNested (
    const google::protobuf::Message& message, 
    google::protobuf::TextFormat::ParseInfoTree* nested_tree, 
    google::protobuf::FieldDescriptor* field_descriptor, int row_start, 
    int& row_end) {
    // TODO(alexanderlin): Implement.
    return nullptr;
}

} // namespace wireless_android_play_analytics