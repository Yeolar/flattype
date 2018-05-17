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

namespace ftt {

template <class Tgt>
void toAppendAny(fbs::Any type, const void* ptr, Tgt* result);

} // namespace ftt

namespace acc {

template <class Tgt> void toAppend(const ftt::fbs::Tuple&,  Tgt*);
template <class Tgt> void toAppend(const ftt::fbs::Item&,   Tgt*);
template <class Tgt> void toAppend(const ftt::fbs::Record&, Tgt*);
template <class Tgt> void toAppend(const ftt::fbs::Matrix&, Tgt*);

} // namespace acc

//////////////////////////////////////////////////////////////////////

#include "flattype/Stringize.h"

namespace ftt {

template <class Tgt>
void toAppendAny(fbs::Any type, const void* ptr, Tgt* result) {
  switch (type) {
#define FTT_ANY_TO_APPEND_CASE(ft) \
    case fbs::Any::ft: \
      acc::toAppend(*reinterpret_cast<const fbs::ft*>(ptr), result); \
      break;

    FTT_ANY_TO_APPEND_CASE(Null)
    FTT_ANY_TO_APPEND_CASE(Bool)
    FTT_ANY_TO_APPEND_CASE(Int8)
    FTT_ANY_TO_APPEND_CASE(Int16)
    FTT_ANY_TO_APPEND_CASE(Int32)
    FTT_ANY_TO_APPEND_CASE(Int64)
    FTT_ANY_TO_APPEND_CASE(UInt8)
    FTT_ANY_TO_APPEND_CASE(UInt16)
    FTT_ANY_TO_APPEND_CASE(UInt32)
    FTT_ANY_TO_APPEND_CASE(UInt64)
    FTT_ANY_TO_APPEND_CASE(Float)
    FTT_ANY_TO_APPEND_CASE(Double)
    FTT_ANY_TO_APPEND_CASE(String)
    FTT_ANY_TO_APPEND_CASE(BoolArray)
    FTT_ANY_TO_APPEND_CASE(Int8Array)
    FTT_ANY_TO_APPEND_CASE(Int16Array)
    FTT_ANY_TO_APPEND_CASE(Int32Array)
    FTT_ANY_TO_APPEND_CASE(Int64Array)
    FTT_ANY_TO_APPEND_CASE(UInt8Array)
    FTT_ANY_TO_APPEND_CASE(UInt16Array)
    FTT_ANY_TO_APPEND_CASE(UInt32Array)
    FTT_ANY_TO_APPEND_CASE(UInt64Array)
    FTT_ANY_TO_APPEND_CASE(FloatArray)
    FTT_ANY_TO_APPEND_CASE(DoubleArray)
    FTT_ANY_TO_APPEND_CASE(StringArray)

#undef FTT_ANY_TO_APPEND_CASE

    case fbs::Any::Tuple:
      acc::toAppend('(', result);
      acc::toAppend(*reinterpret_cast<const fbs::Tuple*>(ptr), result);
      acc::toAppend(')', result);
      break;
    case fbs::Any::NONE:
      break;
  }
}

} // namespace ftt

namespace acc {

template <class Tgt>
void toAppend(const ftt::fbs::Tuple& value, Tgt* result) {
  auto types = value.value_type();
  for (size_t i = 0; i < types->size(); i++) {
    if (i > 0) {
      toAppend(',', result);
    }
    ftt::toAppendAny(
        types->GetEnum<ftt::fbs::Any>(i), value.value()->Get(i), result);
  }
}

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
