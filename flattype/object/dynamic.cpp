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
  constexpr fbs::Json dynamic::TypeInfo<T>::type; \
  //

FTT_DYNAMIC_DEF_TYPEINFO(std::nullptr_t)
FTT_DYNAMIC_DEF_TYPEINFO(bool)
FTT_DYNAMIC_DEF_TYPEINFO(acc::StringPiece)
FTT_DYNAMIC_DEF_TYPEINFO(dynamic::Array)
FTT_DYNAMIC_DEF_TYPEINFO(double)
FTT_DYNAMIC_DEF_TYPEINFO(int64_t)
FTT_DYNAMIC_DEF_TYPEINFO(dynamic::Object)

#undef FTT_DYNAMIC_DEF_TYPEINFO

dynamic::dynamic(fbs::Json type, const void* data)
  : type_(type),
    ptr_(data) {}

dynamic::dynamic(fbs::Json type, const uint8_t* data)
  : type_(type),
    ptr_(data ? ::flatbuffers::GetRoot<fbs::Object>(data) : nullptr) {}

dynamic::dynamic(fbs::Json type, ::flatbuffers::DetachedBuffer&& data)
  : type_(type),
    ptr_(data.data() ? ::flatbuffers::GetRoot<fbs::Object>(data.data())
                     : nullptr),
    data_(std::move(data)) {}

const char* dynamic::typeName() const {
  return typeName(type_);
}

TypeError::TypeError(const std::string& expected, fbs::Json actual)
  : std::runtime_error(acc::to<std::string>("TypeError: expected dynamic "
      "type `", expected, '\'', ", but had type `",
      dynamic::typeName(actual), '\''))
{}

// This is a higher-order preprocessor macro to aid going from runtime
// types to the compile time type system.
#define FTT_DYNAMIC_APPLY(type, apply)      \
  do {                                      \
    switch ((type)) {                       \
      case fbs::Json_Null:                  \
        apply(std::nullptr_t);              \
        break;                              \
      case fbs::Json_Array:                 \
        apply(Array);                       \
        break;                              \
      case fbs::Json_Bool:                  \
        apply(bool);                        \
        break;                              \
      case fbs::Json_Double:                \
        apply(double);                      \
        break;                              \
      case fbs::Json_Int64:                 \
        apply(int64_t);                     \
        break;                              \
      case fbs::Json_Object:                \
        apply(Object);                      \
        break;                              \
      case fbs::Json_String:                \
        apply(acc::StringPiece);            \
        break;                              \
      default:                              \
        ACC_CHECK_THROW(0, acc::Exception); \
    }                                       \
  } while (0)

bool dynamic::operator<(dynamic const& o) const {
  if (UNLIKELY(type_ == fbs::Json_Object || o.type_ == fbs::Json_Object ||
               type_ == fbs::Json_Array || o.type_ == fbs::Json_Array)) {
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
    return dynamic(parray->value_type()->GetEnum<fbs::Json>(idx),
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
    return dynamic(o->value_type(), o->value());
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
  case fbs::Json_Object:
  case fbs::Json_Array:
  case fbs::Json_Null:
    throw TypeError("not null/object/array", type());
  case fbs::Json_Int64:
    return std::hash<int64_t>()(getInt());
  case fbs::Json_Double:
    return std::hash<double>()(getDouble());
  case fbs::Json_Bool:
    return std::hash<bool>()(getBool());
  case fbs::Json_String: {
    const auto& str = getString();
    return ::acc::hash::fnv32_buf(str.data(), str.size());
  }
  default:
    ACC_CHECK_THROW(0, acc::Exception);
  }
}

char const* dynamic::typeName(fbs::Json t) {
#define FTT_X(T) return TypeInfo<T>::name
  FTT_DYNAMIC_APPLY(t, FTT_X);
#undef FTT_X
}

} // namespace ftt
