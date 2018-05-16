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

#include "flattype/CommonIDLs.h"
#include "flattype/Compare.h"

namespace ftt {

bool equal(fbs::Any type, const void* lhs, const void* rhs);

// Tuple
bool operator==(const fbs::Tuple& lhs, const fbs::Tuple& rhs);

// Item
inline bool operator==(const fbs::Item& lhs, const fbs::Item& rhs) {
  return lhs.value_type() == rhs.value_type() &&
    equal(lhs.value_type(), lhs.value(), rhs.value());
}

// Record
inline bool operator==(const fbs::Record& lhs, const fbs::Record& rhs) {
  return *lhs.value() == *rhs.value();
}

// Matrix
inline bool operator==(const fbs::Matrix& lhs, const fbs::Matrix& rhs) {
  return *lhs.value() == *rhs.value();
}

} // namespace ftt
