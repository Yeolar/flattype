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

#include "flattype/bucket/BucketBuilder.h"

namespace ftt {

uint16_t BucketBuilder::getBID() const {
  return bid_;
}

void BucketBuilder::setBID(uint16_t bid) {
  bid_ = bid;
}

std::string BucketBuilder::getName() const {
  return name_;
}

void BucketBuilder::setName(const std::string& name) {
  name_ = name;
}

const std::vector<std::string>& BucketBuilder::getFields() const {
  return fields_;
}

void BucketBuilder::setFields(const std::vector<std::string>& fields) {
  fields_ = fields;
}

bool BucketBuilder::isColumnar() const {
  return columnar_;
}

void BucketBuilder::setColumnar(bool columnar) {
  columnar_ = columnar;
}

void BucketBuilder::buildMatrix(FBBFunc<fbs::Matrix>&& builder) {
  matrix_ = builder(fbb_.get());
}

void BucketBuilder::finish() {
  if (finished_) {
    return;
  }
  fbb_->Finish(
      fbs::CreateBucket(
          *fbb_,
          bid_,
          fbb_->CreateString(name_),
          matrix_,
          fbb_->CreateVectorOfStrings(fields_)));
  data_ = fbb_->Release();
  finished_ = true;
}

} // namespace ftt
