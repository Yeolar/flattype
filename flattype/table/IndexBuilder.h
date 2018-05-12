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
#include "flattype/Builder.h"
#include "flattype/table/Index.h"

namespace ftt {

template <class FT, class KFT, class KType, class IType>
class IndexBuilderBase : public Builder {
 public:
  typedef IndexBase<FT, KFT, KType, IType> Index;

 public:
  IndexBuilderBase() : Builder() {}

  explicit IndexBuilderBase(FBB* fbb, bool owns = false)
    : Builder(fbb, owns) {}

  IndexBuilderBase(const IndexBuilderBase&) = delete;
  IndexBuilderBase& operator=(const IndexBuilderBase&) = delete;

  IndexBuilderBase(IndexBuilderBase&&) = default;
  IndexBuilderBase& operator=(IndexBuilderBase&&) = default;

  std::string getName() const {
    return name_;
  }
  void setName(const std::string& name) {
    name_ = name;
  }

  const std::vector<typename Index::offset>& getKeys() const {
    return keys_;
  }

  void addKeyIndex(typename Index::offset ki) {
    keys_.push_back(ki);
  }

 protected:
  void sort() {
    std::sort(keys_.begin(), keys_.end(),
              [&](const typename Index::offset& a,
                  const typename Index::offset& b) -> bool {
                auto x = ::flatbuffers::GetTemporaryPointer(*fbb_, a);
                auto y = ::flatbuffers::GetTemporaryPointer(*fbb_, b);
                return x->KeyCompareLessThan(y);
              });
  }

  std::string name_;
  std::vector<typename Index::offset> keys_;
};

class Index8Builder
  : public IndexBuilderBase<fbs::Index8,fbs::Key8,uint8_t,uint8_t> {
 public:
  Index8Builder()
    : IndexBuilderBase<fbs::Index8,fbs::Key8,uint8_t,uint8_t>() {}
  explicit Index8Builder(FBB* fbb, bool owns = false)
    : IndexBuilderBase<fbs::Index8,fbs::Key8,uint8_t,uint8_t>(fbb, owns) {}

  void addKeyIndex(uint8_t k, uint8_t i) {
    IndexBuilderBase<fbs::Index8,fbs::Key8,uint8_t,uint8_t>::
      addKeyIndex(fbs::CreateKey8(*fbb_, k, i));
  }

  void finish() override;

  Index8 toIndex() { return toWrapper<Index8>(); }
};

class Index16Builder
  : public IndexBuilderBase<fbs::Index16,fbs::Key16,uint16_t,uint16_t> {
 public:
  Index16Builder()
    : IndexBuilderBase<fbs::Index16,fbs::Key16,uint16_t,uint16_t>() {}
  explicit Index16Builder(FBB* fbb, bool owns = false)
    : IndexBuilderBase<fbs::Index16,fbs::Key16,uint16_t,uint16_t>(fbb, owns) {}

  void addKeyIndex(uint16_t k, uint16_t i) {
    IndexBuilderBase<fbs::Index16,fbs::Key16,uint16_t,uint16_t>::
      addKeyIndex(fbs::CreateKey16(*fbb_, k, i));
  }

  void finish() override;

  Index16 toIndex() { return toWrapper<Index16>(); }
};

class Index32Builder
  : public IndexBuilderBase<fbs::Index32,fbs::Key32,uint32_t,uint32_t> {
 public:
  Index32Builder()
    : IndexBuilderBase<fbs::Index32,fbs::Key32,uint32_t,uint32_t>() {}
  explicit Index32Builder(FBB* fbb, bool owns = false)
    : IndexBuilderBase<fbs::Index32,fbs::Key32,uint32_t,uint32_t>(fbb, owns) {}

  void addKeyIndex(uint32_t k, uint32_t i) {
    IndexBuilderBase<fbs::Index32,fbs::Key32,uint32_t,uint32_t>::
      addKeyIndex(fbs::CreateKey32(*fbb_, k, i));
  }

  void finish() override;

  Index32 toIndex() { return toWrapper<Index32>(); }
};

class Index64Builder
  : public IndexBuilderBase<fbs::Index64,fbs::Key64,uint64_t,uint64_t> {
 public:
  Index64Builder()
    : IndexBuilderBase<fbs::Index64,fbs::Key64,uint64_t,uint64_t>() {}
  explicit Index64Builder(FBB* fbb, bool owns = false)
    : IndexBuilderBase<fbs::Index64,fbs::Key64,uint64_t,uint64_t>(fbb, owns) {}

  void addKeyIndex(uint64_t k, uint64_t i) {
    IndexBuilderBase<fbs::Index64,fbs::Key64,uint64_t,uint64_t>::
      addKeyIndex(fbs::CreateKey64(*fbb_, k, i));
  }

  void finish() override;

  Index64 toIndex() { return toWrapper<Index64>(); }
};

class SIndexBuilder
  : public IndexBuilderBase<fbs::SIndex,fbs::SKey,std::string,uint64_t> {
 public:
  SIndexBuilder()
    : IndexBuilderBase<fbs::SIndex,fbs::SKey,std::string,uint64_t>() {}
  explicit SIndexBuilder(FBB* fbb, bool owns = false)
    : IndexBuilderBase<fbs::SIndex,fbs::SKey,std::string,uint64_t>(fbb, owns) {}

  void addKeyIndex(const std::string& k, uint64_t i) {
    IndexBuilderBase<fbs::SIndex,fbs::SKey,std::string,uint64_t>::
      addKeyIndex(fbs::CreateSKeyDirect(*fbb_, k.c_str(), i));
  }

  void finish() override;

  SIndex toIndex() { return toWrapper<SIndex>(); }
};

} // namespace ftt
