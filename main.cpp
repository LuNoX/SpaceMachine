#include "include/spacemachine/SpaceMachine.hpp"
#include "include/spacemachine/TemplateSpaceMachine.hpp"
#include <iostream>
#include <random>

void testRuntimeStateMachine()
{
    alignas(64) static SpaceMachine::StateMachine stateMachine;
    {
        SpaceMachine::StateMachineBuilder builder(stateMachine);
        const auto& state1
                = builder.createState([]() { std::cout << "State 1: "; });
        const auto& state2
                = builder.createState([]() { std::cout << "State 2: "; });
        const auto& state3
                = builder.createState([]() { std::cout << "State 3: "; });
        const auto& state4
                = builder.createState([]() { std::cout << "State 4: "; });
        const auto& state5
                = builder.createState([]() { std::cout << "State 5: "; });
        const auto& state6
                = builder.createState([]() { std::cout << "State 6: "; });
        const auto& state7
                = builder.createState([]() { std::cout << "State 7: "; });
        const auto& state8 = builder.createState(
                []() { throw std::runtime_error("Terminate"); });

        std::random_device rd; // Will be used to obtain a seed for the random
                               // number engine
        std::mt19937 gen(
                rd()); // Standard mersenne_twister_engine seeded with rd()
        std::uniform_int_distribution dis(1, 100); //
        auto shouldTrigger = [&] {
            auto roll = dis(gen);
            std::cout << "Roll: " << roll << std::endl;
            if (roll != 1) return false;
            std::cout << "Transition triggered! Switching state." << std::endl;
            return true;
        };
        builder.createTransition(state1, state2, shouldTrigger);
        builder.createTransition(state2, state3, shouldTrigger);
        builder.createTransition(state3, state4, shouldTrigger);
        builder.createTransition(state4, state5, shouldTrigger);
        builder.createTransition(state5, state6, shouldTrigger);
        builder.createTransition(state6, state7, shouldTrigger);
        builder.createTransition(state7, state8, shouldTrigger);
        builder.setInitialState(state1);
        builder.build();
    }

    while (true) stateMachine.run();
}

void testCompileTimeStateMachine()
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
    auto s1 = make_state<S1>([] { std::cout << "State 1:" << std::endl; },
                             make_transition<S1>([]() { return true; }));
    auto t2 = make_transition<S3>([]() { return true; });
    auto s2 = make_state<S2>(std::ref(l), t2);
    auto s3 = make_state<S3>(std::ref(l), t2);
    s1.work();
    s2.work();
    s3.work();
}

int main()
{
    testCompileTimeStateMachine();
    // testRuntimeStateMachine();
    return 0;
}
