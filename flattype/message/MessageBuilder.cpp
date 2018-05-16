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

#include "flattype/message/MessageBuilder.h"

namespace ftt {

int MessageBuilder::getCode() const {
  return code_;
}

void MessageBuilder::setCode(int code) {
  code_ = code;
}

std::string MessageBuilder::getMessage() const {
  return message_;
}

void MessageBuilder::setMessage(const std::string& msg) {
  message_ = msg;
}

void MessageBuilder::buildBData(FBBFunc<fbs::Bucket>&& builder) {
  bdata_ = builder(fbb_.get());
}

void MessageBuilder::buildJData(FBBFunc<fbs::Object>&& builder) {
  jdata_ = builder(fbb_.get());
}

void MessageBuilder::buildVData(FBBFunc<fbs::Tuple>&& builder) {
  vdata_ = builder(fbb_.get());
}

void MessageBuilder::finish() {
  if (finished_) {
    return;
  }
  fbb_->Finish(
      fbs::CreateMessageDirect(
          *fbb_, code_, message_.c_str(), bdata_, jdata_, vdata_));
  data_ = fbb_->Release();
  finished_ = true;
}

} // namespace ftt
