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

#include "flattype/matrix/Matrix.h"
#include "flattype/matrix/Stringize.h"

namespace ftt {

std::string Matrix::toDebugString() const {
  std::string out;
  if (!get()) {
    return "<NULL>";
  }

  acc::toAppend("size:", getRowCount(), "x", getColCount(), &out);
  std::string sepline = '\n' + std::string(out.size(), '-');
  acc::toAppend(sepline, &out);

  for (size_t i = 0; i < getRowCount(); i++) {
    std::string row = acc::to<std::string>("\nrow(", i, "):");
    for (size_t j = 0; j < getColCount(); j++) {
      auto item = getItem(i, j);
      acc::toAppend("\n  (", fbs::EnumNameAny(item->value_type()), "):", &row);
      acc::toAppend(*item, &row);
    }
    acc::toAppend(row, sepline, &out);
  }
  return out;
}

} // namespace ftt
