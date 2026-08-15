#ifndef SPACEMACHINE_POLYFILL_FUNCTIONAL_HPP
#define SPACEMACHINE_POLYFILL_FUNCTIONAL_HPP

#if __has_include(<functional>)
#include <functional>
#endif

namespace spacemachine::polyfill {

#if __has_include(<functional>)
using std::invoke;
#endif

} // namespace spacemachine::polyfill

#endif // SPACEMACHINE_POLYFILL_FUNCTIONAL_HPP
