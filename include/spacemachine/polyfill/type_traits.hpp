#ifndef SPACEMACHINE_POLYFILL_TYPE_TRAITS_HPP
#define SPACEMACHINE_POLYFILL_TYPE_TRAITS_HPP

#if __has_include(<type_traits>)
#include <type_traits>
#endif

namespace spacemachine::polyfill {

#if __has_include(<type_traits>)
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
using std::is_nothrow_constructible;
using std::is_nothrow_constructible_v;
using std::is_same;
using std::is_same_v;
using std::true_type;
#endif

} // namespace spacemachine::polyfill

#endif // SPACEMACHINE_POLYFILL_TYPE_TRAITS_HPP
