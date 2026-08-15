#ifndef SPACEMACHINE_STATE_HPP
#define SPACEMACHINE_STATE_HPP

#include "spacemachine/polyfill/cstddef.hpp"
#include "spacemachine/polyfill/type_traits.hpp"
#include "spacemachine/polyfill/utility.hpp"
#include "spacemachine/statemachine/Transition.hpp"
#include "spacemachine/statemachine/callable/Work.hpp"
#include "spacemachine/statemachine/typelist/TypeList.hpp"

namespace spacemachine::statemachine {

template<typename StateID, typename Fn, typename... Transitions>
struct State {
    using ID = StateID;
    using Work = callable::Work<Fn>;

    // TODO: change to loop for better error message
    // TODO: consider moving to factory function for better error message
    static_assert(
            polyfill::conjunction_v<traits::is_transition<Transitions>...>,
            "All Transitions must be of type Transition<ToStateID, Fn>!");

    Work m_work;
    TypeList<Transitions...> m_transitions;

    State() = delete;
    template<typename F, typename... Ts,
             typename = polyfill::enable_if_t<
                     !traits::is_self_constructing_v<State, F>
                     && !polyfill::is_same_v<State, F>
                     && polyfill::is_constructible_v<Work, F&&>
                     && polyfill::conjunction_v<
                             polyfill::is_constructible<Transitions, Ts&&>...>>>
    explicit State(F&& work, Ts&&... transitions) noexcept(
            polyfill::is_nothrow_constructible_v<Work, F&&>
            && polyfill::conjunction_v<
                    polyfill::is_nothrow_constructible<Transitions, Ts&&>...>)
        : m_work(polyfill::forward<F>(work)),
          m_transitions(polyfill::forward<Ts>(transitions)...)
    {}
    ~State() = default;

    // TODO: add doc explaining that when copying with reference captures
    // inside the work the user is responsible for managing the lifetime
    // and any UB that may arise from dangling references.
    State(const State&) = default;
    State& operator=(const State&) = default;

    State(State&&) noexcept = default;
    State& operator=(State&&) noexcept = default;

    void* operator new(polyfill::size_t) = delete;
    void operator delete(void*) = delete;
};

template<typename StateID, typename Fn, typename... Transitions,
         polyfill::enable_if_t<traits::is_valid_work_v<Fn>, int> = 0>
[[nodiscard]] State<StateID, polyfill::decay_t<Fn>,
                    polyfill::decay_t<Transitions>...>
MakeState(Fn&& work, Transitions&&... transitions)
{
    return State<StateID, polyfill::decay_t<Fn>,
                 polyfill::decay_t<Transitions>...>(
            polyfill::forward<Fn>(work),
            polyfill::forward<Transitions>(transitions)...);
}

template<typename, typename Fn, typename... Transitions,
         polyfill::enable_if_t<!traits::is_valid_work_v<Fn>, int> = 0>
auto MakeState(Fn&& /*work*/, Transitions&&... /*transitions*/)
{
    static_assert(traits::is_valid_work_v<Fn>,
                  "Work must be callable with zero arguments!");
}

namespace traits {

namespace impl {

template<typename>
struct is_state_impl : polyfill::false_type {};

template<typename StateID, typename Fn, typename... Transitions>
struct is_state_impl<State<StateID, Fn, Transitions...>> : polyfill::true_type {
};

} // namespace impl

template<typename T>
struct is_state : impl::is_state_impl<polyfill::decay_t<T>> {};

template<typename T>
constexpr bool is_state_v = is_state<T>::value;
} // namespace traits

} // namespace spacemachine::statemachine

#endif // SPACEMACHINE_STATE_HPP
