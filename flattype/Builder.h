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

#include <functional>

#include "accelerator/Range.h"
#include "flatbuffers/flatbuffers.h"

namespace ftt {

using FBB = ::flatbuffers::FlatBufferBuilder;

template <class FT>
using FBBCallback = std::function<flatbuffers::Offset<FT>(FBB*)>;

class Builder {
 public:
  Builder()
    : fbb_(new FBB()) {}
  explicit Builder(FBB* fbb, bool owns = false)
    : fbb_(fbb), owns_(owns) {}

  virtual ~Builder() {
    if (!owns_) {
      fbb_.release();
    }
  }

  Builder(const Builder&) = delete;
  Builder& operator=(const Builder&) = delete;

  Builder(Builder&&) = default;
  Builder& operator=(Builder&&) = default;

  virtual void finish() = 0;

  template <class FTW>
  FTW toWrapper() {
    finish();
    return FTW(detachedData());
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
  ::flatbuffers::DetachedBuffer detachedData() {
    return std::move(data_);
  }

 protected:
  std::unique_ptr<FBB> fbb_;
  bool owns_{true};
  bool finished_{false};

  ::flatbuffers::DetachedBuffer data_;
};

} // namespace ftt
