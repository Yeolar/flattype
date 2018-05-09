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

#pragma once

#include <string>

#include "flattype/Encoding.h"
#include "flattype/Type.h"
#include "flattype/matrix/Encoding.h"

namespace ftt {

template <class T>
::flatbuffers::DetachedBuffer serialize(const T& value) {
  ::flatbuffers::FlatBufferBuilder fbb;
  fbb.Finish(encode(fbb, value));
  return fbb.Release();
}

template <class String, class T>
String serializeToString(const T& value) {
  auto buffer = serialize(value);
  return String((char*)buffer.data(), buffer.size());
}

template <class T>
void unserialize(const ::flatbuffers::DetachedBuffer& buffer, T& value) {
  typedef typename AnyType<T>::type FT;
  const FT* ptr = ::flatbuffers::GetRoot<FT>(buffer.data());
  decode(ptr, value);
}

template <class String, class T>
void unserializeFromString(const String& in, T& value) {
  typedef typename AnyType<T>::type FT;
  const FT* ptr = ::flatbuffers::GetRoot<FT>(in.data());
  decode(ptr, value);
}

} // namespace ftt
