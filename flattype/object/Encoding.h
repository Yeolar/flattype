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

#include <map>
#include <string>
#include <vector>

#include "accelerator/FBString.h"
#include "accelerator/Range.h"
#include "flattype/idl/object_generated.h"
#include "flattype/Type.h"

namespace ftt {
namespace json {

template <class T>
using vvector = std::vector<std::vector<T>>;

// null encoding
inline ::flatbuffers::Offset<fbs::Null>
encode(::flatbuffers::FlatBufferBuilder& fbb, const std::nullptr_t&) {
  return fbs::CreateNull(fbb);
}

// null decoding
inline void
decode(const void* ptr, std::nullptr_t& value) {
  value = nullptr;
}

#define FTT_JSON_ENCODE_BASE(t, ft) \
inline ::flatbuffers::Offset<fbs::ft> \
encode(::flatbuffers::FlatBufferBuilder& fbb, const t& value) { \
  return fbs::Create##ft(fbb, value); \
}

#define FTT_JSON_DECODE_BASE(t, ft) \
inline void \
decode(const void* ptr, t& value) { \
  value = reinterpret_cast<const fbs::ft*>(ptr)->value(); \
}

FTT_JSON_ENCODE_BASE(bool,     Bool)
FTT_JSON_ENCODE_BASE(int8_t,   Int64)
FTT_JSON_ENCODE_BASE(int16_t,  Int64)
FTT_JSON_ENCODE_BASE(int32_t,  Int64)
FTT_JSON_ENCODE_BASE(int64_t,  Int64)
FTT_JSON_ENCODE_BASE(uint8_t,  Int64)
FTT_JSON_ENCODE_BASE(uint16_t, Int64)
FTT_JSON_ENCODE_BASE(uint32_t, Int64)
FTT_JSON_ENCODE_BASE(uint64_t, Int64)
FTT_JSON_ENCODE_BASE(float,    Double)
FTT_JSON_ENCODE_BASE(double,   Double)

FTT_JSON_DECODE_BASE(bool,     Bool)
FTT_JSON_DECODE_BASE(int8_t,   Int64)
FTT_JSON_DECODE_BASE(int16_t,  Int64)
FTT_JSON_DECODE_BASE(int32_t,  Int64)
FTT_JSON_DECODE_BASE(int64_t,  Int64)
FTT_JSON_DECODE_BASE(uint8_t,  Int64)
FTT_JSON_DECODE_BASE(uint16_t, Int64)
FTT_JSON_DECODE_BASE(uint32_t, Int64)
FTT_JSON_DECODE_BASE(uint64_t, Int64)
FTT_JSON_DECODE_BASE(float,    Double)
FTT_JSON_DECODE_BASE(double,   Double)

#undef FTT_JSON_ENCODE_BASE
#undef FTT_JSON_DECODE_BASE

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

// const char* encoding
inline ::flatbuffers::Offset<fbs::String>
encode(::flatbuffers::FlatBufferBuilder& fbb, const char* value) {
  return fbs::CreateString(fbb, fbb.CreateString(value));
}

// const char* decoding (no copy)
inline void
decode(const void* ptr, const char*& value) {
  auto p = reinterpret_cast<const fbs::String*>(ptr);
  value = p->value()->data();
}

// Array decoding
inline fbs::Json
decodeOneType(const fbs::Array* ptr, int i) {
  return ptr->value_type()->GetEnum<fbs::Json>(i);
}

template <class T>
inline void
decodeOne(const fbs::Array* ptr, int i, T& arg) {
  assert(getJsonType<T>() == ptr->value_type()->GetEnum<Json>(i));
  decode(ptr->value()->Get(i), arg);
}

// pair<S, V> encoding
template <class S, class V>
inline ::flatbuffers::Offset<fbs::Pair>
encode(::flatbuffers::FlatBufferBuilder& fbb, const std::pair<S, V>& value) {
  return fbs::CreatePairDirect(
      fbb,
      getJsonType<V>(),
      encode(fbb, value.second).Union(),
      value.first.data());
}

// pair<S, V> decoding
template <class S, class V>
inline void
decode(const void* ptr, std::pair<S, V>& value) {
  auto p = reinterpret_cast<const fbs::Pair*>(ptr);
  decode(p->name(), value.first);
  decode(p->value(), value.second);
}

// vector<pair<S, V>> encoding
template <class S, class V>
inline ::flatbuffers::Offset<fbs::Object>
encode(::flatbuffers::FlatBufferBuilder& fbb,
       const std::vector<std::pair<S, V>>& value) {
  std::vector<flatbuffers::Offset<fbs::Pair>> values;
  for (auto& i : value) {
    values.push_back(encode(fbb, i));
  }
  return fbs::CreateObjectDirect(fbb, &values);
}

// vector<pair<S, V>> decoding
template <class S, class V>
inline void
decode(const void* ptr, std::vector<std::pair<S, V>>& value) {
  auto p = reinterpret_cast<const fbs::Array*>(ptr);
  for (auto i : *p->value()) {
    std::pair<S, V> item;
    decode(i, item);
    value.push_back(std::move(item));
  }
}

// map<S, V> encoding
template <class S, class V>
inline ::flatbuffers::Offset<fbs::Object>
encode(::flatbuffers::FlatBufferBuilder& fbb, const std::map<S, V>& value) {
  std::vector<flatbuffers::Offset<fbs::Pair>> values;
  for (auto& i : value) {
    values.push_back(encode(fbb, i));
  }
  return fbs::CreateObjectDirect(fbb, &values);
}

// map<S, V> decoding
template <class S, class V>
inline void
decode(const void* ptr, std::map<S, V>& value) {
  auto p = reinterpret_cast<const fbs::Object*>(ptr);
  for (auto i : *p->value()) {
    std::pair<S, V> item;
    decode(i, item);
    value.insert(std::move(item));
  }
}

// vector<vector<T>> encoding
template <class T>
inline ::flatbuffers::Offset<fbs::Array>
encode(::flatbuffers::FlatBufferBuilder& fbb, const vvector<T>& value) {
  std::vector<uint8_t> rowTypes;
  std::vector<flatbuffers::Offset<void>> rowValues;
  for (auto& row : value) {
    rowTypes.push_back(fbs::Json_Array);
    rowValues.push_back(encode(fbb, row).Union());
  }
  return fbs::CreateArrayDirect(fbb, &rowTypes, &rowValues);
}

// vector<vector<T>> decoding
template <class T>
inline void
decode(const void* ptr, vvector<T>& value) {
  auto p = reinterpret_cast<const fbs::Array*>(ptr);
  for (auto row : *p->value()) {
    std::vector<T> rowValue;
    decode(row, rowValue);
    value.push_back(std::move(rowValue));
  }
}

//////////////////////////////////////////////////////////////////////

namespace detail {

template <class T>
inline void
vencodeImpl(::flatbuffers::FlatBufferBuilder& fbb,
            std::vector<uint8_t>& types,
            std::vector<flatbuffers::Offset<void>>& values,
            const T& arg) {
  types.push_back(getJsonType<T>());
  values.push_back(encode(fbb, arg).Union());
}

template <class T, class... Args>
inline void
vencodeImpl(::flatbuffers::FlatBufferBuilder& fbb,
            std::vector<uint8_t>& types,
            std::vector<flatbuffers::Offset<void>>& values,
            const T& arg, const Args&... args) {
  vencodeImpl(fbb, types, values, arg);
  vencodeImpl(fbb, types, values, args...);
}

template <int I, class T>
inline void
vdecodeImpl(const ::flatbuffers::Vector<uint8_t>*,
            const ::flatbuffers::Vector<flatbuffers::Offset<void>>* values,
            T& arg) {
  assert(getJsonType<T>() == types->GetEnum<Json>(i));
  decode(values->Get(I), arg);
}

template <int I, class T, class... Args>
inline void
vdecodeImpl(const ::flatbuffers::Vector<uint8_t>* types,
            const ::flatbuffers::Vector<flatbuffers::Offset<void>>* values,
            T& arg, Args&... args) {
  vdecodeImpl<I>(types, values, arg);
  vdecodeImpl<I+1>(types, values, args...);
}

}  // namespace detail

// Array encoding
template <class... Args>
inline ::flatbuffers::Offset<fbs::Array>
vencode(::flatbuffers::FlatBufferBuilder& fbb, const Args&... args) {
  std::vector<uint8_t> types;
  std::vector<flatbuffers::Offset<void>> values;
  detail::vencodeImpl(fbb, types, values, args...);
  return fbs::CreateArrayDirect(fbb, &types, &values);
}

// Array decoding
template <class... Args>
inline void
vdecode(const fbs::Array* ptr, Args&... args) {
  detail::vdecodeImpl<0>(ptr->value_type(), ptr->value(), args...);
}

// Pair decoding
template <class S, class V>
inline void
decode(const fbs::Pair* pair, std::pair<S, V>& value) {
  assert(pair->value_type() == getJsonType<V>());
  decode(pair->name(), value.first);
  decode(pair->value(), value.second);
}

// Object decoding
template <int I, class S, class V>
inline void
decode(const fbs::Object* object, std::pair<S, V>& arg) {
  decode(object->value()->Get(I), arg);
}

template <int I, class S, class V, class... Args>
inline void
decode(const fbs::Object* object, std::pair<S, V>& arg, Args&... args) {
  decode<I>(object, arg);
  decode<I+1>(object, args...);
}

template <class... Args>
inline void
decode(const fbs::Object* object, Args&... args) {
  decode<0>(object, args...);
}

} // namespace json
} // namespace ftt