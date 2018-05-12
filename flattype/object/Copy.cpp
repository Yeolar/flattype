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

#include "accelerator/Exception.h"
#include "flattype/Copy.h"
#include "flattype/object/Copy.h"

namespace ftt {
namespace json {

::flatbuffers::Offset<void>
copy(::flatbuffers::FlatBufferBuilder& fbb, fbs::Json type, const void* obj) {
  switch (type) {
#define FTT_JSON_COPY_CASE(ft) \
    case fbs::Json_##ft: \
      return copy(fbb, *reinterpret_cast<const fbs::ft*>(obj)).Union();

    FTT_JSON_COPY_CASE(Null)
    FTT_JSON_COPY_CASE(Bool)
    FTT_JSON_COPY_CASE(Int64)
    FTT_JSON_COPY_CASE(Double)
    FTT_JSON_COPY_CASE(String)
    FTT_JSON_COPY_CASE(Array)
    FTT_JSON_COPY_CASE(Object)

#undef FTT_JSON_COPY_CASE

    case fbs::Json_NONE:
      ACC_CHECK_THROW(0, acc::Exception);
  }
  return ::flatbuffers::Offset<void>();
}

} // namespace json
} // namespace ftt
