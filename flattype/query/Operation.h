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

#include "accelerator/Conv.h"
#include "flattype/CommonIDLs.h"
#include "flattype/Encoding.h"
#include "flattype/Wrapper.h"
#include "flattype/idl/query_generated.h"

namespace ftt {

template <class... Args>
struct Operation {
  uint32_t cmd;
  std::tuple<Args...> params;

  Operation(uint32_t op, const Args&... args)
    : cmd(op), params(args...) {}
  Operation(uint32_t op, Args&&... args)
    : cmd(op), params(args...) {}
  Operation(const ftt::fbs::Operation* op)
    : cmd(op->cmd()) {
    decode(op->params(), params);
  }
};

template <class Op, class... Args>
typename std::enable_if<std::is_enum<Op>::value, Operation<Args...>>::type
makeOperation(Op op, const Args&... args) {
  return Operation<Args...>(acc::to<uint32_t>(op), args...);
}

template <class Op, class... Args>
typename std::enable_if<std::is_enum<Op>::value, Operation<Args...>>::type
makeOperation(Op op, Args&&... args) {
  return Operation<Args...>(acc::to<uint32_t>(op), std::forward<Args>(args)...);
}

} // namespace ftt
