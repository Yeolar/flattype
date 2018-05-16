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
  types.push_back(getAnyType<T>());
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
vdecodeImpl(::flatbuffers::FlatBufferBuilder& fbb,
            const std::vector<uint8_t>& types,
            const std::vector<flatbuffers::Offset<void>>& values,
            T& arg) {
  assert(getAnyType<T>() == static_cast<fbs::Any>(types[I]));
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

}  // namespace detail

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

// Item decoding
template <class T>
inline void
decode(const fbs::Item* item, T& value) {
  assert(item->value_type() == getAnyType<T>());
  decode(item->value(), value);
}

// Record decoding
template <int I, class T>
inline void
decode(const fbs::Record* record, T& arg) {
  decode(record->value()->Get(I), arg);
}

template <int I, class T, class... Args>
inline void
decode(const fbs::Record* record, T& arg, Args&... args) {
  decode<I>(record, arg);
  decode<I+1>(record, args...);
}

template <class... Args>
inline void
decode(const fbs::Record* record, Args&... args) {
  decode<0>(record, args...);
}

// Matrix decoding
template <int I, class T>
inline void
decodeOne(const fbs::Matrix* matrix, size_t j, T& arg) {
  decode(matrix->value()->Get(I)->value()->Get(j), arg);
}

template <int I, class T, class... Args>
inline void
decodeOne(const fbs::Matrix* matrix, size_t j, T& arg, Args&... args) {
  decodeOne<I>(matrix, j, arg);
  decodeOne<I+1>(matrix, j, args...);
}

template <class... Args>
inline void
decodeOne(const fbs::Matrix* matrix, size_t j, Args&... args) {
  decodeOne<0>(matrix, j, args...);
}

//////////////////////////////////////////////////////////////////////

// vector<Item> encoding
template <class T>
inline void
vencodeItems(::flatbuffers::FlatBufferBuilder& fbb,
             std::vector<flatbuffers::Offset<fbs::Item>>& items,
             const T& arg) {
  auto item = fbs::CreateItem(fbb, getAnyType<T>(), encode(fbb, arg).Union());
  items.push_back(item);
}

template <class T, class... Args>
inline void
vencodeItems(::flatbuffers::FlatBufferBuilder& fbb,
             std::vector<flatbuffers::Offset<fbs::Item>>& items,
             const T& arg, const Args&... args) {
  vencodeItems(fbb, items, arg);
  vencodeItems(fbb, items, args...);
}

// vector<Item> decoding
template <int I, class T>
inline void
vdecodeItems(::flatbuffers::FlatBufferBuilder& fbb,
             const std::vector<flatbuffers::Offset<fbs::Item>>& items,
             T& arg) {
  auto item = ::flatbuffers::GetTemporaryPointer(fbb, items[I]);
  assert(item->value_type() == getAnyType<T>());
  decode(item->value(), arg);
}

template <int I, class T, class... Args>
inline void
vdecodeItems(::flatbuffers::FlatBufferBuilder& fbb,
             const std::vector<flatbuffers::Offset<fbs::Item>>& items,
             T& arg, Args&... args) {
  vdecodeItems<I>(fbb, items, arg);
  vdecodeItems<I+1>(fbb, items, args...);
}

template <class... Args>
inline void
vdecodeItems(::flatbuffers::FlatBufferBuilder& fbb,
             const std::vector<flatbuffers::Offset<fbs::Item>>& items,
             Args&... args) {
  vdecodeItems<0>(fbb, items, args...);
}

// vector<vector<Item>> encoding
template <int I, class T>
inline void
vvencodeItems(::flatbuffers::FlatBufferBuilder& fbb,
              vvector<flatbuffers::Offset<fbs::Item>>& items,
              size_t j, const T& arg) {
  auto item = fbs::CreateItem(fbb, getAnyType<T>(), encode(fbb, arg).Union());
  if (j >= items[I].size()) {
    items[I].resize(j + 1);
  }
  items[I][j] = item;
}

template <int I, class T, class... Args>
inline void
vvencodeItems(::flatbuffers::FlatBufferBuilder& fbb,
              vvector<flatbuffers::Offset<fbs::Item>>& items,
              size_t j, const T& arg, const Args&... args) {
  vvencodeItems<I>(fbb, items, j, arg);
  vvencodeItems<I+1>(fbb, items, j, args...);
}

template <class... Args>
inline void
vvencodeItems(::flatbuffers::FlatBufferBuilder& fbb,
              vvector<flatbuffers::Offset<fbs::Item>>& items,
              size_t j, const Args&... args) {
  vvencodeItems<0>(fbb, items, j, args...);
}

// vector<vector<Item>> decoding
template <int I, class T>
inline void
vvdecodeItems(::flatbuffers::FlatBufferBuilder& fbb,
              const vvector<flatbuffers::Offset<fbs::Item>>& items,
              size_t j, T& arg) {
  auto item = ::flatbuffers::GetTemporaryPointer(fbb, items[I][j]);
  assert(item->value_type() == getAnyType<T>());
  decode(item->value(), arg);
}

template <int I, class T, class... Args>
inline void
vvdecodeItems(::flatbuffers::FlatBufferBuilder& fbb,
              const vvector<flatbuffers::Offset<fbs::Item>>& items,
              size_t j, T& arg, Args&... args) {
  vvdecodeItems<I>(fbb, items, j, arg);
  vvdecodeItems<I+1>(fbb, items, j, args...);
}

template <class... Args>
inline void
vvdecodeItems(::flatbuffers::FlatBufferBuilder& fbb,
              const vvector<flatbuffers::Offset<fbs::Item>>& items,
              size_t j, Args&... args) {
  vvdecodeItems<0>(fbb, items, j, args...);
}

} // namespace ftt
