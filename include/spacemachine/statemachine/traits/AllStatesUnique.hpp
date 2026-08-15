#ifndef SPACEMACHINE_TRAITS_ALLSTATESUNIQUE_HPP
#define SPACEMACHINE_TRAITS_ALLSTATESUNIQUE_HPP

#include "spacemachine/polyfill/type_traits.hpp"
#include "spacemachine/statemachine/traits/IsInPack.hpp"

namespace spacemachine::statemachine::traits {

namespace impl {
template<typename... StateIDs>
struct all_state_ids_unique_impl : polyfill::true_type {};

template<typename Head, typename... Tail>
struct all_state_ids_unique_impl<Head, Tail...>
    : polyfill::integral_constant<
              bool, traits::is_not_in_pack_v<Head, Tail...>
                            && all_state_ids_unique_impl<Tail...>::value> {
    static_assert(
            traits::is_not_in_pack_v<Head, Tail...>,
            "Duplicate state ID detected! See backtrace for offending ID.");
};

} // namespace impl

template<typename... States>
struct all_states_unique : impl::all_state_ids_unique_impl<
                                   typename polyfill::decay_t<States>::ID...> {
};

template<typename... States>
constexpr bool all_states_unique_v = all_states_unique<States...>::value;

} // namespace spacemachine::statemachine::traits

#endif // SPACEMACHINE_TRAITS_ALLSTATESUNIQUE_HPP
