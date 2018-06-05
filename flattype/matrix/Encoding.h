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
#include "flattype/Encoding.h"
#include "flattype/Type.h"

namespace ftt {

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

// get <Args...> types
template <class T>
inline void
getTypes(std::vector<ftt::fbs::Any>& types) {
  types.push_back(getAnyType<T>());
}

template <class T, class... Args>
inline void
getTypes(std::vector<ftt::fbs::Any>& types) {
  getTypes<T>(types);
  getTypes<Args...>(types);
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
