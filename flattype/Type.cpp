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

#include "flattype/Type.h"

namespace ftt {

fbs::Any toAnyType(fbs::Json type) {
  switch (type) {
#define ACC_JSON_TO_ANY_CASE(jft, aft) \
    case fbs::Json::jft: return fbs::Any::aft;

    ACC_JSON_TO_ANY_CASE(Null,   Null)
    ACC_JSON_TO_ANY_CASE(Bool,   Bool)
    ACC_JSON_TO_ANY_CASE(Int64,  Int64)
    ACC_JSON_TO_ANY_CASE(Double, Double)
    ACC_JSON_TO_ANY_CASE(String, String)
    ACC_JSON_TO_ANY_CASE(Array,  Tuple)
    ACC_JSON_TO_ANY_CASE(Object, Tuple)
    ACC_JSON_TO_ANY_CASE(NONE,   NONE)

#undef ACC_JSON_TO_ANY_CASE
  }
  return fbs::Any::NONE;
}

fbs::Json toJsonType(fbs::Any type) {
  switch (type) {
#define ACC_ANY_TO_JSON_CASE(aft, jft) \
    case fbs::Any::aft: return fbs::Json::jft;

    ACC_ANY_TO_JSON_CASE(Null,        Null)
    ACC_ANY_TO_JSON_CASE(Bool,        Bool)
    ACC_ANY_TO_JSON_CASE(Int8,        Int64)
    ACC_ANY_TO_JSON_CASE(Int16,       Int64)
    ACC_ANY_TO_JSON_CASE(Int32,       Int64)
    ACC_ANY_TO_JSON_CASE(Int64,       Int64)
    ACC_ANY_TO_JSON_CASE(UInt8,       Int64)
    ACC_ANY_TO_JSON_CASE(UInt16,      Int64)
    ACC_ANY_TO_JSON_CASE(UInt32,      Int64)
    ACC_ANY_TO_JSON_CASE(UInt64,      Int64)
    ACC_ANY_TO_JSON_CASE(Float,       Double)
    ACC_ANY_TO_JSON_CASE(Double,      Double)
    ACC_ANY_TO_JSON_CASE(String,      String)
    ACC_ANY_TO_JSON_CASE(BoolArray,   Array)
    ACC_ANY_TO_JSON_CASE(Int8Array,   Array)
    ACC_ANY_TO_JSON_CASE(Int16Array,  Array)
    ACC_ANY_TO_JSON_CASE(Int32Array,  Array)
    ACC_ANY_TO_JSON_CASE(Int64Array,  Array)
    ACC_ANY_TO_JSON_CASE(UInt8Array,  Array)
    ACC_ANY_TO_JSON_CASE(UInt16Array, Array)
    ACC_ANY_TO_JSON_CASE(UInt32Array, Array)
    ACC_ANY_TO_JSON_CASE(UInt64Array, Array)
    ACC_ANY_TO_JSON_CASE(FloatArray,  Array)
    ACC_ANY_TO_JSON_CASE(DoubleArray, Array)
    ACC_ANY_TO_JSON_CASE(StringArray, Array)
    ACC_ANY_TO_JSON_CASE(Tuple,       Array)
    ACC_ANY_TO_JSON_CASE(NONE,        NONE)

#undef ACC_ANY_TO_JSON_CASE
  }
  return fbs::Json::NONE;
}

} // namespace ftt
