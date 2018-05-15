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

#include "flattype/object/DynamicBuilder.h"
#include "flattype/object/Encoding.h"

namespace ftt {

void DynamicBuilder::reset(const acc::dynamic& d) {
  dynamic_ = d;
}
void DynamicBuilder::reset(acc::dynamic&& d) {
  dynamic_ = std::move(d);
}

void DynamicBuilder::update(const acc::dynamic& d) {
  dynamic_.update(d);
}

void DynamicBuilder::finish() {
  if (finished_) {
    return;
  }
  switch (dynamic_.type()) {
#define FTT_X(ft) fbb_->Finish(encodeJson##ft(*fbb_, dynamic_))
    case acc::dynamic::NULLT:  FTT_X(Null);   break;
    case acc::dynamic::BOOL:   FTT_X(Bool);   break;
    case acc::dynamic::DOUBLE: FTT_X(Double); break;
    case acc::dynamic::INT64:  FTT_X(Int64);  break;
    case acc::dynamic::STRING: FTT_X(String); break;
    case acc::dynamic::ARRAY:  FTT_X(Array);  break;
    case acc::dynamic::OBJECT: FTT_X(Object); break;
#undef FTT_X
  }
  data_ = fbb_->Release();
  finished_ = true;
}

} // namespace ftt
