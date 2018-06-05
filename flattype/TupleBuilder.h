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

#include "flattype/Builder.h"
#include "flattype/CommonIDLs.h"
#include "flattype/Copy.h"
#include "flattype/Tuple.h"

namespace ftt {

class TupleBuilder : public Builder {
 public:
  TupleBuilder() : Builder() {}

  explicit TupleBuilder(FBB* fbb, bool owns = false)
    : Builder(fbb, owns) {}

  TupleBuilder(const TupleBuilder&) = delete;
  TupleBuilder& operator=(const TupleBuilder&) = delete;

  TupleBuilder(TupleBuilder&&) = default;
  TupleBuilder& operator=(TupleBuilder&&) = default;

  size_t getCount() const;

  const void* getItem(size_t i) const;
  void setItem(size_t i, fbs::Any type, const void* item);

  template <class... Args>
  bool getValue(Args&... args) const;
  template <class... Args>
  bool setValue(const Args&... args);

  template <class T>
  bool getItemValue(size_t i, T& value) const;
  template <class T>
  bool setItemValue(size_t i, const T& value);

  fbs::Any getItemType(size_t i) const;

  void finish() override;

  Tuple toTuple() { return toWrapper<Tuple>(); }

 private:
  void resize(size_t i);

  std::vector<uint8_t> types_;
  std::vector<flatbuffers::Offset<void>> items_;
};

//////////////////////////////////////////////////////////////////////

inline void TupleBuilder::resize(size_t i) {
  if (i >= items_.size()) {
    types_.resize(i + 1);
    items_.resize(i + 1);
  }
}

inline size_t
TupleBuilder::getCount() const {
  return items_.size();
}

inline const void*
TupleBuilder::getItem(size_t i) const {
  if (i < getCount()) {
    return ::flatbuffers::GetTemporaryPointer(*fbb_, items_[i]);
  }
  return nullptr;
}

inline void
TupleBuilder::setItem(size_t i, fbs::Any type, const void* item) {
  assert(item != nullptr);
  resize(i);
  types_[i] = acc::to<uint8_t>(type);
  items_[i] = copy(*fbb_, type, item);
}

template <class... Args>
inline bool
TupleBuilder::getValue(Args&... args) const {
  detail::vdecodeImpl<0>(*fbb_, types_, items_, args...);
  return true;
}

template <class... Args>
inline bool
TupleBuilder::setValue(const Args&... args) {
  detail::vencodeImpl(*fbb_, types_, items_, args...);
  return true;
}

template <class T>
inline bool
TupleBuilder::getItemValue(size_t i, T& value) const {
  auto item = getItem(i);
  if (item) {
    decode(item, value);
    return true;
  }
  return false;
}

template <class T>
inline bool
TupleBuilder::setItemValue(size_t i, const T& value) {
  resize(i);
  types_[i] = getAnyType<T>();
  items_[i] = encode(*fbb_, value).Union();
  return true;
}

inline fbs::Any
TupleBuilder::getItemType(size_t i) const {
  return i < getCount() ? acc::to<fbs::Any>(types_[i]) : fbs::Any::NONE;
}

} // namespace ftt
