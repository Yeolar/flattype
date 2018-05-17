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

#include "accelerator/Conv.h"
#include "flattype/CommonIDLs.h"

namespace ftt {

::flatbuffers::Offset<void>
copy(::flatbuffers::FlatBufferBuilder& fbb, fbs::Any type, const void* obj);

// Tuple
inline ::flatbuffers::Offset<fbs::Tuple>
copy(::flatbuffers::FlatBufferBuilder& fbb, const fbs::Tuple& obj) {
  std::vector<uint8_t> types;
  std::vector<flatbuffers::Offset<void>> values;
  for (size_t i = 0; i < obj.value()->size(); i++) {
    fbs::Any type = obj.value_type()->GetEnum<fbs::Any>(i);
    types.push_back(acc::to<uint8_t>(type));
    values.push_back(copy(fbb, type, obj.value()->GetAs<void>(i)));
  }
  return fbs::CreateTupleDirect(fbb, &types, &values);
}

// Item
inline ::flatbuffers::Offset<fbs::Item>
copy(::flatbuffers::FlatBufferBuilder& fbb, const fbs::Item& obj) {
  return fbs::CreateItem(
      fbb,
      obj.value_type(),
      copy(fbb, obj.value_type(), obj.value()));
}

// Record
inline ::flatbuffers::Offset<fbs::Record>
copy(::flatbuffers::FlatBufferBuilder& fbb, const fbs::Record& obj) {
  std::vector<flatbuffers::Offset<fbs::Item>> values;
  for (const fbs::Item* i : *obj.value()) {
    values.push_back(copy(fbb, *i));
  }
  return fbs::CreateRecordDirect(fbb, &values);
}

// Matrix
inline ::flatbuffers::Offset<fbs::Matrix>
copy(::flatbuffers::FlatBufferBuilder& fbb, const fbs::Matrix& obj) {
  std::vector<flatbuffers::Offset<fbs::Record>> values;
  for (const fbs::Record* i : *obj.value()) {
    values.push_back(copy(fbb, *i));
  }
  return fbs::CreateMatrixDirect(fbb, &values);
}

} // namespace ftt
