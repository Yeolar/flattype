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

#include "flattype/table/IndexBuilder.h"

namespace ftt {

void Index8Builder::finish() {
  if (finished_) {
    return;
  }
  IndexBuilderBase<fbs::Index8,fbs::Key8,uint8_t,uint8_t>::sort();
  fbb_->Finish(fbs::CreateIndex8Direct(*fbb_, name_.c_str(), &keys_));
  data_ = fbb_->Release();
  finished_ = true;
}

void Index16Builder::finish() {
  if (finished_) {
    return;
  }
  IndexBuilderBase<fbs::Index16,fbs::Key16,uint16_t,uint16_t>::sort();
  fbb_->Finish(fbs::CreateIndex16Direct(*fbb_, name_.c_str(), &keys_));
  data_ = fbb_->Release();
  finished_ = true;
}

void Index32Builder::finish() {
  if (finished_) {
    return;
  }
  IndexBuilderBase<fbs::Index32,fbs::Key32,uint32_t,uint32_t>::sort();
  fbb_->Finish(fbs::CreateIndex32Direct(*fbb_, name_.c_str(), &keys_));
  data_ = fbb_->Release();
  finished_ = true;
}

void Index64Builder::finish() {
  if (finished_) {
    return;
  }
  IndexBuilderBase<fbs::Index64,fbs::Key64,uint64_t,uint64_t>::sort();
  fbb_->Finish(fbs::CreateIndex64Direct(*fbb_, name_.c_str(), &keys_));
  data_ = fbb_->Release();
  finished_ = true;
}

void SIndexBuilder::finish() {
  if (finished_) {
    return;
  }
  IndexBuilderBase<fbs::SIndex,fbs::SKey,std::string,uint64_t>::sort();
  fbb_->Finish(fbs::CreateSIndexDirect(*fbb_, name_.c_str(), &keys_));
  data_ = fbb_->Release();
  finished_ = true;
}

} // namespace ftt
