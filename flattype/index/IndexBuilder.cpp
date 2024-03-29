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

#include "flattype/index/IndexBuilder.h"

namespace ftt {

void Index32Builder::finish() {
  if (finished_) {
    return;
  }
  fbb_->Finish(
      fbs::CreateIndex(
          *fbb_,
          fbb_->CreateString(name_),
          fbs::HMap::HMap32,
          hash_));
  data_ = fbb_->Release();
  finished_ = true;
}

void Index64Builder::finish() {
  if (finished_) {
    return;
  }
  fbb_->Finish(
      fbs::CreateIndex(
          *fbb_,
          fbb_->CreateString(name_),
          fbs::HMap::HMap64,
          hash_));
  data_ = fbb_->Release();
  finished_ = true;
}

void IndexSBuilder::finish() {
  if (finished_) {
    return;
  }
  fbb_->Finish(
      fbs::CreateIndex(
          *fbb_,
          fbb_->CreateString(name_),
          fbs::HMap::HMapS,
          hash_));
  data_ = fbb_->Release();
  finished_ = true;
}

} // namespace ftt
