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

#include "accelerator/Random.h"
#include "flattype/Builder.h"
#include "flattype/hash/HashMap.h"

namespace ftt {

template <class S>
class HashMapBuilderBase : public Builder {
 public:
  typedef typename S::key_type key_type;
  typedef typename S::ft_type value_type;

  typedef struct ConstIterator {
    ConstIterator(const HashMapBuilderBase& owner, uint32_t slot)
      : owner_(owner),
        slot_(slot) {}

    ConstIterator(const ConstIterator&) = default;
    ConstIterator& operator=(const ConstIterator&) = default;

    const value_type& operator*() const {
      return *owner_.getSlot(slot_);
    }
    const value_type* operator->() const {
      return owner_.getSlot(slot_);
    }

    const ConstIterator& operator++() {
      while (slot_ > 0) {
        --slot_;
        if (SlotState::state(owner_.getSlot(slot_)) == SlotState::LINKED) {
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
    const HashMapBuilderBase& owner_;
    uint32_t slot_;
  } const_iterator;

  friend ConstIterator;

 public:
  explicit HashMapBuilderBase(size_t maxSize)
    : Builder() {
    init(maxSize);
  }

  HashMapBuilderBase(size_t maxSize, FBB* fbb, bool owns = false)
    : Builder(fbb, owns) {
    init(maxSize);
  }

  void init(size_t maxSize, float maxLoadFactor = 0.8f) {
    size_t capacity = size_t(maxSize / std::min(1.0f, maxLoadFactor) + 128);
    size_t avail = size_t{1} << (8 * sizeof(uint32_t) - 2);
    if (capacity > avail && maxSize < avail) {
      // we'll do our best
      capacity = avail;
    }
    if (capacity < maxSize || capacity > avail) {
      throw std::invalid_argument(
          "HashMap capacity must fit in IndexType with 2 bits left over");
    }

    numSlots_ = capacity;
    slotMask_ = acc::nextPowTwo(capacity * 4) - 1;
    slots_.resize(capacity);
  }

  HashMapBuilderBase(const HashMapBuilderBase&) = delete;
  HashMapBuilderBase& operator=(const HashMapBuilderBase&) = delete;

  HashMapBuilderBase(HashMapBuilderBase&&) = default;
  HashMapBuilderBase& operator=(HashMapBuilderBase&&) = default;

  std::pair<const_iterator, bool>
  findOrConstruct(const key_type& key, const std::vector<uint64_t>& indexes) {
    uint32_t const slot = keyToSlotIdx(key);
    uint32_t prev = SlotState::headAndState(getSlot(slot));

    uint32_t existing = find(key, slot);
    if (existing != 0) {
      return std::make_pair(ConstIterator(*this, existing), false);
    }

    typename value_type::NativeTableType slotObj;
    slotObj.hs = SlotState::LINKED;
    slotObj.next = prev >> 2;
    slotObj.key = key;
    slotObj.indexes = indexes;

    uint32_t idx = allocateNear(slot);
    slots_[idx] = value_type::Pack(*fbb_, &slotObj);

    uint32_t after = idx << 2;
    if (slot == idx) {
      after += SlotState::LINKED;
    } else {
      after += (prev & 3);
    }
    getSlot(slot)->mutate_hs(after);

    return std::make_pair(ConstIterator(*this, idx), true);
  }

  const_iterator find(const key_type& key) const {
    return ConstIterator(*this, find(key, keyToSlotIdx(key)));
  }

  const_iterator cbegin() const {
    uint32_t slot = numSlots_ - 1;
    while (slot > 0 && SlotState::state(getSlot(slot)) != SlotState::LINKED) {
      --slot;
    }
    return ConstIterator(*this, slot);
  }

  const_iterator cend() const {
    return ConstIterator(*this, 0);
  }

 private:
  enum : uint32_t {
    kMaxAllocationTries = 1000,
  };

  uint32_t keyToSlotIdx(const key_type& key) const {
    size_t h = std::hash<key_type>()(key);
    h &= slotMask_;
    while (h >= numSlots_) {
      h -= numSlots_;
    }
    return h;
  }

  uint32_t find(const key_type& key, uint32_t slot) const {
    auto hs = SlotState::headAndState(getSlot(slot));
    for (slot = hs >> 2; slot != 0; slot = SlotState::next(getSlot(slot))) {
      if (key == getSlot(slot)->key()) {
        return slot;
      }
    }
    return 0;
  }

  uint32_t allocateNear(uint32_t start) {
    for (uint32_t tries = 0; tries < kMaxAllocationTries; ++tries) {
      uint32_t slot = allocationAttempt(start, tries);
      if (!getSlot(slot)) {
        return slot;
      }
    }
    throw std::bad_alloc();
  }

  uint32_t allocationAttempt(uint32_t start, uint32_t tries) const {
    if (LIKELY(tries < 8 && start + tries < numSlots_)) {
      return uint32_t(start + tries);
    } else {
      return acc::Random::rand32(0, numSlots_);
    }
  }

  const value_type* getSlot(size_t i) const {
    return ::flatbuffers::GetTemporaryPointer(*fbb_, slots_[i]);
  }

  size_t numSlots_;
  size_t slotMask_;

 protected:
  const std::vector<flatbuffers::Offset<value_type>>& slots_;
};

class HashMap32Builder : public HashMapBuilderBase<Slot32> {
 public:
  explicit HashMap32Builder(size_t maxSize)
    : HashMapBuilderBase(maxSize) {}
  HashMap32Builder(size_t maxSize, FBB* fbb, bool owns = false)
    : HashMapBuilderBase(maxSize, fbb, owns) {}

  void finish() override;

  HashMap32 toHashMap() { return toWrapper<HashMap32>(); }
};

class HashMap64Builder : public HashMapBuilderBase<Slot64> {
 public:
  explicit HashMap64Builder(size_t maxSize)
    : HashMapBuilderBase(maxSize) {}
  HashMap64Builder(size_t maxSize, FBB* fbb, bool owns = false)
    : HashMapBuilderBase(maxSize, fbb, owns) {}

  void finish() override;

  HashMap64 toHashMap() { return toWrapper<HashMap64>(); }
};

class HashMapSBuilder : public HashMapBuilderBase<SlotS> {
 public:
  explicit HashMapSBuilder(size_t maxSize)
    : HashMapBuilderBase(maxSize) {}
  HashMapSBuilder(size_t maxSize, FBB* fbb, bool owns = false)
    : HashMapBuilderBase(maxSize, fbb, owns) {}

  void finish() override;

  HashMapS toHashMap() { return toWrapper<HashMapS>(); }
};

} // namespace ftt
