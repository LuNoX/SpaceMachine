#include "include/spacemachine/SpaceMachine.hpp"
#include <iostream>
#include <random>

int main()
{
    SpaceMachine::StateMachine stateMachine;
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
    return 0;
}
