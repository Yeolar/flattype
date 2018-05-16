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

#pragma once

#include "flattype/idl/message_generated.h"
#include "flattype/Wrapper.h"

namespace ftt {

class Message : public Wrapper<fbs::Message> {
 public:
  Message(const fbs::Message* message) : Wrapper(message) {}

  explicit Message(const uint8_t* data)
    : Wrapper(data) {}
  explicit Message(::flatbuffers::DetachedBuffer&& data)
    : Wrapper(std::move(data)) {}

  Message(const Message&) = delete;
  Message& operator=(const Message&) = delete;

  Message(Message&&) = default;
  Message& operator=(Message&&) = default;

  std::string toDebugString() const override;

  int getCode() const;
  std::string getMessage() const;
  const fbs::Object* getJData() const;
  const fbs::Tuple* getVData() const;
  const fbs::Table* getXData() const;
};

} // namespace ftt
