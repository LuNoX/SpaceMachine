#ifndef SPACEMACHINE_TRAITS_ISINPACK_HPP
#define SPACEMACHINE_TRAITS_ISINPACK_HPP

#include "spacemachine/polyfill/type_traits.hpp"

namespace spacemachine::statemachine::traits {

template<typename T, typename... Ts>
struct is_in_pack
    : polyfill::integral_constant<
              bool, polyfill::disjunction_v<polyfill::is_same<
                            polyfill::decay_t<T>, polyfill::decay_t<Ts>>...>> {
};

template<typename T, typename... Ts>
constexpr bool is_in_pack_v = is_in_pack<T, Ts...>::value;

template<typename T, typename... Ts>
struct is_not_in_pack
    : polyfill::integral_constant<bool, !is_in_pack_v<T, Ts...>> {};

template<typename T, typename... Ts>
constexpr bool is_not_in_pack_v = is_not_in_pack<T, Ts...>::value;
} // namespace spacemachine::statemachine::traits

#endif // SPACEMACHINE_TRAITS_ISINPACK_HPP
