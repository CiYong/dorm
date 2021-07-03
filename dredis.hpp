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

#ifndef __drift_orm_redis_hpp__
#define __drift_orm_redis_hpp__

#include "dvisit.hpp"

#include <string>
#include <vector>
#include <cstdint>

#include <iostream>

#define OPERATOR_HASH(type_)                                                    \
    void operator()(const char* name_, const type_& value_) {                   \
        if (pk_name == name_) {                                                 \
            pk_value = std::to_string(value_);                                  \
        }                                                                       \
        value.append(std::string{name_} + " " + std::to_string(value_) + " ");  \
    }                                                                           \
                                                                                \
    void operator()(const char* name_, const std::vector<type_>& value_) {      \
        std::string temp;                                                       \
        for (auto& v : value_) {                                                \
            temp.append(std::to_string(v) + ",");                               \
        }                                                                       \
        value.append(std::string{name_} + " " + temp + " ");                    \
    }


namespace drift {
namespace orm {

struct RedisPair {
    std::string key;
    std::string value;
};

struct HashVisitor {
    std::string name;
    std::string value;
    std::string pk_name;
    std::string pk_value;

    OPERATOR_HASH(uint8_t)
    OPERATOR_HASH(uint16_t)
    OPERATOR_HASH(uint32_t)
    OPERATOR_HASH(uint64_t)

    OPERATOR_HASH(int8_t)
    OPERATOR_HASH(int16_t)
    OPERATOR_HASH(int32_t)
    OPERATOR_HASH(int64_t)

    void operator()(const char* name_, const std::string& value_) {
        if (pk_name == name_) {
            pk_value = value_;
        }
        value.append(std::string{name_} + " " + value_ + " ");
    }

    void operator()(const char* name_, const std::vector<std::string>& value_) {
        std::string temp;
        for (auto& v : value_) {
            temp.append(v + ",");
        }

        if (pk_name == name_) {
            pk_value = temp;
        }

        value.append(std::string{name_} + " " + temp + " ");
    }

    template<typename T>
    void operator()(const char* name_, const T& value_) {
        HashVisitor visitor;
        drift::for_each(value_, visitor);
        value.append(std::string{name_} + " " + visitor.value + " ");
    }
};

template<typename T>
RedisPair redis_hash_pack(T& value_) {
    HashVisitor visitor;
    visitor.name = drift::traits::visitable<T>::name();
    visitor.pk_name = drift::traits::visitable<T>::primary_key();
    drift::for_each(value_, visitor);

    return {(visitor.name + ":" + visitor.pk_value), visitor.value};
}

template<typename T>
T redis_hash_unpack(std::string& value_) {

    return {};
}

} // namespace orm
} // namespace drift

#endif // __drift_orm_redis_hpp__

