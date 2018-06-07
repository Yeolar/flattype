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

#pragma once

#include "flattype/Builder.h"
#include "flattype/CommonIDLs.h"
#include "flattype/matrix/Encoding.h"
#include "flattype/query/Operation.h"

namespace ftt {

class OperationBuilder : public Builder {
 public:
  OperationBuilder() : Builder() {}

  explicit OperationBuilder(FBB* fbb, bool owns = false)
    : Builder(fbb, owns) {}

  OperationBuilder(const OperationBuilder&) = delete;
  OperationBuilder& operator=(const OperationBuilder&) = delete;

  OperationBuilder(OperationBuilder&&) = default;
  OperationBuilder& operator=(OperationBuilder&&) = default;

  fbs::Op getCmd() const;
  void setCmd(fbs::Op op);

  template <class... Args>
  void getParams(Args&... args) const;
  template <class... Args>
  void setParams(const Args&... args);

  void finish() override;

  Operation toOperation() { return toWrapper<Operation>(); }

 private:
  fbs::Op op_{fbs::Op::None};
  ::flatbuffers::Offset<fbs::Tuple> params_;
};

template <class... Args>
void OperationBuilder::getParams(Args&... args) const {
  vdecode(::flatbuffers::GetTemporaryPointer(*fbb_, params_), args...);
}

template <class... Args>
void OperationBuilder::setParams(const Args&... args) {
  params_ = vencode(*fbb_, args...);
}

} // namespace ftt
