#ifndef SPACEMACHINE_TRANSITION_HPP
#define SPACEMACHINE_TRANSITION_HPP

#include "spacemachine/polyfill/cstddef.hpp"
#include "spacemachine/polyfill/type_traits.hpp"
#include "spacemachine/polyfill/utility.hpp"
#include "spacemachine/statemachine/callable/Condition.hpp"
#include "spacemachine/statemachine/traits/IsSelfConstructing.hpp"

namespace spacemachine::statemachine {

template<typename ToStateID, typename Fn>
struct Transition {
    using ToID = ToStateID;
    using Condition = callable::Condition<Fn>;

    Condition m_shouldTrigger;

    Transition() = delete;
    template<typename F,
             typename = polyfill::enable_if_t<
                     !traits::is_self_constructing_v<Transition, F>
                     && polyfill::is_constructible_v<Condition, F&&>>>
    explicit Transition(F&& shouldTrigger) noexcept(
            polyfill::is_nothrow_constructible_v<Condition, F&&>)
        : m_shouldTrigger{polyfill::forward<F>(shouldTrigger)}
    {}
    ~Transition() = default;

    Transition(const Transition&) = default;
    Transition(Transition&&) noexcept = default;
    Transition& operator=(const Transition&) = default;
    Transition& operator=(Transition&&) noexcept = default;

    void* operator new(polyfill::size_t) = delete;
    void operator delete(void*) = delete;
};

template<typename ToStateID, typename Fn,
         polyfill::enable_if_t<traits::is_valid_condition_v<Fn>, int> = 0>
[[nodiscard]] Transition<ToStateID, polyfill::decay_t<Fn>>
MakeTransition(Fn&& shouldTrigger)
{
    return Transition<ToStateID, polyfill::decay_t<Fn>>(
            polyfill::forward<Fn>(shouldTrigger));
}

template<typename, typename Fn,
         polyfill::enable_if_t<!traits::is_valid_condition_v<Fn>, int> = 0>
auto MakeTransition(Fn&& /*shouldTrigger*/)
{
    static_assert(traits::is_valid_condition_v<Fn>,
                  "Condition must be callable with zero arguments and "
                  "return bool!");
}

namespace traits {

namespace impl {

template<typename>
struct is_transition_impl : polyfill::false_type {};

template<typename ToStateID, typename Fn>
struct is_transition_impl<Transition<ToStateID, Fn>> : polyfill::true_type {};

} // namespace impl

template<typename T>
struct is_transition : impl::is_transition_impl<polyfill::decay_t<T>> {};

template<typename T>
constexpr bool is_transition_v = is_transition<T>::value;

} // namespace traits

} // namespace spacemachine::statemachine

#endif // SPACEMACHINE_TRANSITION_HPP
