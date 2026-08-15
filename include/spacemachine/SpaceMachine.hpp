//
// Created by timob on 22.02.2026.
//

#ifndef SPACEMACHINE_HPP
#define SPACEMACHINE_HPP

#include "spacemachine/statemachine/State.hpp"
#include "spacemachine/statemachine/StateMachine.hpp"
#include "spacemachine/statemachine/Transition.hpp"

namespace spacemachine {
using statemachine::State;
using statemachine::StateMachine;
using statemachine::Transition;

using statemachine::MakeState;
using statemachine::MakeStateMachine;
using statemachine::MakeTransition;
} // namespace spacemachine

#endif // SPACEMACHINE_TEMPLATESPACEMACHINE_HPP
