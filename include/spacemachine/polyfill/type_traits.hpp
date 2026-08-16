#ifndef SPACEMACHINE_POLYFILL_TYPE_TRAITS_HPP
#define SPACEMACHINE_POLYFILL_TYPE_TRAITS_HPP

#if __has_include(<type_traits>)                                               \
        && !defined(SPACEMACHINE_POLYFILL_NO_STD_TYPE_TRAITS)
#include <type_traits>
#else
#include "spacemachine/polyfill/detail/type_traits.hpp"
#endif

namespace spacemachine::polyfill {

#if __has_include(<type_traits>)                                               \
        && !defined(SPACEMACHINE_POLYFILL_NO_STD_TYPE_TRAITS)
using std::conjunction_v;
using std::decay_t;
using std::disjunction_v;
using std::enable_if_t;
using std::false_type;
using std::integral_constant;
using std::is_base_of;
using std::is_base_of_v;
using std::is_constructible;
using std::is_constructible_v;
using std::is_invocable_r_v;
using std::is_invocable_v;
using std::is_lvalue_reference_v;
using std::is_nothrow_constructible;
using std::is_nothrow_constructible_v;
using std::is_same;
using std::is_same_v;
using std::remove_reference_t;
using std::true_type;
#else
using detail::conjunction_v;
using detail::decay_t;
using detail::disjunction_v;
using detail::enable_if_t;
using detail::false_type;
using detail::integral_constant;
using detail::is_base_of;
using detail::is_base_of_v;
using detail::is_constructible;
using detail::is_constructible_v;
using detail::is_lvalue_reference_v;
using detail::is_nothrow_constructible;
using detail::is_nothrow_constructible_v;
using detail::is_same;
using detail::is_same_v;
using detail::remove_reference_t;
using detail::true_type;

template<typename Callable, typename... Args>
constexpr bool is_invocable_v = detail::is_callable_v<Callable, Args...>;

template<typename Result, typename Callable, typename... Args>
constexpr bool is_invocable_r_v
        = detail::is_callable_r_v<Result, Callable, Args...>;
#endif

} // namespace spacemachine::polyfill

#endif // SPACEMACHINE_POLYFILL_TYPE_TRAITS_HPP
