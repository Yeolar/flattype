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

#include "flattype/object/Serialize.h"

#include <boost/next_prior.hpp>

#include "accelerator/Conv.h"
#include "accelerator/Range.h"
#include "flattype/object/dynamic.h"

namespace ftt {

namespace {

struct Printer {
  explicit Printer(
      std::string& out,
      unsigned* indentLevel,
      acc::json::serialization_opts const* opts)
      : out_(out), indentLevel_(indentLevel), opts_(*opts) {}

  void operator()(dynamic const& v) const {
    switch (v.type()) {
    case fbs::Json::Double:
      if (!opts_.allow_nan_inf &&
          (std::isnan(v.asDouble()) || std::isinf(v.asDouble()))) {
        throw std::runtime_error("acc::toJson: JSON object value was a "
          "NaN or INF");
      }
      acc::toAppend(v.asDouble(), &out_,
                    opts_.double_mode,
                    opts_.double_num_digits);
      break;
    case fbs::Json::Int64: {
      auto intval = v.asInt();
      if (opts_.javascript_safe) {
        // Use acc::to to check that this integer can be represented
        // as a double without loss of precision.
        intval = int64_t(acc::to<double>(intval));
      }
      acc::toAppend(intval, &out_);
      break;
    }
    case fbs::Json::Bool:
      out_ += v.asBool() ? "true" : "false";
      break;
    case fbs::Json::Null:
      out_ += "null";
      break;
    case fbs::Json::String:
      escapeString(v.asString(), out_, opts_);
      break;
    case fbs::Json::Object:
      printObject(v);
      break;
    case fbs::Json::Array:
      printArray(v);
      break;
    default:
      ACC_THROW(acc::Exception, "Bad type ", v.type());
    }
  }

 private:
  void printKV(const fbs::Pair& p) const {
    escapeString(p.name()->c_str(), out_, opts_);
    mapColon();
    (*this)(dynamic(p.value_type(), p.value()));
  }

  template <typename Iterator>
  void printKVPairs(Iterator begin, Iterator end) const {
    printKV(*begin);
    for (++begin; begin != end; ++begin) {
      out_ += ',';
      newline();
      printKV(*begin);
    }
  }

  void printObject(dynamic const& o) const {
    if (o.empty()) {
      out_ += "{}";
      return;
    }

    out_ += '{';
    indent();
    newline();
    printKVPairs(o.items().begin(), o.items().end());
    outdent();
    newline();
    out_ += '}';
  }

  void printArray(dynamic const& a) const {
    if (a.empty()) {
      out_ += "[]";
      return;
    }

    out_ += '[';
    indent();
    newline();
    (*this)(a[0]);
    for (size_t i = 1; i < a.size(); i++) {
      out_ += ',';
      newline();
      (*this)(a[i]);
    }
    outdent();
    newline();
    out_ += ']';
  }

 private:
  void outdent() const {
    if (indentLevel_) {
      --*indentLevel_;
    }
  }

  void indent() const {
    if (indentLevel_) {
      ++*indentLevel_;
    }
  }

  void newline() const {
    if (indentLevel_) {
      out_ += acc::to<std::string>('\n', std::string(*indentLevel_ * 2, ' '));
    }
  }

  void mapColon() const {
    out_ += indentLevel_ ? " : " : ":";
  }

 private:
  std::string& out_;
  unsigned* const indentLevel_;
  acc::json::serialization_opts const& opts_;
};

} // namespace

std::string
serialize(dynamic const& dyn, acc::json::serialization_opts const& opts) {
  std::string ret;
  unsigned indentLevel = 0;
  Printer p(ret, opts.pretty_formatting ? &indentLevel : nullptr, &opts);
  p(dyn);
  return ret;
}

std::string toJson(dynamic const& dyn) {
  return serialize(dyn, acc::json::serialization_opts());
}

std::string toPrettyJson(dynamic const& dyn) {
  acc::json::serialization_opts opts;
  opts.pretty_formatting = true;
  return serialize(dyn, opts);
}

std::string toPseudoJson(dynamic const& dyn) {
  acc::json::serialization_opts opts;
  opts.allow_nan_inf = true;
  return serialize(dyn, opts);
}

} // namespace ftt
