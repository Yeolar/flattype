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

#include "flattype/query/QueryBuilder.h"

namespace ftt {

uint64_t QueryBuilder::getKey() const {
  return key_;
}

void QueryBuilder::setKey(uint64_t key) {
  key_ = key;
}

std::string QueryBuilder::getURI() const {
  return uri_;
}

void QueryBuilder::setURI(const std::string& uri) {
  uri_ = uri;
}

size_t QueryBuilder::getOperationCount() const {
  return ops_.size();
}

const fbs::Operation* QueryBuilder::getOperation(size_t i) const {
  return ::flatbuffers::GetTemporaryPointer(*fbb_, ops_[i]);
}

void QueryBuilder::addOperation(FBBFunc<fbs::Operation>&& builder) {
  ops_.push_back(builder(fbb_.get()));
}

size_t QueryBuilder::getBegin() const {
  return opBegin_;
}

void QueryBuilder::setBegin(size_t begin) {
  opBegin_ = begin;
}

size_t QueryBuilder::getEnd() const {
  return opEnd_;
}

void QueryBuilder::setEnd(size_t end) {
  opEnd_ = end;
}

void QueryBuilder::finish() {
  if (finished_) {
    return;
  }
  fbb_->Finish(
      fbs::CreateQueryDirect(
          *fbb_, key_, uri_.c_str(), &ops_, opBegin_, opEnd_));
  data_ = fbb_->Release();
  finished_ = true;
}

} // namespace ftt
