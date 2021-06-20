/*
 *
 * Copyright 2021 dorm authors.
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
 *
 */

#ifndef __drift_orm_msgpack_hpp__
#define __drift_orm_msgpack_hpp__

#include "dexpand.hpp"
#include <msgpack.hpp>

namespace msgpack {
MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS) {
    namespace adaptor {

    namespace internal {
    template <typename T>
    msgpack::object const& convert(msgpack::object const& o, T& v) {
        if (o.type != msgpack::type::ARRAY) {
            throw msgpack::type_error();
        }
        v.resize(o.via.array.size);

        if (o.via.array.size > 0) {
            auto p = o.via.array.ptr;
            auto pend = o.via.array.ptr + o.via.array.size;
            auto it = v.begin();
            do {
                p->convert(*it);
                ++p;
                ++it;
            } while (p < pend);
        }
        return o;
    }

    template <typename T, typename Stream>
    msgpack::packer<Stream>& pack(msgpack::packer<Stream>& o, const T& v) {
        auto size = checked_get_container_size(v.size());
        o.pack_array(size);
        for (auto& it : v) {
            o.pack(it);
        }
        return o;
    }

    template <typename T>
    void object_with_zone(msgpack::object::with_zone& o, const T& v) {
        o.type = msgpack::type::ARRAY;
        if (v.empty()) {
            o.via.array.ptr = nullptr;
            o.via.array.size = 0;
        } else {
            auto size = checked_get_container_size(v.size());
            auto p = static_cast<msgpack::object*>(o.zone.allocate_align(sizeof(msgpack::object) * size, alignof(msgpack::object)));
            auto pend = p + size;
            o.via.array.ptr = p;
            o.via.array.size = size;
            auto it = v.begin();
            do {
                *p = msgpack::object(*it, o.zone);
                ++p;
                ++it;
            } while (p < pend);
        }
    }

    } // namespace internal

#define VEC_ADAPTOR(T) \
    template <> \
    struct convert<std::vector<T, std::allocator<T>>> { \
        msgpack::object const& operator()(msgpack::object const& o, std::vector<T, std::allocator<T>>& v) const { \
            return internal::convert(o, v); \
        } \
    }; \
    template <> \
    struct pack<std::vector<T, std::allocator<T>>> { \
        template <typename Stream> \
        msgpack::packer<Stream>& operator()(msgpack::packer<Stream>& o, const std::vector<T, std::allocator<T>>& v) const { \
            return internal::pack(o, v); \
        } \
    }; \
    template <> \
    struct object_with_zone<std::vector<T, std::allocator<T>>> { \
        void operator()(msgpack::object::with_zone& o, const std::vector<T, std::allocator<T>>& v) const { \
            internal::object_with_zone(o, v); \
        } \
    }; \

    VEC_ADAPTOR(char)
    VEC_ADAPTOR(uint8_t)

#undef VEC_ADAPTOR

} // namespace adaptor
} // MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS)
} // namespace msgpack

namespace drift {
namespace orm {
//using Bytes = std::vector<uint8_t>;

template <typename Bytes = std::vector<uint8_t>, typename T>
Bytes msgpack_pack(T&& arg) {
    msgpack::sbuffer buf;
    msgpack::pack(buf, std::forward<T>(arg));
    return Bytes(buf.data(), buf.data() + buf.size());
}

template <typename T, typename Bytes = std::vector<uint8_t>>
T msgpack_unpack(const Bytes& bytes) {
    auto handle = msgpack::unpack(reinterpret_cast<const char*>(bytes.data()), bytes.size());
    return handle.get().template as<T>();
}

} // namespace orm
} // namespace drift

#define DORM_MSGPACK_FIELD_PACK(field_name)                                                        \
    o.pack(v.field_name);

#define DORM_MSGPACK_FIELD_UNPACK(field_name)                                                      \
    o.via.array.ptr[field_type::field_name].as<decltype(self_type::field_name)>(),


#define DORM_MSGPACK(struct_name_, ...)                                                            \
namespace msgpack {                                                                                \
MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS) {                                            \
namespace adaptor {                                                                                \
                                                                                                   \
struct struct_name_##_field_index {                                                                \
  enum index { __VA_ARGS__ };                                                                      \
};                                                                                                 \
                                                                                                   \
template<>                                                                                         \
struct pack<struct_name_> {                                                                        \
    template <typename Stream>                                                                     \
    packer<Stream>& operator()(msgpack::packer<Stream>& o, struct_name_ const& v) const {          \
        o.pack_array(DRIFT_PP_NARG(__VA_ARGS__));                                                  \
        DRIFT_PP_MAP(DORM_MSGPACK_FIELD_PACK, __VA_ARGS__)                                         \
        return o;                                                                                  \
    }                                                                                              \
};                                                                                                 \
                                                                                                   \
template<>                                                                                         \
struct convert<struct_name_> {                                                                     \
    using self_type = struct_name_;                                                                \
    using field_type = struct_name_##_field_index;                                                 \
    msgpack::object const& operator()(msgpack::object const& o, struct_name_& v) const {           \
        if (o.type != msgpack::type::ARRAY) throw msgpack::type_error();                           \
        if (o.via.array.size != DRIFT_PP_NARG(__VA_ARGS__)) throw msgpack::type_error();           \
        v = struct_name_ {                                                                         \
            DRIFT_PP_MAP(DORM_MSGPACK_FIELD_UNPACK, __VA_ARGS__)                                   \
            };                                                                                     \
        return o;                                                                                  \
    }                                                                                              \
};                                                                                                 \
                                                                                                   \
}                                                                                                  \
}                                                                                                  \
}                                                                                                  \


#endif // __drift_orm_msgpack_hpp__
