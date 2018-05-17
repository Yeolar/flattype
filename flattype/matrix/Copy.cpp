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
#include "flattype/matrix/Copy.h"

namespace ftt {

::flatbuffers::Offset<void>
copy(::flatbuffers::FlatBufferBuilder& fbb, fbs::Any type, const void* obj) {
  switch (type) {
#define FTT_ANY_COPY_CASE(ft) \
    case fbs::Any::ft: \
      return copy(fbb, *reinterpret_cast<const fbs::ft*>(obj)).Union();

    FTT_ANY_COPY_CASE(Null)
    FTT_ANY_COPY_CASE(Bool)
    FTT_ANY_COPY_CASE(Int8)
    FTT_ANY_COPY_CASE(Int16)
    FTT_ANY_COPY_CASE(Int32)
    FTT_ANY_COPY_CASE(Int64)
    FTT_ANY_COPY_CASE(UInt8)
    FTT_ANY_COPY_CASE(UInt16)
    FTT_ANY_COPY_CASE(UInt32)
    FTT_ANY_COPY_CASE(UInt64)
    FTT_ANY_COPY_CASE(Float)
    FTT_ANY_COPY_CASE(Double)
    FTT_ANY_COPY_CASE(String)
    FTT_ANY_COPY_CASE(BoolArray)
    FTT_ANY_COPY_CASE(Int8Array)
    FTT_ANY_COPY_CASE(Int16Array)
    FTT_ANY_COPY_CASE(Int32Array)
    FTT_ANY_COPY_CASE(Int64Array)
    FTT_ANY_COPY_CASE(UInt8Array)
    FTT_ANY_COPY_CASE(UInt16Array)
    FTT_ANY_COPY_CASE(UInt32Array)
    FTT_ANY_COPY_CASE(UInt64Array)
    FTT_ANY_COPY_CASE(FloatArray)
    FTT_ANY_COPY_CASE(DoubleArray)
    FTT_ANY_COPY_CASE(StringArray)
    FTT_ANY_COPY_CASE(Tuple)

#undef FTT_ANY_COPY_CASE

    case fbs::Any::NONE:
      ACC_CHECK_THROW(0, acc::Exception);
  }
  return ::flatbuffers::Offset<void>();
}

} // namespace ftt
