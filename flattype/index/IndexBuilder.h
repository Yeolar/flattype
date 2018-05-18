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
#include "flattype/index/Index.h"

namespace ftt {

template <class HMap>
class IndexBuilderBase : public Builder {
 public:
  typedef IndexBase<HMap> Index;

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

  void buildHash(FBBFunc<void>&& builder) {
    hash_ = builder(fbb_.get());
  }

 protected:
  std::string name_;
  ::flatbuffers::Offset<void> hash_;
};

class Index32Builder : public IndexBuilderBase<HashMap32> {
 public:
  Index32Builder()
    : IndexBuilderBase() {}
  explicit Index32Builder(FBB* fbb, bool owns = false)
    : IndexBuilderBase(fbb, owns) {}

  void finish() override;

  Index32 toIndex() { return toWrapper<Index32>(); }
};

class Index64Builder : public IndexBuilderBase<HashMap64> {
 public:
  Index64Builder()
    : IndexBuilderBase() {}
  explicit Index64Builder(FBB* fbb, bool owns = false)
    : IndexBuilderBase(fbb, owns) {}

  void finish() override;

  Index64 toIndex() { return toWrapper<Index64>(); }
};

class IndexSBuilder : public IndexBuilderBase<HashMapS> {
 public:
  IndexSBuilder()
    : IndexBuilderBase() {}
  explicit IndexSBuilder(FBB* fbb, bool owns = false)
    : IndexBuilderBase(fbb, owns) {}

  void finish() override;

  IndexS toIndex() { return toWrapper<IndexS>(); }
};

} // namespace ftt
