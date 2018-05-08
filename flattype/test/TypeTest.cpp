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

#include <gtest/gtest.h>
#include "flattype/Type.h"

using namespace ftt;

TEST(Type, getAnyType) {
  EXPECT_EQ(fbs::Any_Null, getAnyType<std::nullptr_t>());
  EXPECT_EQ(fbs::Any_Null, getAnyType<decltype(nullptr)>());
  EXPECT_EQ(fbs::Any_Bool, getAnyType<bool>());
  EXPECT_EQ(fbs::Any_Int8, getAnyType<int8_t>());
  EXPECT_EQ(fbs::Any_Int16, getAnyType<int16_t>());
  EXPECT_EQ(fbs::Any_Int32, getAnyType<int32_t>());
  EXPECT_EQ(fbs::Any_Int64, getAnyType<int64_t>());
  EXPECT_EQ(fbs::Any_UInt8, getAnyType<uint8_t>());
  EXPECT_EQ(fbs::Any_UInt16, getAnyType<uint16_t>());
  EXPECT_EQ(fbs::Any_UInt32, getAnyType<uint32_t>());
  EXPECT_EQ(fbs::Any_UInt64, getAnyType<uint64_t>());
  EXPECT_EQ(fbs::Any_Float, getAnyType<float>());
  EXPECT_EQ(fbs::Any_Double, getAnyType<double>());
  EXPECT_EQ(fbs::Any_String, getAnyType<std::string>());
  EXPECT_EQ(fbs::Any_String, getAnyType<acc::StringPiece>());
  EXPECT_EQ(fbs::Any_BoolArray, getAnyType<std::vector<bool>>());
  EXPECT_EQ(fbs::Any_Int8Array, getAnyType<std::vector<int8_t>>());
  EXPECT_EQ(fbs::Any_Int16Array, getAnyType<std::vector<int16_t>>());
  EXPECT_EQ(fbs::Any_Int32Array, getAnyType<std::vector<int32_t>>());
  EXPECT_EQ(fbs::Any_Int64Array, getAnyType<std::vector<int64_t>>());
  EXPECT_EQ(fbs::Any_UInt8Array, getAnyType<std::vector<uint8_t>>());
  EXPECT_EQ(fbs::Any_UInt16Array, getAnyType<std::vector<uint16_t>>());
  EXPECT_EQ(fbs::Any_UInt32Array, getAnyType<std::vector<uint32_t>>());
  EXPECT_EQ(fbs::Any_UInt64Array, getAnyType<std::vector<uint64_t>>());
  EXPECT_EQ(fbs::Any_FloatArray, getAnyType<std::vector<float>>());
  EXPECT_EQ(fbs::Any_DoubleArray, getAnyType<std::vector<double>>());
  EXPECT_EQ(fbs::Any_StringArray, getAnyType<std::vector<std::string>>());
  EXPECT_EQ(fbs::Any_StringArray, getAnyType<std::vector<acc::StringPiece>>());
  EXPECT_EQ(fbs::Any_Tuple, (getAnyType<std::pair<int, int>>()));
  EXPECT_EQ(fbs::Any_Tuple, (getAnyType<std::map<int, int>>()));
  EXPECT_EQ(fbs::Any_Tuple, (getAnyType<std::vector<std::pair<int, int>>>()));
  EXPECT_EQ(fbs::Any_Tuple, getAnyType<std::vector<std::vector<int>>>());
}

TEST(Type, getJsonType) {
  EXPECT_EQ(fbs::Json_Null, getJsonType<std::nullptr_t>());
  EXPECT_EQ(fbs::Json_Null, getJsonType<decltype(nullptr)>());
  EXPECT_EQ(fbs::Json_Bool, getJsonType<bool>());
  EXPECT_EQ(fbs::Json_Int64, getJsonType<int64_t>());
  EXPECT_EQ(fbs::Json_Double, getJsonType<double>());
  EXPECT_EQ(fbs::Json_String, getJsonType<std::string>());
  EXPECT_EQ(fbs::Json_String, getJsonType<acc::StringPiece>());
  EXPECT_EQ(fbs::Json_Array, getJsonType<std::vector<bool>>());
  EXPECT_EQ(fbs::Json_Array, getJsonType<std::vector<int64_t>>());
  EXPECT_EQ(fbs::Json_Array, getJsonType<std::vector<double>>());
  EXPECT_EQ(fbs::Json_Array, getJsonType<std::vector<std::string>>());
  EXPECT_EQ(fbs::Json_Array, getJsonType<std::vector<acc::StringPiece>>());
  EXPECT_EQ(fbs::Json_Object, (getJsonType<std::map<int, int>>()));
}
