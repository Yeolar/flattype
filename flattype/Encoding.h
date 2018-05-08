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

#include <string>
#include <vector>

#include "accelerator/FBString.h"
#include "accelerator/Range.h"
#include "flattype/idl/base_generated.h"

namespace ftt {

// null encoding
inline ::flatbuffers::Offset<fbs::Null>
encode(::flatbuffers::FlatBufferBuilder& fbb, const void*&) {
  return fbs::CreateNull(fbb);
}

// null decoding
inline void
decode(const void* ptr, void*& value) {
  value = nullptr;
}

#define FTT_BASE_ENCODE_BASE(t, ft) \
inline ::flatbuffers::Offset<fbs::ft> \
encode(::flatbuffers::FlatBufferBuilder& fbb, const t& value) { \
  return fbs::Create##ft(fbb, value); \
}

#define FTT_BASE_DECODE_BASE(t, ft) \
inline void \
decode(const void* ptr, t& value) { \
  value = reinterpret_cast<const fbs::ft*>(ptr)->value(); \
}

FTT_BASE_ENCODE_BASE(bool,     Bool)
FTT_BASE_ENCODE_BASE(int8_t,   Int8)
FTT_BASE_ENCODE_BASE(int16_t,  Int16)
FTT_BASE_ENCODE_BASE(int32_t,  Int32)
FTT_BASE_ENCODE_BASE(int64_t,  Int64)
FTT_BASE_ENCODE_BASE(uint8_t,  UInt8)
FTT_BASE_ENCODE_BASE(uint16_t, UInt16)
FTT_BASE_ENCODE_BASE(uint32_t, UInt32)
FTT_BASE_ENCODE_BASE(uint64_t, UInt64)
FTT_BASE_ENCODE_BASE(float,    Float)
FTT_BASE_ENCODE_BASE(double,   Double)

FTT_BASE_DECODE_BASE(bool,     Bool)
FTT_BASE_DECODE_BASE(int8_t,   Int8)
FTT_BASE_DECODE_BASE(int16_t,  Int16)
FTT_BASE_DECODE_BASE(int32_t,  Int32)
FTT_BASE_DECODE_BASE(int64_t,  Int64)
FTT_BASE_DECODE_BASE(uint8_t,  UInt8)
FTT_BASE_DECODE_BASE(uint16_t, UInt16)
FTT_BASE_DECODE_BASE(uint32_t, UInt32)
FTT_BASE_DECODE_BASE(uint64_t, UInt64)
FTT_BASE_DECODE_BASE(float,    Float)
FTT_BASE_DECODE_BASE(double,   Double)

#undef FTT_BASE_ENCODE_BASE
#undef FTT_BASE_DECODE_BASE

// string encoding
inline ::flatbuffers::Offset<fbs::String>
encode(::flatbuffers::FlatBufferBuilder& fbb, const std::string& value) {
  return fbs::CreateString(fbb, fbb.CreateString(value));
}

// string decoding
inline void
decode(const void* ptr, std::string& value) {
  auto p = reinterpret_cast<const fbs::String*>(ptr);
  value.assign(p->value()->data(), p->value()->size());
}

// fbstring encoding
inline ::flatbuffers::Offset<fbs::String>
encode(::flatbuffers::FlatBufferBuilder& fbb, const acc::fbstring& value) {
  return fbs::CreateString(fbb, fbb.CreateString(value.data(), value.size()));
}

// fbstring decoding
inline void
decode(const void* ptr, acc::fbstring& value) {
  auto p = reinterpret_cast<const fbs::String*>(ptr);
  value.assign(p->value()->data(), p->value()->size());
}

// StringPiece encoding
inline ::flatbuffers::Offset<fbs::String>
encode(::flatbuffers::FlatBufferBuilder& fbb, acc::StringPiece value) {
  return fbs::CreateString(fbb, fbb.CreateString(value.data(), value.size()));
}

// StringPiece decoding (no copy)
inline void
decode(const void* ptr, acc::StringPiece& value) {
  auto p = reinterpret_cast<const fbs::String*>(ptr);
  value.reset(p->value()->data(), p->value()->size());
}

#define FTT_BASE_ENCODE_ARRAY(t, ft) \
inline ::flatbuffers::Offset<fbs::ft##Array> \
encode(::flatbuffers::FlatBufferBuilder& fbb, const std::vector<t>& value) { \
  return fbs::Create##ft##ArrayDirect(fbb, &value); \
}

#define FTT_BASE_DECODE_ARRAY(t, ft) \
inline void \
decode(const void* ptr, std::vector<t>& value) { \
  auto p = reinterpret_cast<const fbs::ft##Array*>(ptr); \
  for (const auto& i : *p->value()) { \
    value.push_back(i); \
  } \
}

// bool -> special case
FTT_BASE_ENCODE_ARRAY(int8_t,   Int8)
FTT_BASE_ENCODE_ARRAY(int16_t,  Int16)
FTT_BASE_ENCODE_ARRAY(int32_t,  Int32)
FTT_BASE_ENCODE_ARRAY(int64_t,  Int64)
FTT_BASE_ENCODE_ARRAY(uint8_t,  UInt8)
FTT_BASE_ENCODE_ARRAY(uint16_t, UInt16)
FTT_BASE_ENCODE_ARRAY(uint32_t, UInt32)
FTT_BASE_ENCODE_ARRAY(uint64_t, UInt64)
FTT_BASE_ENCODE_ARRAY(float,    Float)
FTT_BASE_ENCODE_ARRAY(double,   Double)

FTT_BASE_DECODE_ARRAY(bool,     Bool)
FTT_BASE_DECODE_ARRAY(int8_t,   Int8)
FTT_BASE_DECODE_ARRAY(int16_t,  Int16)
FTT_BASE_DECODE_ARRAY(int32_t,  Int32)
FTT_BASE_DECODE_ARRAY(int64_t,  Int64)
FTT_BASE_DECODE_ARRAY(uint8_t,  UInt8)
FTT_BASE_DECODE_ARRAY(uint16_t, UInt16)
FTT_BASE_DECODE_ARRAY(uint32_t, UInt32)
FTT_BASE_DECODE_ARRAY(uint64_t, UInt64)
FTT_BASE_DECODE_ARRAY(float,    Float)
FTT_BASE_DECODE_ARRAY(double,   Double)

#undef FTT_BASE_ENCODE_ARRAY
#undef FTT_BASE_DECODE_ARRAY

// vector<bool> encoding
inline ::flatbuffers::Offset<fbs::BoolArray>
encode(::flatbuffers::FlatBufferBuilder& fbb, const std::vector<bool>& value) {
  std::vector<uint8_t> copy;
  for (auto i : value) {
    copy.push_back(uint8_t(i));
  }
  return fbs::CreateBoolArrayDirect(fbb, &copy);
}

// vector<string> encoding
inline ::flatbuffers::Offset<fbs::StringArray>
encode(::flatbuffers::FlatBufferBuilder& fbb,
       const std::vector<std::string>& value) {
  std::vector<flatbuffers::Offset<flatbuffers::String>> v;
  for (auto& i : value) {
    v.push_back(fbb.CreateString(i));
  }
  return fbs::CreateStringArrayDirect(fbb, &v);
}

// vector<string> decoding
inline void
decode(const void* ptr, std::vector<std::string>& value) {
  auto p = reinterpret_cast<const fbs::StringArray*>(ptr);
  for (auto i : *p->value()) {
    value.push_back(i->str());
  }
}

// vector<fbstring> encoding
inline ::flatbuffers::Offset<fbs::StringArray>
encode(::flatbuffers::FlatBufferBuilder& fbb,
       const std::vector<acc::fbstring>& value) {
  std::vector<flatbuffers::Offset<flatbuffers::String>> v;
  for (auto& i : value) {
    v.push_back(fbb.CreateString(i.data(), i.size()));
  }
  return fbs::CreateStringArrayDirect(fbb, &v);
}

// vector<fbstring> decoding
inline void
decode(const void* ptr, std::vector<acc::fbstring>& value) {
  auto p = reinterpret_cast<const fbs::StringArray*>(ptr);
  for (auto i : *p->value()) {
    value.emplace_back(i->data(), i->size());
  }
}

// vector<StringPiece> encoding
inline ::flatbuffers::Offset<fbs::StringArray>
encode(::flatbuffers::FlatBufferBuilder& fbb,
       const std::vector<acc::StringPiece>& value) {
  std::vector<flatbuffers::Offset<flatbuffers::String>> v;
  for (auto& i : value) {
    v.push_back(fbb.CreateString(i.data(), i.size()));
  }
  return fbs::CreateStringArrayDirect(fbb, &v);
}

// vector<StringPiece> decoding (no copy)
inline void
decode(const void* ptr, std::vector<acc::StringPiece>& value) {
  auto p = reinterpret_cast<const fbs::StringArray*>(ptr);
  for (auto i : *p->value()) {
    value.emplace_back(i->data(), i->size());
  }
}

} // namespace ftt
