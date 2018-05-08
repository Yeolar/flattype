// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_MESSAGE_FTT_FBS_H_
#define FLATBUFFERS_GENERATED_MESSAGE_FTT_FBS_H_

#include "flatbuffers/flatbuffers.h"

#include "base_generated.h"
#include "matrix_generated.h"
#include "object_generated.h"
#include "table_generated.h"

namespace ftt {
namespace fbs {

struct Message;

struct Message FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum {
    VT_CODE = 4,
    VT_MESSAGE = 6,
    VT_JDATA = 8,
    VT_XDATA = 10
  };
  int32_t code() const {
    return GetField<int32_t>(VT_CODE, 0);
  }
  const flatbuffers::String *message() const {
    return GetPointer<const flatbuffers::String *>(VT_MESSAGE);
  }
  const ftt::fbs::Object *jdata() const {
    return GetPointer<const ftt::fbs::Object *>(VT_JDATA);
  }
  const ftt::fbs::Table *xdata() const {
    return GetPointer<const ftt::fbs::Table *>(VT_XDATA);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int32_t>(verifier, VT_CODE) &&
           VerifyOffset(verifier, VT_MESSAGE) &&
           verifier.Verify(message()) &&
           VerifyOffset(verifier, VT_JDATA) &&
           verifier.VerifyTable(jdata()) &&
           VerifyOffset(verifier, VT_XDATA) &&
           verifier.VerifyTable(xdata()) &&
           verifier.EndTable();
  }
};

struct MessageBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_code(int32_t code) {
    fbb_.AddElement<int32_t>(Message::VT_CODE, code, 0);
  }
  void add_message(flatbuffers::Offset<flatbuffers::String> message) {
    fbb_.AddOffset(Message::VT_MESSAGE, message);
  }
  void add_jdata(flatbuffers::Offset<ftt::fbs::Object> jdata) {
    fbb_.AddOffset(Message::VT_JDATA, jdata);
  }
  void add_xdata(flatbuffers::Offset<ftt::fbs::Table> xdata) {
    fbb_.AddOffset(Message::VT_XDATA, xdata);
  }
  MessageBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  MessageBuilder &operator=(const MessageBuilder &);
  flatbuffers::Offset<Message> Finish() {
    const auto end = fbb_.EndTable(start_, 4);
    auto o = flatbuffers::Offset<Message>(end);
    return o;
  }
};

inline flatbuffers::Offset<Message> CreateMessage(
    flatbuffers::FlatBufferBuilder &_fbb,
    int32_t code = 0,
    flatbuffers::Offset<flatbuffers::String> message = 0,
    flatbuffers::Offset<ftt::fbs::Object> jdata = 0,
    flatbuffers::Offset<ftt::fbs::Table> xdata = 0) {
  MessageBuilder builder_(_fbb);
  builder_.add_xdata(xdata);
  builder_.add_jdata(jdata);
  builder_.add_message(message);
  builder_.add_code(code);
  return builder_.Finish();
}

inline flatbuffers::Offset<Message> CreateMessageDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    int32_t code = 0,
    const char *message = nullptr,
    flatbuffers::Offset<ftt::fbs::Object> jdata = 0,
    flatbuffers::Offset<ftt::fbs::Table> xdata = 0) {
  return ftt::fbs::CreateMessage(
      _fbb,
      code,
      message ? _fbb.CreateString(message) : 0,
      jdata,
      xdata);
}

inline const ftt::fbs::Message *GetMessage(const void *buf) {
  return flatbuffers::GetRoot<ftt::fbs::Message>(buf);
}

inline bool VerifyMessageBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifyBuffer<ftt::fbs::Message>(nullptr);
}

inline void FinishMessageBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<ftt::fbs::Message> root) {
  fbb.Finish(root);
}

}  // namespace fbs
}  // namespace ftt

#endif  // FLATBUFFERS_GENERATED_MESSAGE_FTT_FBS_H_
