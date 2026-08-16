#ifndef SPACEMACHINE_POLYFILL_UTILITY_HPP
#define SPACEMACHINE_POLYFILL_UTILITY_HPP

#if __has_include(<utility>)                                                   \
        && !defined(SPACEMACHINE_POLYFILL_NO_STD_UTILITY)
#include <utility>
#else
#include "spacemachine/polyfill/detail/utility.hpp"
#endif

namespace spacemachine::polyfill {

#if __has_include(<utility>)                                                   \
        && !defined(SPACEMACHINE_POLYFILL_NO_STD_UTILITY)
using std::forward;
using std::index_sequence;
using std::make_index_sequence;
using std::move;
#else
using detail::forward;
using detail::index_sequence;
using detail::make_index_sequence;
using detail::move;
#endif

} // namespace spacemachine::polyfill

#endif // SPACEMACHINE_POLYFILL_UTILITY_HPP
