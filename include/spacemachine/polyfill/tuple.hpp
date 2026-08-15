#ifndef SPACEMACHINE_POLYFILL_TUPLE_HPP
#define SPACEMACHINE_POLYFILL_TUPLE_HPP

#if __has_include(<tuple>)
#include <tuple>
#endif

namespace spacemachine::polyfill {

#if __has_include(<tuple>)
using std::tuple;
#endif

} // namespace spacemachine::polyfill

#endif // SPACEMACHINE_POLYFILL_TUPLE_HPP
