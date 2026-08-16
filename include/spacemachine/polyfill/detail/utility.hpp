#ifndef SPACEMACHINE_POLYFILL_DETAIL_UTILITY_HPP
#define SPACEMACHINE_POLYFILL_DETAIL_UTILITY_HPP

#include "spacemachine/polyfill/cstddef.hpp"
#include "spacemachine/polyfill/type_traits.hpp"

namespace spacemachine::polyfill::detail {

template<typename T>
constexpr T&& forward(polyfill::remove_reference_t<T>& arg) noexcept
{ return static_cast<T&&>(arg); }

// NOLINTBEGIN(cppcoreguidelines-rvalue-reference-param-not-moved)
// Clang-tidy will warn that arg isn't moved, but that is intentional here.
template<typename T>
constexpr T&& forward(polyfill::remove_reference_t<T>&& arg) noexcept
{
    static_assert(!polyfill::is_lvalue_reference_v<T>,
                  "Cannot forward an rvalue as an lvalue!");
    return static_cast<T&&>(arg);
}
// NOLINTEND(cppcoreguidelines-rvalue-reference-param-not-moved)

template<typename T>
constexpr polyfill::remove_reference_t<T>&& move(T&& arg) noexcept
{ return static_cast<polyfill::remove_reference_t<T>&&>(arg); }

// We only need the size_t flavor for TypeList, so the other's are left out.
template<polyfill::size_t... Indices>
struct index_sequence {};

namespace impl {

template<typename First, typename Second>
struct ConcatIndexSequence;

template<polyfill::size_t... FirstIndices, polyfill::size_t... SecondIndices>
struct ConcatIndexSequence<index_sequence<FirstIndices...>,
                           index_sequence<SecondIndices...>> {
    using type = index_sequence<FirstIndices...,
                                (sizeof...(FirstIndices) + SecondIndices)...>;
};

template<polyfill::size_t Size>
struct MakeIndexSequence {
    using type = typename ConcatIndexSequence<
            typename MakeIndexSequence<Size / 2>::type,
            typename MakeIndexSequence<Size - (Size / 2)>::type>::type;
};

template<>
struct MakeIndexSequence<0> {
    using type = index_sequence<>;
};

template<>
struct MakeIndexSequence<1> {
    using type = index_sequence<0>;
};

} // namespace impl

template<polyfill::size_t Size>
using make_index_sequence = typename impl::MakeIndexSequence<Size>::type;

} // namespace spacemachine::polyfill::detail

#endif // SPACEMACHINE_POLYFILL_DETAIL_UTILITY_HPP
