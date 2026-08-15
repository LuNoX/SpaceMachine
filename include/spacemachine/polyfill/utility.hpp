#ifndef SPACEMACHINE_POLYFILL_UTILITY_HPP
#define SPACEMACHINE_POLYFILL_UTILITY_HPP

#if __has_include(<utility>)
#include <utility>
#endif

namespace spacemachine::polyfill {

#if __has_include(<utility>)
using std::forward;
using std::index_sequence;
using std::make_index_sequence;
using std::move;
#endif

} // namespace spacemachine::polyfill

#endif // SPACEMACHINE_POLYFILL_UTILITY_HPP
