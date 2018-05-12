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

#include "flattype/idl/table_generated.h"
#include "flattype/Wrapper.h"

namespace ftt {

class Table : public Wrapper<fbs::Table> {
 public:
  static constexpr uint8_t npos = std::numeric_limits<uint8_t>::max();

  Table(const fbs::Table* table) : Wrapper(table) {}

  explicit Table(const uint8_t* data)
    : Wrapper(data) {}
  explicit Table(::flatbuffers::DetachedBuffer&& data)
    : Wrapper(std::move(data)) {}

  Table(const Table&) = delete;
  Table& operator=(const Table&) = delete;

  Table(Table&&) = default;
  Table& operator=(Table&&) = default;

  std::string toDebugString() const override;

  uint8_t getBID() const;
  std::string getName() const;
  std::vector<std::string> getFields() const;
  const fbs::Matrix* getMatrix() const;
};

} // namespace ftt
