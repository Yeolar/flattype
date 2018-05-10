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

#include "flattype/idl/message_generated.h"
#include "flattype/Builder.h"
#include "flattype/message/Message.h"

namespace ftt {

class MessageBuilder : public Builder {
 public:
  MessageBuilder() : Builder() {}

  explicit MessageBuilder(FBB* fbb, bool owns = false)
    : Builder(fbb, owns) {}

  MessageBuilder(const MessageBuilder&) = delete;
  MessageBuilder& operator=(const MessageBuilder&) = delete;

  MessageBuilder(MessageBuilder&&) = default;
  MessageBuilder& operator=(MessageBuilder&&) = default;

  int getCode() const;
  void setCode(int code);

  std::string getMessage() const;
  void setMessage(const std::string& msg);

  void buildJData(FBBCallback<fbs::Object>&& builder);
  void buildXData(FBBCallback<fbs::Table>&& builder);

  void finish() override;

  Message toMessage() { return toWrapper<Message>(); }

 private:
  int code_{0};
  std::string message_;
  ::flatbuffers::Offset<fbs::Object> jdata_;
  ::flatbuffers::Offset<fbs::Table> xdata_;
};

} // namespace ftt
