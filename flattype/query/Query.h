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
#include "flattype/query/Operation.h"

namespace ftt {

typedef const char* (*CmdNameGetter)(uint32_t);

std::string toDebugString(const fbs::Operation* op,
                          CmdNameGetter func = nullptr);

class Query : public Wrapper<fbs::Query> {
 public:
  Query(const fbs::Query* query, CmdNameGetter func = nullptr)
    : Wrapper(query), cmdNameGetter_(func) {}

  explicit Query(const uint8_t* data,
                 CmdNameGetter func = nullptr)
    : Wrapper(data), cmdNameGetter_(func) {}
  explicit Query(::flatbuffers::DetachedBuffer&& data,
                 CmdNameGetter func = nullptr)
    : Wrapper(std::move(data)), cmdNameGetter_(func) {}

  Query(const Query&) = delete;
  Query& operator=(const Query&) = delete;

  Query(Query&&) = default;
  Query& operator=(Query&&) = default;

  void setCmdNameGetter(CmdNameGetter func);

  std::string toDebugString() const override;

  uint64_t getKey() const;
  std::string getURI() const;

  size_t getOperationCount() const;

  const fbs::Operation* getOperation(size_t i) const;
  template <class... Args>
  void getOperation(size_t i, uint32_t& cmd, Args&... args) const;
  template <class... Args>
  void getOperation(size_t i, Operation<Args...>& o) const;

  size_t getBegin() const;
  size_t getEnd() const;

 private:
  CmdNameGetter cmdNameGetter_{nullptr};
};

inline void Query::setCmdNameGetter(CmdNameGetter func) {
  cmdNameGetter_ = func;
}

template <class... Args>
void Query::getOperation(size_t i, uint32_t& cmd, Args&... args) const {
  auto op = getOperation(i);
  cmd = op->cmd();
  vdecode(op->params(), args...);
}

template <class... Args>
void Query::getOperation(size_t i, Operation<Args...>& o) const {
  auto op = getOperation(i);
  o.cmd = op->cmd();
  decode(op->params(), o.params);
}

} // namespace ftt
