#ifndef SPACEMACHINE_CALLABLE_HPP
#define SPACEMACHINE_CALLABLE_HPP

#include "spacemachine/polyfill/cstddef.hpp"
#include "spacemachine/polyfill/functional.hpp"
#include "spacemachine/polyfill/type_traits.hpp"
#include "spacemachine/polyfill/utility.hpp"
#include "spacemachine/statemachine/traits/IsSelfConstructing.hpp"

namespace spacemachine::statemachine::callable {

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

} // namespace spacemachine::statemachine::callable

#endif // SPACEMACHINE_CALLABLE_HPP
