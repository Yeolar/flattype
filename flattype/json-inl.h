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

//////////////////////////////////////////////////////////////////////

namespace std {

template<>
struct hash< ::flatjson::flatjson> {
  size_t operator()(::flatjson::flatjson const& d) const {
    return d.hash();
  }
};

} // namespace std

//////////////////////////////////////////////////////////////////////

// This is a higher-order preprocessor macro to aid going from runtime
// types to the compile time type system.
#define FJSON_FLATJSON_APPLY(type, apply) \
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
        FJSONCHECK(0);                   \
        abort();                       \
    }                                  \
  } while (0)

//////////////////////////////////////////////////////////////////////

namespace flatjson {

struct TypeError : std::runtime_error {
  explicit TypeError(const std::string& expected, flatjson::Type actual);
  explicit TypeError(const std::string& expected,
    flatjson::Type actual1, flatjson::Type actual2);
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
  flatjson numericOp(flatjson const& a, flatjson const& b) {
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
struct flatjson::ObjectImpl : std::unordered_map<flatjson, flatjson> {};

//////////////////////////////////////////////////////////////////////

// Helper object for creating objects conveniently.  See object and
// the flatjson::flatjson(ObjectMaker&&) ctor.
struct flatjson::ObjectMaker {
  friend struct flatjson;

  explicit ObjectMaker() : val_(flatjson::object) {}
  explicit ObjectMaker(flatjson key, flatjson val)
    : val_(flatjson::object)
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
  //  flatjson a = flatjson::object("a", "b")("c", "d")
  ObjectMaker&& operator()(flatjson key, flatjson val) {
    val_.insert(std::move(key), std::move(val));
    return std::move(*this);
  }

 private:
  flatjson val_;
};

inline void flatjson::array(EmptyArrayTag) {}

template <class... Args>
inline flatjson flatjson::array(Args&& ...args) {
  return flatjson(Array{std::forward<Args>(args)...});
}

inline flatjson::ObjectMaker flatjson::object() { return ObjectMaker(); }
inline flatjson::ObjectMaker flatjson::object(flatjson a, flatjson b) {
  return ObjectMaker(std::move(a), std::move(b));
}

//////////////////////////////////////////////////////////////////////

struct flatjson::const_item_iterator
  : boost::iterator_adaptor<flatjson::const_item_iterator,
                            flatjson::ObjectImpl::const_iterator> {
  /* implicit */ const_item_iterator(base_type b) : iterator_adaptor_(b) { }

 private:
  friend class boost::iterator_core_flatjsoness;
};

struct flatjson::const_key_iterator
  : boost::iterator_adaptor<flatjson::const_key_iterator,
                            flatjson::ObjectImpl::const_iterator,
                            flatjson const> {
  /* implicit */ const_key_iterator(base_type b) : iterator_adaptor_(b) { }

 private:
  flatjson const& dereference() const {
    return base_reference()->first;
  }
  friend class boost::iterator_core_flatjsoness;
};

struct flatjson::const_value_iterator
  : boost::iterator_adaptor<flatjson::const_value_iterator,
                            flatjson::ObjectImpl::const_iterator,
                            flatjson const> {
  /* implicit */ const_value_iterator(base_type b) : iterator_adaptor_(b) { }

 private:
  flatjson const& dereference() const {
    return base_reference()->second;
  }
  friend class boost::iterator_core_flatjsoness;
};

//////////////////////////////////////////////////////////////////////

inline flatjson::flatjson() : flatjson(nullptr) {}

inline flatjson::flatjson(std::nullptr_t) : type_(NULLT) {}

inline flatjson::flatjson(void (*)(EmptyArrayTag))
  : type_(ARRAY)
{
  new (&u_.array) Array();
}

inline flatjson::flatjson(ObjectMaker (*)())
  : type_(OBJECT)
{
  new (getAddress<ObjectImpl>()) ObjectImpl();
}

inline flatjson::flatjson(StringPiece s)
  : type_(STRING)
{
  new (&u_.string) std::string(s.data(), s.size());
}

inline flatjson::flatjson(char const* s)
  : type_(STRING)
{
  new (&u_.string) std::string(s);
}

inline flatjson::flatjson(std::string s)
  : type_(STRING)
{
  new (&u_.string) std::string(std::move(s));
}

inline flatjson::flatjson(ObjectMaker&& maker)
  : type_(OBJECT)
{
  new (getAddress<ObjectImpl>())
    ObjectImpl(std::move(*maker.val_.getAddress<ObjectImpl>()));
}

inline flatjson::flatjson(flatjson const& o)
  : type_(NULLT)
{
  *this = o;
}

inline flatjson::flatjson(flatjson&& o) noexcept
  : type_(NULLT)
{
  *this = std::move(o);
}

inline flatjson::~flatjson() noexcept { destroy(); }

// Integral types except bool convert to int64_t, float types to double.
template <class T>
struct flatjson::NumericTypeHelper<
    T, typename std::enable_if<std::is_integral<T>::value>::type> {
  using type = int64_t;
};
template <>
struct flatjson::NumericTypeHelper<bool> {
  using type = bool;
};
template <>
struct flatjson::NumericTypeHelper<float> {
  using type = double;
};
template <>
struct flatjson::NumericTypeHelper<double> {
  using type = double;
};

template<class T, class NumericType /* = typename NumericTypeHelper<T>::type */>
flatjson::flatjson(T t) {
  type_ = TypeInfo<NumericType>::type;
  new (getAddress<NumericType>()) NumericType(t);
}

template <class Iterator>
flatjson::flatjson(Iterator first, Iterator last)
  : type_(ARRAY)
{
  new (&u_.array) Array(first, last);
}

//////////////////////////////////////////////////////////////////////

inline flatjson::const_iterator flatjson::begin() const {
  return get<Array>().begin();
}
inline flatjson::const_iterator flatjson::end() const {
  return get<Array>().end();
}

template <class It>
struct flatjson::IterableProxy {
  typedef It const_iterator;
  typedef typename It::value_type value_type;

  /* implicit */ IterableProxy(const flatjson::ObjectImpl* o) : o_(o) { }

  It begin() const {
    return o_->begin();
  }

  It end() const {
    return o_->end();
  }

 private:
  const flatjson::ObjectImpl* o_;
};

inline flatjson::IterableProxy<flatjson::const_key_iterator> flatjson::keys()
  const {
  return &(get<ObjectImpl>());
}

inline flatjson::IterableProxy<flatjson::const_value_iterator> flatjson::values()
  const {
  return &(get<ObjectImpl>());
}

inline flatjson::IterableProxy<flatjson::const_item_iterator> flatjson::items()
  const {
  return &(get<ObjectImpl>());
}

inline bool flatjson::isString() const {
  return get_nothrow<std::string>();
}
inline bool flatjson::isObject() const { return get_nothrow<ObjectImpl>(); }
inline bool flatjson::isBool()   const { return get_nothrow<bool>(); }
inline bool flatjson::isArray()  const { return get_nothrow<Array>(); }
inline bool flatjson::isDouble() const { return get_nothrow<double>(); }
inline bool flatjson::isInt()    const { return get_nothrow<int64_t>(); }
inline bool flatjson::isNull()   const { return get_nothrow<void*>(); }
inline bool flatjson::isNumber() const { return isInt() || isDouble(); }

inline flatjson::Type flatjson::type() const {
  return type_;
}

inline std::string flatjson::asString() const {
  return asImpl<std::string>();
}
inline double flatjson::asDouble() const {
  return asImpl<double>();
}
inline int64_t flatjson::asInt() const {
  return asImpl<int64_t>();
}
inline bool flatjson::asBool() const {
  return asImpl<bool>();
}

inline const std::string& flatjson::getString() const& {
  return get<std::string>();
}
inline double             flatjson::getDouble() const& { return get<double>(); }
inline int64_t            flatjson::getInt()    const& { return get<int64_t>(); }
inline bool               flatjson::getBool()   const& { return get<bool>(); }

inline std::string& flatjson::getString()& {
  return get<std::string>();
}
inline double&      flatjson::getDouble() & { return get<double>(); }
inline int64_t&     flatjson::getInt()    & { return get<int64_t>(); }
inline bool&        flatjson::getBool()   & { return get<bool>(); }

inline std::string&& flatjson::getString()&& {
  return std::move(get<std::string>());
}
inline double        flatjson::getDouble() && { return get<double>(); }
inline int64_t       flatjson::getInt()    && { return get<int64_t>(); }
inline bool          flatjson::getBool()   && { return get<bool>(); }

inline const char* flatjson::data() const& {
  return get<std::string>().data();
}
inline const char* flatjson::c_str() const& {
  return get<std::string>().c_str();
}
inline StringPiece flatjson::stringPiece() const {
  return get<std::string>();
}

template<class T>
struct flatjson::CompareOp {
  static bool comp(T const& a, T const& b) { return a < b; }
};
template<>
struct flatjson::CompareOp<flatjson::ObjectImpl> {
  static bool comp(ObjectImpl const&, ObjectImpl const&) {
    // This code never executes; it is just here for the compiler.
    return false;
  }
};

inline flatjson& flatjson::operator+=(flatjson const& o) {
  if (type() == STRING && o.type() == STRING) {
    *getAddress<std::string>() += *o.getAddress<std::string>();
    return *this;
  }
  *this = detail::numericOp<std::plus>(*this, o);
  return *this;
}

inline flatjson& flatjson::operator-=(flatjson const& o) {
  *this = detail::numericOp<std::minus>(*this, o);
  return *this;
}

inline flatjson& flatjson::operator*=(flatjson const& o) {
  *this = detail::numericOp<std::multiplies>(*this, o);
  return *this;
}

inline flatjson& flatjson::operator/=(flatjson const& o) {
  *this = detail::numericOp<std::divides>(*this, o);
  return *this;
}

#define FJSON_flatjson_INTEGER_OP(op)                          \
  inline flatjson& flatjson::operator op(flatjson const& o) {  \
    if (!isInt() || !o.isInt()) {                           \
      throw TypeError("int64", type(), o.type());           \
    }                                                       \
    *getAddress<int64_t>() op o.asInt();                    \
    return *this;                                           \
  }

FJSON_flatjson_INTEGER_OP(%=)
FJSON_flatjson_INTEGER_OP(|=)
FJSON_flatjson_INTEGER_OP(&=)
FJSON_flatjson_INTEGER_OP(^=)

#undef FJSON_flatjson_INTEGER_OP

inline flatjson& flatjson::operator++() {
  ++get<int64_t>();
  return *this;
}

inline flatjson& flatjson::operator--() {
  --get<int64_t>();
  return *this;
}

inline flatjson const& flatjson::operator[](flatjson const& idx) const& {
  return at(idx);
}

inline flatjson&& flatjson::operator[](flatjson const& idx) && {
  return std::move((*this)[idx]);
}

template<class K, class V> inline flatjson& flatjson::setDefault(K&& k, V&& v) {
  auto& obj = get<ObjectImpl>();
  return obj.insert(std::make_pair(std::forward<K>(k),
                                   std::forward<V>(v))).first->second;
}

template<class K> inline flatjson& flatjson::setDefault(K&& k, flatjson&& v) {
  auto& obj = get<ObjectImpl>();
  return obj.insert(std::make_pair(std::forward<K>(k),
                                   std::move(v))).first->second;
}

template<class K> inline flatjson& flatjson::setDefault(K&& k, const flatjson& v) {
  auto& obj = get<ObjectImpl>();
  return obj.insert(std::make_pair(std::forward<K>(k), v)).first->second;
}

inline flatjson* flatjson::get_ptr(flatjson const& idx) & {
  return const_cast<flatjson*>(const_cast<flatjson const*>(this)->get_ptr(idx));
}

inline flatjson& flatjson::at(flatjson const& idx) & {
  return const_cast<flatjson&>(const_cast<flatjson const*>(this)->at(idx));
}

inline flatjson&& flatjson::at(flatjson const& idx) && {
  return std::move(at(idx));
}

inline bool flatjson::empty() const {
  if (isNull()) {
    return true;
  }
  return !size();
}

inline std::size_t flatjson::count(flatjson const& key) const {
  return find(key) != items().end();
}

inline flatjson::const_item_iterator flatjson::find(flatjson const& key) const {
  return get<ObjectImpl>().find(key);
}

template<class K, class V> inline void flatjson::insert(K&& key, V&& val) {
  auto& obj = get<ObjectImpl>();
  auto rv = obj.insert({ std::forward<K>(key), nullptr });
  rv.first->second = std::forward<V>(val);
}

inline void flatjson::update(const flatjson& mergeObj) {
  if (!isObject() || !mergeObj.isObject()) {
    throw TypeError("object", type(), mergeObj.type());
  }

  for (const auto& pair : mergeObj.items()) {
    (*this)[pair.first] = pair.second;
  }
}

inline void flatjson::update_missing(const flatjson& mergeObj1) {
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

inline flatjson flatjson::merge(
    const flatjson& mergeObj1,
    const flatjson& mergeObj2) {

  // No checks on type needed here because they are done in update_missing
  // Note that we do update_missing here instead of update() because
  // it will prevent the extra writes that would occur with update()
  auto ret = mergeObj2;
  ret.update_missing(mergeObj1);
  return ret;
}

inline std::size_t flatjson::erase(flatjson const& key) {
  auto& obj = get<ObjectImpl>();
  return obj.erase(key);
}

inline flatjson::const_iterator flatjson::erase(const_iterator it) {
  auto& arr = get<Array>();
  // std::vector doesn't have an erase method that works on const iterators,
  // even though the standard says it should, so this hack converts to a
  // non-const iterator before calling erase.
  return get<Array>().erase(arr.begin() + (it - arr.begin()));
}

inline flatjson::const_key_iterator flatjson::erase(const_key_iterator it) {
  return const_key_iterator(get<ObjectImpl>().erase(it.base()));
}

inline flatjson::const_key_iterator flatjson::erase(const_key_iterator first,
                                                  const_key_iterator last) {
  return const_key_iterator(get<ObjectImpl>().erase(first.base(),
                                                    last.base()));
}

inline flatjson::const_value_iterator flatjson::erase(const_value_iterator it) {
  return const_value_iterator(get<ObjectImpl>().erase(it.base()));
}

inline flatjson::const_value_iterator flatjson::erase(const_value_iterator first,
                                                    const_value_iterator last) {
  return const_value_iterator(get<ObjectImpl>().erase(first.base(),
                                                      last.base()));
}

inline flatjson::const_item_iterator flatjson::erase(const_item_iterator it) {
  return const_item_iterator(get<ObjectImpl>().erase(it.base()));
}

inline flatjson::const_item_iterator flatjson::erase(const_item_iterator first,
                                                   const_item_iterator last) {
  return const_item_iterator(get<ObjectImpl>().erase(first.base(),
                                                     last.base()));
}

inline void flatjson::resize(std::size_t sz, flatjson const& c) {
  auto& arr = get<Array>();
  arr.resize(sz, c);
}

inline void flatjson::push_back(flatjson const& v) {
  auto& arr = get<Array>();
  arr.push_back(v);
}

inline void flatjson::push_back(flatjson&& v) {
  auto& arr = get<Array>();
  arr.push_back(std::move(v));
}

inline void flatjson::pop_back() {
  auto& arr = get<Array>();
  arr.pop_back();
}

//////////////////////////////////////////////////////////////////////

inline flatjson::flatjson(Array&& r) : type_(ARRAY) {
  new (&u_.array) Array(std::move(r));
}

#define FJSON_flatjson_DEC_TYPEINFO(T, str, val) \
  template <> struct flatjson::TypeInfo<T> { \
    static constexpr const char* name = str; \
    static constexpr flatjson::Type type = val; \
  }; \
  //

FJSON_flatjson_DEC_TYPEINFO(void*,               "null",    flatjson::NULLT)
FJSON_flatjson_DEC_TYPEINFO(bool,                "boolean", flatjson::BOOL)
FJSON_flatjson_DEC_TYPEINFO(std::string,         "string",  flatjson::STRING)
FJSON_flatjson_DEC_TYPEINFO(flatjson::Array,      "array",   flatjson::ARRAY)
FJSON_flatjson_DEC_TYPEINFO(double,              "double",  flatjson::DOUBLE)
FJSON_flatjson_DEC_TYPEINFO(int64_t,             "int64",   flatjson::INT64)
FJSON_flatjson_DEC_TYPEINFO(flatjson::ObjectImpl, "object",  flatjson::OBJECT)

#undef FJSON_flatjson_DEC_TYPEINFO

template<class T>
T flatjson::asImpl() const {
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
T* flatjson::get_nothrow() & noexcept {
  if (type_ != TypeInfo<T>::type) {
    return nullptr;
  }
  return getAddress<T>();
}

template<class T>
T const* flatjson::get_nothrow() const& noexcept {
  return const_cast<flatjson*>(this)->get_nothrow<T>();
}

// Return T* for where we can put a T, without type checking.  (Memory
// might be uninitialized, even.)
template<class T>
T* flatjson::getAddress() noexcept {
  return GetAddrImpl<T>::get(u_);
}

template<class T>
T const* flatjson::getAddress() const noexcept {
  return const_cast<flatjson*>(this)->getAddress<T>();
}

template<class T> struct flatjson::GetAddrImpl {};
template<> struct flatjson::GetAddrImpl<void*> {
  static void** get(Data& d) noexcept { return &d.nul; }
};
template<> struct flatjson::GetAddrImpl<flatjson::Array> {
  static Array* get(Data& d) noexcept { return &d.array; }
};
template<> struct flatjson::GetAddrImpl<bool> {
  static bool* get(Data& d) noexcept { return &d.boolean; }
};
template<> struct flatjson::GetAddrImpl<int64_t> {
  static int64_t* get(Data& d) noexcept { return &d.integer; }
};
template<> struct flatjson::GetAddrImpl<double> {
  static double* get(Data& d) noexcept { return &d.doubl; }
};
template <>
struct flatjson::GetAddrImpl<std::string> {
  static std::string* get(Data& d) noexcept {
    return &d.string;
  }
};
template<> struct flatjson::GetAddrImpl<flatjson::ObjectImpl> {
  static_assert(sizeof(ObjectImpl) <= sizeof(Data::objectBuffer),
    "In your implementation, std::unordered_map<> apparently takes different"
    " amount of space depending on its template parameters.  This is "
    "weird.  Make objectBuffer bigger if you want to compile flatjson.");

  static ObjectImpl* get(Data& d) noexcept {
    void* data = &d.objectBuffer;
    return static_cast<ObjectImpl*>(data);
  }
};

template<class T>
T& flatjson::get() {
  if (auto* p = get_nothrow<T>()) {
    return *p;
  }
  throw TypeError(TypeInfo<T>::name, type());
}

template<class T>
T const& flatjson::get() const {
  return const_cast<flatjson*>(this)->get<T>();
}

//////////////////////////////////////////////////////////////////////

/*
 * Helper for implementing operator<<.  Throws if the type shouldn't
 * support it.
 */
template<class T>
struct flatjson::PrintImpl {
  static void print(flatjson const&, std::ostream& out, T const& t) {
    out << t;
  }
};
// Otherwise, null, being (void*)0, would print as 0.
template <>
struct flatjson::PrintImpl<void*> {
  static void print(flatjson const& /* d */,
                    std::ostream& out,
                    void* const& nul) {
    DCHECK_EQ((void*)0, nul);
    out << "null";
  }
};
template<>
struct flatjson::PrintImpl<flatjson::ObjectImpl> {
  static void print(flatjson const& d,
                    std::ostream& out,
                    flatjson::ObjectImpl const&) {
    d.print_as_pseudo_json(out);
  }
};
template<>
struct flatjson::PrintImpl<flatjson::Array> {
  static void print(flatjson const& d,
                    std::ostream& out,
                    flatjson::Array const&) {
    d.print_as_pseudo_json(out);
  }
};

inline void flatjson::print(std::ostream& out) const {
#define FJSON_X(T) PrintImpl<T>::print(*this, out, *getAddress<T>())
  FJSON_FLATJSON_APPLY(type_, FJSON_X);
#undef FJSON_X
}

inline std::ostream& operator<<(std::ostream& out, flatjson const& d) {
  d.print(out);
  return out;
}

//////////////////////////////////////////////////////////////////////

template <class T>
inline typename std::enable_if<
  std::is_same<T, bool>::value, T>::type
as(const flatjson& j) {
  return j.asBool();
}

template <class T>
inline typename std::enable_if<
  std::is_integral<T>::value && !std::is_same<T, bool>::value, T>::type
as(const flatjson& j) {
  return to<T>(j.asInt());
}

template <class T>
inline typename std::enable_if<
  std::is_floating_point<T>::value, T>::type
as(const flatjson& j) {
  return to<T>(j.asDouble());
}

template <class T>
inline typename std::enable_if<
  std::is_same<T, std::string>::value, T>::type
as(const flatjson& j) {
  return j.asString();
}

} // namespace flatjson

#undef FJSON_FLATJSON_APPLY
