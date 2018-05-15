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

#include "accelerator/dynamic.h"
#include "flattype/idl/object_generated.h"
#include "flattype/Builder.h"
#include "flattype/object/dynamic.h"
#include "flattype/object/Type.h"

namespace ftt {

class DynamicBuilder : public Builder {
 public:
  DynamicBuilder()
    : Builder() {}
  DynamicBuilder(const acc::dynamic& d)
    : Builder(), dynamic_(d) {}
  DynamicBuilder(acc::dynamic&& d)
    : Builder(), dynamic_(std::move(d)) {}

  explicit DynamicBuilder(FBB* fbb, bool owns = false)
    : Builder(fbb, owns) {}

  DynamicBuilder(const DynamicBuilder&) = delete;
  DynamicBuilder& operator=(const DynamicBuilder&) = delete;

  DynamicBuilder(DynamicBuilder&&) = default;
  DynamicBuilder& operator=(DynamicBuilder&&) = default;

  void reset(const acc::dynamic& d);
  void reset(acc::dynamic&& d);

  void update(const acc::dynamic& d);

  void finish() override;

  ftt::dynamic toDynamic() {
    finish();
    return ftt::dynamic(toJsonType(dynamic_.type()), detachedData());
  }

 private:
  acc::dynamic dynamic_;
};

} // namespace ftt
