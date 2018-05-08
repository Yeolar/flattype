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
#include "flattype/Serialize.h"
#include "flattype/Stringize.h"

using namespace ftt;

TEST(Stringize, base) {
  {
    std::nullptr_t v = nullptr;
    auto buf = serialize(v);
    auto p = ::flatbuffers::GetRoot<fbs::Null>(buf.data());
    EXPECT_STREQ("<NULL>", acc::to<std::string>(*p).c_str());
  }
  {
    bool v = true;
    auto buf = serialize(v);
    auto p = ::flatbuffers::GetRoot<fbs::Bool>(buf.data());
    EXPECT_STREQ("1", acc::to<std::string>(*p).c_str());
  }
  {
    int8_t v = 10;
    auto buf = serialize(v);
    auto p = ::flatbuffers::GetRoot<fbs::Int8>(buf.data());
    EXPECT_STREQ("10", acc::to<std::string>(*p).c_str());
  }
  {
    uint64_t v = uint64_t(-1);
    auto buf = serialize(v);
    auto p = ::flatbuffers::GetRoot<fbs::UInt64>(buf.data());
    EXPECT_STREQ("18446744073709551615", acc::to<std::string>(*p).c_str());
  }
  {
    acc::StringPiece v = "10";
    auto buf = serialize(v);
    auto p = ::flatbuffers::GetRoot<fbs::String>(buf.data());
    EXPECT_STREQ("10", acc::to<std::string>(*p).c_str());
  }
}

TEST(Stringize, array) {
  {
    std::vector<int32_t> v = {1,2,3,4};
    auto buf = serialize(v);
    auto p = ::flatbuffers::GetRoot<fbs::Int32Array>(buf.data());
    EXPECT_STREQ("1,2,3,4", acc::to<std::string>(*p).c_str());
  }
  {
    std::vector<const char*> v = {"1","2","3","4"};
    auto buf = serialize(v);
    auto p = ::flatbuffers::GetRoot<fbs::StringArray>(buf.data());
    EXPECT_STREQ("1,2,3,4", acc::to<std::string>(*p).c_str());
  }
}
