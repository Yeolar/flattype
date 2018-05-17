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

class Matrix : public Wrapper<fbs::Matrix> {
 public:
  Matrix(const fbs::Matrix* matrix) : Wrapper(matrix) {}

  explicit Matrix(const uint8_t* data)
    : Wrapper(data) {}
  explicit Matrix(::flatbuffers::DetachedBuffer&& data)
    : Wrapper(std::move(data)) {}

  Matrix(const Matrix&) = delete;
  Matrix& operator=(const Matrix&) = delete;

  Matrix(Matrix&&) = default;
  Matrix& operator=(Matrix&&) = default;

  std::string toDebugString() const override;

  size_t getRowCount() const;
  size_t getColCount() const;

  const fbs::Record* getRow(size_t i) const;
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
Matrix::getRowCount() const {
  return ftt::size(ptr_);
}

inline size_t
Matrix::getColCount() const {
  auto row = getRow(0);
  return row ? ftt::size(row) : 0;
}

inline const fbs::Record*
Matrix::getRow(size_t i) const {
  return i < ftt::size(ptr_) ? ftt::at(ptr_, i) : nullptr;
}

inline const fbs::Item*
Matrix::getItem(size_t i, size_t j) const {
  auto row = getRow(i);
  if (row) {
    return j < ftt::size(row) ? ftt::at(row, j) : nullptr;
  }
  return nullptr;
}

template <class... Args>
inline bool
Matrix::getRowValue(size_t i, Args&... args) const {
  auto row = getRow(i);
  if (row) {
    decode(row, args...);
    return true;
  }
  return false;
}

template <class... Args>
inline bool
Matrix::getColValue(size_t j, Args&... args) const {
  if (j < getColCount()) {
    decodeOne(ptr_, j, args...);
    return true;
  }
  return false;
}

template <class T>
inline bool
Matrix::getItemValue(size_t i, size_t j, T& value) const {
  auto item = getItem(i, j);
  if (item) {
    decode(item, value);
    return true;
  }
  return false;
}

inline fbs::Any
Matrix::getItemType(size_t i, size_t j) const {
  auto item = getItem(i, j);
  return item ? item->value_type() : fbs::Any::NONE;
}

} // namespace ftt
