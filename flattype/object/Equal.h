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

#include "flattype/idl/object_generated.h"
#include "flattype/Equal.h"

namespace ftt {
namespace json {

bool equal(fbs::Json type, const void* lhs, const void* rhs);

using ftt::operator==;

// Pair
inline bool operator==(const fbs::Pair& lhs, const fbs::Pair& rhs) {
  return *lhs.name() == *rhs.name() &&
    lhs.value_type() == rhs.value_type() &&
    equal(lhs.value_type(), lhs.value(), rhs.value());
}
inline bool operator!=(const fbs::Pair& lhs, const fbs::Pair& rhs) {
  return !(lhs == rhs);
}

// Array
bool operator==(const fbs::Array& lhs, const fbs::Array& rhs);

inline bool operator!=(const fbs::Array& lhs, const fbs::Array& rhs) {
  return !(lhs == rhs);
}

// Object
inline bool operator==(const fbs::Object& lhs, const fbs::Object& rhs) {
  return *lhs.value() == *rhs.value();
}
inline bool operator!=(const fbs::Object& lhs, const fbs::Object& rhs) {
  return !(lhs == rhs);
}

} // namespace json
} // namespace ftt
