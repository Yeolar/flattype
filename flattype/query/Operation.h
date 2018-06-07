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

#include "flattype/CommonIDLs.h"
#include "flattype/Wrapper.h"

namespace ftt {

class Operation : public Wrapper<fbs::Operation> {
 public:
  Operation(const fbs::Operation* operation) : Wrapper(operation) {}

  explicit Operation(const uint8_t* data)
    : Wrapper(data) {}
  explicit Operation(::flatbuffers::DetachedBuffer&& data)
    : Wrapper(std::move(data)) {}

  Operation(const Operation&) = delete;
  Operation& operator=(const Operation&) = delete;

  Operation(Operation&&) = default;
  Operation& operator=(Operation&&) = default;

  std::string toDebugString() const override;

  fbs::Op getCmd() const;
  const fbs::Tuple* getParams() const;

  template <class... Args>
  void getParams(Args&... args) const;
};

template <class... Args>
void Operation::getParams(Args&... args) const {
  if (ptr_) {
    vdecode(ptr_->params(), args...);
  }
}

} // namespace ftt
