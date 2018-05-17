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
#include "flattype/matrix/Copy.h"
#include "flattype/matrix/Matrix.h"

namespace ftt {

class MatrixBuilder : public Builder {
  using Item = ::flatbuffers::Offset<fbs::Item>;

 public:
  MatrixBuilder() : Builder() {}

  explicit MatrixBuilder(FBB* fbb, bool owns = false)
    : Builder(fbb, owns) {}

  MatrixBuilder(const MatrixBuilder&) = delete;
  MatrixBuilder& operator=(const MatrixBuilder&) = delete;

  MatrixBuilder(MatrixBuilder&&) = default;
  MatrixBuilder& operator=(MatrixBuilder&&) = default;

  size_t getRowCount() const;
  size_t getColCount() const;

  const fbs::Item* getItem(size_t i, size_t j) const;
  void setItem(size_t i, size_t j, const fbs::Item* item);

  template <class... Args>
  bool getRowValue(size_t i, Args&... args) const;
  template <class... Args>
  bool setRowValue(size_t i, const Args&... args);

  template <class... Args>
  bool getColValue(size_t j, Args&... args) const;
  template <class... Args>
  bool setColValue(size_t j, const Args&... args);

  template <class T>
  bool getItemValue(size_t i, size_t j, T& value) const;
  template <class T>
  bool setItemValue(size_t i, size_t j, const T& value);

  fbs::Any getItemType(size_t i, size_t j) const;

  void finish() override;

  Matrix toMatrix() { return toWrapper<Matrix>(); }

 private:
  void resize(size_t i, size_t j);

  std::vector<std::vector<Item>> records_;
};

//////////////////////////////////////////////////////////////////////

inline void MatrixBuilder::resize(size_t i, size_t j) {
  if (i >= records_.size()) {
    records_.resize(i + 1);
  }
  if (j >= records_[i].size()) {
    records_[i].resize(j + 1);
  }
}

inline size_t
MatrixBuilder::getRowCount() const {
  return records_.size();
}

inline size_t
MatrixBuilder::getColCount() const {
  return records_.empty() ? 0 : records_.front().size();
}

inline const fbs::Item*
MatrixBuilder::getItem(size_t i, size_t j) const {
  if (i < getRowCount() && j < getColCount()) {
    return ::flatbuffers::GetTemporaryPointer(*fbb_, records_[i][j]);
  }
  return nullptr;
}

inline void
MatrixBuilder::setItem(size_t i, size_t j, const fbs::Item* item) {
  assert(item != nullptr);
  resize(i, j);
  records_[i][j] = copy(*fbb_, *item);
}

template <class... Args>
inline bool
MatrixBuilder::getRowValue(size_t i, Args&... args) const {
  if (i < getRowCount()) {
    vdecodeItems(*fbb_, records_[i], args...);
    return true;
  }
  return false;
}

template <class... Args>
inline bool
MatrixBuilder::setRowValue(size_t i, const Args&... args) {
  resize(i, 0);
  records_[i].clear();
  vencodeItems(*fbb_, records_[i], args...);
  return true;
}

template <class... Args>
inline bool
MatrixBuilder::getColValue(size_t j, Args&... args) const {
  if (j < getColCount()) {
    vvdecodeItems(*fbb_, records_, j, args...);
    return true;
  }
  return false;
}

template <class... Args>
inline bool
MatrixBuilder::setColValue(size_t j, const Args&... args) {
  resize(sizeof...(Args), 0);
  vvencodeItems(*fbb_, records_, j, args...);
  return true;
}

template <class T>
inline bool
MatrixBuilder::getItemValue(size_t i, size_t j, T& value) const {
  auto item = getItem(i, j);
  if (item) {
    decode(item, value);
    return true;
  }
  return false;
}

template <class T>
inline bool
MatrixBuilder::setItemValue(size_t i, size_t j, const T& value) {
  resize(i, j);
  auto item = encode(*fbb_, value);
  records_[i][j] = fbs::CreateItem(*fbb_, getAnyType<T>(), item.Union());
  return true;
}

inline fbs::Any
MatrixBuilder::getItemType(size_t i, size_t j) const {
  auto item = getItem(i, j);
  return item ? item->value_type() : fbs::Any::NONE;
}

} // namespace ftt
