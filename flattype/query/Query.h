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

class Query : public Wrapper<fbs::Query> {
 public:
  Query(const fbs::Query* query) : Wrapper(query) {}

  explicit Query(const uint8_t* data)
    : Wrapper(data) {}
  explicit Query(::flatbuffers::DetachedBuffer&& data)
    : Wrapper(std::move(data)) {}

  Query(const Query&) = delete;
  Query& operator=(const Query&) = delete;

  Query(Query&&) = default;
  Query& operator=(Query&&) = default;

  std::string toDebugString() const override;

  uint64_t getKey() const;
  std::string getURI() const;

  const fbs::Operation* getOperation(size_t i) const;
  size_t getOperationCount() const;
  size_t getBegin() const;
  size_t getEnd() const;
};

} // namespace ftt
