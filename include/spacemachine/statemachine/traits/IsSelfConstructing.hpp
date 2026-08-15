#ifndef SPACEMACHINE_TRAITS_ISSELFCONSTRUCTING_HPP
#define SPACEMACHINE_TRAITS_ISSELFCONSTRUCTING_HPP

#include "spacemachine/polyfill/type_traits.hpp"

namespace spacemachine::statemachine::traits {

template<typename, typename...>
struct is_self_constructing : polyfill::false_type {};

template<typename Self, typename T>
struct is_self_constructing<Self, T>
    : polyfill::integral_constant<
              bool, polyfill::is_base_of_v<polyfill::decay_t<Self>,
                                           polyfill::decay_t<T>>
                            && !polyfill::is_same_v<polyfill::decay_t<Self>,
                                                    polyfill::decay_t<T>>> {};

template<typename Self, typename T>
constexpr bool is_self_constructing_v = is_self_constructing<Self, T>::value;

// TODOL: better name
template<typename Self, typename... Ts>
struct is_constructing_from_self : polyfill::false_type {};

template<typename Self, typename T>
struct is_constructing_from_self<Self, T> : is_self_constructing<Self, T> {};

template<typename Self, typename... Ts>
constexpr bool is_constructing_from_self_v
        = is_constructing_from_self<Self, Ts...>::value;

} // namespace spacemachine::statemachine::traits

#endif // SPACEMACHINE_TRAITS_ISSELFCONSTRUCTING_HPP
