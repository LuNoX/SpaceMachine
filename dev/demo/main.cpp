#include <functional>
#include <iostream>
#include <spacemachine/SpaceMachine.hpp>

// NOLINTBEGIN(readability-identifier-length)
// Clang-tidy will warn about short identifiers, but they are readable here.
namespace {
struct S1 {};
struct S2 {};
struct S3 {};
} // namespace

int main()
{
    using namespace spacemachine;
    int a = 1;
    bool workedState1 = false;

    auto s1 = MakeState<S1>([] { std::cout << "State 1:" << '\n'; },
                            MakeTransition<S2>([&workedState1]() {
                                if (workedState1) { return true; }
                                workedState1 = true;
                                return false;
                            }));
    auto t2 = MakeTransition<S3>([]() { return true; });
    auto work = [&]() { std::cout << ++a << '\n'; };
    auto s2 = MakeState<S2>(std::ref(work), t2);
    auto s3 = MakeState<S3>(std::ref(work), t2);

    auto sm = MakeStateMachine<S1>(s1, s2, s3);
    for (int step = 0; step < 3; ++step) { sm.Step(); }

    return 0;
}

// NOLINTEND(readability-identifier-length)
