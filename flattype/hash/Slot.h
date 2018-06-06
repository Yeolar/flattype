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

namespace ftt {

namespace detail {

struct BIndex {
  uint16_t block;
  uint16_t keep_;
  uint32_t index;
};

union BIndexData {
  BIndex bi;
  uint64_t v;
  explicit BIndexData(BIndex data) : bi(data) {}
  explicit BIndexData(uint64_t data) : v(data) {}
};

} // namespace detail

typedef detail::BIndex BIndex;

inline BIndex u64ToBIndex(uint64_t i) {
  return detail::BIndexData(i).bi;
}
inline uint64_t bindexToU64(BIndex i) {
  return detail::BIndexData(i).v;
}

template <class T>
struct SlotIndexType;

template <> struct SlotIndexType<fbs::HSlot32> { using type = uint32_t; };
template <> struct SlotIndexType<fbs::HSlot64> { using type = uint64_t; };
template <> struct SlotIndexType<fbs::HSlotS>  {
  using type = ::flatbuffers::String*;
};

template <class T, class S>
inline typename std::enable_if<
  std::is_same<S, fbs::HSlot32>::value ||
  std::is_same<S, fbs::HSlot64>::value ||
  std::is_same<S, fbs::HSlotS>::value
  >::type
decode(const S* slot, T& value) {
  assert(slot->value_type() == getAnyType<T>());
  decode(slot->value(), value);
}

template <class S>
inline typename std::enable_if<
  std::is_same<S, fbs::HSlot32>::value ||
  std::is_same<S, fbs::HSlot64>::value ||
  std::is_same<S, fbs::HSlotS>::value
  >::type
forEachIndex(const S* slot, const std::function<void(BIndex)>& func) {
  for (uint64_t i : *slot->indexes()) {
    func(u64ToBIndex(i));
  }
}

struct SlotState {
  enum : uint32_t {
    EMPTY = 0,
    CONSTRUCTING = 1,
    LINKED = 2,
  };

  template <class FT>
  static uint32_t headAndState(const FT* slot) {
    return slot ? slot->hs() : 0;
  }

  template <class FT>
  static uint32_t state(const FT* slot) {
    return headAndState(slot) & 3;
  }

  template <class FT>
  static uint32_t next(const FT* slot) {
    return slot ? slot->next() : 0;
  }
};

} // namespace ftt
