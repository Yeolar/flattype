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

#include <iostream>
#include <gtest/gtest.h>
#include "flattype/object/DynamicBuilder.h"

using namespace ftt;

TEST(Dynamic, build) {
  acc::dynamic value = acc::dynamic::object
    ("foo", "bar")
    ("junk", 12)
    ("another", 32.2)
    ("a",
      acc::dynamic::array(
        acc::dynamic::object("a", "b")
                            ("c", "d"),
        12.5,
        "Yo Dawg",
        acc::dynamic::array("heh"),
        nullptr
      )
    );

  DynamicBuilder builder(value);
  builder.finish();
  ftt::dynamic result = builder.toDynamic();

  EXPECT_STREQ(acc::toJson(value).c_str(), toJson(result).c_str());
}

