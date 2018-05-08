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

#include <string>
#include <vector>

#include "flattype/idl/base_generated.h"

namespace ftt {

inline bool operator==(
    const ::flatbuffers::String& lhs, const ::flatbuffers::String& rhs) {
  return (lhs.size() == rhs.size() &&
          memcmp(lhs.data(), rhs.data(), lhs.size()) == 0);
}

inline bool operator!=(
    const ::flatbuffers::String& lhs, const ::flatbuffers::String& rhs) {
  return !(lhs == rhs);
}

//////////////////////////////////////////////////////////////////////

inline bool operator==(const fbs::Null& lhs, const fbs::Null& rhs) {
  return true;
}

#define FTT_BASE_EQUAL_BASE(ft) \
inline bool operator==(const fbs::ft& lhs, const fbs::ft& rhs) { \
  return lhs.value() == rhs.value(); \
}

FTT_BASE_EQUAL_BASE(Bool)
FTT_BASE_EQUAL_BASE(Int8)
FTT_BASE_EQUAL_BASE(Int16)
FTT_BASE_EQUAL_BASE(Int32)
FTT_BASE_EQUAL_BASE(Int64)
FTT_BASE_EQUAL_BASE(UInt8)
FTT_BASE_EQUAL_BASE(UInt16)
FTT_BASE_EQUAL_BASE(UInt32)
FTT_BASE_EQUAL_BASE(UInt64)
FTT_BASE_EQUAL_BASE(Float)  // treat as literal equal for performance
FTT_BASE_EQUAL_BASE(Double) // same as above

#undef FTT_BASE_EQUAL_BASE

inline bool operator==(const fbs::String& lhs, const fbs::String& rhs) {
  return *lhs.value() == *rhs.value();
}

#define FTT_BASE_EQUAL_ARRAY(ft) \
inline bool operator==(const fbs::ft& lhs, const fbs::ft& rhs) { \
  auto lvalue = lhs.value(); \
  auto rvalue = rhs.value(); \
  if (lvalue->size() != rvalue->size()) { \
    return false; \
  } \
  return memcmp(lvalue->data(), rvalue->data(), lvalue->size()) == 0; \
}

FTT_BASE_EQUAL_ARRAY(BoolArray)
FTT_BASE_EQUAL_ARRAY(Int8Array)
FTT_BASE_EQUAL_ARRAY(Int16Array)
FTT_BASE_EQUAL_ARRAY(Int32Array)
FTT_BASE_EQUAL_ARRAY(Int64Array)
FTT_BASE_EQUAL_ARRAY(UInt8Array)
FTT_BASE_EQUAL_ARRAY(UInt16Array)
FTT_BASE_EQUAL_ARRAY(UInt32Array)
FTT_BASE_EQUAL_ARRAY(UInt64Array)
FTT_BASE_EQUAL_ARRAY(FloatArray)  // treat as literal equal for performance
FTT_BASE_EQUAL_ARRAY(DoubleArray) // same as above

#undef FTT_BASE_EQUAL_ARRAY

inline bool operator==(
    const fbs::StringArray& lhs, const fbs::StringArray& rhs) {
  auto lvalue = lhs.value();
  auto rvalue = rhs.value();
  if (lvalue->size() != rvalue->size()) {
    return false;
  }
  for (::flatbuffers::uoffset_t i = 0; i < lvalue->size(); i++) {
    if (*lvalue->Get(i) != *rvalue->Get(i)) {
      return false;
    }
  }
  return true;
}

#define FTT_BASE_UNEQUAL(ft) \
inline bool operator!=(const fbs::ft& lhs, const fbs::ft& rhs) { \
  return !(lhs == rhs); \
}

FTT_BASE_UNEQUAL(Null)
FTT_BASE_UNEQUAL(Bool)
FTT_BASE_UNEQUAL(Int8)
FTT_BASE_UNEQUAL(Int16)
FTT_BASE_UNEQUAL(Int32)
FTT_BASE_UNEQUAL(Int64)
FTT_BASE_UNEQUAL(UInt8)
FTT_BASE_UNEQUAL(UInt16)
FTT_BASE_UNEQUAL(UInt32)
FTT_BASE_UNEQUAL(UInt64)
FTT_BASE_UNEQUAL(Float)
FTT_BASE_UNEQUAL(Double)
FTT_BASE_UNEQUAL(String)
FTT_BASE_UNEQUAL(BoolArray)
FTT_BASE_UNEQUAL(Int8Array)
FTT_BASE_UNEQUAL(Int16Array)
FTT_BASE_UNEQUAL(Int32Array)
FTT_BASE_UNEQUAL(Int64Array)
FTT_BASE_UNEQUAL(UInt8Array)
FTT_BASE_UNEQUAL(UInt16Array)
FTT_BASE_UNEQUAL(UInt32Array)
FTT_BASE_UNEQUAL(UInt64Array)
FTT_BASE_UNEQUAL(FloatArray)
FTT_BASE_UNEQUAL(DoubleArray)
FTT_BASE_UNEQUAL(StringArray)

#undef FTT_BASE_UNEQUAL

} // namespace ftt
