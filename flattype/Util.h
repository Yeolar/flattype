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

#include <cstdlib>

#include "accelerator/Range.h"
#include "flattype/CommonIDLs.h"
#include "flattype/Type.h"

namespace ftt {

// get value

template <class T>
inline typename std::enable_if<
  std::is_arithmetic<T>::value, T>::type
getValue(const void* ptr) {
  return reinterpret_cast<const typename AnyType<T>::type*>(ptr)->value();
}

template <class T>
inline typename std::enable_if<
  std::is_same<T, acc::StringPiece>::value, acc::StringPiece>::type
getValue(const void* ptr) {
  auto s = reinterpret_cast<const typename AnyType<T>::type*>(ptr)->value();
  return acc::StringPiece(s->data(), s->size());
}

// get value pointer

template <class T>
inline const T* getValuePtr(const void* ptr) {
  return reinterpret_cast<const T*>(
      reinterpret_cast<const ::flatbuffers::Table*>(ptr)
      ->GetAddressOf(AnyType<T>::type::VT_VALUE));
}

// mutate value

template <class T>
inline typename std::enable_if<
  std::is_arithmetic<T>::value && !std::is_same<T, bool>::value, bool>::type
mutateValue(void* ptr, T value) {
  return reinterpret_cast<::flatbuffers::Table*>(ptr)
      ->SetField(AnyType<T>::type::VT_VALUE, value, T());
}

template <class T>
inline typename std::enable_if<
  std::is_same<T, bool>::value, bool>::type
mutateValue(void* ptr, bool value) {
  return reinterpret_cast<::flatbuffers::Table*>(ptr)
      ->SetField(AnyType<T>::type::VT_VALUE, uint8_t(value), uint8_t());
}

//////////////////////////////////////////////////////////////////////

namespace detail {

template <class T, class K>
inline int keyCompare(const void* ap, const void* bp) {
  const K* key = reinterpret_cast<const K*>(ap);
  const uint8_t* data = reinterpret_cast<const uint8_t*>(bp);
  auto table = ::flatbuffers::IndirectHelper<T>::Read(data, 0);

  // std::bsearch compares with the operands transposed, so we negate the
  // result here.
  return -table->KeyCompareWithValue(*key);
}

template <class T, class K, class Iter>
inline Iter lookupByKey(const ::flatbuffers::Vector<T>* vector, const K& key) {
  void* p = std::bsearch(&key,
                         vector->Data(),
                         vector->size(),
                         ::flatbuffers::IndirectHelper<T>::element_stride,
                         detail::keyCompare<T, K>);
  return p ? Iter(reinterpret_cast<const uint8_t*>(p), 0) : vector->end();
}

} // namespace detail

template <class T, class K>
inline typename ::flatbuffers::Vector<T>::iterator
find(const ::flatbuffers::Vector<T>* vector, const K& key) {
  return detail::lookupByKey<T, K,
         typename ::flatbuffers::Vector<T>::iterator>(vector, key);
}

template <class T, class K>
inline typename ::flatbuffers::Vector<T>::const_iterator
findConst(const ::flatbuffers::Vector<T>* vector, const K& key) {
  return detail::lookupByKey<T, K,
         typename ::flatbuffers::Vector<T>::const_iterator>(vector, key);
}

//////////////////////////////////////////////////////////////////////

template <class FT>
inline size_t size(const FT* array) {
  return array->value()->size();
}

template <class FT>
inline auto at(const FT* array, size_t i)
  -> decltype(array->value()->Get(i)) {
  return array->value()->Get(i);
}

template <class FT>
inline auto begin(const FT* array)
  -> decltype(array->value()->begin()) {
  return array->value()->begin();
}

template <class FT>
inline auto end(const FT* array)
  -> decltype(array->value()->end()) {
  return array->value()->end();
}

//////////////////////////////////////////////////////////////////////

inline acc::StringPiece stringPiece(const ::flatbuffers::String* s) {
  return acc::StringPiece(s->data(), s->size());
}

} // namespace ftt
