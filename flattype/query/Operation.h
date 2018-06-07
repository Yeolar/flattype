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

template <class... Args>
struct Operation {
  ftt::fbs::Op cmd;
  std::tuple<Args...> params;

  Operation(ftt::fbs::Op op, const Args&... args)
    : cmd(op), params(args...) {}
  Operation(ftt::fbs::Op op, Args&&... args)
    : cmd(op), params(args...) {}
};

template <class... Args>
Operation<Args...> makeOperation(fbs::Op op, const Args&... args) {
  return Operation<Args...>(op, args...);
}

template <class... Args>
Operation<Args...> makeOperation(fbs::Op op, Args&&... args) {
  return Operation<Args...>(op, std::forward<Args...>(args)...);
}

} // namespace ftt
