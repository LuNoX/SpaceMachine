#ifndef SPACEMACHINE_WORK_HPP
#define SPACEMACHINE_WORK_HPP

#include "spacemachine/statemachine/callable/Callable.hpp"

namespace spacemachine::statemachine::traits {
template<typename Fn>
constexpr bool is_valid_work_v = polyfill::is_invocable_v<Fn>;
}

namespace spacemachine::statemachine::callable {

template<typename Fn>
struct Work : Callable<Fn> {
    static_assert(traits::is_valid_work_v<Fn>,
                  "Work must be callable with zero arguments!");

    using Base = Callable<Fn>;
    using Base::Base;
    using Base::operator();
    using Base::operator new;
    using Base::operator delete;
};

} // namespace spacemachine::statemachine::callable

#endif // SPACEMACHINE_WORK_HPP
