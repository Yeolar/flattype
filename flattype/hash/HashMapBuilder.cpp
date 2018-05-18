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

#include "flattype/hash/HashMapBuilder.h"

namespace ftt {

void HashMap32Builder::finish() {
  if (finished_) {
    return;
  }
  fbb_->Finish(fbs::CreateHMap32Direct(*fbb_, &slots_));
  data_ = fbb_->Release();
  finished_ = true;
}

void HashMap64Builder::finish() {
  if (finished_) {
    return;
  }
  fbb_->Finish(fbs::CreateHMap64Direct(*fbb_, &slots_));
  data_ = fbb_->Release();
  finished_ = true;
}

void HashMapSBuilder::finish() {
  if (finished_) {
    return;
  }
  fbb_->Finish(fbs::CreateHMapSDirect(*fbb_, &slots_));
  data_ = fbb_->Release();
  finished_ = true;
}

} // namespace ftt
