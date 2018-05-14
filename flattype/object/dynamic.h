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

#include <cstdint>
#include <memory>
#include <ostream>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>
#include <boost/operators.hpp>

#include "flattype/Type.h"
#include "flattype/Util.h"
#include "flattype/idl/object_generated.h"

namespace ftt {

//////////////////////////////////////////////////////////////////////

struct dynamic;
struct TypeError;

//////////////////////////////////////////////////////////////////////

struct dynamic : private boost::operators<dynamic> {
  enum Type {
    NULLT  = fbs::Json_Null,
    ARRAY  = fbs::Json_Array,
    BOOL   = fbs::Json_Bool,
    DOUBLE = fbs::Json_Double,
    INT64  = fbs::Json_Int64,
    OBJECT = fbs::Json_Object,
    STRING = fbs::Json_String,
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
  typedef std::vector<void*> Array;
  typedef std::map<void*, void*> Object;
  typedef ::flatbuffers::Vector<flatbuffers::Offset<void>> ArrayImpl;
  typedef ::flatbuffers::Vector<flatbuffers::Offset<fbs::Pair>> ObjectImpl;
 public:
  typedef ArrayImpl::const_iterator const_iterator;
  struct const_key_iterator;
  struct const_item_iterator;

 public:
  dynamic(Type type, const void* data);
  ~dynamic() noexcept {}

  dynamic(dynamic const&) = default;
  dynamic& operator=(dynamic const&) = default;

  dynamic(dynamic&&) noexcept = default;
  dynamic& operator=(dynamic&&) noexcept = default;

  /*
   * "Deep" equality comparison.  This will compare all the way down
   * an object or array, and is potentially expensive.
   */
  bool operator==(dynamic const& o) const;

  /*
   * For all types except object this returns the natural ordering on
   * those types.  For objects, we throw TypeError.
   */
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
  dynamic& operator%=(int64_t i);
  dynamic& operator|=(int64_t i);
  dynamic& operator&=(int64_t i);
  dynamic& operator^=(int64_t i);
  dynamic& operator++();
  dynamic& operator--();

  dynamic& operator+=(double d);
  dynamic& operator-=(double d);
  dynamic& operator*=(double d);
  dynamic& operator/=(double d);

  /*
   * For simple dynamics (not arrays or objects), this prints the
   * value to an std::ostream in the expected way.  Respects the
   * formatting manipulators that have been sent to the stream
   * already.
   *
   * If the dynamic holds an object or array, this prints them in a
   * format very similar to JSON.  (It will in fact actually be JSON
   * as long as the dynamic validly represents a JSON object---i.e. it
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
  acc::StringPiece getString() const;
  double           getDouble() const;
  int64_t          getInt() const;
  bool             getBool() const;
  double&          getDouble();
  int64_t&         getInt();

  /*
   * It is occasionally useful to access a string's internal pointer
   * directly, without the type conversion of `asString()`.
   *
   * These will throw a TypeError if the dynamic is not a string.
   */
  const char* data() const;
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
  IterableProxy<const_key_iterator> keys() const;
  IterableProxy<const_item_iterator> items() const;

  /*
   * AssociativeContainer-style find interface for objects.  Throws if
   * this is not an object.
   *
   * Returns: items().end() if the key is not present, or a
   * const_item_iterator pointing to the item.
   */
  const_item_iterator find(acc::StringPiece) const;

  /*
   * If this is an object, returns whether it contains a field with
   * the given name.  Otherwise throws TypeError.
   */
  std::size_t count(acc::StringPiece) const;

  /*
   * For objects or arrays, provides access to sub-fields by index or
   * field name.
   *
   * Using these with dynamic objects that are not arrays or objects
   * will throw a TypeError.  Using an index that is out of range or
   * object-element that's not present throws std::out_of_range.
   */
  dynamic at(size_t) const;
  dynamic at(acc::StringPiece) const;

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
  dynamic operator[](size_t) const;
  dynamic operator[](acc::StringPiece) const;

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
  template<class T> struct PrintImpl;

  template<class T> T  getValue() const;
  template<class T> T& getValue();

  template<class T, class FT = typename JsonType<T>::type>
  FT const& get() const;
  template<class T, class FT = typename JsonType<T>::type>
  FT const* get_nothrow() const noexcept;
  template<class T, class FT = typename JsonType<T>::type>
  FT const* getAddress() const noexcept;

  template<class T> T asImpl() const;

  static char const* typeName(Type);

  void print(std::ostream&) const;
  void print_as_pseudo_json(std::ostream&) const; // see json.cpp

 private:
  Type type_;
  const void* data_;
};

} // namespace ftt

//#include "dynamic-inl.h"

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
  size_t operator()(::ftt::dynamic const& d) const {
    return d.hash();
  }
};

} // namespace std

//////////////////////////////////////////////////////////////////////

namespace ftt {

struct TypeError : std::runtime_error {
  explicit TypeError(const std::string& expected, dynamic::Type actual);
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
    static constexpr dynamic::Type type = val; \
  }; \
  //

FTT_DYNAMIC_DEC_TYPEINFO(std::nullptr_t,      "null",    dynamic::NULLT)
FTT_DYNAMIC_DEC_TYPEINFO(bool,                "boolean", dynamic::BOOL)
FTT_DYNAMIC_DEC_TYPEINFO(acc::StringPiece,    "string",  dynamic::STRING)
FTT_DYNAMIC_DEC_TYPEINFO(dynamic::Array,      "array",   dynamic::ARRAY)
FTT_DYNAMIC_DEC_TYPEINFO(double,              "double",  dynamic::DOUBLE)
FTT_DYNAMIC_DEC_TYPEINFO(int64_t,             "int64",   dynamic::INT64)
FTT_DYNAMIC_DEC_TYPEINFO(dynamic::Object,     "object",  dynamic::OBJECT)

#undef FTT_DYNAMIC_DEC_TYPEINFO

template<class T>
T dynamic::asImpl() const {
  switch (type()) {
  case INT64:  return acc::to<T>(ftt::getValue<int64_t>(data_));
  case DOUBLE: return acc::to<T>(ftt::getValue<double>(data_));
  case BOOL:   return acc::to<T>(ftt::getValue<bool>(data_));
  case STRING: return acc::to<T>(ftt::getValue<acc::StringPiece>(data_));
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
  return reinterpret_cast<const typename JsonType<T>::type*>(data_);
}

template<class T>
T& dynamic::getValue() {
  if (type_ != TypeInfo<T>::type) {
    throw TypeError(TypeInfo<T>::name, type());
  }
  switch (type()) {
  case INT64:
  case DOUBLE:
    return *const_cast<T*>(ftt::getValuePtr<T>(data_));
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
  case INT64:
  case DOUBLE:
  case BOOL:
  case STRING:
    return ftt::getValue<T>(data_);
  default:
    throw TypeError("int/double/bool/string", type());
  }
}

//////////////////////////////////////////////////////////////////////

inline void dynamic::print(std::ostream& out) const {
  switch (type_) {
    case NULLT:
      out << "null";
      break;
    case ARRAY:
      this->print_as_pseudo_json(out);
      break;
    case BOOL:
      out << getAddress<bool>()->value();
      break;
    case DOUBLE:
      out << getAddress<double>()->value();
      break;
    case INT64:
      out << getAddress<int64_t>()->value();
      break;
    case OBJECT:
      this->print_as_pseudo_json(out);
      break;
    case STRING:
      out << getAddress<acc::StringPiece>()->value()->data();
      break;
    default:
      ACCCHECK(0);
  }
}

inline std::ostream& operator<<(std::ostream& out, dynamic const& d) {
  d.print(out);
  return out;
}

} // namespace ftt
