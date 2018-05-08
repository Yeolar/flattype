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
#include "flattype/Copy.h"
#include "flattype/Equal.h"
#include "flattype/Serialize.h"

using namespace ftt;

TEST(Value, base) {
  {
    std::nullptr_t v = nullptr;
    auto buf1 = serialize(v);
    auto p = ::flatbuffers::GetRoot<fbs::Null>(buf1.data());
    ::flatbuffers::FlatBufferBuilder fbb;
    fbb.Finish(copy(fbb, *p));
    auto buf2 = fbb.Release();
    auto q = ::flatbuffers::GetRoot<fbs::Null>(buf2.data());
    EXPECT_TRUE(*p == *q);
  }
  {
    bool v = true;
    auto buf1 = serialize(v);
    auto p = ::flatbuffers::GetRoot<fbs::Bool>(buf1.data());
    ::flatbuffers::FlatBufferBuilder fbb;
    fbb.Finish(copy(fbb, *p));
    auto buf2 = fbb.Release();
    auto q = ::flatbuffers::GetRoot<fbs::Bool>(buf2.data());
    EXPECT_TRUE(*p == *q);
  }
  {
    int8_t v = 10;
    auto buf1 = serialize(v);
    auto p = ::flatbuffers::GetRoot<fbs::Int8>(buf1.data());
    ::flatbuffers::FlatBufferBuilder fbb;
    fbb.Finish(copy(fbb, *p));
    auto buf2 = fbb.Release();
    auto q = ::flatbuffers::GetRoot<fbs::Int8>(buf2.data());
    EXPECT_TRUE(*p == *q);
  }
  {
    uint64_t v = uint64_t(-1);
    auto buf1 = serialize(v);
    auto p = ::flatbuffers::GetRoot<fbs::UInt64>(buf1.data());
    ::flatbuffers::FlatBufferBuilder fbb;
    fbb.Finish(copy(fbb, *p));
    auto buf2 = fbb.Release();
    auto q = ::flatbuffers::GetRoot<fbs::UInt64>(buf2.data());
    EXPECT_TRUE(*p == *q);
  }
  {
    acc::StringPiece v = "10";
    auto buf1 = serialize(v);
    auto p = ::flatbuffers::GetRoot<fbs::String>(buf1.data());
    ::flatbuffers::FlatBufferBuilder fbb;
    fbb.Finish(copy(fbb, *p));
    auto buf2 = fbb.Release();
    auto q = ::flatbuffers::GetRoot<fbs::String>(buf2.data());
    EXPECT_TRUE(*p == *q);
  }
}

TEST(Value, array) {
  {
    std::vector<int32_t> v = {1,2,3,4};
    auto buf1 = serialize(v);
    auto p = ::flatbuffers::GetRoot<fbs::Int32Array>(buf1.data());
    ::flatbuffers::FlatBufferBuilder fbb;
    fbb.Finish(copy(fbb, *p));
    auto buf2 = fbb.Release();
    auto q = ::flatbuffers::GetRoot<fbs::Int32Array>(buf2.data());
    EXPECT_TRUE(*p == *q);
  }
  {
    std::vector<const char*> v = {"1","2","3","4"};
    auto buf1 = serialize(v);
    auto p = ::flatbuffers::GetRoot<fbs::StringArray>(buf1.data());
    ::flatbuffers::FlatBufferBuilder fbb;
    fbb.Finish(copy(fbb, *p));
    auto buf2 = fbb.Release();
    auto q = ::flatbuffers::GetRoot<fbs::StringArray>(buf2.data());
    EXPECT_TRUE(*p == *q);
  }
}
