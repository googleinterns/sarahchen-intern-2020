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

#include "protovalue.h"

namespace wireless_android_play_analytics {

ProtoValue::ProtoValue(const std::string& name_in)
    : name_(name_in) {}

std::unique_ptr<ProtoValue> ProtoValue::Create(std::string& text_proto, 
    google::protobuf::Descriptor* descriptor) {
    // TODO(alexanderlin): Implement.
    return nullptr;
}

const std::string& ProtoValue::GetCommentAboveField() {
    return comment_above_field_;
}

const std::string& ProtoValue::GetName() {
    return name_;
}

const std::string& ProtoValue::GetCommentBehindField() {
    return comment_behind_field_;
}
    
void ProtoValue::SetCommentBehindField(const std::string& val) {
    comment_behind_field_ = val;
}

void ProtoValue::SetCommentAboveField(const std::string& val) {
    comment_above_field_ = val;
}

std::string PrintToTextProto(int indent_count = 0) {
    // TODO(alexanderlin): Implement.
    return std::string();
}

std::unique_ptr<ProtoValue> ProtoValue::ProtoParser::ParseVal(
    google::protobuf::FieldDescriptor* field_descriptor, 
    google::protobuf::Reflection* reflection, int row_start, int row_end, 
    int index) {
    // TODO(alexanderlin): Implement.
    return nullptr;
}

std::unique_ptr<ProtoValue> ProtoValue::ProtoParser::ParseNested (
    google::protobuf::Descriptor* descriptor, 
    google::protobuf::Reflection* reflection, 
    google::protobuf::TextFormat::ParseInfoTree* nested_tree, 
    google::protobuf::FieldDescriptor* field_descriptor, int row_start, 
    int& row_end) {
    // TODO(alexanderlin): Implement.
    return nullptr;
}


const std::vector<std::unique_ptr<ProtoValue>>& MessageValue::GetFields() {
    return fields_;
}
    
std::vector<std::unique_ptr<ProtoValue>>& MessageValue::GetFieldsMutable() {
    return fields_;
}

const absl::variant<double, float, int64_t, uint64_t, bool>& PrimitiveValue::GetVal() {
    return val_;
}

absl::variant<double, float, int64_t, uint64_t, bool>& PrimitiveValue::GetValMutable() {
    return val_;
}


} // namespace wireless_android_play_analytics