// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: outside.proto

#ifndef PROTOBUF_outside_2eproto__INCLUDED
#define PROTOBUF_outside_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2006000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2006001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

namespace outside {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_outside_2eproto();
void protobuf_AssignDesc_outside_2eproto();
void protobuf_ShutdownFile_outside_2eproto();

class outside_inner;

// ===================================================================

class outside_inner : public ::google::protobuf::Message {
 public:
  outside_inner();
  virtual ~outside_inner();

  outside_inner(const outside_inner& from);

  inline outside_inner& operator=(const outside_inner& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const outside_inner& default_instance();

  void Swap(outside_inner* other);

  // implements Message ----------------------------------------------

  outside_inner* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const outside_inner& from);
  void MergeFrom(const outside_inner& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional string key = 1;
  inline bool has_key() const;
  inline void clear_key();
  static const int kKeyFieldNumber = 1;
  inline const ::std::string& key() const;
  inline void set_key(const ::std::string& value);
  inline void set_key(const char* value);
  inline void set_key(const char* value, size_t size);
  inline ::std::string* mutable_key();
  inline ::std::string* release_key();
  inline void set_allocated_key(::std::string* key);

  // optional string value = 2;
  inline bool has_value() const;
  inline void clear_value();
  static const int kValueFieldNumber = 2;
  inline const ::std::string& value() const;
  inline void set_value(const ::std::string& value);
  inline void set_value(const char* value);
  inline void set_value(const char* value, size_t size);
  inline ::std::string* mutable_value();
  inline ::std::string* release_value();
  inline void set_allocated_value(::std::string* value);

  // @@protoc_insertion_point(class_scope:outside.outside_inner)
 private:
  inline void set_has_key();
  inline void clear_has_key();
  inline void set_has_value();
  inline void clear_has_value();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::std::string* key_;
  ::std::string* value_;
  friend void  protobuf_AddDesc_outside_2eproto();
  friend void protobuf_AssignDesc_outside_2eproto();
  friend void protobuf_ShutdownFile_outside_2eproto();

  void InitAsDefaultInstance();
  static outside_inner* default_instance_;
};
// ===================================================================


// ===================================================================

// outside_inner

// optional string key = 1;
inline bool outside_inner::has_key() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void outside_inner::set_has_key() {
  _has_bits_[0] |= 0x00000001u;
}
inline void outside_inner::clear_has_key() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void outside_inner::clear_key() {
  if (key_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    key_->clear();
  }
  clear_has_key();
}
inline const ::std::string& outside_inner::key() const {
  // @@protoc_insertion_point(field_get:outside.outside_inner.key)
  return *key_;
}
inline void outside_inner::set_key(const ::std::string& value) {
  set_has_key();
  if (key_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    key_ = new ::std::string;
  }
  key_->assign(value);
  // @@protoc_insertion_point(field_set:outside.outside_inner.key)
}
inline void outside_inner::set_key(const char* value) {
  set_has_key();
  if (key_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    key_ = new ::std::string;
  }
  key_->assign(value);
  // @@protoc_insertion_point(field_set_char:outside.outside_inner.key)
}
inline void outside_inner::set_key(const char* value, size_t size) {
  set_has_key();
  if (key_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    key_ = new ::std::string;
  }
  key_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:outside.outside_inner.key)
}
inline ::std::string* outside_inner::mutable_key() {
  set_has_key();
  if (key_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    key_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:outside.outside_inner.key)
  return key_;
}
inline ::std::string* outside_inner::release_key() {
  clear_has_key();
  if (key_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = key_;
    key_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void outside_inner::set_allocated_key(::std::string* key) {
  if (key_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete key_;
  }
  if (key) {
    set_has_key();
    key_ = key;
  } else {
    clear_has_key();
    key_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:outside.outside_inner.key)
}

// optional string value = 2;
inline bool outside_inner::has_value() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void outside_inner::set_has_value() {
  _has_bits_[0] |= 0x00000002u;
}
inline void outside_inner::clear_has_value() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void outside_inner::clear_value() {
  if (value_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    value_->clear();
  }
  clear_has_value();
}
inline const ::std::string& outside_inner::value() const {
  // @@protoc_insertion_point(field_get:outside.outside_inner.value)
  return *value_;
}
inline void outside_inner::set_value(const ::std::string& value) {
  set_has_value();
  if (value_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    value_ = new ::std::string;
  }
  value_->assign(value);
  // @@protoc_insertion_point(field_set:outside.outside_inner.value)
}
inline void outside_inner::set_value(const char* value) {
  set_has_value();
  if (value_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    value_ = new ::std::string;
  }
  value_->assign(value);
  // @@protoc_insertion_point(field_set_char:outside.outside_inner.value)
}
inline void outside_inner::set_value(const char* value, size_t size) {
  set_has_value();
  if (value_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    value_ = new ::std::string;
  }
  value_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:outside.outside_inner.value)
}
inline ::std::string* outside_inner::mutable_value() {
  set_has_value();
  if (value_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    value_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:outside.outside_inner.value)
  return value_;
}
inline ::std::string* outside_inner::release_value() {
  clear_has_value();
  if (value_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = value_;
    value_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void outside_inner::set_allocated_value(::std::string* value) {
  if (value_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete value_;
  }
  if (value) {
    set_has_value();
    value_ = value;
  } else {
    clear_has_value();
    value_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:outside.outside_inner.value)
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace outside

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_outside_2eproto__INCLUDED