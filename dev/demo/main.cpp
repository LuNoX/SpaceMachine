#include <iostream>
#include <spacemachine/SpaceMachine.hpp>

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
// NOLINTBEGIN(misc-use-internal-linkage)
// This is the README example, kept as simple as possible for readers.

// State IDs are plain tag types.
struct Off {};
struct Firing {};
struct Cooling {};
// NOLINTEND(misc-use-internal-linkage)

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

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
