#ifndef SPACEMACHINE_STATEMACHINE_HPP
#define SPACEMACHINE_STATEMACHINE_HPP

#include "spacemachine/polyfill/cstddef.hpp"
#include "spacemachine/polyfill/type_traits.hpp"
#include "spacemachine/polyfill/utility.hpp"
#include "spacemachine/statemachine/State.hpp"
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
    TypeList<States...> m_states;
    polyfill::size_t m_activeStateIndex;

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
          m_activeStateIndex(detail::FindStateIndexImpl<InitialStateID, 0,
                                                        States...>::value)
    {}
    ~StateMachine() = default;

    StateMachine(const StateMachine&) = default;
    StateMachine& operator=(const StateMachine&) = default;
    StateMachine(StateMachine&&) noexcept = default;
    StateMachine& operator=(StateMachine&&) noexcept = default;

    void* operator new(polyfill::size_t) = delete;
    void operator delete(void*) = delete;
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
