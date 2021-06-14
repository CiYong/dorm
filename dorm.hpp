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

#ifndef __drift_orm_hpp__
#define __drift_orm_hpp__

#include "dvisit.hpp"
#include "dmsgpack.hpp"

#define DORM(struct_name_, store_type_, primary_key_, ...)           \
    DORM_VISIT(struct_name_, store_type_, primary_key_, __VA_ARGS__) \
    DORM_MSGPACK(struct_name_, __VA_ARGS__)

#endif // __drift_orm_hpp__
