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

#include "flattype/idl/matrix_generated.h"
#include "flattype/Encoding.h"
#include "flattype/Type.h"

namespace ftt {

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
  types.push_back(getAnyType<typename std::remove_const<K>::type>());
  types.push_back(getAnyType<V>());
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
    types.push_back(fbs::Any_Tuple);
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
    types.push_back(fbs::Any_Tuple);
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
encode(::flatbuffers::FlatBufferBuilder& fbb,
       const std::vector<std::vector<T>>& value) {
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
decode(const void* ptr, std::vector<std::vector<T>>& value) {
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
  types.push_back(getAnyType<T>());
  values.push_back(encode(fbb, arg).Union());
}

template <class T, class... Args>
inline void
vencodeImpl(::flatbuffers::FlatBufferBuilder& fbb,
            std::vector<uint8_t>& types,
            std::vector<flatbuffers::Offset<void>>& values,
            const T& arg, const Args&... args) {
  types.push_back(getAnyType<T>());
  values.push_back(encode(fbb, arg).Union());
  vencodeImpl(fbb, types, values, args...);
}

template <class T>
inline void
vdecodeImpl(const ::flatbuffers::Vector<uint8_t>*,
            const ::flatbuffers::Vector<flatbuffers::Offset<void>>* values,
            int i, T& arg) {
  assert(getAnyType<T>() == types->GetEnum<Any>(i));
  decode(values->Get(i), arg);
}

template <class T, class... Args>
inline void
vdecodeImpl(const ::flatbuffers::Vector<uint8_t>* types,
            const ::flatbuffers::Vector<flatbuffers::Offset<void>>* values,
            int i, T& arg, Args&... args) {
  assert(getAnyType<T>() == types->GetEnum<Any>(i));
  decode(values->Get(i), arg);
  vdecodeImpl(types, values, i + 1, args...);
}

}  // namespace detail

template <class... Args>
inline ::flatbuffers::Offset<fbs::Tuple>
vencode(::flatbuffers::FlatBufferBuilder& fbb, const Args&... args) {
  std::vector<uint8_t> types;
  std::vector<flatbuffers::Offset<void>> values;
  detail::vencodeImpl(fbb, types, values, args...);
  return fbs::CreateTupleDirect(fbb, &types, &values);
}

template <class... Args>
inline void
vdecode(const fbs::Tuple* ptr, Args&... args) {
  detail::vdecodeImpl(ptr->value_type(), ptr->value(), 0, args...);
}

} // namespace ftt
