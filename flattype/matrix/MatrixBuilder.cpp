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

#include "flattype/matrix/MatrixBuilder.h"

namespace ftt {

void MatrixBuilder::finish() {
  if (finished_) {
    return;
  }
  std::vector<flatbuffers::Offset<fbs::Record>> records;
  for (auto& record : records_) {
    records.push_back(fbs::CreateRecordDirect(*fbb_, &record));
  }
  fbb_->Finish(fbs::CreateMatrixDirect(*fbb_, &records));
  data_ = fbb_->Release();
  finished_ = true;
}

} // namespace ftt
