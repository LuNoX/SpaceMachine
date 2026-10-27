#ifndef SPACEMACHINE_STATEMACHINE_HPP
#define SPACEMACHINE_STATEMACHINE_HPP

#include "spacemachine/polyfill/cstddef.hpp"
#include "spacemachine/polyfill/type_traits.hpp"
#include "spacemachine/polyfill/utility.hpp"
#include "spacemachine/statemachine/State.hpp"
#include "spacemachine/statemachine/Transition.hpp"
#include "spacemachine/statemachine/traits/AllStatesUnique.hpp"
#include "spacemachine/statemachine/traits/TransitionTargetsRegisteredStates.hpp" // IWYU pragma: keep
#include "spacemachine/statemachine/typelist/TypeList.hpp"

namespace spacemachine::statemachine {

/// TODO: turn this comment into a doc
/// States and transitions are evaluated in declaration order.
/// The first matching transition takes priority. To express priority,
/// declare higher-priority transitions earlier.
template<typename InitialStateID, typename... States>
struct StateMachine {
    StateMachine() = delete;
    template<typename... Ss,
             typename = polyfill::enable_if_t<
                     !traits::is_constructing_from_self_v<StateMachine, Ss...>
                     && polyfill::conjunction_v<
                             polyfill::is_constructible<States, Ss&&>...>>>
    explicit StateMachine(Ss&&... states) noexcept(
            polyfill::conjunction_v<
                    polyfill::is_nothrow_constructible<States, Ss&&>...>)
        : m_states(polyfill::forward<Ss>(states)...),
          m_activeStateIndex(indexOf<InitialStateID>)
    {}
    ~StateMachine() = default;

    StateMachine(const StateMachine&) = default;
    StateMachine& operator=(const StateMachine&) = default;
    StateMachine(StateMachine&&) noexcept = default;
    StateMachine& operator=(StateMachine&&) noexcept = default;

    void* operator new(polyfill::size_t) = delete;
    void operator delete(void*) = delete;

    void Step() { StepImpl(stateIndices); }

    template<typename StateID>
    [[nodiscard]] bool IsActive() const
    {
        static_assert(traits::is_known_state_id_v<StateID, States...>,
                      "State ID is not among the state machine's states!");
        // this-> because cppcheck otherwise thinks this could be static.
        return this->m_activeStateIndex == indexOf<StateID>;
    }

private:
    template<typename StateID>
    static constexpr polyfill::size_t indexOf
            = detail::FindStateIndexImpl<StateID, 0, States...>::value;

    template<polyfill::size_t Index>
    using StateIndex = polyfill::integral_constant<polyfill::size_t, Index>;

    static constexpr polyfill::make_index_sequence<sizeof...(States)>
            stateIndices{};

    template<polyfill::size_t... StateIndices>
    void StepImpl(polyfill::index_sequence<StateIndices...> /*stateIndices*/)
    {
        // StepFrom can mutate m_activeStateIndex, so store it before the fold.
        const polyfill::size_t active = m_activeStateIndex;
        ((active == StateIndices ? StepFrom<StateIndices>() : void()), ...);
    }

    template<polyfill::size_t ActiveStateIndex>
    void StepFrom()
    {
        WithNextState<ActiveStateIndex>([this](auto next) {
            constexpr polyfill::size_t target = decltype(next)::value;
            if constexpr (target != ActiveStateIndex) {
                m_activeStateIndex = target;
            }
            detail::GetValue<target>(m_states).m_work();
        });
    }

    template<polyfill::size_t ActiveStateIndex, typename OnNext>
    void WithNextState(const OnNext& onNext)
    {
        if (!TryTransitions(onNext, detail::GetValue<ActiveStateIndex>(m_states)
                                            .m_transitions)) {
            onNext(StateIndex<ActiveStateIndex>{});
        }
    }

    template<typename OnNext, polyfill::size_t... TransitionIndices,
             typename... Transitions>
    static bool TryTransitions(
            const OnNext& onNext,
            detail::TypeListImpl<polyfill::index_sequence<TransitionIndices...>,
                                 Transitions...>& transitions)
    {
        return (TryTransition(onNext,
                              detail::GetValue<TransitionIndices>(transitions))
                || ...);
    }

    template<typename OnNext, typename ToStateID, typename ConditionCallable>
    static bool
    TryTransition(const OnNext& onNext,
                  Transition<ToStateID, ConditionCallable>& transition)
    {
        if (!transition.m_shouldTrigger()) { return false; }
        onNext(StateIndex<indexOf<ToStateID>>{});
        return true;
    }

    TypeList<States...> m_states;
    polyfill::size_t m_activeStateIndex;
};

template<typename InitialStateID, typename... States,
         polyfill::enable_if_t<
                 polyfill::conjunction_v<traits::is_state<States>...>
                         && traits::is_known_state_id_v<InitialStateID,
                                                        States...>
                         && traits::all_states_unique<States...>::value
                         && for_each_type<
                                 traits::transitions_target_registered_states<
                                         States...>::template type,
                                 States...>::value,
                 int> = 0>
[[nodiscard]] StateMachine<InitialStateID, polyfill::decay_t<States>...>
MakeStateMachine(States&&... states)
{
    return StateMachine<InitialStateID, polyfill::decay_t<States>...>(
            polyfill::forward<States>(states)...);
}

template<typename InitialStateID, typename... States,
         polyfill::enable_if_t<
                 !polyfill::conjunction_v<traits::is_state<States>...>, int>
         = 0>
auto MakeStateMachine(States&&... /*ss*/)
{
    static_assert(for_each_type<traits::is_state, States...>::value,
                  "One or more arguments are not states! See backtrace for "
                  "offending argument.");
}

template<typename InitialStateID, typename... States,
         polyfill::enable_if_t<
                 polyfill::conjunction_v<traits::is_state<States>...>
                         && !traits::is_known_state_id_v<InitialStateID,
                                                         States...>,
                 int> = 0>
auto MakeStateMachine(States&&... /*ss*/)
{
    static_assert(traits::is_known_state_id_v<InitialStateID, States...>,
                  "Initial state ID is not among the provided states!");
}

template<typename InitialStateID, typename... States,
         polyfill::enable_if_t<
                 polyfill::conjunction_v<traits::is_state<States>...>
                         && traits::is_known_state_id_v<InitialStateID,
                                                        States...>
                         && !traits::all_states_unique<States...>::value,
                 int> = 0>
auto MakeStateMachine(States&&... /*ss*/)
{
    static_assert(traits::all_states_unique<States...>::value,
                  "Duplicate state ID! See backtrace for offending ID.");
}

template<typename InitialStateID, typename... States,
         polyfill::enable_if_t<
                 polyfill::conjunction_v<traits::is_state<States>...>
                         && traits::is_known_state_id_v<InitialStateID,
                                                        States...>
                         && traits::all_states_unique<States...>::value
                         && !for_each_type<
                                 traits::transitions_target_registered_states<
                                         States...>::template type,
                                 States...>::value,
                 int> = 0>
auto MakeStateMachine(States&&... /*ss*/)
{
    static_assert(for_each_type<traits::transitions_target_registered_states<
                                        States...>::template type,
                                States...>::value,
                  "Transition targets unknown state ID! See backtrace for "
                  "offending state.");
}
} // namespace spacemachine::statemachine

#endif // SPACEMACHINE_STATEMACHINE_HPP
