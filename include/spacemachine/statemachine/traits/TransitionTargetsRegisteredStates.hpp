#ifndef SPACEMACHINE_TRAITS_TRANSITIONTARGETSREGISTEREDSTATES_HPP
#define SPACEMACHINE_TRAITS_TRANSITIONTARGETSREGISTEREDSTATES_HPP

#include "spacemachine/polyfill/type_traits.hpp"
#include "spacemachine/statemachine/fwd/State.hpp"
#include "spacemachine/statemachine/traits/IsKnownStateID.hpp"

namespace spacemachine::statemachine::traits {

namespace impl {
template<typename State, typename... States>
struct transitions_target_known_ids_impl;

template<typename StateID, typename Fn, typename... Transitions,
         typename... States>
struct transitions_target_known_ids_impl<State<StateID, Fn, Transitions...>,
                                         States...>
    : polyfill::integral_constant<
              bool, polyfill::conjunction_v<is_known_state_id<
                            typename Transitions::ToID, States...>...>> {};

} // namespace impl

// TODO: better name
// TODO: add _v variant
template<typename State, typename... States>
struct transitions_target_known_ids
    : impl::transitions_target_known_ids_impl<polyfill::decay_t<State>,
                                              States...> {};

// TODO: add _v variant
template<typename... States>
struct transitions_target_registered_states {
    template<typename State>
    using type = transitions_target_known_ids<State, States...>;
};

} // namespace spacemachine::statemachine::traits

#endif // SPACEMACHINE_TRAITS_TRANSITIONTARGETSREGISTEREDSTATES_HPP
