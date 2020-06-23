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

#ifndef PROTO_VALUE_H_
#define PROTO_VALUE_H_
#include <gtest/gtest.h>
#include <assert.h>
#include <google/protobuf/text_format.h>
#include <vector>
#include <memory>
#include "absl/types/variant.h"
#include "absl/strings/string_view.h"

namespace wireless_android_play_analytics {

class ProtoValue {

public:
    // The only place where name can be set publicly since once set it should
    // not be changed.
    explicit ProtoValue(absl::string_view field_name) 
        : field_name_(field_name) {}

    virtual ~ProtoValue() = default;
    
    static std::unique_ptr<ProtoValue> Create(absl::string_view text_proto, 
    google::protobuf::Descriptor* descriptor);

    // Gets the name of current variable.
    const std::string& GetName() const {
        return field_name_;
    }

    // Gets the comment above current field.
    const std::string& GetCommentAboveField() const {
        return comment_above_field_;
    }

    // Gets the comment behind current field.
    const std::string& GetCommentBehindField() const {
        return comment_behind_field_;
    }

    // Sets the comment behind current field.
    void SetCommentBehindField(absl::string_view val) {
        comment_behind_field_ = val;
    }

    // Sets the comment behind current field.
    void SetCommentAboveField(absl::string_view val) {
        comment_above_field_ = val;
    }

    // Outputs content of this ProtoValue and all its nested types as a string 
    // including any comments.
    std::string PrintToTextProto();
private:

    std::string comment_above_field_;
    std::string comment_behind_field_;
    std::string field_name_;

    // Helper function to print text proto in order to abstract indent_count
    // from the user.
    std::string PrintToTextProtoHelper(int indent_count = 0);

    // Private interface for parsing a text_proto.
    class ProtoParser {

    public:
        // Creates a PrimitiveValue.
        static std::unique_ptr<ProtoValue> ParseVal(
            const google::protobuf::Message& message, int row_start, 
            int row_end, int index = -1);
        // Creates a MessageValue.
        static std::unique_ptr<ProtoValue> ParseNested (
            const google::protobuf::Message& message, 
            google::protobuf::TextFormat::ParseInfoTree* const nested_tree, 
            google::protobuf::FieldDescriptor* const field_descriptor, 
            int row_start, int& row_end);
    private:
        google::protobuf::TextFormat::Parser Parser_;
        std::vector<std::string> lines_;
    };
};

} // namespace wireless_android_play_analytics

#endif // PROTO_VALUE_H_