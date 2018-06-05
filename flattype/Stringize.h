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

inline const char* anyTypeToString(fbs::Any type) {
  return fbs::EnumNameAny(type);
}

inline const char* jsonTypeToString(fbs::Json type) {
  return fbs::EnumNameJson(type);
}

template <class Tgt>
void toAppendAny(fbs::Any type, const void* ptr, Tgt* result);

} // namespace ftt

namespace acc {

template <class Tgt>
void toAppend(const ::flatbuffers::String* value, Tgt* result);

template <class Tgt, class T>
void toAppend(const ::flatbuffers::Vector<T>* value, Tgt* result);

template <class Tgt> void toAppend(const ftt::fbs::Null&,        Tgt*);
template <class Tgt> void toAppend(const ftt::fbs::Bool&,        Tgt*);
template <class Tgt> void toAppend(const ftt::fbs::Int8&,        Tgt*);
template <class Tgt> void toAppend(const ftt::fbs::Int16&,       Tgt*);
template <class Tgt> void toAppend(const ftt::fbs::Int32&,       Tgt*);
template <class Tgt> void toAppend(const ftt::fbs::Int64&,       Tgt*);
template <class Tgt> void toAppend(const ftt::fbs::UInt8&,       Tgt*);
template <class Tgt> void toAppend(const ftt::fbs::UInt16&,      Tgt*);
template <class Tgt> void toAppend(const ftt::fbs::UInt32&,      Tgt*);
template <class Tgt> void toAppend(const ftt::fbs::UInt64&,      Tgt*);
template <class Tgt> void toAppend(const ftt::fbs::Float&,       Tgt*);
template <class Tgt> void toAppend(const ftt::fbs::Double&,      Tgt*);
template <class Tgt> void toAppend(const ftt::fbs::String&,      Tgt*);
template <class Tgt> void toAppend(const ftt::fbs::BoolArray&,   Tgt*);
template <class Tgt> void toAppend(const ftt::fbs::Int8Array&,   Tgt*);
template <class Tgt> void toAppend(const ftt::fbs::Int16Array&,  Tgt*);
template <class Tgt> void toAppend(const ftt::fbs::Int32Array&,  Tgt*);
template <class Tgt> void toAppend(const ftt::fbs::Int64Array&,  Tgt*);
template <class Tgt> void toAppend(const ftt::fbs::UInt8Array&,  Tgt*);
template <class Tgt> void toAppend(const ftt::fbs::UInt16Array&, Tgt*);
template <class Tgt> void toAppend(const ftt::fbs::UInt32Array&, Tgt*);
template <class Tgt> void toAppend(const ftt::fbs::UInt64Array&, Tgt*);
template <class Tgt> void toAppend(const ftt::fbs::FloatArray&,  Tgt*);
template <class Tgt> void toAppend(const ftt::fbs::DoubleArray&, Tgt*);
template <class Tgt> void toAppend(const ftt::fbs::StringArray&, Tgt*);
template <class Tgt> void toAppend(const ftt::fbs::Tuple&,       Tgt*);

} // namespace acc

//////////////////////////////////////////////////////////////////////

#include "accelerator/Conv.h"

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
inline void toAppend(const ::flatbuffers::String* value, Tgt* result) {
  toAppend(acc::StringPiece(value->data(), value->size()), result);
}

template <class Tgt, class T>
inline void toAppend(const ::flatbuffers::Vector<T>* value, Tgt* result) {
  auto it = value->begin();
  if (it != value->end()) {
    toAppend(*it, result);
  }
  while (++it != value->end()) {
    toAppend(',', result);
    toAppend(*it, result);
  }
}

template <class Tgt>
inline void toAppend(const ftt::fbs::Null& value, Tgt* result) {
  toAppend("<NULL>", result);
}

#define FTT_BASE_TO_APPEND(ft) \
template <class Tgt> \
inline void toAppend(const ftt::fbs::ft& value, Tgt* result) { \
  toAppend(value.value(), result); \
}

FTT_BASE_TO_APPEND(Bool)
FTT_BASE_TO_APPEND(Int8)
FTT_BASE_TO_APPEND(Int16)
FTT_BASE_TO_APPEND(Int32)
FTT_BASE_TO_APPEND(Int64)
FTT_BASE_TO_APPEND(UInt8)
FTT_BASE_TO_APPEND(UInt16)
FTT_BASE_TO_APPEND(UInt32)
FTT_BASE_TO_APPEND(UInt64)
FTT_BASE_TO_APPEND(Float)
FTT_BASE_TO_APPEND(Double)
FTT_BASE_TO_APPEND(String)
FTT_BASE_TO_APPEND(BoolArray)
FTT_BASE_TO_APPEND(Int8Array)
FTT_BASE_TO_APPEND(Int16Array)
FTT_BASE_TO_APPEND(Int32Array)
FTT_BASE_TO_APPEND(Int64Array)
FTT_BASE_TO_APPEND(UInt8Array)
FTT_BASE_TO_APPEND(UInt16Array)
FTT_BASE_TO_APPEND(UInt32Array)
FTT_BASE_TO_APPEND(UInt64Array)
FTT_BASE_TO_APPEND(FloatArray)
FTT_BASE_TO_APPEND(DoubleArray)
FTT_BASE_TO_APPEND(StringArray)

#undef FTT_BASE_TO_APPEND

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

} // namespace acc
