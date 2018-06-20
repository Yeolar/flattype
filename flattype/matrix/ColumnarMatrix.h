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
#include "flattype/Util.h"
#include "flattype/Wrapper.h"
#include "flattype/matrix/Encoding.h"

namespace ftt {

class ColumnarMatrix : public Wrapper<fbs::Matrix> {
 public:
  ColumnarMatrix(const fbs::Matrix* matrix) : Wrapper(matrix) {}

  explicit ColumnarMatrix(const uint8_t* data)
    : Wrapper(data) {}
  explicit ColumnarMatrix(::flatbuffers::DetachedBuffer&& data)
    : Wrapper(std::move(data)) {}

  ColumnarMatrix(const ColumnarMatrix&) = delete;
  ColumnarMatrix& operator=(const ColumnarMatrix&) = delete;

  ColumnarMatrix(ColumnarMatrix&&) = default;
  ColumnarMatrix& operator=(ColumnarMatrix&&) = default;

  std::string toDebugString() const override;

  size_t getRowCount() const;
  size_t getColCount() const;

  const fbs::Record* getCol(size_t j) const;
  const fbs::Item* getItem(size_t i, size_t j) const;

  template <class... Args>
  bool getRowValue(size_t i, Args&... args) const;
  template <class... Args>
  bool getColValue(size_t j, Args&... args) const;
  template <class T>
  bool getItemValue(size_t i, size_t j, T& value) const;

  fbs::Any getItemType(size_t i, size_t j) const;
};

//////////////////////////////////////////////////////////////////////

inline size_t
ColumnarMatrix::getRowCount() const {
  auto col = getCol(0);
  return col ? ftt::size(col) : 0;
}

inline size_t
ColumnarMatrix::getColCount() const {
  return ftt::size(ptr_);
}

inline const fbs::Record*
ColumnarMatrix::getCol(size_t j) const {
  return j < ftt::size(ptr_) ? ftt::at(ptr_, j) : nullptr;
}

inline const fbs::Item*
ColumnarMatrix::getItem(size_t i, size_t j) const {
  auto col = getCol(j);
  if (col) {
    return i < ftt::size(col) ? ftt::at(col, i) : nullptr;
  }
  return nullptr;
}

template <class... Args>
inline bool
ColumnarMatrix::getRowValue(size_t i, Args&... args) const {
  if (i < getRowCount()) {
    decodeOne(ptr_, i, args...);
    return true;
  }
  return false;
}

template <class... Args>
inline bool
ColumnarMatrix::getColValue(size_t j, Args&... args) const {
  auto col = getCol(j);
  if (col) {
    decode(col, args...);
    return true;
  }
  return false;
}

template <class T>
inline bool
ColumnarMatrix::getItemValue(size_t i, size_t j, T& value) const {
  auto item = getItem(i, j);
  if (item) {
    decode(item, value);
    return true;
  }
  return false;
}

inline fbs::Any
ColumnarMatrix::getItemType(size_t i, size_t j) const {
  auto item = getItem(i, j);
  return item ? item->value_type() : fbs::Any::NONE;
}

} // namespace ftt
