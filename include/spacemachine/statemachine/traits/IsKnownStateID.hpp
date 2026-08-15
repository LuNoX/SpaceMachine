#ifndef SPACEMACHINE_TRAITS_ISKNOWNSTATEID_HPP
#define SPACEMACHINE_TRAITS_ISKNOWNSTATEID_HPP

#include "spacemachine/polyfill/type_traits.hpp"
#include "spacemachine/statemachine/traits/IsInPack.hpp"

namespace spacemachine::statemachine::traits {

namespace impl {

template<typename TargetID, typename... States>
struct is_known_state_id_impl
    : traits::is_in_pack<TargetID, typename States::ID...> {};

} // namespace impl

template<typename TargetID, typename... States>
struct is_known_state_id
    : impl::is_known_state_id_impl<TargetID, polyfill::decay_t<States>...> {};

template<typename TargetID, typename... States>
constexpr bool is_known_state_id_v
        = is_known_state_id<TargetID, States...>::value;

} // namespace spacemachine::statemachine::traits

#endif // SPACEMACHINE_TRAITS_ISKNOWNSTATEID_HPP
