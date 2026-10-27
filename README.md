# SpaceMachine

A C++ 17 and newer, header only, high performance, zero runtime cost, finite state machine designed for aerospace applications. 

## Status

SpaceMachine is at version 0.1.0 and under active development, so the API may still change. Planned work is tracked in [TODO.md](TODO.md).

## Example

```cpp
#include <iostream>
#include <spacemachine/SpaceMachine.hpp>

// State IDs are plain tag types.
struct Off {};
struct Firing {};
struct Cooling {};

int main()
{
    using namespace spacemachine;

    bool fireCommanded = true;
    int temperature = 0;
    auto work = [&](const char* state, int change) {
        temperature += change;
        std::cout << state << ", temperature " << temperature << '\n';
    };

    // Declare transitions.
    auto ignite = MakeTransition<Firing>([&] { return fireCommanded; });
    auto overheat = MakeTransition<Cooling>([&] { return temperature >= 100; });
    auto shutDown = MakeTransition<Off>([&] { return !fireCommanded; });

    // Declare states.
    auto firing = MakeState<Firing>(
            [&] { work("Firing", 50); },
            // Transitions are checked in declaration order, so overheat wins.
            overheat, shutDown);
    auto cooling = MakeState<Cooling>(
            [&] { work("Cooling", -25); },
            // Transitions can also be declared inline.
            MakeTransition<Off>([&] { return temperature <= 0; }));

    // Declare the state machine.
    auto thruster = MakeStateMachine<Off>(
            // Just like transitions, states can also be declared inline.
            MakeState<Off>([&] { work("Off", 0); }, ignite), firing, cooling);

    for (int step = 0; step < 7; ++step) { thruster.Step(); }

    std::cout << "\nThruster is off?: " << std::boolalpha
              << thruster.IsActive<Off>() << '\n';
    return 0;
}
```

Running it prints:

```text
Firing, temperature 50
Firing, temperature 100
Cooling, temperature 75
Cooling, temperature 50
Cooling, temperature 25
Cooling, temperature 0
Off, temperature 0

Thruster is off?: true
```

## Requirements

- A C++17 compiler.
- No dependencies. The standard library is optional, see [Freestanding and embedded use](#freestanding-and-embedded-use).
- CMake 3.21 or newer, if you consume SpaceMachine through CMake.

## Integration

SpaceMachine is header only and provides the CMake target `SpaceMachine::SpaceMachine`.

### FetchContent

```cmake
include(FetchContent)
FetchContent_Declare(
    SpaceMachine
    GIT_REPOSITORY https://github.com/LuNoX/SpaceMachine.git
    GIT_TAG main
)
FetchContent_MakeAvailable(SpaceMachine)

target_link_libraries(your_target PRIVATE SpaceMachine::SpaceMachine)
```

Pin `GIT_TAG` to a commit hash for reproducible builds. `add_subdirectory(path/to/SpaceMachine)` works the same way. As a subproject, SpaceMachine adds no dev targets and, by default, no install rules, see [CMake options](#cmake-options).

### Installed package

Install the headers and CMake package files:

```sh
cmake -S . -B build
cmake --install build --prefix <install prefix>
```

Then find the package in your project, with the install prefix in `CMAKE_PREFIX_PATH`:

```cmake
find_package(SpaceMachine 0.1 REQUIRED)
target_link_libraries(your_target PRIVATE SpaceMachine::SpaceMachine)
```

### CMake options

| Option | Default | Effect |
| --- | --- | --- |
| `SPACEMACHINE_INSTALL` | `ON` when SpaceMachine is the top-level project, otherwise `OFF` | Generates install rules and the package files for `find_package`. |
| `SPACEMACHINE_DEV_MODE` | `OFF` | Adds the demo and the dev checks. Has no effect when SpaceMachine is a subproject. See [CONTRIBUTING.md](CONTRIBUTING.md#cmake-options) for this and the other dev options. |

### Without CMake

Copy [include/](include/) into your project and add it to your include path.

## How it works

SpaceMachine is built from three class templates in the `spacemachine` namespace: `Transition`, `State` and `StateMachine`. Each has a `Make` function, for easy construction. Every state, work and condition keeps its own type, so there is no type erasure and no virtual dispatch.

### Transition

`Transition<ToStateID, Fn>` moves the state machine to the state `ToStateID` when its condition returns `true`. The condition, of type `Fn`, is a callable that takes no arguments and returns something convertible to `bool`.

```cpp
auto overheat = MakeTransition<Cooling>([&] { return temperature >= 100; });
```

### State

`State<StateID, Fn, Transitions...>` binds a state ID to its work and its outgoing transitions.

- The state ID is any type, usually an empty tag struct. It only names the state.
- The work, of type `Fn`, is a callable that takes no arguments. It runs on every step that ends in this state.
- The transitions are checked in declaration order, see [Stepping](#stepping).

```cpp
auto cooling = MakeState<Cooling>(
        [&] { work("Cooling", -25); },
        MakeTransition<Off>([&] { return temperature <= 0; }));
```

### StateMachine

`StateMachine<InitialStateID, States...>` holds its states by value and tracks which one is active, starting with `InitialStateID`. `MakeStateMachine` also checks at compile time that the initial state exists, that no two states share an ID, and that every transition leads to one of the states.

```cpp
auto thruster = MakeStateMachine<Off>(off, firing, cooling);
```

It has two member functions:

- `Step()` advances the state machine by one step, see [Stepping](#stepping).
- `IsActive<StateID>()` returns whether `StateID` is the active state.

### Stepping

Each call to `Step()`:

1. Checks the active state's transitions in declaration order. The first condition that returns `true` wins, and later conditions aren't evaluated.
2. If a transition triggered, makes its target the active state and runs the target's work. Otherwise, runs the active state's own work.

This means:

- At most one transition happens per step. The new state's transitions are first checked on the next step.
- Only the active state's conditions and work run.
- To give a transition priority, declare it earlier.
- A transition from a state to itself runs that state's work and keeps it active.

### Copies and lifetimes

States, transitions and state machines are copyable and movable, and their work and conditions are copied with them. A lambda that captures by reference still refers to the original objects after a copy, so keeping those objects alive is up to you. To share one callable between states, pass it as `std::ref(callable)`.

### Memory

SpaceMachine never allocates. Every type deletes `operator new`.

State machines often live for the whole program. If none of a state machine's work and conditions refer to local variables, for example because they only use static data, it can be declared `static` too. That keeps it off the stack and gives it a fixed address, which may improve cache locality.

## Freestanding and embedded use

SpaceMachine doesn't throw or use run-time type information, so it builds with exceptions and RTTI disabled.

From the standard library, it only uses `<cstddef>`, `<type_traits>` and `<utility>`. When one of them is unavailable, it falls back to its own implementation in [include/spacemachine/polyfill/](include/spacemachine/polyfill/). To force a fallback even when the standard header exists, define the matching macro:

- `SPACEMACHINE_POLYFILL_NO_STD_CSTDDEF`
- `SPACEMACHINE_POLYFILL_NO_STD_TYPE_TRAITS`
- `SPACEMACHINE_POLYFILL_NO_STD_UTILITY`

For example, to use SpaceMachine's own `<type_traits>` in a CMake project:

```cmake
target_compile_definitions(
    your_target PRIVATE SPACEMACHINE_POLYFILL_NO_STD_TYPE_TRAITS
)
```

With other build systems, pass `-DSPACEMACHINE_POLYFILL_NO_STD_TYPE_TRAITS` to the compiler. Alternatively, define it in a wrapper header and include that instead of `<spacemachine/SpaceMachine.hpp>`:

```cpp
// my_spacemachine.hpp
#define SPACEMACHINE_POLYFILL_NO_STD_TYPE_TRAITS
// Include after the define, otherwise SpaceMachine cannot see it.
#include <spacemachine/SpaceMachine.hpp>
```

Every translation unit that includes SpaceMachine must use the same macros, so prefer a wrapper header over defining the macros before each include.

### Custom implementations

To use your own implementations instead, for example from your platform's support library, define the polyfill header's include guard and provide the same names in the `spacemachine::polyfill` namespace before including SpaceMachine:

- `SPACEMACHINE_POLYFILL_CSTDDEF_HPP`
- `SPACEMACHINE_POLYFILL_TYPE_TRAITS_HPP`
- `SPACEMACHINE_POLYFILL_UTILITY_HPP`

The names to provide are the ones the matching header in [include/spacemachine/polyfill/](include/spacemachine/polyfill/) declares. Put the replacement in a wrapper header and include it instead of `<spacemachine/SpaceMachine.hpp>`, so every translation unit uses the same implementations:

```cpp
// my_spacemachine.hpp
#define SPACEMACHINE_POLYFILL_UTILITY_HPP
#include "my_utility.hpp"

namespace spacemachine::polyfill {
using my::forward;
using my::index_sequence;
using my::make_index_sequence;
using my::move;
} // namespace spacemachine::polyfill

#include <spacemachine/SpaceMachine.hpp>
```

## License

MIT, see [LICENSE](LICENSE).
