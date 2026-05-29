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

template<typename ToStateID, typename Fn>
struct Transition;

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
using std::is_same_v;
using std::make_index_sequence;
using std::true_type;
#endif

#if __has_include(<utility>)
using std::forward;
using std::move;
#endif

} // namespace polyfill

namespace traits {
template<typename Fn>
constexpr bool is_valid_work_v = polyfill::is_invocable_v<Fn>;

template<typename Fn>
constexpr bool is_valid_condition_v = polyfill::is_invocable_r_v<bool, Fn>;

template<typename>
struct is_transition : polyfill::false_type {};

template<typename ToStateID, typename Fn>
struct is_transition<Transition<ToStateID, Fn>> : polyfill::true_type {};

template<typename T>
constexpr bool is_transition_v = is_transition<T>::value;

template<typename, typename...>
struct is_self_constructing : polyfill::false_type {};

template<typename Self, typename T>
struct is_self_constructing<Self, T>
    : polyfill::is_base_of<Self, polyfill::decay_t<T>> {};

template<typename Self, typename T>
constexpr bool is_self_constructing_v = is_self_constructing<Self, T>::value;
} // namespace traits

namespace detail {

template<typename Fn>
struct Callable {
    Fn callable;

    template<typename F = Fn,
             typename = polyfill::enable_if_t<polyfill::is_invocable_v<F>>>
    auto operator()() noexcept(noexcept(callable())) -> decltype(callable())
    {
        return polyfill::invoke(callable);
    }

    Callable() = delete;
    template<typename F = Fn,
             typename = polyfill::enable_if_t<
                     !traits::is_self_constructing_v<Callable, F>
                     && polyfill::is_invocable_v<F>
                     && polyfill::is_constructible_v<Fn, F>>>
    explicit Callable(F&& callable) noexcept( // NOSONAR
            polyfill::is_nothrow_constructible_v<Fn, F&&>)
        : callable(polyfill::forward<F>(callable))
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
    Type value;

    TypeListNode() = delete;
    template<typename T, typename = polyfill::enable_if_t<
                                 !traits::is_self_constructing_v<Type, T>
                                 && polyfill::is_constructible_v<Type, T&&>>>
    explicit TypeListNode(T&& v) noexcept( // NOSONAR
            polyfill::is_nothrow_constructible_v<Type, T&&>)
        : value(polyfill::forward<T>(v))
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
{
    return node.value;
}

template<polyfill::size_t I, typename T>
const T& GetValue(const TypeListNode<I, T>& node) noexcept
{
    return node.value;
}

template<polyfill::size_t I, typename T>
T&& GetValue(TypeListNode<I, T>&& node) noexcept
{
    return polyfill::move(node.value);
}

template<typename IndexSequence, typename... Types>
struct TypeListImpl;

template<polyfill::size_t... Indices, typename... Types>
struct TypeListImpl<polyfill::index_sequence<Indices...>, Types...>
    : TypeListNode<Indices, Types>... {
    TypeListImpl() = delete;
    template<typename... Ts,
             typename = polyfill::enable_if_t<
                     !traits::is_self_constructing_v<TypeListImpl, Ts...>
                     && sizeof...(Ts) == sizeof...(Types)
                     && polyfill::conjunction_v<
                             polyfill::is_constructible<Types, Ts&&>...>>>
    explicit TypeListImpl(Ts&&... vs) noexcept(
            polyfill::conjunction_v<
                    polyfill::is_nothrow_constructible<Types, Ts&&>...>)
        : TypeListNode<Indices, Types>(polyfill::forward<Ts>(vs))...
    {}
    ~TypeListImpl() = default;

    TypeListImpl(const TypeListImpl&) = default;
    TypeListImpl(TypeListImpl&&) noexcept = default;
    TypeListImpl& operator=(const TypeListImpl&) = default;
    TypeListImpl& operator=(TypeListImpl&&) noexcept = default;

    void* operator new(polyfill::size_t) = delete;
    void operator delete(void*) = delete;
};

template<typename TargetID, std::size_t Index, typename... States>
struct FindStateIndexImpl;

template<typename TargetID, std::size_t Index, typename Current,
         typename... Rest>
struct FindStateIndexImpl<TargetID, Index, Current, Rest...> {
    static constexpr std::size_t value
            = std::is_same_v<typename Current::ID, TargetID>
                      ? Index
                      : FindStateIndexImpl<TargetID, Index + 1, Rest...>::value;
};

template<typename>
inline constexpr bool always_false_v = false;

template<typename TargetID, std::size_t Index>
struct FindStateIndexImpl<TargetID, Index> {
    static_assert(always_false_v<TargetID>,
                  "State with given target ID not in provided TypeList!");
};

} // namespace detail

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
    using To = ToStateID;
    using Condition = detail::Condition<Fn>;

    Condition shouldTrigger;

    Transition() = delete;
    template<typename F,
             typename = polyfill::enable_if_t<
                     traits::is_self_constructing_v<Transition, F>
                     && polyfill::is_constructible_v<Condition, F&&>>>
    explicit Transition(F&& shouldTrigger) noexcept( // NOSONAR
            polyfill::is_nothrow_constructible_v<Condition, F&&>)
        : shouldTrigger{polyfill::forward<F>(shouldTrigger)}
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
Transition<ToStateID, polyfill::decay_t<Fn>> MakeTransition(Fn&& shouldTrigger)
{
    return Transition<ToStateID, polyfill::decay_t<Fn>>(
            polyfill::forward<Fn>(shouldTrigger));
}

template<typename, typename Fn,
         polyfill::enable_if_t<!traits::is_valid_condition_v<Fn>, int> = 0>
auto MakeTransition(Fn&&)
{
    static_assert(
            traits::is_valid_condition_v<Fn>,
            "Condition must be callable with zero arguments and return bool!");
}

template<typename StateID, typename Fn, typename... Transitions>
struct State {
    using ID = StateID;
    using Work = detail::Work<Fn>;

    static_assert(
            polyfill::conjunction_v<traits::is_transition<Transitions>...>,
            "All Transitions must be of type Transition<ToStateID, Fn>!");

    Work work;
    TypeList<Transitions...> transitions;

    State() = delete;
    template<typename F, typename... Ts,
             typename = polyfill::enable_if_t<
                     !polyfill::is_same_v<State, polyfill::decay_t<F>>
                     && polyfill::is_constructible_v<Work, F&&>
                     && polyfill::conjunction_v<
                             polyfill::is_constructible<Transitions, Ts&&>...>>>
    explicit State(F&& work, Ts&&... transitions) noexcept(
            polyfill::is_nothrow_constructible_v<Work, F&&>
            && polyfill::conjunction_v<
                    polyfill::is_nothrow_constructible<Transitions, Ts&&>...>)
        : work(polyfill::forward<F>(work)),
          transitions(polyfill::forward<Ts>(transitions)...)
    {}
    ~State() = default;
    State(State&) = default;
    State(State&&) noexcept = default;
    State& operator=(State&) = delete;
    State& operator=(State&&) noexcept = default;

    void* operator new(polyfill::size_t) = delete;
    void operator delete(void*) = delete;
};

template<typename StateID, typename Fn, typename... Transitions,
         polyfill::enable_if_t<traits::is_valid_work_v<Fn>, int> = 0>
State<StateID, polyfill::decay_t<Fn>, polyfill::decay_t<Transitions>...>
MakeState(Fn&& work, Transitions&&... transitions)
{
    return State<StateID, polyfill::decay_t<Fn>,
                 polyfill::decay_t<Transitions>...>(
            polyfill::forward<Fn>(work),
            polyfill::forward<Transitions>(transitions)...);
}

template<typename, typename Fn, typename... Transitions,
         polyfill::enable_if_t<!traits::is_valid_work_v<Fn>, int> = 0>
auto MakeState(Fn&&, Transitions&&...)
{
    static_assert(traits::is_valid_work_v<Fn>,
                  "Condition must be callable with zero arguments!");
}

template<typename InitialStateID, typename... States>
struct StateMachine {
    TypeList<States...> states;
    polyfill::size_t activeStateIndex;
};

} // namespace SpaceMachine

#endif // SPACEMACHINE_TEMPLATESPACEMACHINE_HPP
