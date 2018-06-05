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

#include "accelerator/Conv.h"
#include "accelerator/FBString.h"
#include "accelerator/Range.h"
#include "flattype/CommonIDLs.h"
#include "flattype/Type.h"

namespace ftt {

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

// vector<const char*> encoding
inline ::flatbuffers::Offset<fbs::StringArray>
encode(::flatbuffers::FlatBufferBuilder& fbb,
       const std::vector<const char*>& value) {
  std::vector<flatbuffers::Offset<flatbuffers::String>> v;
  for (auto& i : value) {
    v.push_back(fbb.CreateString(i));
  }
  return fbs::CreateStringArrayDirect(fbb, &v);
}

// vector<const char*> decoding (no copy)
inline void
decode(const void* ptr, std::vector<const char*>& value) {
  auto p = reinterpret_cast<const fbs::StringArray*>(ptr);
  for (auto i : *p->value()) {
    value.push_back(i->data());
  }
}

//////////////////////////////////////////////////////////////////////

template <class T>
using vvector = std::vector<std::vector<T>>;

// Tuple decoding
inline fbs::Any
decodeOneType(const fbs::Tuple* ptr, int i) {
  return ptr->value_type()->GetEnum<fbs::Any>(i);
}

template <class T>
inline void
decodeOne(const fbs::Tuple* ptr, int i, T& arg) {
  assert(getAnyType<T>() == ptr->value_type()->GetEnum<Any>(i));
  decode(ptr->value()->Get(i), arg);
}

// pair<K, V> encoding
template <class K, class V>
inline ::flatbuffers::Offset<fbs::Tuple>
encode(::flatbuffers::FlatBufferBuilder& fbb, const std::pair<K, V>& value) {
  std::vector<uint8_t> types;
  std::vector<flatbuffers::Offset<void>> values;
  types.push_back(
      acc::to<uint8_t>(getAnyType<typename std::remove_const<K>::type>()));
  types.push_back(acc::to<uint8_t>(getAnyType<V>()));
  values.push_back(encode(fbb, value.first).Union());
  values.push_back(encode(fbb, value.second).Union());
  return fbs::CreateTupleDirect(fbb, &types, &values);
}

// pair<K, V> decoding
template <class K, class V>
inline void
decode(const void* ptr, std::pair<K, V>& value) {
  auto p = reinterpret_cast<const fbs::Tuple*>(ptr);
  decode(p->value()->Get(0), value.first);
  decode(p->value()->Get(1), value.second);
}

// vector<pair<K, V>> encoding
template <class K, class V>
inline ::flatbuffers::Offset<fbs::Tuple>
encode(::flatbuffers::FlatBufferBuilder& fbb,
       const std::vector<std::pair<K, V>>& value) {
  std::vector<uint8_t> types;
  std::vector<flatbuffers::Offset<void>> values;
  for (auto& i : value) {
    types.push_back(acc::to<uint8_t>(fbs::Any::Tuple));
    values.push_back(encode(fbb, i).Union());
  }
  return fbs::CreateTupleDirect(fbb, &types, &values);
}

// vector<pair<K, V>> decoding
template <class K, class V>
inline void
decode(const void* ptr, std::vector<std::pair<K, V>>& value) {
  auto p = reinterpret_cast<const fbs::Tuple*>(ptr);
  for (auto i : *p->value()) {
    std::pair<K, V> item;
    decode(i, item);
    value.push_back(std::move(item));
  }
}

// map<K, V> encoding
template <class K, class V>
inline ::flatbuffers::Offset<fbs::Tuple>
encode(::flatbuffers::FlatBufferBuilder& fbb, const std::map<K, V>& value) {
  std::vector<uint8_t> types;
  std::vector<flatbuffers::Offset<void>> values;
  for (auto& i : value) {
    types.push_back(acc::to<uint8_t>(fbs::Any::Tuple));
    values.push_back(encode(fbb, i).Union());
  }
  return fbs::CreateTupleDirect(fbb, &types, &values);
}

// map<K, V> decoding
template <class K, class V>
inline void
decode(const void* ptr, std::map<K, V>& value) {
  auto p = reinterpret_cast<const fbs::Tuple*>(ptr);
  for (auto i : *p->value()) {
    std::pair<K, V> item;
    decode(i, item);
    value.insert(std::move(item));
  }
}

// vector<vector<T>> encoding
template <class T>
inline ::flatbuffers::Offset<fbs::Tuple>
encode(::flatbuffers::FlatBufferBuilder& fbb, const vvector<T>& value) {
  std::vector<uint8_t> rowTypes;
  std::vector<flatbuffers::Offset<void>> rowValues;
  for (auto& row : value) {
    rowTypes.push_back(getAnyType<std::vector<T>>());
    rowValues.push_back(encode(fbb, row).Union());
  }
  return fbs::CreateTupleDirect(fbb, &rowTypes, &rowValues);
}

// vector<vector<T>> decoding
template <class T>
inline void
decode(const void* ptr, vvector<T>& value) {
  auto p = reinterpret_cast<const fbs::Tuple*>(ptr);
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
  types.push_back(acc::to<uint8_t>(getAnyType<T>()));
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
vdecodeImpl(const ::flatbuffers::Vector<uint8_t>* types,
            const ::flatbuffers::Vector<flatbuffers::Offset<void>>* values,
            T& arg) {
  assert(getAnyType<T>() == types->GetEnum<Any>(I));
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

template <int I, class T>
inline void
vdecodeImpl(::flatbuffers::FlatBufferBuilder& fbb,
            const std::vector<uint8_t>& types,
            const std::vector<flatbuffers::Offset<void>>& values,
            T& arg) {
  assert(acc::to<uint8_t>(getAnyType<T>()) == types[I]);
  decode(::flatbuffers::GetTemporaryPointer(fbb, values[I]), arg);
}

template <int I, class T, class... Args>
inline void
vdecodeImpl(::flatbuffers::FlatBufferBuilder& fbb,
            const std::vector<uint8_t>& types,
            const std::vector<flatbuffers::Offset<void>>& values,
            T& arg, Args&... args) {
  vdecodeImpl<I>(fbb, types, values, arg);
  vdecodeImpl<I+1>(fbb, types, values, args...);
}

} // namespace detail

// Tuple encoding
template <class... Args>
inline ::flatbuffers::Offset<fbs::Tuple>
vencode(::flatbuffers::FlatBufferBuilder& fbb, const Args&... args) {
  std::vector<uint8_t> types;
  std::vector<flatbuffers::Offset<void>> values;
  detail::vencodeImpl(fbb, types, values, args...);
  return fbs::CreateTupleDirect(fbb, &types, &values);
}

// Tuple decoding
template <class... Args>
inline void
vdecode(const fbs::Tuple* ptr, Args&... args) {
  detail::vdecodeImpl<0>(ptr->value_type(), ptr->value(), args...);
}

} // namespace ftt
