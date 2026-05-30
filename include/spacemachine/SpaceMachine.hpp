//
// Created by timob on 22.02.2026.
//

#ifndef SPACEMACHINE_TEMPLATESPACEMACHINE_HPP
#define SPACEMACHINE_TEMPLATESPACEMACHINE_HPP

#if __has_include(<cstddef>)
#include <cstddef>
#endif

#if __has_include(<functional>)
#include <functional>
#endif
#if __has_include(<tuple>)
#include <tuple>
#endif
#if __has_include(<type_traits>)
#include <type_traits>
#endif
#if __has_include(<utility>)
#include <utility>
#endif

namespace SpaceMachine {

namespace polyfill {
#if __has_include(<cstddef>)
using std::size_t;
#endif

#if __has_include(<functional>)
using std::invoke;
#endif

#if __has_include(<tuple>)
using std::tuple;
#endif

#if __has_include(<type_traits>)
using std::conjunction_v;
using std::decay_t;
using std::disjunction_v;
using std::enable_if_t;
using std::false_type;
using std::index_sequence;
using std::integral_constant;
using std::is_base_of;
using std::is_base_of_v;
using std::is_constructible;
using std::is_constructible_v;
using std::is_invocable_r_v;
using std::is_invocable_v;
using std::is_nothrow_constructible;
using std::is_nothrow_constructible_v;
using std::is_same;
using std::is_same_v;
using std::make_index_sequence;
using std::true_type;
#endif

#if __has_include(<utility>)
using std::forward;
using std::move;
#endif

} // namespace polyfill

// Forward declaration for use inside traits::is_transition
template<typename ToStateID, typename Fn>
struct Transition;

// Forward declaration for use inside traits::is_state and is_known_state_id
template<typename StateID, typename Fn, typename... Transitions>
struct State;

namespace traits {
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

template<typename Fn>
constexpr bool is_valid_work_v = polyfill::is_invocable_v<Fn>;

template<typename Fn>
constexpr bool is_valid_condition_v = polyfill::is_invocable_r_v<bool, Fn>;

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

namespace detail {

template<typename>
struct is_transition_impl : polyfill::false_type {};

template<typename ToStateID, typename Fn>
struct is_transition_impl<Transition<ToStateID, Fn>> : polyfill::true_type {};

template<typename>
struct is_state_impl : polyfill::false_type {};

template<typename StateID, typename Fn, typename... Transitions>
struct is_state_impl<State<StateID, Fn, Transitions...>> : polyfill::true_type {
};

template<typename TargetID, typename... States>
struct is_known_state_id_impl
    : traits::is_in_pack<TargetID, typename States::ID...> {};

template<typename... StateIDs>
struct all_state_ids_unique_impl : polyfill::true_type {};

template<typename Head, typename... Tail>
struct all_state_ids_unique_impl<Head, Tail...>
    : polyfill::integral_constant<
              bool, traits::is_not_in_pack_v<Head, Tail...>
                            && all_state_ids_unique_impl<Tail...>::value> {
    static_assert(
            traits::is_not_in_pack_v<Head, Tail...>,
            "Duplicate state ID detected! See backtrace for offending ID.");
};
} // namespace detail

template<typename TargetID, typename... States>
struct is_known_state_id
    : detail::is_known_state_id_impl<TargetID, polyfill::decay_t<States>...> {};

template<typename TargetID, typename... States>
constexpr bool is_known_state_id_v
        = is_known_state_id<TargetID, States...>::value;

template<typename... States>
struct all_states_unique : detail::all_state_ids_unique_impl<
                                   typename polyfill::decay_t<States>::ID...> {
};

template<typename T>
struct is_transition : detail::is_transition_impl<polyfill::decay_t<T>> {};

template<typename T>
constexpr bool is_transition_v = is_transition<T>::value;

template<typename T>
struct is_state : detail::is_state_impl<polyfill::decay_t<T>> {};

template<typename T>
constexpr bool is_state_v = is_state<T>::value;

namespace detail {
template<typename State, typename... States>
struct transitions_target_known_ids_impl;

template<typename StateID, typename Fn, typename... Transitions,
         typename... States>
struct transitions_target_known_ids_impl<
        SpaceMachine::State<StateID, Fn, Transitions...>, States...>
    : polyfill::integral_constant<
              bool, polyfill::conjunction_v<is_known_state_id<
                            typename Transitions::ToID, States...>...>> {};
} // namespace detail

template<typename State, typename... States>
struct transitions_target_known_ids
    : detail::transitions_target_known_ids_impl<polyfill::decay_t<State>,
                                                States...> {};

template<typename... States>
struct transitions_target_registered_states {
    template<typename State>
    using type = transitions_target_known_ids<State, States...>;
};

} // namespace traits

namespace detail {
template<typename Fn>
struct Callable {
    Fn m_callable;

    template<typename F = Fn,
             typename = polyfill::enable_if_t<polyfill::is_invocable_v<F>>>
    auto operator()() noexcept(noexcept(m_callable())) -> decltype(m_callable())
    { return polyfill::invoke(m_callable); }

    Callable() = delete;
    template<typename F = Fn,
             typename = polyfill::enable_if_t<
                     !traits::is_self_constructing_v<Callable, F>
                     && polyfill::is_invocable_v<F>
                     && polyfill::is_constructible_v<Fn, F>>>
    explicit Callable(F&& callable) noexcept(
            polyfill::is_nothrow_constructible_v<Fn, F&&>)
        : m_callable(polyfill::forward<F>(callable))
    {}
    ~Callable() = default;

    Callable(const Callable&) = default;
    Callable(Callable&&) noexcept = default;
    Callable& operator=(const Callable&) = default;
    Callable& operator=(Callable&&) noexcept = default;

    void* operator new(polyfill::size_t) = delete;
    void operator delete(void*) = delete;
};

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

template<polyfill::size_t Index, typename Type>
struct TypeListNode {
    Type m_value;

    TypeListNode() = delete;
    template<typename T, typename = polyfill::enable_if_t<
                                 !traits::is_self_constructing_v<Type, T>
                                 && polyfill::is_constructible_v<Type, T&&>>>
    explicit TypeListNode(T&& value) noexcept(
            polyfill::is_nothrow_constructible_v<Type, T&&>)
        : m_value(polyfill::forward<T>(value))
    {}
    ~TypeListNode() = default;

    TypeListNode(const TypeListNode&) = default;
    TypeListNode(TypeListNode&&) noexcept = default;
    TypeListNode& operator=(const TypeListNode&) = default;
    TypeListNode& operator=(TypeListNode&&) noexcept = default;

    void* operator new(polyfill::size_t) = delete;
    void operator delete(void*) = delete;
};

template<polyfill::size_t I, typename T>
T& GetValue(TypeListNode<I, T>& node) noexcept
{ return node.m_value; }

template<polyfill::size_t I, typename T>
const T& GetValue(const TypeListNode<I, T>& node) noexcept
{ return node.m_value; }

// NOLINTBEGIN(cppcoreguidelines-rvalue-reference-param-not-moved)
// Clang-tidy will warn that node isn't moved, but that is intentional here.
template<polyfill::size_t I, typename T>
T&& GetValue(TypeListNode<I, T>&& node) noexcept
{ return polyfill::move(node.m_value); }
// NOLINTEND(cppcoreguidelines-rvalue-reference-param-not-moved)

template<typename IndexSequence, typename... Types>
struct TypeListImpl;

// NOLINTBEGIN(misc-multiple-inheritance)
template<polyfill::size_t... Indices, typename... Types>
struct TypeListImpl<polyfill::index_sequence<Indices...>, Types...>
    : TypeListNode<Indices, Types>... {
    TypeListImpl() = delete;
    template<typename... Ts,
             typename = polyfill::enable_if_t<
                     sizeof...(Ts) == sizeof...(Types)
                     && polyfill::conjunction_v<
                             polyfill::is_constructible<Types, Ts&&>...>>>
    explicit TypeListImpl(Ts&&... values) noexcept(
            polyfill::conjunction_v<
                    polyfill::is_nothrow_constructible<Types, Ts&&>...>)
        : TypeListNode<Indices, Types>(polyfill::forward<Ts>(values))...
    {}
    ~TypeListImpl() = default;

    TypeListImpl(const TypeListImpl&) = default;
    TypeListImpl(TypeListImpl&&) noexcept = default;
    TypeListImpl& operator=(const TypeListImpl&) = default;
    TypeListImpl& operator=(TypeListImpl&&) noexcept = default;

    void* operator new(polyfill::size_t) = delete;
    void operator delete(void*) = delete;
};
// NOLINTEND(misc-multiple-inheritance)

template<typename TargetID, polyfill::size_t Index, typename... States>
struct FindStateIndexImpl;

template<typename TargetID, polyfill::size_t Index, typename Current,
         typename... Rest>
struct FindStateIndexImpl<TargetID, Index, Current, Rest...>
    : FindStateIndexImpl<TargetID, Index + 1, Rest...> {};

template<typename TargetID, polyfill::size_t Index, typename Fn,
         typename... Transitions, typename... Rest>
struct FindStateIndexImpl<TargetID, Index,
                          SpaceMachine::State<TargetID, Fn, Transitions...>,
                          Rest...>
    : polyfill::integral_constant<polyfill::size_t, Index> {};

template<typename>
inline constexpr bool always_false_v = false;

template<typename TargetID, polyfill::size_t Index>
struct FindStateIndexImpl<TargetID, Index> {
    // TODO: use something like id_not_found_v<TargetID> for better error
    // message
    static_assert(always_false_v<TargetID>,
                  "State with given target ID not in provided TypeList!");
};
} // namespace detail

template<template<typename> class Trait, typename... Types>
struct for_each_type;

template<template<typename> class Trait>
struct for_each_type<Trait> : polyfill::true_type {};

template<template<typename> class Trait, typename Head, typename... Tail>
struct for_each_type<Trait, Head, Tail...>
    : polyfill::integral_constant<
              bool,
              Trait<Head>::value && for_each_type<Trait, Tail...>::value> {
    static_assert(Trait<Head>::value,
                  "Check failed on type! See template instantiation backtrace "
                  "for offending type.");
};

template<typename... Types>
struct TypeList
    : detail::TypeListImpl<polyfill::make_index_sequence<sizeof...(Types)>,
                           Types...> {
    using Base = detail::TypeListImpl<
            polyfill::make_index_sequence<sizeof...(Types)>, Types...>;
    using Base::Base;

    static constexpr polyfill::size_t size = sizeof...(Types);

    void* operator new(polyfill::size_t) = delete;
    void operator delete(void*) = delete;
};

template<typename ToStateID, typename Fn>
struct Transition {
    using ToID = ToStateID;
    using Condition = detail::Condition<Fn>;

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

template<typename StateID, typename Fn, typename... Transitions>
struct State {
    using ID = StateID;
    using Work = detail::Work<Fn>;

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
                     !polyfill::is_same_v<State, F>
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
                  "Condition must be callable with zero arguments!");
}

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
             typename = polyfill::enable_if_t<polyfill::conjunction_v<
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
                  "offending "
                  "state.");
}

} // namespace SpaceMachine

#endif // SPACEMACHINE_TEMPLATESPACEMACHINE_HPP
