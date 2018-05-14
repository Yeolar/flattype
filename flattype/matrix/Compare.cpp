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
#include "flattype/matrix/Compare.h"

namespace ftt {

bool equal(fbs::Any type, const void* lhs, const void* rhs) {
  switch (type) {
#define FTT_ANY_EQUAL_CASE(ft) \
    case fbs::Any_##ft: \
      if (!(*reinterpret_cast<const fbs::ft*>(lhs) == \
            *reinterpret_cast<const fbs::ft*>(rhs))) \
        return false; \
      break;

    FTT_ANY_EQUAL_CASE(Null)
    FTT_ANY_EQUAL_CASE(Bool)
    FTT_ANY_EQUAL_CASE(Int8)
    FTT_ANY_EQUAL_CASE(Int16)
    FTT_ANY_EQUAL_CASE(Int32)
    FTT_ANY_EQUAL_CASE(Int64)
    FTT_ANY_EQUAL_CASE(UInt8)
    FTT_ANY_EQUAL_CASE(UInt16)
    FTT_ANY_EQUAL_CASE(UInt32)
    FTT_ANY_EQUAL_CASE(UInt64)
    FTT_ANY_EQUAL_CASE(Float)
    FTT_ANY_EQUAL_CASE(Double)
    FTT_ANY_EQUAL_CASE(String)
    FTT_ANY_EQUAL_CASE(BoolArray)
    FTT_ANY_EQUAL_CASE(Int8Array)
    FTT_ANY_EQUAL_CASE(Int16Array)
    FTT_ANY_EQUAL_CASE(Int32Array)
    FTT_ANY_EQUAL_CASE(Int64Array)
    FTT_ANY_EQUAL_CASE(UInt8Array)
    FTT_ANY_EQUAL_CASE(UInt16Array)
    FTT_ANY_EQUAL_CASE(UInt32Array)
    FTT_ANY_EQUAL_CASE(UInt64Array)
    FTT_ANY_EQUAL_CASE(FloatArray)
    FTT_ANY_EQUAL_CASE(DoubleArray)
    FTT_ANY_EQUAL_CASE(StringArray)
    FTT_ANY_EQUAL_CASE(Tuple)

#undef FTT_ANY_EQUAL_CASE

    case fbs::Any_NONE:
      ACC_CHECK_THROW(0, acc::Exception);
  }
  return true;
}

bool operator==(const fbs::Tuple& lhs, const fbs::Tuple& rhs) {
  auto lvalue = lhs.value();
  auto rvalue = rhs.value();
  if (lvalue->size() != rvalue->size()) {
    return false;
  }
  if (memcmp(lhs.value_type()->data(),
             rhs.value_type()->data(),
             lvalue->size()) != 0) {
    return false;
  }
  for (::flatbuffers::uoffset_t i = 0; i < lvalue->size(); i++) {
    if (!equal(lhs.value_type()->GetEnum<fbs::Any>(i),
               lvalue->Get(i),
               rvalue->Get(i))) {
      return false;
    }
  }
  return true;
}

} // namespace ftt
