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

namespace ftt {

template <class Tgt>
void toAppendJson(fbs::Json type, const void* ptr, Tgt* result);

} // namespace ftt

namespace acc {

template <class Tgt> void toAppend(const ftt::fbs::Pair&,   Tgt*);
template <class Tgt> void toAppend(const ftt::fbs::Array&,  Tgt*);
template <class Tgt> void toAppend(const ftt::fbs::Object&, Tgt*);

} // namespace acc

//////////////////////////////////////////////////////////////////////

#include "flattype/Stringize.h"

namespace ftt {

template <class Tgt>
void toAppendJson(fbs::Json type, const void* ptr, Tgt* result) {
  switch (type) {
#define FTT_JSON_TO_APPEND_CASE(ft, b, e) \
    case fbs::Json::ft: \
      if (b) acc::toAppend(b, result); \
      acc::toAppend(*reinterpret_cast<const fbs::ft*>(ptr), result); \
      if (e) acc::toAppend(e, result); \
      break;

    FTT_JSON_TO_APPEND_CASE(Null,   0,   0)
    FTT_JSON_TO_APPEND_CASE(Bool,   0,   0)
    FTT_JSON_TO_APPEND_CASE(Int64,  0,   0)
    FTT_JSON_TO_APPEND_CASE(Double, 0,   0)
    FTT_JSON_TO_APPEND_CASE(String, '"', '"')
    FTT_JSON_TO_APPEND_CASE(Array,  '[', ']')
    FTT_JSON_TO_APPEND_CASE(Object, '{', '}')

#undef FTT_JSON_TO_APPEND_CASE

    case fbs::Json::NONE:
      break;
  }
}

} // namespace ftt

namespace acc {

template <class Tgt>
void toAppend(const ftt::fbs::Pair& value, Tgt* result) {
  toAppend("\"", value.name(), "\":", result);
  ftt::toAppendJson(value.value_type(), value.value(), result);
}

template <class Tgt>
void toAppend(const ftt::fbs::Array& value, Tgt* result) {
  auto types = value.value_type();
  for (size_t i = 0; i < types->size(); i++) {
    if (i > 0) {
      toAppend(',', result);
    }
    ftt::toAppendJson(
        types->GetEnum<ftt::fbs::Json>(i), value.value()->Get(i), result);
  }
}

template <class Tgt>
void toAppend(const ftt::fbs::Object& value, Tgt* result) {
  toAppend(value.value(), result);
}

} // namespace acc
