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

#include "accelerator/Conv.h"
#include "flattype/matrix/Tuple.h"
#include "flattype/query/Operation.h"

namespace ftt {

std::string Operation::toDebugString() const {
  std::string out;
  if (!get()) {
    return "Operation::<NULL>";
  }
  acc::toAppend("Operation::",
                ", cmd:", fbs::EnumNameOp(getCmd()),
                ", rtype:", getRType(),
                ", params:", Tuple(getParams()).toDebugString(),
                &out);
  return out;
}

fbs::Op Operation::getCmd() const {
  return ptr_ ? ptr_->cmd() : fbs::Op::None;
}

uint8_t Operation::getRType() const {
  return ptr_ ? ptr_->rtype() : 0;
}

} // namespace ftt
