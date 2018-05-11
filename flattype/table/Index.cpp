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

#include "accelerator/Conv.h"
#include "flattype/table/Index.h"

namespace ftt {

std::string Index8::toDebugString() const {
  return get() ?
    acc::to<std::string>("Index8:", getName(), "[", size(), "]") : "";
}

std::string Index16::toDebugString() const {
  return get() ?
    acc::to<std::string>("Index16:", getName(), "[", size(), "]") : "";
}

std::string Index32::toDebugString() const {
  return get() ?
    acc::to<std::string>("Index32:", getName(), "[", size(), "]") : "";
}

std::string Index64::toDebugString() const {
  return get() ?
    acc::to<std::string>("Index64:", getName(), "[", size(), "]") : "";
}

std::string SIndex::toDebugString() const {
  return get() ?
    acc::to<std::string>("SIndex:", getName(), "[", size(), "]") : "";
}

} // namespace ftt
