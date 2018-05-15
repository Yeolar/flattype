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
#include <map>
#include <memory>
#include <ostream>
#include <string>
#include <type_traits>
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
  dynamic(fbs::Json type, const void* data);
  dynamic(fbs::Json type, const uint8_t* data);
  dynamic(fbs::Json type, ::flatbuffers::DetachedBuffer&& data);
  ~dynamic() noexcept {}

  dynamic(dynamic const&) = delete;
  dynamic& operator=(dynamic const&) = delete;

  dynamic(dynamic&&) = default;
  dynamic& operator=(dynamic&&) = default;

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
  fbs::Json type() const;

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
  const_iterator begin() const;
  const_iterator end() const;

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

  static char const* typeName(fbs::Json);

  void print(std::ostream&) const;

 private:
  fbs::Json type_;
  const void* ptr_;
  ::flatbuffers::DetachedBuffer data_;
};

} // namespace ftt

#include "dynamic-inl.h"
