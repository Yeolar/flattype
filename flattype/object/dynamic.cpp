/*
 * Copyright 2018 Yeolar
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "flattype/object/dynamic.h"

#include <bits/functexcept.h>

#include "accelerator/Hash.h"
#include "flattype/object/Compare.h"

namespace ftt {

//////////////////////////////////////////////////////////////////////

#define FTT_DYNAMIC_DEF_TYPEINFO(T) \
  constexpr const char* dynamic::TypeInfo<T>::name; \
  constexpr dynamic::Type dynamic::TypeInfo<T>::type; \
  //

FTT_DYNAMIC_DEF_TYPEINFO(std::nullptr_t)
FTT_DYNAMIC_DEF_TYPEINFO(bool)
FTT_DYNAMIC_DEF_TYPEINFO(acc::StringPiece)
FTT_DYNAMIC_DEF_TYPEINFO(dynamic::Array)
FTT_DYNAMIC_DEF_TYPEINFO(double)
FTT_DYNAMIC_DEF_TYPEINFO(int64_t)
FTT_DYNAMIC_DEF_TYPEINFO(dynamic::Object)

#undef FTT_DYNAMIC_DEF_TYPEINFO

const char* dynamic::typeName() const {
  return typeName(type_);
}

TypeError::TypeError(const std::string& expected, dynamic::Type actual)
  : std::runtime_error(acc::to<std::string>("TypeError: expected dynamic "
      "type `", expected, '\'', ", but had type `",
      dynamic::typeName(actual), '\''))
{}

// This is a higher-order preprocessor macro to aid going from runtime
// types to the compile time type system.
#define FTT_DYNAMIC_APPLY(type, apply) \
  do {                                 \
    switch ((type)) {                  \
      case NULLT:                      \
        apply(std::nullptr_t);         \
        break;                         \
      case ARRAY:                      \
        apply(Array);                  \
        break;                         \
      case BOOL:                       \
        apply(bool);                   \
        break;                         \
      case DOUBLE:                     \
        apply(double);                 \
        break;                         \
      case INT64:                      \
        apply(int64_t);                \
        break;                         \
      case OBJECT:                     \
        apply(Object);                 \
        break;                         \
      case STRING:                     \
        apply(acc::StringPiece);       \
        break;                         \
      default:                         \
        ACCCHECK(0);                   \
    }                                  \
  } while (0)

bool dynamic::operator<(dynamic const& o) const {
  if (UNLIKELY(type_ == OBJECT || o.type_ == OBJECT ||
               type_ == ARRAY || o.type_ == ARRAY)) {
    throw TypeError("object", type_);
  }
  if (type_ != o.type_) {
    return type_ < o.type_;
  }

#define FTT_X(T) return *getAddress<T>() < *o.getAddress<T>();
  FTT_DYNAMIC_APPLY(type_, FTT_X);
#undef FTT_X
}

bool dynamic::operator==(dynamic const& o) const {
  if (type() != o.type()) {
    if (isNumber() && o.isNumber()) {
      auto& integ = isInt() ? *this : o;
      auto& doubl = isInt() ? o     : *this;
      return integ.asInt() == doubl.asDouble();
    }
    return false;
  }

#define FTT_X(T) return *getAddress<T>() == *o.getAddress<T>();
  FTT_DYNAMIC_APPLY(type_, FTT_X);
#undef FTT_X
}

dynamic dynamic::operator[](size_t k) const {
  return at(k);
}

dynamic dynamic::operator[](acc::StringPiece k) const {
  return at(k);
}

dynamic dynamic::at(size_t idx) const {
  if (auto* parray = get_nothrow<Array>()) {
    if (idx >= parray->value()->size()) {
      std::__throw_out_of_range("out of range in dynamic array");
    }
    return dynamic((Type) parray->value_type()->GetEnum<fbs::Json>(idx),
                   parray->value()->Get(idx));
  } else {
    throw TypeError("array", type());
  }
}

dynamic dynamic::at(acc::StringPiece idx) const {
  if (auto* pobject = get_nothrow<Object>()) {
    auto o = pobject->value()->LookupByKey(idx.data());
    if (o == nullptr) {
      throw std::out_of_range(acc::to<std::string>(
          "couldn't find key ", idx, " in dynamic object"));
    }
    return dynamic((Type) o->value_type(), o->value());
  } else {
    throw TypeError("object", type());
  }
}

std::size_t dynamic::size() const {
  if (auto* ar = get_nothrow<Array>()) {
    return ar->value()->size();
  }
  if (auto* obj = get_nothrow<Object>()) {
    return obj->value()->size();
  }
  if (auto* str = get_nothrow<acc::StringPiece>()) {
    return str->value()->size();
  }
  throw TypeError("string/array/object", type());
}

std::size_t dynamic::hash() const {
  switch (type()) {
  case OBJECT:
  case ARRAY:
  case NULLT:
    throw TypeError("not null/object/array", type());
  case INT64:
    return std::hash<int64_t>()(getInt());
  case DOUBLE:
    return std::hash<double>()(getDouble());
  case BOOL:
    return std::hash<bool>()(getBool());
  case STRING: {
    // keep it compatible with ACCString
    const auto& str = getString();
    return ::acc::hash::fnv32_buf(str.data(), str.size());
  }
  default:
    ACCCHECK(0);
  }
}

char const* dynamic::typeName(Type t) {
#define FTT_X(T) return TypeInfo<T>::name
  FTT_DYNAMIC_APPLY(t, FTT_X);
#undef FTT_X
}

} // namespace ftt
