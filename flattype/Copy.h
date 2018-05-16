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

// Null copy
inline ::flatbuffers::Offset<fbs::Null>
copy(::flatbuffers::FlatBufferBuilder& fbb, const fbs::Null& obj) {
  return fbs::CreateNull(fbb);
}

#define FTT_BASE_COPY_BASE(ft) \
inline ::flatbuffers::Offset<fbs::ft> \
copy(::flatbuffers::FlatBufferBuilder& fbb, const fbs::ft& obj) { \
  return fbs::Create##ft(fbb, obj.value()); \
}

FTT_BASE_COPY_BASE(Bool)
FTT_BASE_COPY_BASE(Int8)
FTT_BASE_COPY_BASE(Int16)
FTT_BASE_COPY_BASE(Int32)
FTT_BASE_COPY_BASE(Int64)
FTT_BASE_COPY_BASE(UInt8)
FTT_BASE_COPY_BASE(UInt16)
FTT_BASE_COPY_BASE(UInt32)
FTT_BASE_COPY_BASE(UInt64)
FTT_BASE_COPY_BASE(Float)
FTT_BASE_COPY_BASE(Double)

#undef FTT_BASE_COPY_BASE

// String copy
inline ::flatbuffers::Offset<fbs::String>
copy(::flatbuffers::FlatBufferBuilder& fbb, const fbs::String& obj) {
  return fbs::CreateString(fbb, fbb.CreateString(obj.value()));
}

#define FTT_BASE_COPY_ARRAY(t, ft) \
inline ::flatbuffers::Offset<fbs::ft##Array> \
copy(::flatbuffers::FlatBufferBuilder& fbb, const fbs::ft##Array& obj) { \
  return fbs::Create##ft##Array( \
      fbb, \
      fbb.CreateVector<t>(obj.value()->data(), obj.value()->size())); \
}

FTT_BASE_COPY_ARRAY(uint8_t,  Bool)
FTT_BASE_COPY_ARRAY(int8_t,   Int8)
FTT_BASE_COPY_ARRAY(int16_t,  Int16)
FTT_BASE_COPY_ARRAY(int32_t,  Int32)
FTT_BASE_COPY_ARRAY(int64_t,  Int64)
FTT_BASE_COPY_ARRAY(uint8_t,  UInt8)
FTT_BASE_COPY_ARRAY(uint16_t, UInt16)
FTT_BASE_COPY_ARRAY(uint32_t, UInt32)
FTT_BASE_COPY_ARRAY(uint64_t, UInt64)
FTT_BASE_COPY_ARRAY(float,    Float)
FTT_BASE_COPY_ARRAY(double,   Double)

#undef FTT_BASE_COPY_ARRAY

// StringArray copy
inline ::flatbuffers::Offset<fbs::StringArray>
copy(::flatbuffers::FlatBufferBuilder& fbb, const fbs::StringArray& obj) {
  std::vector<flatbuffers::Offset<flatbuffers::String>> v;
  for (auto i : *obj.value()) {
    v.push_back(fbb.CreateString(i));
  }
  return fbs::CreateStringArrayDirect(fbb, &v);
}

} // namespace ftt
