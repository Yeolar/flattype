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

#include "flattype/idl/object_generated.h"

namespace ftt {
namespace json {

::flatbuffers::Offset<void>
copy(::flatbuffers::FlatBufferBuilder& fbb, fbs::Json type, const void* obj);

// Pair
inline ::flatbuffers::Offset<fbs::Pair>
copy(::flatbuffers::FlatBufferBuilder& fbb, const fbs::Pair& obj) {
  return fbs::CreatePair(
      fbb,
      obj.value_type(),
      copy(fbb, obj.value_type(), obj.value()),
      fbb.CreateString(obj.name()));
}

// Array
inline ::flatbuffers::Offset<fbs::Array>
copy(::flatbuffers::FlatBufferBuilder& fbb, const fbs::Array& obj) {
  std::vector<uint8_t> types;
  std::vector<flatbuffers::Offset<void>> values;
  for (size_t i = 0; i < obj.value()->size(); i++) {
    fbs::Json type = obj.value_type()->GetEnum<fbs::Json>(i);
    types.push_back(type);
    values.push_back(copy(fbb, type, obj.value()->GetAs<void>(i)));
  }
  return fbs::CreateArrayDirect(fbb, &types, &values);
}

// Object
inline ::flatbuffers::Offset<fbs::Object>
copy(::flatbuffers::FlatBufferBuilder& fbb, const fbs::Object& obj) {
  std::vector<flatbuffers::Offset<fbs::Pair>> values;
  for (const fbs::Pair* i : *obj.value()) {
    values.push_back(copy(fbb, *i));
  }
  return fbs::CreateObjectDirect(fbb, &values);
}

} // namespace json
} // namespace ftt
