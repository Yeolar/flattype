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
#include "accelerator/GTestHelper.h"
#include "flattype/Serialize.h"

using namespace ftt;

TEST(Serialize, serialize) {
  std::map<int8_t, int8_t> h = {{1,2}, {3,4}};
  auto buf = serialize(h);
  std::map<int8_t, int8_t> v;
  unserialize(buf, v);
  acc::test::expectEq(h, v);
}

TEST(Serialize, serializeVariant) {
  std::nullptr_t _ = nullptr;
  bool a = true;
  int32_t b = 10;
  float c = 1.234;
  std::string d = "abc";
  acc::StringPiece e = "def";
  std::pair<int32_t, int32_t> f = std::make_pair(1,2);
  std::vector<bool> g = {true, false, true};
  std::map<acc::StringPiece, acc::StringPiece> h = {{"1","2"}, {"3","4"}};
  acc::fbstring i = "fbstring";
  auto buf = serializeVariant(_,a,b,c,d,e,f,g,h,i);
  std::nullptr_t n;
  bool o;
  int32_t p;
  float q;
  std::string r;
  acc::StringPiece s;
  std::pair<int32_t, int32_t> t;
  std::vector<bool> u;
  std::map<acc::StringPiece, acc::StringPiece> v;
  acc::fbstring w;
  unserializeVariant(buf, n,o,p,q,r,s,t,u,v,w);
  acc::test::expectEq(_, n);
  acc::test::expectEq(a, o);
  acc::test::expectEq(b, p);
  acc::test::expectEq(c, q);
  acc::test::expectEq(d, r);
  acc::test::expectEq(e, s);
  acc::test::expectEq(f, t);
  acc::test::expectEq(g, u);
  acc::test::expectEq(h, v);
  acc::test::expectEq(i, w);
}
