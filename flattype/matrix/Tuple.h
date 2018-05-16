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

#include "flattype/idl/matrix_generated.h"
#include "flattype/Util.h"
#include "flattype/Wrapper.h"
#include "flattype/matrix/Encoding.h"

namespace ftt {

class Tuple : public Wrapper<fbs::Tuple> {
 public:
  Tuple(const fbs::Tuple* tuple) : Wrapper(tuple) {}

  explicit Tuple(const uint8_t* data)
    : Wrapper(data) {}
  explicit Tuple(::flatbuffers::DetachedBuffer&& data)
    : Wrapper(std::move(data)) {}

  Tuple(const Tuple&) = delete;
  Tuple& operator=(const Tuple&) = delete;

  Tuple(Tuple&&) = default;
  Tuple& operator=(Tuple&&) = default;

  std::string toDebugString() const override;

  size_t getCount() const;

  const void* getItem(size_t i) const;

  template <class... Args>
  bool getValue(Args&... args) const;
  template <class T>
  bool getItemValue(size_t i, T& value) const;

  fbs::Any getItemType(size_t i) const;
};

//////////////////////////////////////////////////////////////////////

inline size_t
Tuple::getCount() const {
  return ftt::size(ptr_);
}

inline const void*
Tuple::getItem(size_t i) const {
  return i < ftt::size(ptr_) ? ftt::at(ptr_, i) : nullptr;
}

template <class... Args>
inline bool
Tuple::getValue(Args&... args) const {
  decode(ptr_, args...);
  return true;
}

template <class T>
inline bool
Tuple::getItemValue(size_t i, T& value) const {
  auto item = getItem(i);
  if (item) {
    decode(item, value);
    return true;
  }
  return false;
}

inline fbs::Any
Tuple::getItemType(size_t i) const {
  return i < ftt::size(ptr_) ? decodeOneType(ptr_, i) : fbs::Any_NONE;
}

} // namespace ftt
