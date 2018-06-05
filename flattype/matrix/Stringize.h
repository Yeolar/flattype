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

#include "flattype/CommonIDLs.h"

namespace acc {

template <class Tgt> void toAppend(const ftt::fbs::Item&,   Tgt*);
template <class Tgt> void toAppend(const ftt::fbs::Record&, Tgt*);
template <class Tgt> void toAppend(const ftt::fbs::Matrix&, Tgt*);

} // namespace acc

//////////////////////////////////////////////////////////////////////

#include "flattype/Stringize.h"

namespace acc {

template <class Tgt>
void toAppend(const ftt::fbs::Item& value, Tgt* result) {
  ftt::toAppendAny(value.value_type(), value.value(), result);
}

template <class Tgt>
void toAppend(const ftt::fbs::Record& value, Tgt* result) {
  toAppend(value.value(), result);
}

template <class Tgt>
void toAppend(const ftt::fbs::Matrix& value, Tgt* result) {
  toAppend(value.value(), result);
}

} // namespace acc
