#ifndef SPACEMACHINE_POLYFILL_DETAIL_TYPETRAITS_HPP
#define SPACEMACHINE_POLYFILL_DETAIL_TYPETRAITS_HPP

#include "spacemachine/polyfill/cstddef.hpp"

namespace spacemachine::polyfill::detail {

template<typename T, T ConstantValue>
struct integral_constant {
    using value_type = T;
    using type = integral_constant;

    static constexpr T value = ConstantValue;
};

template<bool ConstantValue>
using bool_constant = integral_constant<bool, ConstantValue>;

using true_type = bool_constant<true>;
using false_type = bool_constant<false>;

template<typename...>
using void_t = void;

template<bool Condition, typename IfTrue, typename IfFalse>
struct conditional {
    using type = IfTrue;
};

template<typename IfTrue, typename IfFalse>
struct conditional<false, IfTrue, IfFalse> {
    using type = IfFalse;
};

template<bool Condition, typename IfTrue, typename IfFalse>
using conditional_t = typename conditional<Condition, IfTrue, IfFalse>::type;

template<bool Condition, typename T = void>
struct enable_if {};

template<typename T>
struct enable_if<true, T> {
    using type = T;
};

template<bool Condition, typename T = void>
using enable_if_t = typename enable_if<Condition, T>::type;

template<typename T>
T&& declval() noexcept;

template<typename>
struct is_const : false_type {};

template<typename T>
struct is_const<const T> : true_type {};

template<typename>
struct is_reference : false_type {};

template<typename T>
struct is_reference<T&> : true_type {};

template<typename T>
struct is_reference<T&&> : true_type {};

template<typename>
struct is_lvalue_reference : false_type {};

template<typename T>
struct is_lvalue_reference<T&> : true_type {};

template<typename T>
constexpr bool is_lvalue_reference_v = is_lvalue_reference<T>::value;

template<typename T>
struct remove_reference {
    using type = T;
};

template<typename T>
struct remove_reference<T&> {
    using type = T;
};

template<typename T>
struct remove_reference<T&&> {
    using type = T;
};

template<typename T>
using remove_reference_t = typename remove_reference<T>::type;

template<typename T>
struct remove_cv {
    using type = T;
};

template<typename T>
struct remove_cv<const T> {
    using type = T;
};

template<typename T>
struct remove_cv<volatile T> {
    using type = T;
};

template<typename T>
struct remove_cv<const volatile T> {
    using type = T;
};

template<typename T>
using remove_cv_t = typename remove_cv<T>::type;

// NOLINTBEGIN(*-avoid-c-arrays)
template<typename>
struct is_array : false_type {};

template<typename T>
struct is_array<T[]> : true_type {};

template<typename T, size_t Extent>
struct is_array<T[Extent]> : true_type {};

template<typename T>
struct remove_extent {
    using type = T;
};

template<typename T>
struct remove_extent<T[]> {
    using type = T;
};

template<typename T, size_t Extent>
struct remove_extent<T[Extent]> {
    using type = T;
};
// NOLINTEND(*-avoid-c-arrays)

template<typename T>
using remove_extent_t = typename remove_extent<T>::type;

template<typename T>
struct is_function
    : bool_constant<!is_const<const T>::value && !is_reference<T>::value> {};

template<typename, typename>
struct is_same : false_type {};

template<typename T>
struct is_same<T, T> : true_type {};

template<typename T, typename U>
constexpr bool is_same_v = is_same<T, U>::value;

template<typename Base, typename Derived>
struct is_base_of : bool_constant<__is_base_of(Base, Derived)> {};

template<typename Base, typename Derived>
constexpr bool is_base_of_v = is_base_of<Base, Derived>::value;

template<typename T, typename... Args>
struct is_constructible : bool_constant<__is_constructible(T, Args...)> {};

template<typename T, typename... Args>
constexpr bool is_constructible_v = is_constructible<T, Args...>::value;

template<typename T, typename... Args>
struct is_nothrow_constructible
    : bool_constant<__is_nothrow_constructible(T, Args...)> {};

template<typename T, typename... Args>
constexpr bool is_nothrow_constructible_v
        = is_nothrow_constructible<T, Args...>::value;

namespace impl {

template<typename To>
void ConvertibleSink(To) noexcept;

template<typename From, typename To, typename = void>
struct IsConvertible : false_type {};

template<typename From, typename To>
struct IsConvertible<From, To,
                     void_t<decltype(ConvertibleSink<To>(declval<From>()))>>
    : true_type {};

} // namespace impl

template<typename From, typename To>
struct is_convertible : impl::IsConvertible<From, To> {};

namespace impl {

template<typename T, bool IsArray = is_array<T>::value,
         bool IsFunction = is_function<T>::value>
struct Decay {
    using type = remove_cv_t<T>;
};

template<typename T>
struct Decay<T, true, false> {
    using type = remove_extent_t<T>*;
};

template<typename T>
struct Decay<T, false, true> {
    using type = T*;
};

} // namespace impl

template<typename T>
struct decay {
    using type = typename impl::Decay<remove_reference_t<T>>::type;
};

template<typename T>
using decay_t = typename decay<T>::type;

template<typename...>
struct conjunction : true_type {};

template<typename First>
struct conjunction<First> : First {};

template<typename First, typename... Rest>
struct conjunction<First, Rest...>
    : conditional_t<bool(First::value), conjunction<Rest...>, First> {};

template<typename... Bs>
constexpr bool conjunction_v = conjunction<Bs...>::value;

template<typename...>
struct disjunction : false_type {};

template<typename First>
struct disjunction<First> : First {};

template<typename First, typename... Rest>
struct disjunction<First, Rest...>
    : conditional_t<bool(First::value), First, disjunction<Rest...>> {};

template<typename... Bs>
constexpr bool disjunction_v = disjunction<Bs...>::value;

namespace impl {

template<typename Void, typename Callable>
struct IsCallable : false_type {};

template<typename Callable>
struct IsCallable<void_t<decltype(declval<Callable>()())>, Callable>
    : true_type {};

template<typename Void, typename Result, typename Callable>
struct IsCallableR : false_type {};

template<typename Result, typename Callable>
struct IsCallableR<void_t<decltype(declval<Callable>()())>, Result, Callable>
    : bool_constant<is_same<remove_cv_t<Result>, void>::value
                    || is_convertible<decltype(declval<Callable>()()),
                                      Result>::value> {};

} // namespace impl

template<typename Callable, typename... Args>
struct is_callable : impl::IsCallable<void, Callable> {
    static_assert(sizeof...(Args) == 0,
                  "polyfill::is_callable only models zero-argument calls. "
                  "Implement the standard's INVOKE in the polyfill if work or "
                  "conditions ever need to take arguments.");
};

template<typename Callable, typename... Args>
constexpr bool is_callable_v = is_callable<Callable, Args...>::value;

template<typename Result, typename Callable, typename... Args>
struct is_callable_r : impl::IsCallableR<void, Result, Callable> {
    static_assert(sizeof...(Args) == 0,
                  "polyfill::is_callable_r only models zero-argument calls. "
                  "Implement the standard's INVOKE in the polyfill if work or "
                  "conditions ever need to take arguments.");
};

template<typename Result, typename Callable, typename... Args>
constexpr bool is_callable_r_v
        = is_callable_r<Result, Callable, Args...>::value;

} // namespace spacemachine::polyfill::detail

#endif // SPACEMACHINE_POLYFILL_DETAIL_TYPETRAITS_HPP
