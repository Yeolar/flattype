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

class Bucket : public Wrapper<fbs::Bucket> {
 public:
  static constexpr uint8_t npos = std::numeric_limits<uint8_t>::max();

  Bucket(const fbs::Bucket* bucket) : Wrapper(bucket) {}

  explicit Bucket(const uint8_t* data)
    : Wrapper(data) {}
  explicit Bucket(::flatbuffers::DetachedBuffer&& data)
    : Wrapper(std::move(data)) {}

  Bucket(const Bucket&) = delete;
  Bucket& operator=(const Bucket&) = delete;

  Bucket(Bucket&&) = default;
  Bucket& operator=(Bucket&&) = default;

  std::string toDebugString() const override;

  uint8_t getBID() const;
  std::string getName() const;
  std::vector<std::string> getFields() const;
  const fbs::Matrix* getMatrix() const;
};

} // namespace ftt
