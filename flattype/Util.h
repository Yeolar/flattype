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

#include "flatbuffers/flatbuffers.h"

namespace ftt {

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
  return p ? Iter(reinterpret_cast<const uint8_t*>(p)) : vector->end();
}

} // namespace detail

template <class T, class K>
inline typename ::flatbuffers::Vector<T>::iterator
find(const ::flatbuffers::Vector<T>* vector, const K& key) {
  return detail::lookupByKey<T, K, ::flatbuffers::Vector<T>::iterator>(
      vector, key);
}

template <class T, class K>
inline typename ::flatbuffers::Vector<T>::const_iterator
findConst(const ::flatbuffers::Vector<T>* vector, const K& key) {
  return detail::lookupByKey<T, K, ::flatbuffers::Vector<T>::const_iterator>(
      vector, key);
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

} // namespace ftt
