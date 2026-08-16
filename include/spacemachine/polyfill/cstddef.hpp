#ifndef SPACEMACHINE_POLYFILL_CSTDDEF_HPP
#define SPACEMACHINE_POLYFILL_CSTDDEF_HPP

#if __has_include(<cstddef>) && !defined(SPACEMACHINE_POLYFILL_NO_STD_CSTDDEF)
#include <cstddef>
#endif

namespace spacemachine::polyfill {

#if __has_include(<cstddef>) && !defined(SPACEMACHINE_POLYFILL_NO_STD_CSTDDEF)
using std::size_t;
#else
using size_t = decltype(sizeof(0));
#endif

} // namespace spacemachine::polyfill

#endif // SPACEMACHINE_POLYFILL_CSTDDEF_HPP
