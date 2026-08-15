#ifndef SPACEMACHINE_CONDITION_HPP
#define SPACEMACHINE_CONDITION_HPP

#include "spacemachine/statemachine/callable/Callable.hpp"

namespace spacemachine::statemachine::traits {

template<typename Fn>
constexpr bool is_valid_condition_v = polyfill::is_invocable_r_v<bool, Fn>;

}

namespace spacemachine::statemachine::callable {

template<typename Fn>
struct Condition : Callable<Fn> {
    static_assert(traits::is_valid_condition_v<Fn>,
                  "Condition must be callable with zero arguments and "
                  "return bool!");

    using Base = Callable<Fn>;
    using Base::Base;
    using Base::operator();
    using Base::operator new;
    using Base::operator delete;
};

} // namespace spacemachine::statemachine::callable

#endif // SPACEMACHINE_CONDITION_HPP
