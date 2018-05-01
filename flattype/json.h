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

#include <accelerator/Conv.h>
#include <accelerator/Range.h>
#include <flatbuffers/flatbuffers.h>

#include "flattype/idl/json_generated.h"

namespace flattype {

template <class T>
const T* asType(const void* ptr) {
  return static_cast<const T*>(ptr);
}

template <class FString>
acc::StringPiece stringPiece(const FString* ptr) {
  return acc::StringPiece(ptr->value()->data(), ptr->value()->size());
}

struct TypeError;

class dynamic {
 public:
  enum Type {
    NULLT  = fbs::json::Type_Null,
    ARRAY  = fbs::json::Type_Array,
    BOOL   = fbs::json::Type_Bool,
    DOUBLE = fbs::json::Type_Double,
    INT64  = fbs::json::Type_Int64,
    OBJECT = fbs::json::Type_Object,
    STRING = fbs::json::Type_String,
  };
  template<class T, class Enable = void> struct NumericTypeHelper;

  /*
   * We support direct iteration of arrays, and indirect iteration of objects.
   * See begin(), end(), keys(), values(), and items() for more.
   *
   * Array iterators dereference as the elements in the array.
   * Object key iterators dereference as the keys in the object.
   * Object value iterators dereference as the values in the object.
   * Object item iterators dereference as pairs of (key, value).
   */
 private:
  typedef ::flatbuffers::Vector<::flatbuffers::Offset<void>> Array;
  typedef ::flatbuffers::Vector<::flatbuffers::Offset<fbs::json::Pair>> Object;
 public:
  typedef Array::const_iterator const_iterator;
  typedef dynamic value_type;
  struct const_item_iterator;

 public:
  dynamic(Type type, const void* data);
  ~dynamic() noexcept {}

  dynamic(dynamic const&) = default;
  dynamic& operator=(dynamic const&) = default;

  dynamic(dynamic&&) noexcept = default;
  dynamic& operator=(dynamic&&) noexcept = default;

  bool operator==(dynamic const& o) const;
  bool operator<(dynamic const& o) const;

  /*
   * General operators.
   *
   * These throw TypeError when used with types or type combinations
   * that don't support them.
   *
   * These functions may also throw if you use 64-bit integers with
   * doubles when the integers are too big to fit in a double.
   */
  dynamic& operator+=(int64_t i);
  dynamic& operator-=(int64_t i);
  dynamic& operator*=(int64_t i);
  dynamic& operator/=(int64_t i);

  dynamic& operator+=(double d);
  dynamic& operator-=(double d);
  dynamic& operator*=(double d);
  dynamic& operator/=(double d);

  dynamic& operator%=(int64_t i);
  dynamic& operator|=(int64_t i);
  dynamic& operator&=(int64_t i);
  dynamic& operator^=(int64_t i);

  dynamic& operator++();
  dynamic& operator--();

  /*
   * For simple jsons (not arrays or objects), this prints the
   * value to an std::ostream in the expected way.  Respects the
   * formatting manipulators that have been sent to the stream
   * already.
   *
   * If the dynamic holds an object or array, this prints them in a
   * format very similar to dynamic.  (It will in fact actually be dynamic
   * as long as the dynamic validly represents a dynamic object---i.e. it
   * can't have non-string keys.)
   */
  friend std::ostream& operator<<(std::ostream&, dynamic const&);

  /*
   * Returns true if this dynamic is of the specified type.
   */
  bool isString() const;
  bool isObject() const;
  bool isBool() const;
  bool isNull() const;
  bool isArray() const;
  bool isDouble() const;
  bool isInt() const;

  /*
   * Returns: isInt() || isDouble().
   */
  bool isNumber() const;

  /*
   * Returns the type of this dynamic.
   */
  Type type() const;

  /*
   * Returns the type of this dynamic as a printable string.
   */
  const char* typeName() const;

  /*
   * Extract a value while trying to convert to the specified type.
   * Throws exceptions if we cannot convert from the real type to the
   * requested type.
   *
   * Note you can only use this to access integral types or strings,
   * since arrays and objects are generally best dealt with as a
   * dynamic.
   */
  std::string asString() const;
  double      asDouble() const;
  int64_t     asInt() const;
  bool        asBool() const;

  /*
   * Extract the value stored in this dynamic without type conversion.
   *
   * These will throw a TypeError if the dynamic has a different type.
   */
  std::string getString() const;
  double      getDouble() const;
  int64_t     getInt() const;
  bool        getBool() const;

  const char* data() const;
  const char* c_str() const;
  acc::StringPiece stringPiece() const;

  /*
   * Returns: true if this dynamic is null, an empty array, an empty
   * object, or an empty string.
   */
  bool empty() const;

  /*
   * If this is an array or an object, returns the number of elements
   * contained.  If it is a string, returns the length.  Otherwise
   * throws TypeError.
   */
  std::size_t size() const;

  /*
   * You can iterate over the values of the array.  Calling these on
   * non-arrays will throw a TypeError.
   */
  const_iterator begin()  const;
  const_iterator end()    const;

 private:
  /*
   * Helper object returned by keys(), values(), and items().
   */
  template <class T> struct IterableProxy;

 public:
  /*
   * You can iterate over the keys, values, or items (std::pair of key and
   * value) in an object.  Calling these on non-objects will throw a TypeError.
   */
  IterableProxy<const_item_iterator> items() const;

  /*
   * AssociativeContainer-style find interface for objects.  Throws if
   * this is not an object.
   *
   * Returns: items().end() if the key is not present, or a
   * const_item_iterator pointing to the item.
   */
  const_item_iterator find(dynamic const&) const;

  /*
   * If this is an object, returns whether it contains a field with
   * the given name.  Otherwise throws TypeError.
   */
  std::size_t count(dynamic const&) const;

  /*
   * For objects or arrays, provides access to sub-fields by index or
   * field name.
   *
   * Using these with dynamic objects that are not arrays or objects
   * will throw a TypeError.  Using an index that is out of range or
   * object-element that's not present throws std::out_of_range.
   */
  template <class T>
  dynamic const& at(T const&) const;

  dynamic const& at(dynamic const&) const&;
  dynamic&       at(dynamic const&) &;
  dynamic&&      at(dynamic const&) &&;

  /*
   * Like 'at', above, except it returns either a pointer to the contained
   * object or nullptr if it wasn't found. This allows a key to be tested for
   * containment and retrieved in one operation. Example:
   *
   *   if (auto* found = d.get_ptr(key))
   *     // use *found;
   *
   * Using these with dynamic objects that are not arrays or objects
   * will throw a TypeError.
   */
  const dynamic* get_ptr(dynamic const&) const&;
  dynamic* get_ptr(dynamic const&) &;
  dynamic* get_ptr(dynamic const&) && = delete;

  /*
   * This works for access to both objects and arrays.
   *
   * In the case of an array, the index must be an integer, and this will throw
   * std::out_of_range if it is less than zero or greater than size().
   *
   * In the case of an object, the non-const overload inserts a null
   * value if the key isn't present.  The const overload will throw
   * std::out_of_range if the key is not present.
   *
   * These functions do not invalidate iterators.
   */
  dynamic&       operator[](dynamic const&) &;
  dynamic const& operator[](dynamic const&) const&;
  dynamic&&      operator[](dynamic const&) &&;

  /*
   * Only defined for objects, throws TypeError otherwise.
   *
   * getDefault will return the value associated with the supplied key, the
   * supplied default otherwise. setDefault will set the key to the supplied
   * default if it is not yet set, otherwise leaving it. setDefault returns
   * a reference to the existing value if present, the new value otherwise.
   */
  dynamic
  getDefault(const dynamic& k, const dynamic& v = dynamic::object) const&;
  dynamic getDefault(const dynamic& k, dynamic&& v) const&;
  dynamic getDefault(const dynamic& k, const dynamic& v = dynamic::object) &&;
  dynamic getDefault(const dynamic& k, dynamic&& v) &&;

  /*
   * Get a hash code.  This function is called by a std::hash<>
   * specialization, also.
   *
   * Throws TypeError if this is an object, array, or null.
   */
  std::size_t hash() const;

 private:
  friend struct TypeError;

  template<class T> struct TypeInfo;
  template<class T> struct CompareOp;
  template<class T> struct PrintImpl;

  template<class T> T get() const;
  template<class T> const T& getIterable() const;

  template<class T> T asImpl() const;

  static char const* typeName(Type);

  void print(std::ostream&) const;
  void print_as_pseudo_json(std::ostream&) const; // see dynamic.cpp

 private:
  Type type_;
  const void* data_;
};

} // namespace flattype


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

#include <functional>
#include <boost/iterator/iterator_adaptor.hpp>
#include <boost/iterator/iterator_facade.hpp>

//////////////////////////////////////////////////////////////////////

namespace std {

template<>
struct hash< ::dynamic::dynamic> {
  size_t operator()(::dynamic::dynamic const& d) const {
    return d.hash();
  }
};

} // namespace std

//////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////

namespace flattype {

struct TypeError : std::runtime_error {
  explicit TypeError(const std::string& expected, dynamic::Type actual);
  ~TypeError() {}
};


//////////////////////////////////////////////////////////////////////

namespace detail {

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
      return Op<double>()(acc::to<double>(integ.asInt()), nonint.asDouble());
    }
    if (a.isDouble()) {
      return Op<double>()(a.asDouble(), b.asDouble());
    }
    return Op<int64_t>()(a.asInt(), b.asInt());
  }

} // namespace detail

//////////////////////////////////////////////////////////////////////

struct dynamic::const_item_iterator
  : boost::iterator_adaptor<dynamic::const_item_iterator,
                            dynamic::Object::const_iterator,
                            fbs::json::Pair const> {
  /* implicit */ const_item_iterator(base_type b) : iterator_adaptor_(b) { }

 private:
  fbs::json::Pair const& dereference() const {
    return **base_reference();
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
  return getIterable<Array>().begin();
}
inline dynamic::const_iterator dynamic::end() const {
  return getIterable<Array>().end();
}

template <class It>
struct dynamic::IterableProxy {
  typedef It const_iterator;
  typedef typename It::value_type value_type;

  /* implicit */ IterableProxy(const dynamic::Object* o) : o_(o) { }

  It begin() const {
    return o_->begin();
  }

  It end() const {
    return o_->end();
  }

 private:
  const dynamic::Object* o_;
};

inline dynamic::IterableProxy<dynamic::const_item_iterator> dynamic::items()
  const {
  return &(getIterable<Object>());
}

inline bool dynamic::isString() const { return type_ == STRING; }
inline bool dynamic::isObject() const { return type_ == OBJECT; }
inline bool dynamic::isBool()   const { return type_ == BOOL; }
inline bool dynamic::isArray()  const { return type_ == ARRAY; }
inline bool dynamic::isDouble() const { return type_ == DOUBLE; }
inline bool dynamic::isInt()    const { return type_ == INT64; }
inline bool dynamic::isNull()   const { return type_ == NULLT; }
inline bool dynamic::isNumber() const { return isInt() || isDouble(); }

inline dynamic::Type dynamic::type() const {
  return type_;
}

inline std::string dynamic::asString() const { return asImpl<std::string>(); }
inline double      dynamic::asDouble() const { return asImpl<double>(); }
inline int64_t     dynamic::asInt()    const { return asImpl<int64_t>(); }
inline bool        dynamic::asBool()   const { return asImpl<bool>(); }

inline std::string dynamic::getString() const { return get<acc::StringPiece>().str(); }
inline double      dynamic::getDouble() const { return get<double>(); }
inline int64_t     dynamic::getInt()    const { return get<int64_t>(); }
inline bool        dynamic::getBool()   const { return get<bool>(); }

inline const char* dynamic::data() const {
  return get<acc::StringPiece>().data();
}
inline const char* dynamic::c_str() const {
  return get<acc::StringPiece>().data();
}
inline acc::StringPiece dynamic::stringPiece() const {
  return get<acc::StringPiece>();
}

template<class T>
struct dynamic::CompareOp {
  static bool comp(T const& a, T const& b) { return a < b; }
};
template<>
struct dynamic::CompareOp<dynamic::Object> {
  static bool comp(Object const&, Object const&) {
    // This code never executes; it is just here for the compiler.
    return false;
  }
};

inline dynamic& dynamic::operator+=(dynamic const& o) {
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

#define FTYPE_DYNAMIC_INTEGER_OP(op)                           \
  inline dynamic& dynamic::operator op(int64_t i) {  \
    if (!isInt()) { \
      throw TypeError("int64", type()); \
    }                                                       \
    *getAddress<int64_t>() op o.asInt();                    \
    return *this;                                           \
  }

FTYPE_DYNAMIC_INTEGER_OP(%=)
FTYPE_DYNAMIC_INTEGER_OP(|=)
FTYPE_DYNAMIC_INTEGER_OP(&=)
FTYPE_DYNAMIC_INTEGER_OP(^=)

#undef FTYPE_DYNAMIC_INTEGER_OP

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

//////////////////////////////////////////////////////////////////////

inline dynamic::dynamic(Array&& r) : type_(ARRAY) {
  new (&u_.array) Array(std::move(r));
}

#define FTYPE_DYNAMIC_DEC_TYPEINFO(T, str, val) \
  template <> struct dynamic::TypeInfo<T> { \
    static constexpr const char* name = str; \
    static constexpr dynamic::Type type = val; \
  }; \
  //

FTYPE_DYNAMIC_DEC_TYPEINFO(void*,            "null",    dynamic::NULLT)
FTYPE_DYNAMIC_DEC_TYPEINFO(bool,             "boolean", dynamic::BOOL)
FTYPE_DYNAMIC_DEC_TYPEINFO(acc::StringPiece, "string",  dynamic::STRING)
FTYPE_DYNAMIC_DEC_TYPEINFO(dynamic::Array,   "array",   dynamic::ARRAY)
FTYPE_DYNAMIC_DEC_TYPEINFO(double,           "double",  dynamic::DOUBLE)
FTYPE_DYNAMIC_DEC_TYPEINFO(int64_t,          "int64",   dynamic::INT64)
FTYPE_DYNAMIC_DEC_TYPEINFO(dynamic::Object,  "object",  dynamic::OBJECT)

#undef FTYPE_DYNAMIC_DEC_TYPEINFO

template<class T>
T dynamic::asImpl() const {
  switch (type()) {
  case INT64:  return to<T>(asType<fbs::json::Int64>(data_)->value());
  case DOUBLE: return to<T>(asType<fbs::json::Double>(data_)->value());
  case BOOL:   return to<T>(asType<fbs::json::Bool>(data_)->value());
  case STRING: return to<T>(asType<fbs::json::String>(data_)->value()->str());
  default:
    throw TypeError("int/double/bool/string", type());
  }
}

template<class T>
T dynamic::get() const {
  if (type_ != TypeInfo<T>::type) {
    throw TypeError(TypeInfo<T>::name, type());
  }
  switch (type()) {
  case INT64:  return asType<fbs::json::Int64>(data_)->value();
  case DOUBLE: return asType<fbs::json::Double>(data_)->value();
  case BOOL:   return asType<fbs::json::Bool>(data_)->value();
  case STRING: return stringPiece(asType<fbs::json::String>(data_));
  default:
    throw TypeError("int/double/bool/string", type());
  }
}

template<class T>
const T& dynamic::getIterable() const {
  if (type_ != TypeInfo<T>::type) {
    throw TypeError(TypeInfo<T>::name, type());
  }
  switch (type()) {
  case ARRAY:  return *(asType<fbs::json::Array>(data_)->value());
  case OBJECT: return *(asType<fbs::json::Object>(data_)->value());
  default:
    throw TypeError("array/object", type());
  }
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
#define FTYPE_X(T) PrintImpl<T>::print(*this, out, *getAddress<T>())
  FTYPE_DYNAMIC_APPLY(type_, FTYPE_X);
#undef FTYPE_X
}

inline std::ostream& operator<<(std::ostream& out, dynamic const& d) {
  d.print(out);
  return out;
}

} // namespace flattype

#undef FTYPE_DYNAMIC_APPLY
