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

#include <algorithm>
#include <string>
#include <vector>

#include "flattype/CommonIDLs.h"

namespace ftt {

inline bool operator==(
    const ::flatbuffers::String& lhs, const ::flatbuffers::String& rhs) {
  return strcmp(lhs.c_str(), rhs.c_str()) == 0;
}

template <class FT>
inline bool operator==(
    const ::flatbuffers::Vector<flatbuffers::Offset<FT>>& lhs,
    const ::flatbuffers::Vector<flatbuffers::Offset<FT>>& rhs) {
  return lhs.size() == rhs.size() &&
    std::equal(
        lhs.begin(), lhs.end(),
        rhs.begin(),
        [](const FT* a, const FT* b) { return *a == *b; });
}

template <class FT>
inline bool operator<(
    const ::flatbuffers::Vector<flatbuffers::Offset<FT>>& lhs,
    const ::flatbuffers::Vector<flatbuffers::Offset<FT>>& rhs) {
  return std::lexicographical_compare(
      lhs.begin(), lhs.end(),
      rhs.begin(), rhs.end(),
      [](const FT* a, const FT* b) { return *a < *b; });
}

//////////////////////////////////////////////////////////////////////

inline bool operator==(const fbs::Null&, const fbs::Null&) {
  return true;
}
inline bool operator<(const fbs::Null&, const fbs::Null&) {
  return false;
}

#define FTT_BASE_COMP_BASE(ft) \
inline bool operator==(const fbs::ft& lhs, const fbs::ft& rhs) { \
  return lhs.value() == rhs.value(); \
} \
inline bool operator<(const fbs::ft& lhs, const fbs::ft& rhs) { \
  return lhs.value() < rhs.value(); \
}

FTT_BASE_COMP_BASE(Bool)
FTT_BASE_COMP_BASE(Int8)
FTT_BASE_COMP_BASE(Int16)
FTT_BASE_COMP_BASE(Int32)
FTT_BASE_COMP_BASE(Int64)
FTT_BASE_COMP_BASE(UInt8)
FTT_BASE_COMP_BASE(UInt16)
FTT_BASE_COMP_BASE(UInt32)
FTT_BASE_COMP_BASE(UInt64)
FTT_BASE_COMP_BASE(Float)  // treat as literal equal for performance
FTT_BASE_COMP_BASE(Double) // same as above

#undef FTT_BASE_COMP_BASE

inline bool operator==(const fbs::String& lhs, const fbs::String& rhs) {
  return *lhs.value() == *rhs.value();
}
inline bool operator<(const fbs::String& lhs, const fbs::String& rhs) {
  return *lhs.value() < *rhs.value();
}

#define FTT_BASE_COMP_ARRAY(ft) \
inline bool operator==(const fbs::ft& lhs, const fbs::ft& rhs) { \
  auto lvalue = lhs.value(); \
  auto rvalue = rhs.value(); \
  if (lvalue->size() != rvalue->size()) { \
    return false; \
  } \
  return memcmp(lvalue->data(), rvalue->data(), lvalue->size()) == 0; \
} \
inline bool operator<(const fbs::ft& lhs, const fbs::ft& rhs) { \
  return std::lexicographical_compare( \
      lhs.value()->begin(), lhs.value()->end(), \
      rhs.value()->begin(), rhs.value()->end()); \
}

FTT_BASE_COMP_ARRAY(BoolArray)
FTT_BASE_COMP_ARRAY(Int8Array)
FTT_BASE_COMP_ARRAY(Int16Array)
FTT_BASE_COMP_ARRAY(Int32Array)
FTT_BASE_COMP_ARRAY(Int64Array)
FTT_BASE_COMP_ARRAY(UInt8Array)
FTT_BASE_COMP_ARRAY(UInt16Array)
FTT_BASE_COMP_ARRAY(UInt32Array)
FTT_BASE_COMP_ARRAY(UInt64Array)
FTT_BASE_COMP_ARRAY(FloatArray)  // treat as literal equal for performance
FTT_BASE_COMP_ARRAY(DoubleArray) // same as above

#undef FTT_BASE_COMP_ARRAY

inline bool operator==(
    const fbs::StringArray& lhs, const fbs::StringArray& rhs) {
  return *lhs.value() == *rhs.value();
}

inline bool operator<(
    const fbs::StringArray& lhs, const fbs::StringArray& rhs) {
  return *lhs.value() < *rhs.value();
}

} // namespace ftt
