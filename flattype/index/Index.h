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
#include "flattype/hash/HashMap.h"

namespace ftt {

template <class HMap>
class IndexBase : public Wrapper<fbs::Index> {
 public:
  typedef typename HMap::ft_type FTHMap;
  typedef typename HMap::key_type key_type;
  typedef typename HMap::const_iterator const_iterator;

  IndexBase(const fbs::Index* index)
    : Wrapper(index),
      hmap_(ptr_->hash_as<FTHMap>()) {}

  explicit IndexBase(const uint8_t* data)
    : Wrapper(data),
      hmap_(ptr_->hash_as<FTHMap>()) {}
  explicit IndexBase(::flatbuffers::DetachedBuffer&& data)
    : Wrapper(std::move(data)),
      hmap_(ptr_->hash_as<FTHMap>()) {}

  IndexBase(const IndexBase&) = delete;
  IndexBase& operator=(const IndexBase&) = delete;

  IndexBase(IndexBase&&) = default;
  IndexBase& operator=(IndexBase&&) = default;

  std::string getName() const {
    return ptr_->name()->str();
  }

  fbs::HMap getHashType() const {
    return ptr_->hash_type();
  }

  const HMap& getHash() const {
    return hmap_;
  }

  const_iterator find(const key_type& key) const {
    return hmap_.find(key);
  }

  const_iterator begin() const {
    return hmap_.cbegin();
  }

  const_iterator end() const {
    return hmap_.cend();
  }

 private:
  HMap hmap_;
};

class Index32 : public IndexBase<HashMap32> {
 public:
  Index32(const fbs::Index* index)
    : IndexBase(index) {}

  explicit Index32(const uint8_t* data)
    : IndexBase(data) {}
  explicit Index32(::flatbuffers::DetachedBuffer&& data)
    : IndexBase(std::move(data)) {}

  std::string toDebugString() const override;
};

class Index64 : public IndexBase<HashMap64> {
 public:
  Index64(const fbs::Index* index)
    : IndexBase(index) {}

  explicit Index64(const uint8_t* data)
    : IndexBase(data) {}
  explicit Index64(::flatbuffers::DetachedBuffer&& data)
    : IndexBase(std::move(data)) {}

  std::string toDebugString() const override;
};

class IndexS : public IndexBase<HashMapS> {
 public:
  IndexS(const fbs::Index* index)
    : IndexBase(index) {}

  explicit IndexS(const uint8_t* data)
    : IndexBase(data) {}
  explicit IndexS(::flatbuffers::DetachedBuffer&& data)
    : IndexBase(std::move(data)) {}

  std::string toDebugString() const override;
};

} // namespace ftt
