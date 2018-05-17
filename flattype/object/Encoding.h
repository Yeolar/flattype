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

#include "accelerator/dynamic.h"
#include "accelerator/FBString.h"
#include "accelerator/Range.h"
#include "flattype/CommonIDLs.h"
#include "flattype/Type.h"

namespace ftt {

template <class T>
using vvector = std::vector<std::vector<T>>;

// null encoding
inline ::flatbuffers::Offset<fbs::Null>
encodeJson(::flatbuffers::FlatBufferBuilder& fbb, const std::nullptr_t&) {
  return fbs::CreateNull(fbb);
}

// null decoding
inline void
decodeJson(const void* ptr, std::nullptr_t& value) {
  value = nullptr;
}

#define FTT_JSON_ENCODE_BASE(t, ft) \
inline ::flatbuffers::Offset<fbs::ft> \
encodeJson(::flatbuffers::FlatBufferBuilder& fbb, const t& value) { \
  return fbs::Create##ft(fbb, value); \
}

#define FTT_JSON_DECODE_BASE(t, ft) \
inline void \
decodeJson(const void* ptr, t& value) { \
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
encodeJson(::flatbuffers::FlatBufferBuilder& fbb, const std::string& value) {
  return fbs::CreateString(fbb, fbb.CreateString(value));
}

// string decoding
inline void
decodeJson(const void* ptr, std::string& value) {
  auto p = reinterpret_cast<const fbs::String*>(ptr);
  value.assign(p->value()->data(), p->value()->size());
}

// fbstring encoding
inline ::flatbuffers::Offset<fbs::String>
encodeJson(::flatbuffers::FlatBufferBuilder& fbb, const acc::fbstring& value) {
  return fbs::CreateString(fbb, fbb.CreateString(value.data(), value.size()));
}

// fbstring decoding
inline void
decodeJson(const void* ptr, acc::fbstring& value) {
  auto p = reinterpret_cast<const fbs::String*>(ptr);
  value.assign(p->value()->data(), p->value()->size());
}

// StringPiece encoding
inline ::flatbuffers::Offset<fbs::String>
encodeJson(::flatbuffers::FlatBufferBuilder& fbb, acc::StringPiece value) {
  return fbs::CreateString(fbb, fbb.CreateString(value.data(), value.size()));
}

// StringPiece decoding (no copy)
inline void
decodeJson(const void* ptr, acc::StringPiece& value) {
  auto p = reinterpret_cast<const fbs::String*>(ptr);
  value.reset(p->value()->data(), p->value()->size());
}

// const char* encoding
inline ::flatbuffers::Offset<fbs::String>
encodeJson(::flatbuffers::FlatBufferBuilder& fbb, const char* value) {
  return fbs::CreateString(fbb, fbb.CreateString(value));
}

// const char* decoding (no copy)
inline void
decodeJson(const void* ptr, const char*& value) {
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
  decodeJson(ptr->value()->Get(i), arg);
}

// pair<S, V> encoding
template <class S, class V>
inline ::flatbuffers::Offset<fbs::Pair>
encodeJson(::flatbuffers::FlatBufferBuilder& fbb,
           const std::pair<S, V>& value) {
  return fbs::CreatePairDirect(
      fbb,
      getJsonType<V>(),
      encodeJson(fbb, value.second).Union(),
      value.first.data());
}

// pair<S, V> decoding
template <class S, class V>
inline void
decodeJson(const void* ptr, std::pair<S, V>& value) {
  auto p = reinterpret_cast<const fbs::Pair*>(ptr);
  decodeJson(p->name(), value.first);
  decodeJson(p->value(), value.second);
}

// vector<pair<S, V>> encoding
template <class S, class V>
inline ::flatbuffers::Offset<fbs::Object>
encodeJson(::flatbuffers::FlatBufferBuilder& fbb,
       const std::vector<std::pair<S, V>>& value) {
  std::vector<flatbuffers::Offset<fbs::Pair>> values;
  for (auto& i : value) {
    values.push_back(encodeJson(fbb, i));
  }
  return fbs::CreateObjectDirect(fbb, &values);
}

// vector<pair<S, V>> decoding
template <class S, class V>
inline void
decodeJson(const void* ptr, std::vector<std::pair<S, V>>& value) {
  auto p = reinterpret_cast<const fbs::Array*>(ptr);
  for (auto i : *p->value()) {
    std::pair<S, V> item;
    decodeJson(i, item);
    value.push_back(std::move(item));
  }
}

// map<S, V> encoding
template <class S, class V>
inline ::flatbuffers::Offset<fbs::Object>
encodeJson(::flatbuffers::FlatBufferBuilder& fbb, const std::map<S, V>& value) {
  std::vector<flatbuffers::Offset<fbs::Pair>> values;
  for (auto& i : value) {
    values.push_back(encodeJson(fbb, i));
  }
  return fbs::CreateObjectDirect(fbb, &values);
}

// map<S, V> decoding
template <class S, class V>
inline void
decodeJson(const void* ptr, std::map<S, V>& value) {
  auto p = reinterpret_cast<const fbs::Object*>(ptr);
  for (auto i : *p->value()) {
    std::pair<S, V> item;
    decodeJson(i, item);
    value.insert(std::move(item));
  }
}

// vector<vector<T>> encoding
template <class T>
inline ::flatbuffers::Offset<fbs::Array>
encodeJson(::flatbuffers::FlatBufferBuilder& fbb, const vvector<T>& value) {
  std::vector<uint8_t> rowTypes;
  std::vector<flatbuffers::Offset<void>> rowValues;
  for (auto& row : value) {
    rowTypes.push_back(acc::to<uint8_t>(fbs::Json::Array));
    rowValues.push_back(encodeJson(fbb, row).Union());
  }
  return fbs::CreateArrayDirect(fbb, &rowTypes, &rowValues);
}

// vector<vector<T>> decoding
template <class T>
inline void
decodeJson(const void* ptr, vvector<T>& value) {
  auto p = reinterpret_cast<const fbs::Array*>(ptr);
  for (auto row : *p->value()) {
    std::vector<T> rowValue;
    decodeJson(row, rowValue);
    value.push_back(std::move(rowValue));
  }
}

//////////////////////////////////////////////////////////////////////

namespace detail {

template <class T>
inline void
vencodeJsonImpl(::flatbuffers::FlatBufferBuilder& fbb,
                std::vector<uint8_t>& types,
                std::vector<flatbuffers::Offset<void>>& values,
                const T& arg) {
  types.push_back(getJsonType<T>());
  values.push_back(encodeJson(fbb, arg).Union());
}

template <class T, class... Args>
inline void
vencodeJsonImpl(::flatbuffers::FlatBufferBuilder& fbb,
                std::vector<uint8_t>& types,
                std::vector<flatbuffers::Offset<void>>& values,
                const T& arg, const Args&... args) {
  vencodeJsonImpl(fbb, types, values, arg);
  vencodeJsonImpl(fbb, types, values, args...);
}

template <int I, class T>
inline void
vdecodeJsonImpl(const ::flatbuffers::Vector<uint8_t>*,
                const ::flatbuffers::Vector<flatbuffers::Offset<void>>* values,
                T& arg) {
  assert(getJsonType<T>() == types->GetEnum<Json>(i));
  decodeJson(values->Get(I), arg);
}

template <int I, class T, class... Args>
inline void
vdecodeJsonImpl(const ::flatbuffers::Vector<uint8_t>* types,
                const ::flatbuffers::Vector<flatbuffers::Offset<void>>* values,
                T& arg, Args&... args) {
  vdecodeJsonImpl<I>(types, values, arg);
  vdecodeJsonImpl<I+1>(types, values, args...);
}

}  // namespace detail

// Array encoding
template <class... Args>
inline ::flatbuffers::Offset<fbs::Array>
vencode(::flatbuffers::FlatBufferBuilder& fbb, const Args&... args) {
  std::vector<uint8_t> types;
  std::vector<flatbuffers::Offset<void>> values;
  detail::vencodeJsonImpl(fbb, types, values, args...);
  return fbs::CreateArrayDirect(fbb, &types, &values);
}

// Array decoding
template <class... Args>
inline void
vdecode(const fbs::Array* ptr, Args&... args) {
  detail::vdecodeJsonImpl<0>(ptr->value_type(), ptr->value(), args...);
}

// Pair decoding
template <class S, class V>
inline void
decodeJson(const fbs::Pair* pair, std::pair<S, V>& value) {
  assert(pair->value_type() == getJsonType<V>());
  decodeJson(pair->name(), value.first);
  decodeJson(pair->value(), value.second);
}

// Object decoding
template <int I, class S, class V>
inline void
decodeJson(const fbs::Object* object, std::pair<S, V>& arg) {
  decodeJson(object->value()->Get(I), arg);
}

template <int I, class S, class V, class... Args>
inline void
decodeJson(const fbs::Object* object, std::pair<S, V>& arg, Args&... args) {
  decodeJson<I>(object, arg);
  decodeJson<I+1>(object, args...);
}

template <class... Args>
inline void
decodeJson(const fbs::Object* object, Args&... args) {
  decodeJson<0>(object, args...);
}

//////////////////////////////////////////////////////////////////////

inline ::flatbuffers::Offset<fbs::Null>
encodeJsonNull(::flatbuffers::FlatBufferBuilder& fbb, const acc::dynamic&) {
  return fbs::CreateNull(fbb);
}

inline ::flatbuffers::Offset<fbs::Bool>
encodeJsonBool(::flatbuffers::FlatBufferBuilder& fbb, const acc::dynamic& d) {
  return fbs::CreateBool(fbb, d.getBool());
}

inline ::flatbuffers::Offset<fbs::Double>
encodeJsonDouble(::flatbuffers::FlatBufferBuilder& fbb, const acc::dynamic& d) {
  return fbs::CreateDouble(fbb, d.getDouble());
}

inline ::flatbuffers::Offset<fbs::Int64>
encodeJsonInt64(::flatbuffers::FlatBufferBuilder& fbb, const acc::dynamic& d) {
  return fbs::CreateInt64(fbb, d.getInt());
}

inline ::flatbuffers::Offset<fbs::String>
encodeJsonString(::flatbuffers::FlatBufferBuilder& fbb, const acc::dynamic& d) {
  return fbs::CreateStringDirect(fbb, d.c_str());
}

inline ::flatbuffers::Offset<fbs::Object>
encodeJsonObject(::flatbuffers::FlatBufferBuilder& fbb, const acc::dynamic& d);

inline ::flatbuffers::Offset<fbs::Array>
encodeJsonArray(::flatbuffers::FlatBufferBuilder& fbb, const acc::dynamic& d) {
  std::vector<uint8_t> types;
  std::vector<flatbuffers::Offset<void>> values;
  for (auto& i : d) {
    switch (i.type()) {

#define FTT_X(ft) \
        types.push_back(acc::to<uint8_t>(fbs::Json::ft)); \
        values.push_back(encodeJson##ft(fbb, i).Union())

      case acc::dynamic::NULLT:  FTT_X(Null);   break;
      case acc::dynamic::BOOL:   FTT_X(Bool);   break;
      case acc::dynamic::DOUBLE: FTT_X(Double); break;
      case acc::dynamic::INT64:  FTT_X(Int64);  break;
      case acc::dynamic::STRING: FTT_X(String); break;
      case acc::dynamic::ARRAY:  FTT_X(Array);  break;
      case acc::dynamic::OBJECT: FTT_X(Object); break;

#undef FTT_X

    }
  }
  return fbs::CreateArrayDirect(fbb, &types, &values);
}

inline ::flatbuffers::Offset<fbs::Object>
encodeJsonObject(::flatbuffers::FlatBufferBuilder& fbb, const acc::dynamic& d) {
  std::vector<flatbuffers::Offset<fbs::Pair>> values;
  for (auto& p : d.items()) {
    switch (p.second.type()) {

#define FTT_X(ft) \
        values.push_back( \
            fbs::CreatePairDirect( \
                fbb, \
                fbs::Json::ft, \
                encodeJson##ft(fbb, p.second).Union(), \
                p.first.c_str()))

      case acc::dynamic::NULLT:  FTT_X(Null);   break;
      case acc::dynamic::BOOL:   FTT_X(Bool);   break;
      case acc::dynamic::DOUBLE: FTT_X(Double); break;
      case acc::dynamic::INT64:  FTT_X(Int64);  break;
      case acc::dynamic::STRING: FTT_X(String); break;
      case acc::dynamic::ARRAY:  FTT_X(Array);  break;
      case acc::dynamic::OBJECT: FTT_X(Object); break;

#undef FTT_X

    }
  }
  return fbs::CreateObject(fbb, fbb.CreateVectorOfSortedTables(&values));
}

} // namespace ftt
