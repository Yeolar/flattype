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

#include <map>
#include <string>
#include <vector>

#include "accelerator/Range.h"
#include "accelerator/Traits.h"
#include "flattype/CommonIDLs.h"

namespace ftt {

//////////////////////////////////////////////////////////////////////

template <class T, class Enable = void>
struct AnyType;

// scalar

#define FTT_ANY_TYPE(t, aft) \
template <> \
struct AnyType<t> { \
  using type = fbs::aft; \
};

FTT_ANY_TYPE(std::nullptr_t,        Null)
FTT_ANY_TYPE(bool,                  Bool)
FTT_ANY_TYPE(int8_t,                Int8)
FTT_ANY_TYPE(int16_t,               Int16)
FTT_ANY_TYPE(int32_t,               Int32)
FTT_ANY_TYPE(int64_t,               Int64)
FTT_ANY_TYPE(uint8_t,               UInt8)
FTT_ANY_TYPE(uint16_t,              UInt16)
FTT_ANY_TYPE(uint32_t,              UInt32)
FTT_ANY_TYPE(uint64_t,              UInt64)
FTT_ANY_TYPE(float,                 Float)
FTT_ANY_TYPE(double,                Double)
FTT_ANY_TYPE(std::vector<bool>,     BoolArray)
FTT_ANY_TYPE(std::vector<int8_t>,   Int8Array)
FTT_ANY_TYPE(std::vector<int16_t>,  Int16Array)
FTT_ANY_TYPE(std::vector<int32_t>,  Int32Array)
FTT_ANY_TYPE(std::vector<int64_t>,  Int64Array)
FTT_ANY_TYPE(std::vector<uint8_t>,  UInt8Array)
FTT_ANY_TYPE(std::vector<uint16_t>, UInt16Array)
FTT_ANY_TYPE(std::vector<uint32_t>, UInt32Array)
FTT_ANY_TYPE(std::vector<uint64_t>, UInt64Array)
FTT_ANY_TYPE(std::vector<float>,    FloatArray)
FTT_ANY_TYPE(std::vector<double>,   DoubleArray)

#undef FTT_ANY_TYPE

// string
template <class T>
struct AnyType<T,
  typename std::enable_if<
    std::is_convertible<T, acc::StringPiece>::value>::type> {
  using type = fbs::String;
};

// vector<string>
template <class T>
struct AnyType<T,
  typename std::enable_if<
    acc::IsSpecialization<T, std::vector>::value &&
    std::is_convertible<typename T::value_type, acc::StringPiece>::value
  >::type> {
  using type = fbs::StringArray;
};

// pair<>
template <class T>
struct AnyType<T,
  typename std::enable_if<
    acc::IsSpecialization<T, std::pair>::value>::type> {
  using type = fbs::Tuple;
};

// map<>
template <class T>
struct AnyType<T,
  typename std::enable_if<
    acc::IsSpecialization<T, std::map>::value>::type> {
  using type = fbs::Tuple;
};

// vector<pair<>>
template <class T>
struct AnyType<T,
  typename std::enable_if<
    acc::IsSpecialization<T, std::vector>::value &&
    acc::IsSpecialization<typename T::value_type, std::pair>::value
  >::type> {
  using type = fbs::Tuple;
};

// vector<vector<>>
template <class T>
struct AnyType<T,
  typename std::enable_if<
    acc::IsSpecialization<T, std::vector>::value &&
    acc::IsSpecialization<typename T::value_type, std::vector>::value
  >::type> {
  using type = fbs::Tuple;
};

template <class T>
inline fbs::Any getAnyType() {
  return fbs::AnyTraits<typename AnyType<T>::type>::enum_value;
}

//////////////////////////////////////////////////////////////////////

template <class T, class Enable = void>
struct JsonType;

// scalar

#define FTT_JSON_TYPE(t, jft) \
template <> \
struct JsonType<t> { \
  using type = fbs::jft; \
};

FTT_JSON_TYPE(std::nullptr_t, Null)
FTT_JSON_TYPE(bool,           Bool)
FTT_JSON_TYPE(int64_t,        Int64)
FTT_JSON_TYPE(double,         Double)

#undef FTT_JSON_TYPE

// string
template <class T>
struct JsonType<T,
  typename std::enable_if<
    std::is_convertible<T, acc::StringPiece>::value>::type> {
  using type = fbs::String;
};

// vector<>
template <class T>
struct JsonType<T,
  typename std::enable_if<
    acc::IsSpecialization<T, std::vector>::value>::type> {
  using type = fbs::Array;
};

// pair<>
template <class T>
struct JsonType<T,
  typename std::enable_if<
    acc::IsSpecialization<T, std::pair>::value>::type> {
  using type = fbs::Pair;
};

// map<>
template <class T>
struct JsonType<T,
  typename std::enable_if<
    acc::IsSpecialization<T, std::map>::value>::type> {
  using type = fbs::Object;
};

template <class T>
inline fbs::Json getJsonType() {
  return fbs::JsonTraits<typename JsonType<T>::type>::enum_value;
}

//////////////////////////////////////////////////////////////////////

fbs::Any toAnyType(fbs::Json type);
fbs::Json toJsonType(fbs::Any type);

} // namespace ftt
