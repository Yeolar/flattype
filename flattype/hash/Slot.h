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

template <class FT, class Key>
struct Slot {
  typedef FT ft_type;
  typedef Key key_type;

  Slot(const FT* slot) : slot_(slot) {}

  Key key() {
    return slot_->Key();
  }

  template <class T>
  T value() const {
    T v;
    assert(slot_->value_type() == getAnyType<T>());
    decode(slot_->value(), v);
    return v;
  }

  const ::flatbuffers::Vector<uint64_t>* indexes() {
    return slot_->indexes();
  }

  BIndex operator[](size_t i) const {
    return u64ToBIndex(slot_->indexes()->Get(i));
  }

 private:
  const FT* slot_;
};

typedef Slot<fbs::HSlot32, uint32_t> Slot32;
typedef Slot<fbs::HSlot64, uint64_t> Slot64;
typedef Slot<fbs::HSlotS,  ::flatbuffers::String*> SlotS;

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
