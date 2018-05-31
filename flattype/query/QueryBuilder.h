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
#include "flattype/query/Query.h"

namespace ftt {

class QueryBuilder : public Builder {
 public:
  QueryBuilder() : Builder() {}

  explicit QueryBuilder(FBB* fbb, bool owns = false)
    : Builder(fbb, owns) {}

  QueryBuilder(const QueryBuilder&) = delete;
  QueryBuilder& operator=(const QueryBuilder&) = delete;

  QueryBuilder(QueryBuilder&&) = default;
  QueryBuilder& operator=(QueryBuilder&&) = default;

  uint64_t getKey() const;
  void setKey(uint64_t key);

  std::string getURI() const;
  void setURI(const std::string& uri);

  size_t getOperationCount() const;
  const fbs::Operation* getOperation(size_t i) const;
  void addOperation(FBBFunc<fbs::Operation>&& builder);

  size_t getBegin() const;
  void setBegin(size_t begin);

  size_t getEnd() const;
  void setEnd(size_t end);

  void finish() override;

  Query toQuery() { return toWrapper<Query>(); }

 private:
  uint64_t key_;
  std::string uri_;
  std::vector<flatbuffers::Offset<fbs::Operation>> ops_;
  size_t opBegin_{0};
  size_t opEnd_{std::numeric_limits<size_t>::max()};
};

} // namespace ftt
