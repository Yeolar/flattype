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

#include "flattype/idl/table_generated.h"
#include "flattype/Util.h"
#include "flattype/Wrapper.h"

namespace ftt {

template <class FT, class KFT, class KType, class IType>
class IndexBase : public Wrapper<FT> {
 public:
  typedef KType key_type;
  typedef IType index_type;
  typedef ::flatbuffers::Offset<KFT> offset;
  typedef ::flatbuffers::Vector<offset> vector;
  typedef typename vector::iterator iterator;
  typedef typename vector::const_iterator const_iterator;

  static constexpr index_type npos = std::numeric_limits<index_type>::max();

 public:
  IndexBase(const FT* index) : Wrapper<FT>(index) {}

  explicit IndexBase(const uint8_t* data)
    : Wrapper<FT>(data) {}
  explicit IndexBase(::flatbuffers::DetachedBuffer&& data)
    : Wrapper<FT>(std::move(data)) {}

  IndexBase(const IndexBase&) = delete;
  IndexBase& operator=(const IndexBase&) = delete;

  IndexBase(IndexBase&&) = default;
  IndexBase& operator=(IndexBase&&) = default;

  std::string getName() const {
    return Wrapper<FT>::ptr_->name()->str();
  }

  const vector* getKeys() const {
    return Wrapper<FT>::ptr_->keys();
  }

  index_type getIndex(const key_type& key) const {
    auto k = getKeys()->LookupByKey(key);
    return k ? k->i() : npos;
  }

  size_t size() const {
    return getKeys()->size();
  }

  iterator begin() {
    return iterator(getKeys()->Data(), 0);
  }
  const_iterator begin() const {
    return const_iterator(getKeys()->Data(), 0);
  }

  iterator end() {
    return iterator(getKeys()->Data(), getKeys()->size());
  }
  const_iterator end() const {
    return const_iterator(getKeys()->Data(), getKeys()->size());
  }

  iterator find(const key_type& key) {
    return ftt::find(getKeys(), key);
  }
  const_iterator find(const key_type& key) const {
    return ftt::findConst(getKeys(), key);
  }
};

class Index8 : public IndexBase<fbs::Index8,fbs::Key8,uint8_t,uint8_t> {
 public:
  Index8(const fbs::Index8* index)
    : IndexBase<fbs::Index8,fbs::Key8,uint8_t,uint8_t>(index) {}
  explicit Index8(const uint8_t* data)
    : IndexBase<fbs::Index8,fbs::Key8,uint8_t,uint8_t>(data) {}
  explicit Index8(::flatbuffers::DetachedBuffer&& data)
    : IndexBase<fbs::Index8,fbs::Key8,uint8_t,uint8_t>(std::move(data)) {}

  std::string toDebugString() const override;
};

class Index16 : public IndexBase<fbs::Index16,fbs::Key16,uint16_t,uint16_t> {
 public:
  Index16(const fbs::Index16* index)
    : IndexBase<fbs::Index16,fbs::Key16,uint16_t,uint16_t>(index) {}
  explicit Index16(const uint8_t* data)
    : IndexBase<fbs::Index16,fbs::Key16,uint16_t,uint16_t>(data) {}
  explicit Index16(::flatbuffers::DetachedBuffer&& data)
    : IndexBase<fbs::Index16,fbs::Key16,uint16_t,uint16_t>(std::move(data)) {}

  std::string toDebugString() const override;
};

class Index32 : public IndexBase<fbs::Index32,fbs::Key32,uint32_t,uint32_t> {
 public:
  Index32(const fbs::Index32* index)
    : IndexBase<fbs::Index32,fbs::Key32,uint32_t,uint32_t>(index) {}
  explicit Index32(const uint8_t* data)
    : IndexBase<fbs::Index32,fbs::Key32,uint32_t,uint32_t>(data) {}
  explicit Index32(::flatbuffers::DetachedBuffer&& data)
    : IndexBase<fbs::Index32,fbs::Key32,uint32_t,uint32_t>(std::move(data)) {}

  std::string toDebugString() const override;
};

class Index64 : public IndexBase<fbs::Index64,fbs::Key64,uint64_t,uint64_t> {
 public:
  Index64(const fbs::Index64* index)
    : IndexBase<fbs::Index64,fbs::Key64,uint64_t,uint64_t>(index) {}
  explicit Index64(const uint8_t* data)
    : IndexBase<fbs::Index64,fbs::Key64,uint64_t,uint64_t>(data) {}
  explicit Index64(::flatbuffers::DetachedBuffer&& data)
    : IndexBase<fbs::Index64,fbs::Key64,uint64_t,uint64_t>(std::move(data)) {}

  std::string toDebugString() const override;
};

class SIndex : public IndexBase<fbs::SIndex,fbs::SKey,std::string,uint64_t> {
 public:
  SIndex(const fbs::SIndex* index)
    : IndexBase<fbs::SIndex,fbs::SKey,std::string,uint64_t>(index) {}
  explicit SIndex(const uint8_t* data)
    : IndexBase<fbs::SIndex,fbs::SKey,std::string,uint64_t>(data) {}
  explicit SIndex(::flatbuffers::DetachedBuffer&& data)
    : IndexBase<fbs::SIndex,fbs::SKey,std::string,uint64_t>(std::move(data)) {}

  std::string toDebugString() const override;
};

} // namespace ftt
