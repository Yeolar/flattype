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
#include "flattype/Builder.h"
#include "flattype/table/Table.h"

namespace ftt {

class TableBuilder : public Builder {
 public:
  TableBuilder() : Builder() {}

  explicit TableBuilder(FBB* fbb, bool owns = false)
    : Builder(fbb, owns) {}

  TableBuilder(const TableBuilder&) = delete;
  TableBuilder& operator=(const TableBuilder&) = delete;

  TableBuilder(TableBuilder&&) = default;
  TableBuilder& operator=(TableBuilder&&) = default;

  uint8_t getBID() const;
  void setBID(uint8_t bid);

  std::string getName() const;
  void setName(const std::string& name);

  const std::vector<std::string>& getFields() const;
  void setFields(const std::vector<std::string>& fields);

  void buildMatrix(FBBFunc<fbs::Matrix>&& builder);

  void finish() override;

  Table toTable() { return toWrapper<Table>(); }

 private:
  uint8_t bid_{Table::npos};
  std::string name_;
  std::vector<std::string> fields_;
  ::flatbuffers::Offset<fbs::Matrix> matrix_;
};

} // namespace ftt
