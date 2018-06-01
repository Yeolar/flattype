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
#include "flattype/query/Operation.h"
#include "flattype/query/Query.h"

namespace ftt {

std::string Query::toDebugString() const {
  std::string out;
  if (!get()) {
    return "{}";
  }
  acc::toAppend("{ key:", getKey(),
                ", uri:", getURI(),
                ", ops:[",
                &out);
  size_t b = getBegin();
  size_t e = std::min(getEnd(), getOperationCount());
  for (size_t i = b; i < e; i++) {
    acc::toAppend(i > b ? ", " : "",
                  Operation(getOperation(i)).toDebugString(),
                  &out);
  }
  acc::toAppend("] }", &out);
  return out;
}

uint64_t Query::getKey() const {
  return ptr_ ? ptr_->key() : std::numeric_limits<uint64_t>::max();
}

std::string Query::getURI() const {
  return ptr_ ? ptr_->uri()->str() : "";
}

const fbs::Operation* Query::getOperation(size_t i) const {
  return ptr_ ? ptr_->operations()->Get(i) : nullptr;
}

size_t Query::getOperationCount() const {
  return ptr_ ? ptr_->operations()->size() : 0;
}

size_t Query::getBegin() const {
  return ptr_ ? ptr_->opbegin() : 0;
}

size_t Query::getEnd() const {
  return ptr_ ? ptr_->opend() : std::numeric_limits<size_t>::max();
}

} // namespace ftt
