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

#ifndef __drift_orm_visit_hpp__
#define __drift_orm_visit_hpp__

#include "dexpand.hpp"

namespace drift {
namespace traits {

template <typename T, typename Enable = void>
struct visitable;

template <typename T, typename Enable = void>
struct is_visitable : std::false_type {};

template <typename T>
struct is_visitable<T, typename std::enable_if<visitable<T>::value>::type> : std::true_type {};

template <typename T>
struct clean {
  typedef typename std::remove_cv<typename std::remove_reference<T>::type>::type type;
};

template <typename T>
using clean_t = typename clean<T>::type;


template <typename T, typename U>
struct common_type {
  typedef decltype(true ? std::declval<T>() : std::declval<U>()) type;
};

} // namespace traits


template <typename T>
struct type_c { using type = T; };


template <typename MemPtr, MemPtr ptr>
struct accessor {
  template <typename T>
  DRIFT_CONSTEXPR auto operator()(T && t) const -> decltype(std::forward<T>(t).*ptr) {
    return std::forward<T>(t).*ptr;
  }
};


template <typename Struct>
DRIFT_CONSTEXPR std::size_t field_count()
{
  return traits::visitable<traits::clean_t<Struct>>::field_count;
}

template <typename Struct>
DRIFT_CONSTEXPR std::size_t field_count(Struct &&) { return field_count<Struct>(); }



template <typename Struct, typename Visitor>
DRIFT_CONSTEXPR auto apply_visitor(Visitor&& visitor_, Struct && struct_) ->
  typename std::enable_if<
             traits::is_visitable<traits::clean_t<Struct>>::value
           >::type
{
  traits::visitable<traits::clean_t<Struct>>::apply(std::forward<Visitor>(visitor_), std::forward<Struct>(struct_));
}


template <typename Struct1, typename Struct2, typename Visitor>
DRIFT_CONSTEXPR auto apply_visitor(Visitor&& visitor, Struct1 && struct1, Struct2 && struct2) ->
  typename std::enable_if<
             traits::is_visitable<
               traits::clean_t<typename traits::common_type<Struct1, Struct2>::type>
             >::value
           >::type
{
  using common_S = typename traits::common_type<Struct1, Struct2>::type;
  traits::visitable<traits::clean_t<common_S>>::apply(std::forward<Visitor>(visitor),
                                                      std::forward<Struct1>(struct1),
                                                      std::forward<Struct2>(struct2));
}


template <typename Visitor, typename Struct>
DRIFT_CONSTEXPR auto serialize(Struct && struct_, Visitor&& visitor_) ->
  typename std::enable_if<
             traits::is_visitable<traits::clean_t<Struct>>::value
           >::type
{
  traits::visitable<traits::clean_t<Struct>>::apply(std::forward<Visitor>(visitor_), std::forward<Struct>(struct_));
}


template <typename Visitor, typename Struct>
DRIFT_CONSTEXPR auto for_each(Struct && struct_, Visitor&& visitor_) ->
  typename std::enable_if<
             traits::is_visitable<traits::clean_t<Struct>>::value
           >::type
{
  traits::visitable<traits::clean_t<Struct>>::apply(std::forward<Visitor>(visitor_), std::forward<Struct>(struct_));
}


template <typename Struct1, typename Struct2, typename Visitor>
DRIFT_CONSTEXPR auto for_each(Struct1 && struct1_, Struct2 && struct2_, Visitor&& visitor_) ->
  typename std::enable_if<
             traits::is_visitable<
               traits::clean_t<typename traits::common_type<Struct1, Struct2>::type>
             >::value
           >::type
{
  using common_S = typename traits::common_type<Struct1, Struct2>::type;
  traits::visitable<traits::clean_t<common_S>>::apply(std::forward<Visitor>(visitor_),
                                                      std::forward<Struct1>(struct1_),
                                                      std::forward<Struct2>(struct2_));
}


template <typename Struct, typename Visitor>
DRIFT_CONSTEXPR auto types(Visitor&& visitor_) ->
  typename std::enable_if<
             traits::is_visitable<traits::clean_t<Struct>>::value
           >::type
{
  traits::visitable<traits::clean_t<Struct>>::types(std::forward<Visitor>(visitor_));
}


template <typename Struct, typename Visitor>
DRIFT_CONSTEXPR auto pointers(Visitor&& visitor_) ->
  typename std::enable_if<
             traits::is_visitable<traits::clean_t<Struct>>::value
           >::type
{
  traits::visitable<traits::clean_t<Struct>>::pointers(std::forward<Visitor>(visitor_));
}


template <typename Struct, typename Visitor>
DRIFT_CONSTEXPR auto visit_accessors(Visitor&& visitor_) ->
  typename std::enable_if<
             traits::is_visitable<traits::clean_t<Struct>>::value
           >::type
{
  traits::visitable<traits::clean_t<Struct>>::visit_accessors(std::forward<Visitor>(visitor_));
}


template <typename Struct, typename Visitor>
DRIFT_CONSTEXPR auto apply_visitor(Visitor&& visitor_) ->
  typename std::enable_if<
             traits::is_visitable<traits::clean_t<Struct>>::value
           >::type
{
  drift::pointers<Struct>(std::forward<Visitor>(visitor_));
}


template <int idx, typename Struct>
DRIFT_CONSTEXPR auto get(Struct && struct_) ->
  typename std::enable_if<
             traits::is_visitable<traits::clean_t<Struct>>::value,
             decltype(traits::visitable<traits::clean_t<Struct>>::get_value(std::integral_constant<int, idx>{}, std::forward<Struct>(struct_)))
           >::type
{
  return traits::visitable<traits::clean_t<Struct>>::get_value(std::integral_constant<int, idx>{}, std::forward<Struct>(struct_));
}


template <int idx, typename Struct>
DRIFT_CONSTEXPR auto name() ->
  typename std::enable_if<
             traits::is_visitable<traits::clean_t<Struct>>::value,
             decltype(traits::visitable<traits::clean_t<Struct>>::name(std::integral_constant<int, idx>{}))
           >::type
{
  return traits::visitable<traits::clean_t<Struct>>::name(std::integral_constant<int, idx>{});
}

template <int idx, typename Struct>
DRIFT_CONSTEXPR auto name(Struct &&) -> decltype(name<idx, Struct>()) {
  return name<idx, Struct>();
}

template <int idx, typename Struct>
DRIFT_CONSTEXPR auto field() -> decltype(name<idx, Struct>()) {
  return name<idx, Struct>();
}

template <int idx, typename Struct>
DRIFT_CONSTEXPR auto pointer() ->
  typename std::enable_if<
             traits::is_visitable<traits::clean_t<Struct>>::value,
             decltype(traits::visitable<traits::clean_t<Struct>>::pointer(std::integral_constant<int, idx>{}))
           >::type
{
  return traits::visitable<traits::clean_t<Struct>>::pointer(std::integral_constant<int, idx>{});
}

template <int idx, typename Struct>
DRIFT_CONSTEXPR auto pointer(Struct &&) -> decltype(pointer<idx, Struct>()) {
  return pointer<idx, Struct>();
}


template <int idx, typename Struct>
DRIFT_CONSTEXPR auto get_accessor() ->
  typename std::enable_if<
             traits::is_visitable<traits::clean_t<Struct>>::value,
             decltype(traits::visitable<traits::clean_t<Struct>>::get_accessor(std::integral_constant<int, idx>{}))
           >::type
{
  return traits::visitable<traits::clean_t<Struct>>::get_accessor(std::integral_constant<int, idx>{});
}

template <int idx, typename Struct>
DRIFT_CONSTEXPR auto get_accessor(Struct &&) -> decltype(get_accessor<idx, Struct>()) {
  return get_accessor<idx, Struct>();
}


template <int idx, typename Struct>
struct type_at_s {
  using type_c = decltype(traits::visitable<traits::clean_t<Struct>>::type_at(std::integral_constant<int, idx>{}));
  using type = typename type_c::type;
};

template <int idx, typename Struct>
using type_at = typename type_at_s<idx, Struct>::type;


template <typename Struct>
DRIFT_CONSTEXPR auto name() ->
  typename std::enable_if<
             traits::is_visitable<traits::clean_t<Struct>>::value,
             decltype(traits::visitable<traits::clean_t<Struct>>::name())
           >::type
{
  return traits::visitable<traits::clean_t<Struct>>::name();
}

template <typename Struct>
DRIFT_CONSTEXPR auto name(Struct &&) -> decltype(name<Struct>()) {
  return name<Struct>();
}

} // namespace drift


enum RedisType {
    None    = -1,
    String  = (1 << 1),
    Hash    = (1 << 2),
    Set     = (1 << 3),
    Sql     = (1 << 4),
};

namespace drift {

#define DRIFT_FIELD_COUNT(field_name)                                                       \
  + 1

#define DRIFT_FIELD_HELPER(field_name)                                                      \
  std::forward<Visitor>(visitor)(#field_name, std::forward<Struct>(struct_instance_).field_name);

#define DRIFT_FIELD_HELPER_PTR(field_name)                                                  \
  std::forward<Visitor>(visitor)(#field_name, &self_type::field_name);

#define DRIFT_FIELD_HELPER_TYPE(field_name)                                                 \
  std::forward<Visitor>(visitor)(#field_name, drift::type_c<decltype(self_type::field_name)>{});

#define DRIFT_FIELD_HELPER_ACC(field_name)                                                  \
  std::forward<Visitor>(visitor)(#field_name, drift::accessor<decltype(&self_type::field_name), &self_type::field_name>{});

#define DRIFT_FIELD_HELPER_PAIR(field_name)                                                 \
  std::forward<Visitor>(visitor)(#field_name, std::forward<Struct1>(struct1_).field_name, std::forward<Struct2>(struct2_).field_name);


#define DRIFT_MAKE_GETTERS(field_name)                                                             \
  template <typename Struct>                                                                       \
  static DRIFT_CONSTEXPR auto                                                                      \
    get_value(std::integral_constant<int, fields::field_name>, Struct && struct_) ->               \
    decltype((std::forward<Struct>(struct_).field_name)) {                                         \
    return std::forward<Struct>(struct_).field_name;                                               \
  }                                                                                                \
                                                                                                   \
  static DRIFT_CONSTEXPR auto                                                                      \
    name(std::integral_constant<int, fields::field_name>) ->                                       \
      decltype(#field_name) {                                                                      \
    return #field_name;                                                                            \
  }                                                                                                \
                                                                                                   \
  static DRIFT_CONSTEXPR auto                                                                      \
    pointer(std::integral_constant<int, fields::field_name>) ->                                    \
      decltype(&self_type::field_name) {                                                           \
    return &self_type::field_name;                                                                 \
  }                                                                                                \
                                                                                                   \
  static DRIFT_CONSTEXPR auto                                                                      \
    get_accessor(std::integral_constant<int, fields::field_name>) ->                               \
      drift::accessor<decltype(&self_type::field_name), &self_type::field_name > {                 \
    return {};                                                                                     \
  }                                                                                                \
                                                                                                   \
  static auto                                                                                      \
    type_at(std::integral_constant<int, fields::field_name>) ->                                    \
      drift::type_c<decltype(self_type::field_name)>;


} // namespace drift


#define DORM_VISIT(struct_name_, orm_type_, primary_key_, ...)                                     \
namespace drift {                                                                                  \
namespace traits {                                                                                 \
                                                                                                   \
template <>                                                                                        \
struct visitable<struct_name_, void> {                                                             \
  using self_type = struct_name_;                                                                  \
  static DRIFT_CONSTEXPR int count = DRIFT_PP_NARG(__VA_ARGS__);                                   \
                                                                                                   \
  static DRIFT_CONSTEXPR auto name()                                                               \
    -> decltype(#struct_name_) {                                                                   \
    return #struct_name_;                                                                          \
  }                                                                                                \
  static DRIFT_CONSTEXPR auto primary_key()                                                        \
    -> decltype(#primary_key_) {                                                                   \
    return #primary_key_;                                                                          \
  }                                                                                                \
                                                                                                   \
  static DRIFT_CONSTEXPR auto orm_type()                                                           \
    -> decltype(orm_type_) {                                                                       \
    return orm_type_;                                                                              \
  }                                                                                                \
                                                                                                   \
  static DRIFT_CONSTEXPR const std::size_t field_count = 0                                         \
    DRIFT_PP_MAP(DRIFT_FIELD_COUNT, __VA_ARGS__);                                                  \
                                                                                                   \
  template <typename Visitor, typename Struct>                                                     \
  DRIFT_CONSTEXPR static void apply(Visitor&& visitor, Struct&& struct_instance_)                  \
  {                                                                                                \
    DRIFT_PP_MAP(DRIFT_FIELD_HELPER, __VA_ARGS__)                                                  \
  }                                                                                                \
                                                                                                   \
  template <typename Visitor, typename Struct1, typename Struct2>                                  \
  DRIFT_CONSTEXPR static void apply(Visitor&& visitor, Struct1 && struct1_, Struct2 && struct2_)   \
  {                                                                                                \
    DRIFT_PP_MAP(DRIFT_FIELD_HELPER_PAIR, __VA_ARGS__)                                             \
  }                                                                                                \
                                                                                                   \
  template <typename Visitor>                                                                      \
  DRIFT_CONSTEXPR static void pointers(Visitor&& visitor)                                          \
  {                                                                                                \
    DRIFT_PP_MAP(DRIFT_FIELD_HELPER_PTR, __VA_ARGS__)                                              \
  }                                                                                                \
                                                                                                   \
  template <typename Visitor>                                                                      \
  DRIFT_CONSTEXPR static void types(Visitor&& visitor)                                             \
  {                                                                                                \
    DRIFT_PP_MAP(DRIFT_FIELD_HELPER_TYPE, __VA_ARGS__)                                             \
  }                                                                                                \
                                                                                                   \
  template <typename Visitor>                                                                      \
  DRIFT_CONSTEXPR static void visit_accessors(Visitor&& visitor)                                   \
  {                                                                                                \
    DRIFT_PP_MAP(DRIFT_FIELD_HELPER_ACC, __VA_ARGS__)                                              \
  }                                                                                                \
                                                                                                   \
  struct fields {                                                                                  \
    enum index { __VA_ARGS__ };                                                                    \
  };                                                                                               \
                                                                                                   \
  DRIFT_PP_MAP(DRIFT_MAKE_GETTERS, __VA_ARGS__)                                                    \
                                                                                                   \
  static DRIFT_CONSTEXPR const bool value = true;                                                  \
};                                                                                                 \
                                                                                                   \
}                                                                                                  \
}                                                                                                  \


#endif // __drift_orm_visit_hpp__
