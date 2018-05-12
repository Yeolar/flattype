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
#include "accelerator/String.h"
#include "flattype/matrix/Matrix.h"
#include "flattype/table/Table.h"

namespace ftt {

std::string Table::toDebugString() const {
  std::string out;
  if (!get()) {
    return "Table::<NULL>";
  }
  acc::toAppend("Table::", getName(),
                ", bid:", getBID(),
                ", fields:", acc::join(',', getFields()),
                ", matrix:", Matrix(getMatrix()).toDebugString(),
                &out);
  return out;
}

uint8_t Table::getBID() const {
  return ptr_ ? ptr_->bid() : npos;
}

std::string Table::getName() const {
  return ptr_ ? ptr_->name()->str() : "";
}

std::vector<std::string> Table::getFields() const {
  std::vector<std::string> fields;
  if (ptr_ && ptr_->fields()) {
    for (auto i : *ptr_->fields()) {
      fields.push_back(i->str());
    }
  }
  return fields;
}

const fbs::Matrix* Table::getMatrix() const {
  return ptr_ ? ptr_->matrix() : nullptr;
}

} // namespace ftt
