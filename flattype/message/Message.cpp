/*
 * Copyright 2018 Yeolar
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "accelerator/Conv.h"
#include "flattype/message/Message.h"
#include "flattype/object/dynamic.h"
#include "flattype/table/Table.h"

namespace ftt {

std::string Message::toDebugString() const {
  std::string out;
  if (!get()) {
    return "Message::<NULL>";
  }
  acc::toAppend("Message::",
                ", code:", getCode(),
                ", message:", getMessage(),
                ", jdata:", toPseudoJson(dynamic(fbs::Json_Object, getJData())),
                ", xdata:", Table(getXData()).toDebugString(),
                &out);
  return out;
}

int Message::getCode() const {
  return ptr_ ? ptr_->code() : 0;
}

std::string Message::getMessage() const {
  return ptr_ ? ptr_->message()->str() : "";
}

const fbs::Object* Message::getJData() const {
  return ptr_ ? ptr_->jdata() : nullptr;
}

const fbs::Table* Message::getXData() const {
  return ptr_ ? ptr_->xdata() : nullptr;
}

} // namespace ftt
