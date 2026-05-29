#include "include/spacemachine/SpaceMachine.hpp"
#include <iostream>
#include <random>

void testStateMachine()
{
    using namespace SpaceMachine;
    struct S1 {};
    struct S2 {};
    struct S3 {};
    int a = 1;
    auto l = [&]() {
        a++;
        std::cout << a << std::endl;
    };
    auto s1 = MakeState<S1>([] { std::cout << "State 1:" << std::endl; },
                            MakeTransition<S1>([]() { return true; }));
    auto t2 = MakeTransition<S3>([]() { return true; });
    auto s2 = MakeState<S2>(std::ref(l), t2);
    auto s3 = MakeState<S3>(std::ref(l), t2);
    s1.work();
    s2.work();
    s3.work();
}

int main()
{
    testStateMachine();
    return 0;
}
