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

#ifndef PRIMITIVE_VALUE_H_
#define PRIMITIVE_VALUE_H_
#include "proto_value.h";

namespace wireless_android_play_analytics {

class PrimitiveValue : public ProtoValue {

public:
    // Gets the value of the current Primitve.
    const absl::variant<double, float, int64_t, uint64_t, bool>& GetVal() const {
        return val_;
    }

    const std::string& GetValAsString() const {
        return val_as_string_;
    }

    void SetVal(const 
        absl::variant<double, float, int64_t, uint64_t, bool>& val) {
        val_ = val;
    }

    // Gets the value of the current primitive with intention to modify it.
    absl::variant<double, float, int64_t, uint64_t, bool>& GetValMutable();
private:

    std::string val_as_string_;
    absl::variant<double, float, int64_t, uint64_t, bool> val_;
};

} // namespace wireless_android_play_analytics

#endif // PRIMITIVE_VALUE_H