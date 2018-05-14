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

#include <functional>
#include <boost/iterator/iterator_adaptor.hpp>
#include <boost/iterator/iterator_facade.hpp>

#include "accelerator/Conv.h"
#include "accelerator/Logging.h"
#include "accelerator/Macro.h"

//////////////////////////////////////////////////////////////////////

namespace std {

template<>
struct hash< ::ftt::dynamic> {
  size_t operator()(::acc::dynamic const& d) const {
    return d.hash();
  }
};

} // namespace std

//////////////////////////////////////////////////////////////////////

// This is a higher-order preprocessor macro to aid going from runtime
// types to the compile time type system.
#define ACC_DYNAMIC_APPLY(type, apply) \
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
        ACCCHECK(0);                   \
        abort();                       \
    }                                  \
  } while (0)

//////////////////////////////////////////////////////////////////////

namespace acc {

struct TypeError : std::runtime_error {
  explicit TypeError(const std::string& expected, dynamic::Type actual);
  explicit TypeError(const std::string& expected,
    dynamic::Type actual1, dynamic::Type actual2);
  ~TypeError();
};


//////////////////////////////////////////////////////////////////////

namespace detail {

  // This helper is used in destroy() to be able to run destructors on
  // types like "int64_t" without a compiler error.
  struct Destroy {
    template<class T> static void destroy(T* t) { t->~T(); }
  };

  /*
   * Helper for implementing numeric conversions in operators on
   * numbers.  Just promotes to double when one of the arguments is
   * double, or throws if either is not a numeric type.
   */
  template<template<class> class Op>
  dynamic numericOp(dynamic const& a, dynamic const& b) {
    if (!a.isNumber() || !b.isNumber()) {
      throw TypeError("numeric", a.type(), b.type());
    }
    if (a.type() != b.type()) {
      auto& integ  = a.isInt() ? a : b;
      auto& nonint = a.isInt() ? b : a;
      return Op<double>()(to<double>(integ.asInt()), nonint.asDouble());
    }
    if (a.isDouble()) {
      return Op<double>()(a.asDouble(), b.asDouble());
    }
    return Op<int64_t>()(a.asInt(), b.asInt());
  }

} // namespace detail

//////////////////////////////////////////////////////////////////////

/*
 * We're doing this instead of a simple member typedef to avoid the
 * undefined behavior of parameterizing std::unordered_map<> with an
 * incomplete type.
 *
 * Note: Later we may add separate order tracking here (a multi-index
 * type of thing.)
 */
struct dynamic::ObjectImpl : std::unordered_map<dynamic, dynamic> {};

//////////////////////////////////////////////////////////////////////

// Helper object for creating objects conveniently.  See object and
// the dynamic::dynamic(ObjectMaker&&) ctor.
struct dynamic::ObjectMaker {
  friend struct dynamic;

  explicit ObjectMaker() : val_(dynamic::object) {}
  explicit ObjectMaker(dynamic key, dynamic val)
    : val_(dynamic::object)
  {
    val_.insert(std::move(key), std::move(val));
  }

  // Make sure no one tries to save one of these into an lvalue with
  // auto or anything like that.
  ObjectMaker(ObjectMaker&&) = default;
  ObjectMaker(ObjectMaker const&) = delete;
  ObjectMaker& operator=(ObjectMaker const&) = delete;
  ObjectMaker& operator=(ObjectMaker&&) = delete;

  // This returns an rvalue-reference instead of an lvalue-reference
  // to allow constructs like this to moved instead of copied:
  //  dynamic a = dynamic::object("a", "b")("c", "d")
  ObjectMaker&& operator()(dynamic key, dynamic val) {
    val_.insert(std::move(key), std::move(val));
    return std::move(*this);
  }

 private:
  dynamic val_;
};

inline void dynamic::array(EmptyArrayTag) {}

template <class... Args>
inline dynamic dynamic::array(Args&& ...args) {
  return dynamic(Array{std::forward<Args>(args)...});
}

inline dynamic::ObjectMaker dynamic::object() { return ObjectMaker(); }
inline dynamic::ObjectMaker dynamic::object(dynamic a, dynamic b) {
  return ObjectMaker(std::move(a), std::move(b));
}

//////////////////////////////////////////////////////////////////////

struct dynamic::const_item_iterator
  : boost::iterator_adaptor<dynamic::const_item_iterator,
                            dynamic::ObjectImpl::const_iterator> {
  /* implicit */ const_item_iterator(base_type b) : iterator_adaptor_(b) { }

 private:
  friend class boost::iterator_core_access;
};

struct dynamic::const_key_iterator
  : boost::iterator_adaptor<dynamic::const_key_iterator,
                            dynamic::ObjectImpl::const_iterator,
                            dynamic const> {
  /* implicit */ const_key_iterator(base_type b) : iterator_adaptor_(b) { }

 private:
  dynamic const& dereference() const {
    return base_reference()->first;
  }
  friend class boost::iterator_core_access;
};

struct dynamic::const_value_iterator
  : boost::iterator_adaptor<dynamic::const_value_iterator,
                            dynamic::ObjectImpl::const_iterator,
                            dynamic const> {
  /* implicit */ const_value_iterator(base_type b) : iterator_adaptor_(b) { }

 private:
  dynamic const& dereference() const {
    return base_reference()->second;
  }
  friend class boost::iterator_core_access;
};

//////////////////////////////////////////////////////////////////////

inline dynamic::dynamic() : dynamic(nullptr) {}

inline dynamic::dynamic(std::nullptr_t) : type_(NULLT) {}

inline dynamic::dynamic(void (*)(EmptyArrayTag))
  : type_(ARRAY)
{
  new (&u_.array) Array();
}

inline dynamic::dynamic(ObjectMaker (*)())
  : type_(OBJECT)
{
  new (getAddress<ObjectImpl>()) ObjectImpl();
}

inline dynamic::dynamic(StringPiece s)
  : type_(STRING)
{
  new (&u_.string) std::string(s.data(), s.size());
}

inline dynamic::dynamic(char const* s)
  : type_(STRING)
{
  new (&u_.string) std::string(s);
}

inline dynamic::dynamic(std::string s)
  : type_(STRING)
{
  new (&u_.string) std::string(std::move(s));
}

inline dynamic::dynamic(ObjectMaker&& maker)
  : type_(OBJECT)
{
  new (getAddress<ObjectImpl>())
    ObjectImpl(std::move(*maker.val_.getAddress<ObjectImpl>()));
}

inline dynamic::dynamic(dynamic const& o)
  : type_(NULLT)
{
  *this = o;
}

inline dynamic::dynamic(dynamic&& o) noexcept
  : type_(NULLT)
{
  *this = std::move(o);
}

inline dynamic::~dynamic() noexcept { destroy(); }

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

template<class T, class NumericType /* = typename NumericTypeHelper<T>::type */>
dynamic::dynamic(T t) {
  type_ = TypeInfo<NumericType>::type;
  new (getAddress<NumericType>()) NumericType(t);
}

template <class Iterator>
dynamic::dynamic(Iterator first, Iterator last)
  : type_(ARRAY)
{
  new (&u_.array) Array(first, last);
}

//////////////////////////////////////////////////////////////////////

inline dynamic::const_iterator dynamic::begin() const {
  return get<Array>().begin();
}
inline dynamic::const_iterator dynamic::end() const {
  return get<Array>().end();
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
  return &(get<ObjectImpl>());
}

inline dynamic::IterableProxy<dynamic::const_value_iterator> dynamic::values()
  const {
  return &(get<ObjectImpl>());
}

inline dynamic::IterableProxy<dynamic::const_item_iterator> dynamic::items()
  const {
  return &(get<ObjectImpl>());
}

inline bool dynamic::isString() const {
  return get_nothrow<std::string>();
}
inline bool dynamic::isObject() const { return get_nothrow<ObjectImpl>(); }
inline bool dynamic::isBool()   const { return get_nothrow<bool>(); }
inline bool dynamic::isArray()  const { return get_nothrow<Array>(); }
inline bool dynamic::isDouble() const { return get_nothrow<double>(); }
inline bool dynamic::isInt()    const { return get_nothrow<int64_t>(); }
inline bool dynamic::isNull()   const { return get_nothrow<void*>(); }
inline bool dynamic::isNumber() const { return isInt() || isDouble(); }

inline dynamic::Type dynamic::type() const {
  return type_;
}

inline std::string dynamic::asString() const {
  return asImpl<std::string>();
}
inline double dynamic::asDouble() const {
  return asImpl<double>();
}
inline int64_t dynamic::asInt() const {
  return asImpl<int64_t>();
}
inline bool dynamic::asBool() const {
  return asImpl<bool>();
}

inline const std::string& dynamic::getString() const& {
  return get<std::string>();
}
inline double             dynamic::getDouble() const& { return get<double>(); }
inline int64_t            dynamic::getInt()    const& { return get<int64_t>(); }
inline bool               dynamic::getBool()   const& { return get<bool>(); }

inline std::string& dynamic::getString()& {
  return get<std::string>();
}
inline double&      dynamic::getDouble() & { return get<double>(); }
inline int64_t&     dynamic::getInt()    & { return get<int64_t>(); }
inline bool&        dynamic::getBool()   & { return get<bool>(); }

inline std::string&& dynamic::getString()&& {
  return std::move(get<std::string>());
}
inline double        dynamic::getDouble() && { return get<double>(); }
inline int64_t       dynamic::getInt()    && { return get<int64_t>(); }
inline bool          dynamic::getBool()   && { return get<bool>(); }

inline const char* dynamic::data() const& {
  return get<std::string>().data();
}
inline const char* dynamic::c_str() const& {
  return get<std::string>().c_str();
}
inline StringPiece dynamic::stringPiece() const {
  return get<std::string>();
}

template<class T>
struct dynamic::CompareOp {
  static bool comp(T const& a, T const& b) { return a < b; }
};
template<>
struct dynamic::CompareOp<dynamic::ObjectImpl> {
  static bool comp(ObjectImpl const&, ObjectImpl const&) {
    // This code never executes; it is just here for the compiler.
    return false;
  }
};

inline dynamic& dynamic::operator+=(dynamic const& o) {
  if (type() == STRING && o.type() == STRING) {
    *getAddress<std::string>() += *o.getAddress<std::string>();
    return *this;
  }
  *this = detail::numericOp<std::plus>(*this, o);
  return *this;
}

inline dynamic& dynamic::operator-=(dynamic const& o) {
  *this = detail::numericOp<std::minus>(*this, o);
  return *this;
}

inline dynamic& dynamic::operator*=(dynamic const& o) {
  *this = detail::numericOp<std::multiplies>(*this, o);
  return *this;
}

inline dynamic& dynamic::operator/=(dynamic const& o) {
  *this = detail::numericOp<std::divides>(*this, o);
  return *this;
}

#define ACC_DYNAMIC_INTEGER_OP(op)                          \
  inline dynamic& dynamic::operator op(dynamic const& o) {  \
    if (!isInt() || !o.isInt()) {                           \
      throw TypeError("int64", type(), o.type());           \
    }                                                       \
    *getAddress<int64_t>() op o.asInt();                    \
    return *this;                                           \
  }

ACC_DYNAMIC_INTEGER_OP(%=)
ACC_DYNAMIC_INTEGER_OP(|=)
ACC_DYNAMIC_INTEGER_OP(&=)
ACC_DYNAMIC_INTEGER_OP(^=)

#undef ACC_DYNAMIC_INTEGER_OP

inline dynamic& dynamic::operator++() {
  ++get<int64_t>();
  return *this;
}

inline dynamic& dynamic::operator--() {
  --get<int64_t>();
  return *this;
}

inline dynamic const& dynamic::operator[](dynamic const& idx) const& {
  return at(idx);
}

inline dynamic&& dynamic::operator[](dynamic const& idx) && {
  return std::move((*this)[idx]);
}

template<class K, class V> inline dynamic& dynamic::setDefault(K&& k, V&& v) {
  auto& obj = get<ObjectImpl>();
  return obj.insert(std::make_pair(std::forward<K>(k),
                                   std::forward<V>(v))).first->second;
}

template<class K> inline dynamic& dynamic::setDefault(K&& k, dynamic&& v) {
  auto& obj = get<ObjectImpl>();
  return obj.insert(std::make_pair(std::forward<K>(k),
                                   std::move(v))).first->second;
}

template<class K> inline dynamic& dynamic::setDefault(K&& k, const dynamic& v) {
  auto& obj = get<ObjectImpl>();
  return obj.insert(std::make_pair(std::forward<K>(k), v)).first->second;
}

inline dynamic* dynamic::get_ptr(dynamic const& idx) & {
  return const_cast<dynamic*>(const_cast<dynamic const*>(this)->get_ptr(idx));
}

inline dynamic& dynamic::at(dynamic const& idx) & {
  return const_cast<dynamic&>(const_cast<dynamic const*>(this)->at(idx));
}

inline dynamic&& dynamic::at(dynamic const& idx) && {
  return std::move(at(idx));
}

inline bool dynamic::empty() const {
  if (isNull()) {
    return true;
  }
  return !size();
}

inline std::size_t dynamic::count(dynamic const& key) const {
  return find(key) != items().end();
}

inline dynamic::const_item_iterator dynamic::find(dynamic const& key) const {
  return get<ObjectImpl>().find(key);
}

template<class K, class V> inline void dynamic::insert(K&& key, V&& val) {
  auto& obj = get<ObjectImpl>();
  auto rv = obj.insert({ std::forward<K>(key), nullptr });
  rv.first->second = std::forward<V>(val);
}

inline void dynamic::update(const dynamic& mergeObj) {
  if (!isObject() || !mergeObj.isObject()) {
    throw TypeError("object", type(), mergeObj.type());
  }

  for (const auto& pair : mergeObj.items()) {
    (*this)[pair.first] = pair.second;
  }
}

inline void dynamic::update_missing(const dynamic& mergeObj1) {
  if (!isObject() || !mergeObj1.isObject()) {
    throw TypeError("object", type(), mergeObj1.type());
  }

  // Only add if not already there
  for (const auto& pair : mergeObj1.items()) {
    if ((*this).find(pair.first) == (*this).items().end()) {
      (*this)[pair.first] = pair.second;
    }
  }
}

inline dynamic dynamic::merge(
    const dynamic& mergeObj1,
    const dynamic& mergeObj2) {

  // No checks on type needed here because they are done in update_missing
  // Note that we do update_missing here instead of update() because
  // it will prevent the extra writes that would occur with update()
  auto ret = mergeObj2;
  ret.update_missing(mergeObj1);
  return ret;
}

inline std::size_t dynamic::erase(dynamic const& key) {
  auto& obj = get<ObjectImpl>();
  return obj.erase(key);
}

inline dynamic::const_iterator dynamic::erase(const_iterator it) {
  auto& arr = get<Array>();
  // std::vector doesn't have an erase method that works on const iterators,
  // even though the standard says it should, so this hack converts to a
  // non-const iterator before calling erase.
  return get<Array>().erase(arr.begin() + (it - arr.begin()));
}

inline dynamic::const_key_iterator dynamic::erase(const_key_iterator it) {
  return const_key_iterator(get<ObjectImpl>().erase(it.base()));
}

inline dynamic::const_key_iterator dynamic::erase(const_key_iterator first,
                                                  const_key_iterator last) {
  return const_key_iterator(get<ObjectImpl>().erase(first.base(),
                                                    last.base()));
}

inline dynamic::const_value_iterator dynamic::erase(const_value_iterator it) {
  return const_value_iterator(get<ObjectImpl>().erase(it.base()));
}

inline dynamic::const_value_iterator dynamic::erase(const_value_iterator first,
                                                    const_value_iterator last) {
  return const_value_iterator(get<ObjectImpl>().erase(first.base(),
                                                      last.base()));
}

inline dynamic::const_item_iterator dynamic::erase(const_item_iterator it) {
  return const_item_iterator(get<ObjectImpl>().erase(it.base()));
}

inline dynamic::const_item_iterator dynamic::erase(const_item_iterator first,
                                                   const_item_iterator last) {
  return const_item_iterator(get<ObjectImpl>().erase(first.base(),
                                                     last.base()));
}

inline void dynamic::resize(std::size_t sz, dynamic const& c) {
  auto& arr = get<Array>();
  arr.resize(sz, c);
}

inline void dynamic::push_back(dynamic const& v) {
  auto& arr = get<Array>();
  arr.push_back(v);
}

inline void dynamic::push_back(dynamic&& v) {
  auto& arr = get<Array>();
  arr.push_back(std::move(v));
}

inline void dynamic::pop_back() {
  auto& arr = get<Array>();
  arr.pop_back();
}

//////////////////////////////////////////////////////////////////////

inline dynamic::dynamic(Array&& r) : type_(ARRAY) {
  new (&u_.array) Array(std::move(r));
}

#define ACC_DYNAMIC_DEC_TYPEINFO(T, str, val) \
  template <> struct dynamic::TypeInfo<T> { \
    static constexpr const char* name = str; \
    static constexpr dynamic::Type type = val; \
  }; \
  //

ACC_DYNAMIC_DEC_TYPEINFO(void*,               "null",    dynamic::NULLT)
ACC_DYNAMIC_DEC_TYPEINFO(bool,                "boolean", dynamic::BOOL)
ACC_DYNAMIC_DEC_TYPEINFO(std::string,         "string",  dynamic::STRING)
ACC_DYNAMIC_DEC_TYPEINFO(dynamic::Array,      "array",   dynamic::ARRAY)
ACC_DYNAMIC_DEC_TYPEINFO(double,              "double",  dynamic::DOUBLE)
ACC_DYNAMIC_DEC_TYPEINFO(int64_t,             "int64",   dynamic::INT64)
ACC_DYNAMIC_DEC_TYPEINFO(dynamic::ObjectImpl, "object",  dynamic::OBJECT)

#undef ACC_DYNAMIC_DEC_TYPEINFO

template<class T>
T dynamic::asImpl() const {
  switch (type()) {
  case INT64:    return to<T>(*get_nothrow<int64_t>());
  case DOUBLE:   return to<T>(*get_nothrow<double>());
  case BOOL:     return to<T>(*get_nothrow<bool>());
  case STRING:
    return to<T>(*get_nothrow<std::string>());
  default:
    throw TypeError("int/double/bool/string", type());
  }
}

// Return a T* to our type, or null if we're not that type.
template<class T>
T* dynamic::get_nothrow() & noexcept {
  if (type_ != TypeInfo<T>::type) {
    return nullptr;
  }
  return getAddress<T>();
}

template<class T>
T const* dynamic::get_nothrow() const& noexcept {
  return const_cast<dynamic*>(this)->get_nothrow<T>();
}

// Return T* for where we can put a T, without type checking.  (Memory
// might be uninitialized, even.)
template<class T>
T* dynamic::getAddress() noexcept {
  return GetAddrImpl<T>::get(u_);
}

template<class T>
T const* dynamic::getAddress() const noexcept {
  return const_cast<dynamic*>(this)->getAddress<T>();
}

template<class T> struct dynamic::GetAddrImpl {};
template<> struct dynamic::GetAddrImpl<void*> {
  static void** get(Data& d) noexcept { return &d.nul; }
};
template<> struct dynamic::GetAddrImpl<dynamic::Array> {
  static Array* get(Data& d) noexcept { return &d.array; }
};
template<> struct dynamic::GetAddrImpl<bool> {
  static bool* get(Data& d) noexcept { return &d.boolean; }
};
template<> struct dynamic::GetAddrImpl<int64_t> {
  static int64_t* get(Data& d) noexcept { return &d.integer; }
};
template<> struct dynamic::GetAddrImpl<double> {
  static double* get(Data& d) noexcept { return &d.doubl; }
};
template <>
struct dynamic::GetAddrImpl<std::string> {
  static std::string* get(Data& d) noexcept {
    return &d.string;
  }
};
template<> struct dynamic::GetAddrImpl<dynamic::ObjectImpl> {
  static_assert(sizeof(ObjectImpl) <= sizeof(Data::objectBuffer),
    "In your implementation, std::unordered_map<> apparently takes different"
    " amount of space depending on its template parameters.  This is "
    "weird.  Make objectBuffer bigger if you want to compile dynamic.");

  static ObjectImpl* get(Data& d) noexcept {
    void* data = &d.objectBuffer;
    return static_cast<ObjectImpl*>(data);
  }
};

template<class T>
T& dynamic::get() {
  if (auto* p = get_nothrow<T>()) {
    return *p;
  }
  throw TypeError(TypeInfo<T>::name, type());
}

template<class T>
T const& dynamic::get() const {
  return const_cast<dynamic*>(this)->get<T>();
}

//////////////////////////////////////////////////////////////////////

/*
 * Helper for implementing operator<<.  Throws if the type shouldn't
 * support it.
 */
template<class T>
struct dynamic::PrintImpl {
  static void print(dynamic const&, std::ostream& out, T const& t) {
    out << t;
  }
};
// Otherwise, null, being (void*)0, would print as 0.
template <>
struct dynamic::PrintImpl<void*> {
  static void print(dynamic const& /* d */,
                    std::ostream& out,
                    void* const& nul) {
    DCHECK_EQ((void*)0, nul);
    out << "null";
  }
};
template<>
struct dynamic::PrintImpl<dynamic::ObjectImpl> {
  static void print(dynamic const& d,
                    std::ostream& out,
                    dynamic::ObjectImpl const&) {
    d.print_as_pseudo_json(out);
  }
};
template<>
struct dynamic::PrintImpl<dynamic::Array> {
  static void print(dynamic const& d,
                    std::ostream& out,
                    dynamic::Array const&) {
    d.print_as_pseudo_json(out);
  }
};

inline void dynamic::print(std::ostream& out) const {
#define ACC_X(T) PrintImpl<T>::print(*this, out, *getAddress<T>())
  ACC_DYNAMIC_APPLY(type_, ACC_X);
#undef ACC_X
}

inline std::ostream& operator<<(std::ostream& out, dynamic const& d) {
  d.print(out);
  return out;
}

//////////////////////////////////////////////////////////////////////

template <class T>
inline typename std::enable_if<
  std::is_same<T, bool>::value, T>::type
as(const dynamic& j) {
  return j.asBool();
}

template <class T>
inline typename std::enable_if<
  std::is_integral<T>::value && !std::is_same<T, bool>::value, T>::type
as(const dynamic& j) {
  return to<T>(j.asInt());
}

template <class T>
inline typename std::enable_if<
  std::is_floating_point<T>::value, T>::type
as(const dynamic& j) {
  return to<T>(j.asDouble());
}

template <class T>
inline typename std::enable_if<
  std::is_same<T, std::string>::value, T>::type
as(const dynamic& j) {
  return j.asString();
}

} // namespace acc

#undef ACC_DYNAMIC_APPLY