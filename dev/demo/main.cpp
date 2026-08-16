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

    auto s1 = MakeState<S1>([] { std::cout << "State 1:" << '\n'; },
                            MakeTransition<S1>([]() { return true; }));
    auto t2 = MakeTransition<S3>([]() { return true; });
    auto work = [&]() { std::cout << ++a << '\n'; };
    auto s2 = MakeState<S2>(std::ref(work), t2);
    auto s3 = MakeState<S3>(std::ref(work), t2);

    s1.m_work();
    s2.m_work();
    s3.m_work();

    auto sm = MakeStateMachine<S1>(s1, s2, s3);
    (void)sm; // silence unused variable warning

    return 0;
}

// NOLINTEND(readability-identifier-length)
