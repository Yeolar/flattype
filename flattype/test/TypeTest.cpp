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
  EXPECT_EQ(fbs::Any::Null, getAnyType<std::nullptr_t>());
  EXPECT_EQ(fbs::Any::Null, getAnyType<decltype(nullptr)>());
  EXPECT_EQ(fbs::Any::Bool, getAnyType<bool>());
  EXPECT_EQ(fbs::Any::Int8, getAnyType<int8_t>());
  EXPECT_EQ(fbs::Any::Int16, getAnyType<int16_t>());
  EXPECT_EQ(fbs::Any::Int32, getAnyType<int32_t>());
  EXPECT_EQ(fbs::Any::Int64, getAnyType<int64_t>());
  EXPECT_EQ(fbs::Any::UInt8, getAnyType<uint8_t>());
  EXPECT_EQ(fbs::Any::UInt16, getAnyType<uint16_t>());
  EXPECT_EQ(fbs::Any::UInt32, getAnyType<uint32_t>());
  EXPECT_EQ(fbs::Any::UInt64, getAnyType<uint64_t>());
  EXPECT_EQ(fbs::Any::Float, getAnyType<float>());
  EXPECT_EQ(fbs::Any::Double, getAnyType<double>());
  EXPECT_EQ(fbs::Any::String, getAnyType<std::string>());
  EXPECT_EQ(fbs::Any::String, getAnyType<acc::StringPiece>());
  EXPECT_EQ(fbs::Any::BoolArray, getAnyType<std::vector<bool>>());
  EXPECT_EQ(fbs::Any::Int8Array, getAnyType<std::vector<int8_t>>());
  EXPECT_EQ(fbs::Any::Int16Array, getAnyType<std::vector<int16_t>>());
  EXPECT_EQ(fbs::Any::Int32Array, getAnyType<std::vector<int32_t>>());
  EXPECT_EQ(fbs::Any::Int64Array, getAnyType<std::vector<int64_t>>());
  EXPECT_EQ(fbs::Any::UInt8Array, getAnyType<std::vector<uint8_t>>());
  EXPECT_EQ(fbs::Any::UInt16Array, getAnyType<std::vector<uint16_t>>());
  EXPECT_EQ(fbs::Any::UInt32Array, getAnyType<std::vector<uint32_t>>());
  EXPECT_EQ(fbs::Any::UInt64Array, getAnyType<std::vector<uint64_t>>());
  EXPECT_EQ(fbs::Any::FloatArray, getAnyType<std::vector<float>>());
  EXPECT_EQ(fbs::Any::DoubleArray, getAnyType<std::vector<double>>());
  EXPECT_EQ(fbs::Any::StringArray, getAnyType<std::vector<std::string>>());
  EXPECT_EQ(fbs::Any::StringArray, getAnyType<std::vector<acc::StringPiece>>());
  EXPECT_EQ(fbs::Any::Tuple, (getAnyType<std::pair<int, int>>()));
  EXPECT_EQ(fbs::Any::Tuple, (getAnyType<std::map<int, int>>()));
  EXPECT_EQ(fbs::Any::Tuple, (getAnyType<std::vector<std::pair<int, int>>>()));
  EXPECT_EQ(fbs::Any::Tuple, getAnyType<std::vector<std::vector<int>>>());
}

TEST(Type, getJsonType) {
  EXPECT_EQ(fbs::Json::Null, getJsonType<std::nullptr_t>());
  EXPECT_EQ(fbs::Json::Null, getJsonType<decltype(nullptr)>());
  EXPECT_EQ(fbs::Json::Bool, getJsonType<bool>());
  EXPECT_EQ(fbs::Json::Int64, getJsonType<int64_t>());
  EXPECT_EQ(fbs::Json::Double, getJsonType<double>());
  EXPECT_EQ(fbs::Json::String, getJsonType<std::string>());
  EXPECT_EQ(fbs::Json::String, getJsonType<acc::StringPiece>());
  EXPECT_EQ(fbs::Json::Array, getJsonType<std::vector<bool>>());
  EXPECT_EQ(fbs::Json::Array, getJsonType<std::vector<int64_t>>());
  EXPECT_EQ(fbs::Json::Array, getJsonType<std::vector<double>>());
  EXPECT_EQ(fbs::Json::Array, getJsonType<std::vector<std::string>>());
  EXPECT_EQ(fbs::Json::Array, getJsonType<std::vector<acc::StringPiece>>());
  EXPECT_EQ(fbs::Json::Object, (getJsonType<std::map<int, int>>()));
}
