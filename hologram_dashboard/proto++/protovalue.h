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

#pragma once
#include <iostream>
#include <gtest/gtest.h>
#include <assert.h>
#include <fstream>
#include <google/protobuf/text_format.h>
#include <vector>
#include <memory>
#include "absl/types/variant.h"

namespace wireless_android_play_analytics {

class ProtoValue {

public:
    // The only place where name can be set publicly since once set it should
    // not be changed.
    ProtoValue(const std::string& name_in);
    
    static std::unique_ptr<ProtoValue> Create(std::string& text_proto, 
    google::protobuf::Descriptor* descriptor);

    // Gets the name of current variable.
    const std::string& GetName();

    // Gets the comment above current field.
    const std::string& GetCommentAboveField();

    // Gets the comment behind current field.
    const std::string& GetCommentBehindField();

    // Sets the comment behind current field.
    void SetCommentBehindField(const std::string& val); 

    // Sets the comment behind current field.
    void SetCommentAboveField(const std::string& val);
    // Outputs content of this ProtoValue and all its nested types as a string 
    // including any comments.
    std::string PrintToTextProto(int indent_count = 0);

    // Need at least one virtual function to make these classes polymorphic
    virtual ~ProtoValue() = default;
private:
    std::string comment_above_field_;
    std::string comment_behind_field_;
    std::string name_;

    // Private interface for parsing a text_proto.
    class ProtoParser {

            google::protobuf::TextFormat::Parser Parser_;
            std::vector<std::string> lines;
            // Creates a PrimitiveValue.
            static std::unique_ptr<ProtoValue> ParseVal(
                google::protobuf::FieldDescriptor* field_descriptor, 
                google::protobuf::Reflection* reflection, int row_start, 
                int row_end, int index = -1);
            // Creates a MessageValue.
            static std::unique_ptr<ProtoValue> ParseNested (
                google::protobuf::Descriptor* descriptor, 
                google::protobuf::Reflection* reflection, 
                google::protobuf::TextFormat::ParseInfoTree* nested_tree, 
                google::protobuf::FieldDescriptor* field_descriptor, 
                int row_start, int& row_end);
    };
};

class MessageValue : public ProtoValue {

public:
    // Gets all the fields of the current Message
    const std::vector<std::unique_ptr<ProtoValue>>& GetFields();

    // Gets all the fields of the current Message with intention to modify it.
    std::vector<std::unique_ptr<ProtoValue>>& GetFieldsMutable();
private:
    std::vector<std::unique_ptr<ProtoValue>> fields_;
};

class PrimitiveValue : public ProtoValue {

public:
    // Gets the value of the current Primitve.
    const absl::variant<double, float, int64_t, uint64_t, bool>& GetVal();

    // Gets the value of the current primitive with intention to modify it.
    absl::variant<double, float, int64_t, uint64_t, bool>& GetValMutable();
private:
    std::string val_as_string_;
    absl::variant<double, float, int64_t, uint64_t, bool> val_;
};


} // namespace wireless_android_play_analytics