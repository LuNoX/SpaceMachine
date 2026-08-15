#ifndef SPACEMACHINE_TYPELIST_HPP
#define SPACEMACHINE_TYPELIST_HPP

#include "spacemachine/polyfill/cstddef.hpp"
#include "spacemachine/polyfill/type_traits.hpp"
#include "spacemachine/polyfill/utility.hpp"
#include "spacemachine/statemachine/fwd/State.hpp"
#include "spacemachine/statemachine/traits/IsSelfConstructing.hpp"

namespace spacemachine::statemachine {

namespace detail {

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
                     !traits::is_constructing_from_self_v<TypeListImpl, Ts...>
                     && sizeof...(Ts) == sizeof...(Types)
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
struct FindStateIndexImpl<TargetID, Index, State<TargetID, Fn, Transitions...>,
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
} // namespace spacemachine::statemachine

#endif // SPACEMACHINE_TYPELIST_HPP
