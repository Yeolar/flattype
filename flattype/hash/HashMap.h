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

#include <functional>

#include "accelerator/Bits.h"
#include "flattype/CommonIDLs.h"
#include "flattype/Util.h"
#include "flattype/hash/Slot.h"

namespace ftt {

template <class FT, class S>
class HashMapBase {
 public:
  typedef FT ft_type;
  typedef S value_type;
  typedef typename SlotIndexType<S>::type key_type;

  typedef struct ConstIterator {
    ConstIterator(const HashMapBase& owner, uint32_t slot)
      : owner_(owner),
        slot_(slot) {}

    ConstIterator(const ConstIterator&) = default;
    ConstIterator& operator=(const ConstIterator&) = default;

    const value_type& operator*() const {
      return *owner_.slots_[slot_];
    }
    const value_type* operator->() const {
      return owner_.slots_[slot_];
    }

    const ConstIterator& operator++() {
      while (slot_ > 0) {
        --slot_;
        if (SlotState::state(owner_.slots_[slot_]) == SlotState::LINKED) {
          break;
        }
      }
      return *this;
    }

    ConstIterator operator++(int) {
      auto prev = *this;
      ++*this;
      return prev;
    }

    bool operator==(const ConstIterator& rhs) const {
      return slot_ == rhs.slot_;
    }
    bool operator!=(const ConstIterator& rhs) const {
      return !(*this == rhs);
    }

   private:
    const HashMapBase& owner_;
    uint32_t slot_;
  } const_iterator;

  friend ConstIterator;

 public:
  HashMapBase(const FT* hmap)
    : ptr_(hmap),
      slots_(*hmap->slots()) {
    numSlots_ = slots_.size();
    slotMask_ = acc::nextPowTwo(numSlots_ * 4) - 1;
  }

  explicit HashMapBase(const uint8_t* data)
    : HashMapBase(data ? ::flatbuffers::GetRoot<FT>(data) : nullptr) {}
  explicit HashMapBase(::flatbuffers::DetachedBuffer&& data)
    : HashMapBase(data.data()) {
    data_ = std::move(data);
  }

  HashMapBase(const HashMapBase&) = delete;
  HashMapBase& operator=(const HashMapBase&) = delete;

  HashMapBase(HashMapBase&&) = default;
  HashMapBase& operator=(HashMapBase&&) = default;

  const_iterator find(const key_type& key) const {
    return ConstIterator(*this, find(key, keyToSlotIdx(key)));
  }

  const_iterator cbegin() const {
    uint32_t slot = numSlots_ - 1;
    while (slot > 0 && SlotState::state(slots_[slot]) != SlotState::LINKED) {
      --slot;
    }
    return ConstIterator(*this, slot);
  }

  const_iterator cend() const {
    return ConstIterator(*this, 0);
  }

 private:
  uint32_t keyToSlotIdx(const key_type& key) const {
    size_t h = std::hash<key_type>()(key);
    h &= slotMask_;
    while (h >= numSlots_) {
      h -= numSlots_;
    }
    return h;
  }

  uint32_t find(const key_type& key, uint32_t slot) const {
    auto hs = SlotState::headAndState(slots_[slot]);
    for (slot = hs >> 2; slot != 0; slot = SlotState::next(slots_[slot])) {
      if (key == slots_[slot]->key()) {
        return slot;
      }
    }
    return 0;
  }

  size_t numSlots_;
  size_t slotMask_;

  const FT* ptr_{nullptr};
  const ::flatbuffers::Vector<flatbuffers::Offset<value_type>>& slots_;
  ::flatbuffers::DetachedBuffer data_;
};

typedef HashMapBase<fbs::HMap32, fbs::HSlot32> HashMap32;
typedef HashMapBase<fbs::HMap64, fbs::HSlot64> HashMap64;
typedef HashMapBase<fbs::HMapS,  fbs::HSlotS>  HashMapS;

} // namespace ftt
