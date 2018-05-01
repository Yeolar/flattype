/*
 * Copyright 2017 Facebook, Inc.
 * Copyright 2017 Yeolar
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

#include "flattype/json.h"

#include <bits/functexcept.h>

namespace flattype {

//////////////////////////////////////////////////////////////////////

#define FTYPE_DYNAMIC_DEF_TYPEINFO(T) \
  constexpr const char* dynamic::TypeInfo<T>::name; \
  constexpr dynamic::Type dynamic::TypeInfo<T>::type; \
  //

FTYPE_DYNAMIC_DEF_TYPEINFO(void*)
FTYPE_DYNAMIC_DEF_TYPEINFO(bool)
FTYPE_DYNAMIC_DEF_TYPEINFO(acc::StringPiece)
FTYPE_DYNAMIC_DEF_TYPEINFO(dynamic::Array)
FTYPE_DYNAMIC_DEF_TYPEINFO(double)
FTYPE_DYNAMIC_DEF_TYPEINFO(int64_t)
FTYPE_DYNAMIC_DEF_TYPEINFO(dynamic::Object)

#undef FTYPE_DYNAMIC_DEF_TYPEINFO

const char* dynamic::typeName() const {
  return fbs::json::EnumNameType(data_->value_type());
}

TypeError::TypeError(const std::string& expected, dynamic::Type actual)
  : std::runtime_error(
      acc::to<std::string>("TypeError: expected dynamic type `",
                           expected, '\'', ", but had type `",
                           dynamic::typeName(actual), '\''))
{}

// This is a higher-order preprocessor macro to aid going from runtime
// types to the compile time type system.
#define FTYPE_DYNAMIC_APPLY(type, apply) \
  do {                                 \
    switch ((type)) {                  \
      case NULLT:                      \
        apply(void*);                  \
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
        apply(ObjectImpl);             \
        break;                         \
      case STRING:                     \
        apply(std::string);            \
        break;                         \
      default:                         \
        FTYPECHECK(0);                   \
        abort();                       \
    }                                  \
  } while (0)

bool dynamic::operator<(dynamic const& o) const {
  if (UNLIKELY(type_ == OBJECT || o.type_ == OBJECT)) {
    throw TypeError("object", type_);
  }
  if (type_ != o.type_) {
    return type_ < o.type_;
  }

#define FTYPE_X(T) return CompareOp<T>::comp(*getAddress<T>(),   \
                                           *o.getAddress<T>())
  FTYPE_DYNAMIC_APPLY(type_, FTYPE_X);
#undef FTYPE_X
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

#define FTYPE_X(T) return *getAddress<T>() == *o.getAddress<T>();
  FTYPE_DYNAMIC_APPLY(type_, FTYPE_X);
#undef FTYPE_X
}

dynamic& dynamic::operator[](dynamic const& k) & {
  if (!isObject() && !isArray()) {
    throw TypeError("object/array", type());
  }
  if (isArray()) {
    return at(k);
  }
  auto& obj = get<ObjectImpl>();
  auto ret = obj.insert({k, nullptr});
  return ret.first->second;
}

dynamic dynamic::getDefault(const dynamic& k, const dynamic& v) const& {
  auto& obj = get<ObjectImpl>();
  auto it = obj.find(k);
  return it == obj.end() ? v : it->second;
}

dynamic dynamic::getDefault(const dynamic& k, dynamic&& v) const& {
  auto& obj = get<ObjectImpl>();
  auto it = obj.find(k);
  // Avoid clang bug with ternary
  if (it == obj.end()) {
    return std::move(v);
  } else {
    return it->second;
  }
}

dynamic dynamic::getDefault(const dynamic& k, const dynamic& v) && {
  auto& obj = get<ObjectImpl>();
  auto it = obj.find(k);
  // Avoid clang bug with ternary
  if (it == obj.end()) {
    return v;
  } else {
    return std::move(it->second);
  }
}

dynamic dynamic::getDefault(const dynamic& k, dynamic&& v) && {
  auto& obj = get<ObjectImpl>();
  auto it = obj.find(k);
  return std::move(it == obj.end() ? v : it->second);
}

const dynamic* dynamic::get_ptr(dynamic const& idx) const& {
  if (auto* parray = get_nothrow<Array>()) {
    if (!idx.isInt()) {
      throw TypeError("int64", idx.type());
    }
    if (idx < 0 || idx >= parray->size()) {
      return nullptr;
    }
    return &(*parray)[idx.asInt()];
  } else if (auto* pobject = get_nothrow<ObjectImpl>()) {
    auto it = pobject->find(idx);
    if (it == pobject->end()) {
      return nullptr;
    }
    return &it->second;
  } else {
    throw TypeError("object/array", type());
  }
}

template <class T>
typename std::enable_if<std::is_integral<T>::value, dynamic>::type
dynamic::at(T const& idx) const {
  if (type_ == ARRAY) {
    const Array& array = getIterable<Array>();
    if (idx < 0 || idx >= array->size()) {
      std::__throw_out_of_range("out of range in dynamic array");
    }
    return array[i];
  } else if (type_ == OBJECT) {
    const Object& object = getIterable<Object>();
    auto it = pobject->find(idx);
    if (it == pobject->end()) {
      throw std::out_of_range(to<std::string>(
          "couldn't find key ", idx.asString(), " in dynamic object"));
    }
    return it->second;
  } else {
    throw TypeError("object/array", type());
  }
}

dynamic const& dynamic::at(dynamic const& idx) const& {
  if (type_ == ARRAY) {
    if (!idx.isInt()) {
      throw TypeError("int64", idx.type());
    }
    if (idx < 0 || idx >= parray->size()) {
      std::__throw_out_of_range("out of range in dynamic array");
    }
    return (*parray)[idx.asInt()];
  } else if (auto* pobject = get_nothrow<ObjectImpl>()) {
    auto it = pobject->find(idx);
    if (it == pobject->end()) {
      throw std::out_of_range(to<std::string>(
          "couldn't find key ", idx.asString(), " in dynamic object"));
    }
    return it->second;
  } else {
    throw TypeError("object/array", type());
  }
}

std::size_t dynamic::size() const {
  switch (type()) {
  case ARRAY:  return asType<fbs::json::Array>(data_)->value()->size();
  case OBJECT: return asType<fbs::json::Object>(data_)->value()->size();
  case STRING: return asType<fbs::json::String>(data_)->value()->size();
  default:
    throw TypeError("array/object/string", type());
  }
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
    // keep it compatible with FTYPEString
    const auto& str = getString();
    return ::dynamic::hash::fnv32_buf(str.data(), str.size());
  }
  default:
    FTYPECHECK(0); abort();
  }
}

char const* dynamic::typeName(Type t) {
#define FTYPE_X(T) return TypeInfo<T>::name
  FTYPE_DYNAMIC_APPLY(t, FTYPE_X);
#undef FTYPE_X
}

} // namespace flattype
