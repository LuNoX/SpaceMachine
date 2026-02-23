//
// Created by timob on 22.02.2026.
//

#ifndef SPACEMACHINE_TEMPLATESPACEMACHINE_HPP
#define SPACEMACHINE_TEMPLATESPACEMACHINE_HPP

#include <type_traits>
#include <utility>

namespace SpaceMachine {

template<typename ToStateID, typename Fn>
struct Transition;

namespace detail {
template<typename Fn>
struct Callable {
    Fn callable;

    template<typename F = Fn,
             typename = std::enable_if_t<std::is_invocable_v<F>>>
    auto operator()() noexcept(noexcept(callable())) -> decltype(callable())
    {
        return callable();
    }

    Callable() = delete;
    template<typename F = Fn,
             typename = std::enable_if_t<
                     std::is_invocable_v<F> && std::is_constructible_v<Fn, F>
                     && !std::is_base_of_v<Callable, std::decay_t<F>>
                     && !std::is_base_of_v<F, std::decay_t<Callable>>>>
    explicit Callable(F&& callable) noexcept(
            std::is_nothrow_constructible_v<Fn, F&&>)
        : callable(std::forward<F>(callable))
    {}
    ~Callable() = default;
    Callable(const Callable&) = default;
    Callable(Callable&&) noexcept = default;
    Callable& operator=(const Callable&) = default;
    Callable& operator=(Callable&&) noexcept = default;

    void* operator new(size_t) = delete;
    void operator delete(void*) = delete;
};
template<typename Fn>
constexpr bool is_valid_work_v = std::is_invocable_v<Fn>;

template<typename Fn>
struct Work : Callable<Fn> {
    static_assert(is_valid_work_v<Fn>,
                  "Work must be callable with zero arguments!");

    using Base = Callable<Fn>;
    using Base::Base;
    using Base::operator();
    using Base::operator new;
    using Base::operator delete;
};

template<typename Fn>
constexpr bool is_valid_condition_v = std::is_invocable_r_v<bool, Fn>;

template<typename Fn>
struct Condition : Callable<Fn> {
    static_assert(is_valid_condition_v<Fn>,
                  "Condition must be callable with zero arguments and "
                  "return bool!");

    using Base = Callable<Fn>;
    using Base::Base;
    using Base::operator();
    using Base::operator new;
    using Base::operator delete;
};

template<typename>
struct is_transition : std::false_type {};

template<typename ToStateID, typename Fn>
struct is_transition<Transition<ToStateID, Fn>> : std::true_type {};

template<typename T>
constexpr bool is_transition_v = is_transition<T>::value;
} // namespace detail

template<typename ToStateID, typename Fn>
struct Transition {
    using To = ToStateID;
    using Condition = detail::Condition<Fn>;

    Condition condition;

    Transition() = delete;
    template<typename F,
             typename
             = std::enable_if_t<!std::is_same_v<std::decay_t<F>, Transition>
                                && std::is_constructible_v<Condition, F&&>>>
    explicit Transition(F&& f) noexcept(
            std::is_nothrow_constructible_v<Condition, F&&>)
        : condition{std::forward<F>(f)}
    {}
    ~Transition() = default;

    Transition(const Transition&) = default;
    Transition(Transition&&) noexcept = default;
    Transition& operator=(const Transition&) = default;
    Transition& operator=(Transition&&) noexcept = default;

    void* operator new(std::size_t) = delete;
    void operator delete(void*) = delete;
};

template<typename ToStateID, typename Fn,
         std::enable_if_t<detail::is_valid_condition_v<Fn>, int> = 0>
Transition<ToStateID, Fn> make_transition(Fn&& condition)
{
    return Transition<ToStateID, Fn>(std::forward<Fn>(condition));
}

template<typename, typename Fn,
         std::enable_if_t<!detail::is_valid_condition_v<Fn>, int> = 0>
auto make_transition(Fn&&)
{
    static_assert(
            detail::is_valid_condition_v<Fn>,
            "Condition must be callable with zero arguments and return bool!");
}

template<typename StateID, typename Fn, typename... Transitions>
struct State {
    using ID = StateID;
    using Work = detail::Work<Fn>;

    static_assert((detail::is_transition_v<Transitions> && ...),
                  "All Transitions must be of type Transition<ToStateID, Fn>!");

    Work work;
    std::tuple<Transitions...> transitions;

    State() = delete;
    template<typename F, typename... Ts,
             typename
             = std::enable_if_t<!std::is_same_v<std::decay_t<F>, Work>
                                && std::is_constructible_v<Work, F&&>
                                && std::conjunction_v<std::is_constructible<
                                        Transitions, Ts&&>...>>>
    explicit State(F&& work, Ts&&... transitions) noexcept(
            std::is_nothrow_constructible_v<Work, F&&>
            && std::conjunction_v<
                    std::is_nothrow_constructible<Transitions, Ts&&>...>)
        : work(std::forward<F>(work)),
          transitions(std::forward<Ts>(transitions)...)
    {}
    ~State() = default;
    State(State&) = default;
    State(State&&) noexcept = default;
    State& operator=(State&) = delete;
    State& operator=(State&&) noexcept = default;

    void* operator new(std::size_t) = delete;
    void operator delete(void*) = delete;
};

} // namespace SpaceMachine

#endif // SPACEMACHINE_TEMPLATESPACEMACHINE_HPP
