// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: infer_contact.proto

#include "infer_contact.pb.h"

#include <algorithm>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
namespace InferContact {
class dataDefaultTypeInternal {
 public:
  ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<data> _instance;
} _data_default_instance_;
}  // namespace InferContact
static void InitDefaultsscc_info_data_infer_5fcontact_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::InferContact::_data_default_instance_;
    new (ptr) ::InferContact::data();
    ::PROTOBUF_NAMESPACE_ID::internal::OnShutdownDestroyMessage(ptr);
  }
}

::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<0> scc_info_data_infer_5fcontact_2eproto =
    {{ATOMIC_VAR_INIT(::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase::kUninitialized), 0, 0, InitDefaultsscc_info_data_infer_5fcontact_2eproto}, {}};

static ::PROTOBUF_NAMESPACE_ID::Metadata file_level_metadata_infer_5fcontact_2eproto[1];
static constexpr ::PROTOBUF_NAMESPACE_ID::EnumDescriptor const** file_level_enum_descriptors_infer_5fcontact_2eproto = nullptr;
static constexpr ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor const** file_level_service_descriptors_infer_5fcontact_2eproto = nullptr;

const ::PROTOBUF_NAMESPACE_ID::uint32 TableStruct_infer_5fcontact_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::InferContact::data, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::InferContact::data, buf_),
};
static const ::PROTOBUF_NAMESPACE_ID::internal::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, sizeof(::InferContact::data)},
};

static ::PROTOBUF_NAMESPACE_ID::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::InferContact::_data_default_instance_),
};

const char descriptor_table_protodef_infer_5fcontact_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\023infer_contact.proto\022\014InferContact\"\023\n\004d"
  "ata\022\013\n\003buf\030\001 \001(\0142\261\001\n\020InferContactGrpc\0224\n"
  "\010cmd_node\022\022.InferContact.data\032\022.InferCon"
  "tact.data\"\000\0223\n\007ai_init\022\022.InferContact.da"
  "ta\032\022.InferContact.data\"\000\0222\n\006unload\022\022.Inf"
  "erContact.data\032\022.InferContact.data\"\000b\006pr"
  "oto3"
  ;
static const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable*const descriptor_table_infer_5fcontact_2eproto_deps[1] = {
};
static ::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase*const descriptor_table_infer_5fcontact_2eproto_sccs[1] = {
  &scc_info_data_infer_5fcontact_2eproto.base,
};
static ::PROTOBUF_NAMESPACE_ID::internal::once_flag descriptor_table_infer_5fcontact_2eproto_once;
const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_infer_5fcontact_2eproto = {
  false, false, descriptor_table_protodef_infer_5fcontact_2eproto, "infer_contact.proto", 244,
  &descriptor_table_infer_5fcontact_2eproto_once, descriptor_table_infer_5fcontact_2eproto_sccs, descriptor_table_infer_5fcontact_2eproto_deps, 1, 0,
  schemas, file_default_instances, TableStruct_infer_5fcontact_2eproto::offsets,
  file_level_metadata_infer_5fcontact_2eproto, 1, file_level_enum_descriptors_infer_5fcontact_2eproto, file_level_service_descriptors_infer_5fcontact_2eproto,
};

// Force running AddDescriptors() at dynamic initialization time.
static bool dynamic_init_dummy_infer_5fcontact_2eproto = (static_cast<void>(::PROTOBUF_NAMESPACE_ID::internal::AddDescriptors(&descriptor_table_infer_5fcontact_2eproto)), true);
namespace InferContact {

// ===================================================================

class data::_Internal {
 public:
};

data::data(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);
  // @@protoc_insertion_point(arena_constructor:InferContact.data)
}
data::data(const data& from)
  : ::PROTOBUF_NAMESPACE_ID::Message() {
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  buf_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (!from._internal_buf().empty()) {
    buf_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_buf(), 
      GetArena());
  }
  // @@protoc_insertion_point(copy_constructor:InferContact.data)
}

void data::SharedCtor() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&scc_info_data_infer_5fcontact_2eproto.base);
  buf_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

data::~data() {
  // @@protoc_insertion_point(destructor:InferContact.data)
  SharedDtor();
  _internal_metadata_.Delete<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

void data::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
  buf_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

void data::ArenaDtor(void* object) {
  data* _this = reinterpret_cast< data* >(object);
  (void)_this;
}
void data::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void data::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const data& data::default_instance() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&::scc_info_data_infer_5fcontact_2eproto.base);
  return *internal_default_instance();
}


void data::Clear() {
// @@protoc_insertion_point(message_clear_start:InferContact.data)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  buf_.ClearToEmpty();
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* data::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {
      // bytes buf = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 10)) {
          auto str = _internal_mutable_buf();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag,
            _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
            ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }  // switch
  }  // while
success:
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* data::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:InferContact.data)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // bytes buf = 1;
  if (this->buf().size() > 0) {
    target = stream->WriteBytesMaybeAliased(
        1, this->_internal_buf(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:InferContact.data)
  return target;
}

size_t data::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:InferContact.data)
  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // bytes buf = 1;
  if (this->buf().size() > 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::BytesSize(
        this->_internal_buf());
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    return ::PROTOBUF_NAMESPACE_ID::internal::ComputeUnknownFieldsSize(
        _internal_metadata_, total_size, &_cached_size_);
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void data::MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:InferContact.data)
  GOOGLE_DCHECK_NE(&from, this);
  const data* source =
      ::PROTOBUF_NAMESPACE_ID::DynamicCastToGenerated<data>(
          &from);
  if (source == nullptr) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:InferContact.data)
    ::PROTOBUF_NAMESPACE_ID::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:InferContact.data)
    MergeFrom(*source);
  }
}

void data::MergeFrom(const data& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:InferContact.data)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from.buf().size() > 0) {
    _internal_set_buf(from._internal_buf());
  }
}

void data::CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:InferContact.data)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void data::CopyFrom(const data& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:InferContact.data)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool data::IsInitialized() const {
  return true;
}

void data::InternalSwap(data* other) {
  using std::swap;
  _internal_metadata_.Swap<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(&other->_internal_metadata_);
  buf_.Swap(&other->buf_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}

::PROTOBUF_NAMESPACE_ID::Metadata data::GetMetadata() const {
  return GetMetadataStatic();
}


// @@protoc_insertion_point(namespace_scope)
}  // namespace InferContact
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::InferContact::data* Arena::CreateMaybeMessage< ::InferContact::data >(Arena* arena) {
  return Arena::CreateMessageInternal< ::InferContact::data >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>
