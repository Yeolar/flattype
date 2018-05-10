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

#include "accelerator/Range.h"
#include "flatbuffers/flatbuffers.h"

namespace ftt {

template <class FT>
class Wrapper {
 public:
  Wrapper(const FT* ptr) : ptr_(ptr) {}

  explicit Wrapper(const uint8_t* data)
    : ptr_(data ? ::flatbuffers::GetRoot<FT>(data) : nullptr) {}
  explicit Wrapper(::flatbuffers::DetachedBuffer&& data)
    : ptr_(data.data() ? ::flatbuffers::GetRoot<FT>(data.data()) : nullptr),
      data_(std::move(data)) {}

  virtual ~Wrapper() {}

  Wrapper(const Wrapper&) = delete;
  Wrapper& operator=(const Wrapper&) = delete;

  Wrapper(Wrapper&&) = default;
  Wrapper& operator=(Wrapper&&) = default;

  explicit operator bool() const {
    return ptr_ != nullptr;
  }
  const FT* get() const {
    return ptr_;
  }
  const FT* operator->() const {
    return ptr_;
  }
  const FT& operator*() const {
    return *ptr_;
  }
  const uint8_t* data() const {
    return data_.data();
  }
  size_t size() const {
    return data_.size();
  }
  acc::ByteRange range() const {
    return acc::ByteRange(data(), size());
  }
  std::string toString() const {
    return range().toString();
  }

  virtual std::string toDebugString() const = 0;

 protected:
  const FT* ptr_{nullptr};
  ::flatbuffers::DetachedBuffer data_;
};

} // namespace ftt
