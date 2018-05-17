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

#pragma once

#include <boost/iterator/iterator_adaptor.hpp>
#include <boost/iterator/iterator_facade.hpp>

#include "accelerator/Conv.h"
#include "accelerator/Exception.h"
#include "accelerator/Macro.h"
#include "flattype/object/Serialize.h"

//////////////////////////////////////////////////////////////////////

namespace std {

template<>
struct hash< ::ftt::dynamic> {
  size_t operator()(::ftt::dynamic const& d) const {
    return d.hash();
  }
};

} // namespace std

//////////////////////////////////////////////////////////////////////

namespace ftt {

struct TypeError : std::runtime_error {
  explicit TypeError(const std::string& expected, fbs::Json actual);
  ~TypeError() {}
};

//////////////////////////////////////////////////////////////////////

struct dynamic::const_item_iterator
  : boost::iterator_adaptor<dynamic::const_item_iterator,
                            dynamic::ObjectImpl::const_iterator,
                            fbs::Pair const> {
  /* implicit */ const_item_iterator(base_type b) : iterator_adaptor_(b) { }

 private:
  fbs::Pair const& dereference() const {
    return **base_reference();
  }
  friend class boost::iterator_core_access;
};

struct dynamic::const_key_iterator
  : boost::iterator_adaptor<dynamic::const_key_iterator,
                            dynamic::ObjectImpl::const_iterator,
                            ::flatbuffers::String const> {
  /* implicit */ const_key_iterator(base_type b) : iterator_adaptor_(b) { }

 private:
  ::flatbuffers::String const& dereference() const {
    return *(*base_reference())->name();
  }
  friend class boost::iterator_core_access;
};

//////////////////////////////////////////////////////////////////////

// Integral types except bool convert to int64_t, float types to double.
template <class T>
struct dynamic::NumericTypeHelper<
    T, typename std::enable_if<std::is_integral<T>::value>::type> {
  using type = int64_t;
};
template <>
struct dynamic::NumericTypeHelper<bool> {
  using type = bool;
};
template <>
struct dynamic::NumericTypeHelper<float> {
  using type = double;
};
template <>
struct dynamic::NumericTypeHelper<double> {
  using type = double;
};

//////////////////////////////////////////////////////////////////////

inline dynamic::const_iterator dynamic::begin() const {
  return get<Array>().value()->begin();
}
inline dynamic::const_iterator dynamic::end() const {
  return get<Array>().value()->end();
}

template <class It>
struct dynamic::IterableProxy {
  typedef It const_iterator;
  typedef typename It::value_type value_type;

  /* implicit */ IterableProxy(const dynamic::ObjectImpl* o) : o_(o) { }

  It begin() const {
    return o_->begin();
  }

  It end() const {
    return o_->end();
  }

 private:
  const dynamic::ObjectImpl* o_;
};

inline dynamic::IterableProxy<dynamic::const_key_iterator> dynamic::keys()
  const {
  return get<Object>().value();
}

inline dynamic::IterableProxy<dynamic::const_item_iterator> dynamic::items()
  const {
  return get<Object>().value();
}

inline bool dynamic::isString() const { return type_ == fbs::Json::String; }
inline bool dynamic::isObject() const { return type_ == fbs::Json::Object; }
inline bool dynamic::isBool()   const { return type_ == fbs::Json::Bool; }
inline bool dynamic::isArray()  const { return type_ == fbs::Json::Array; }
inline bool dynamic::isDouble() const { return type_ == fbs::Json::Double; }
inline bool dynamic::isInt()    const { return type_ == fbs::Json::Int64; }
inline bool dynamic::isNull()   const { return type_ == fbs::Json::Null; }
inline bool dynamic::isNumber() const { return isInt() || isDouble(); }

inline fbs::Json dynamic::type() const {
  return type_;
}

inline std::string dynamic::asString() const { return asImpl<std::string>(); }
inline double      dynamic::asDouble() const { return asImpl<double>(); }
inline int64_t     dynamic::asInt()    const { return asImpl<int64_t>(); }
inline bool        dynamic::asBool()   const { return asImpl<bool>(); }

inline acc::StringPiece dynamic::getString() const { return getValue<acc::StringPiece>(); }
inline double           dynamic::getDouble() const { return getValue<double>(); }
inline int64_t          dynamic::getInt()    const { return getValue<int64_t>(); }
inline bool             dynamic::getBool()   const { return getValue<bool>(); }

inline double&          dynamic::getDouble() { return getValue<double>(); }
inline int64_t&         dynamic::getInt()    { return getValue<int64_t>(); }

inline const char* dynamic::data() const {
  return getValue<acc::StringPiece>().data();
}
inline acc::StringPiece dynamic::stringPiece() const {
  return getValue<acc::StringPiece>();
}

inline dynamic& dynamic::operator+=(int64_t i) {
  getValue<int64_t>() += i;
  return *this;
}

inline dynamic& dynamic::operator-=(int64_t i) {
  getValue<int64_t>() -= i;
  return *this;
}

inline dynamic& dynamic::operator*=(int64_t i) {
  getValue<int64_t>() *= i;
  return *this;
}

inline dynamic& dynamic::operator/=(int64_t i) {
  getValue<int64_t>() /= i;
  return *this;
}

inline dynamic& dynamic::operator%=(int64_t i) {
  getValue<int64_t>() %= i;
  return *this;
}

inline dynamic& dynamic::operator|=(int64_t i) {
  getValue<int64_t>() |= i;
  return *this;
}

inline dynamic& dynamic::operator&=(int64_t i) {
  getValue<int64_t>() &= i;
  return *this;
}

inline dynamic& dynamic::operator^=(int64_t i) {
  getValue<int64_t>() ^= i;
  return *this;
}

inline dynamic& dynamic::operator++() {
  ++getValue<int64_t>();
  return *this;
}

inline dynamic& dynamic::operator--() {
  --getValue<int64_t>();
  return *this;
}

inline dynamic& dynamic::operator+=(double i) {
  getValue<double>() += i;
  return *this;
}

inline dynamic& dynamic::operator-=(double i) {
  getValue<double>() -= i;
  return *this;
}

inline dynamic& dynamic::operator*=(double i) {
  getValue<double>() *= i;
  return *this;
}

inline dynamic& dynamic::operator/=(double i) {
  getValue<double>() /= i;
  return *this;
}

inline bool dynamic::empty() const {
  if (isNull()) {
    return true;
  }
  return !size();
}

inline std::size_t dynamic::count(acc::StringPiece key) const {
  return find(key) != items().end();
}

inline dynamic::const_item_iterator dynamic::find(acc::StringPiece key) const {
  return ftt::findConst(get<Object>().value(), key.data());
}

//////////////////////////////////////////////////////////////////////

#define FTT_DYNAMIC_DEC_TYPEINFO(T, str, val) \
  template <> struct dynamic::TypeInfo<T> { \
    static constexpr const char* name = str; \
    static constexpr fbs::Json type = val; \
  }; \
  //

FTT_DYNAMIC_DEC_TYPEINFO(std::nullptr_t,      "null",    fbs::Json::Null)
FTT_DYNAMIC_DEC_TYPEINFO(bool,                "boolean", fbs::Json::Bool)
FTT_DYNAMIC_DEC_TYPEINFO(acc::StringPiece,    "string",  fbs::Json::String)
FTT_DYNAMIC_DEC_TYPEINFO(dynamic::Array,      "array",   fbs::Json::Array)
FTT_DYNAMIC_DEC_TYPEINFO(double,              "double",  fbs::Json::Double)
FTT_DYNAMIC_DEC_TYPEINFO(int64_t,             "int64",   fbs::Json::Int64)
FTT_DYNAMIC_DEC_TYPEINFO(dynamic::Object,     "object",  fbs::Json::Object)

#undef FTT_DYNAMIC_DEC_TYPEINFO

template<class T>
T dynamic::asImpl() const {
  switch (type()) {
  case fbs::Json::Int64:  return acc::to<T>(ftt::getValue<int64_t>(ptr_));
  case fbs::Json::Double: return acc::to<T>(ftt::getValue<double>(ptr_));
  case fbs::Json::Bool:   return acc::to<T>(ftt::getValue<bool>(ptr_));
  case fbs::Json::String:
    return acc::to<T>(ftt::getValue<acc::StringPiece>(ptr_));
  default:
    throw TypeError("int/double/bool/string", type());
  }
}

template<class T, class FT>
FT const& dynamic::get() const {
  if (type_ != TypeInfo<T>::type) {
    throw TypeError(TypeInfo<T>::name, type());
  }
  return *getAddress<T>();
}


template<class T, class FT>
FT const* dynamic::get_nothrow() const noexcept {
  if (type_ != TypeInfo<T>::type) {
    return nullptr;
  }
  return getAddress<T>();
}

template<class T, class FT>
FT const* dynamic::getAddress() const noexcept {
  return reinterpret_cast<const typename JsonType<T>::type*>(ptr_);
}

template<class T>
T& dynamic::getValue() {
  if (type_ != TypeInfo<T>::type) {
    throw TypeError(TypeInfo<T>::name, type());
  }
  switch (type()) {
  case fbs::Json::Int64:
  case fbs::Json::Double:
    return *const_cast<T*>(ftt::getValuePtr<T>(ptr_));
  default:
    throw TypeError("int/double", type());
  }
}

template<class T>
T dynamic::getValue() const {
  if (type_ != TypeInfo<T>::type) {
    throw TypeError(TypeInfo<T>::name, type());
  }
  switch (type()) {
  case fbs::Json::Int64:
  case fbs::Json::Double:
  case fbs::Json::Bool:
  case fbs::Json::String:
    return ftt::getValue<T>(ptr_);
  default:
    throw TypeError("int/double/bool/string", type());
  }
}

//////////////////////////////////////////////////////////////////////

inline void dynamic::print(std::ostream& out) const {
  switch (type_) {
    case fbs::Json::Null:
      out << "null";
      break;
    case fbs::Json::Bool:
      out << getAddress<bool>()->value();
      break;
    case fbs::Json::Double:
      out << getAddress<double>()->value();
      break;
    case fbs::Json::Int64:
      out << getAddress<int64_t>()->value();
      break;
    case fbs::Json::String:
      out << getAddress<acc::StringPiece>()->value()->data();
      break;
    case fbs::Json::Array:
    case fbs::Json::Object: {
      out << toPseudoJson(*this);
      break;
    }
    default:
      ACC_CHECK_THROW(0, acc::Exception);
  }
}

inline std::ostream& operator<<(std::ostream& out, dynamic const& d) {
  d.print(out);
  return out;
}

} // namespace ftt
