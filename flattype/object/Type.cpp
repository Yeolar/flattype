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

#include "flattype/object/Type.h"

namespace ftt {

fbs::Json toJsonType(acc::dynamic::Type type) {
  switch (type) {
    case acc::dynamic::NULLT:  return fbs::Json_Null;
    case acc::dynamic::ARRAY:  return fbs::Json_Array;
    case acc::dynamic::BOOL:   return fbs::Json_Bool;
    case acc::dynamic::DOUBLE: return fbs::Json_Double;
    case acc::dynamic::INT64:  return fbs::Json_Int64;
    case acc::dynamic::OBJECT: return fbs::Json_Object;
    case acc::dynamic::STRING: return fbs::Json_String;
  }
}

} // namespace ftt
