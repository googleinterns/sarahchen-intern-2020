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

#ifndef MESSAGE_VALUE_H_
#define MESSAGE_VALUE_H_
#include "proto_value.h";

namespace wireless_android_play_analytics {

class MessageValue : public ProtoValue {

public:
    // Gets all the fields of the current Message
    const std::vector<std::unique_ptr<ProtoValue>>& GetFields() const {
        return fields_;
    }

    // Gets all the fields of the current Message with intention to modify it.
    std::vector<std::unique_ptr<ProtoValue>>& GetFieldsMutable() {
        return fields_;
    }
private:

    std::vector<std::unique_ptr<ProtoValue>> fields_;
};

} // namespace wireless_android_play_analytics

#endif // MESSAGE_VALUE_H_