//
// Created by timob on 19.02.2026.
//

#ifndef SPACEMACHINE_SPACEMACHINE_HPP
#define SPACEMACHINE_SPACEMACHINE_HPP

#if __has_include(<cstdint>)
#include <cstdint>
#else
#include <stdint.h>
#endif
#include <functional>
#include <stdexcept>
#include <utility>
#include <vector>

namespace SpaceMachine {

template<size_t, size_t> class StateMachineBuilder;

constexpr std::size_t STATE_MACHINE_MAX_SIZE = 4096;
constexpr std::size_t STATE_SIZE = sizeof(std::function<void()>);
constexpr std::size_t TRANSITION_SIZE = sizeof(std::function<bool()>);
constexpr std::size_t TRANSITION_RATIO = 4;
// We want to guarantee an average of 4 Transitions per State
// MaxNumTransitions >= TRANSITION_RATIO * MaxNumStates
// We also want to guarantee the default StateMachine takes up no more than 4KiB
// sizeof(StateMachine<>) = 3 + (STATE_SIZE + 1) * MaxNumStates
//                          + (TRANSITION_SIZE + 1) * MaxNumTransitions
// Maximizing the quantity (MaxNumStates + MaxNumTransitions) yields:
// MaxNumStates = floor(4093 /
//              ((1 + STATE_SIZE) + TRANSITION_RATIO * (1 + TRANSITION_SIZE)))
// MaxNumTransitions = max(TRANSITION_RATIO * MaxNumStates,
//                         floor( (4093 - (STATE_SIZE + 1) * MaxNumStates /
//                                (1 + TRANSITION_SIZE)))
constexpr size_t MAX_NUM_STATES
        = (STATE_MACHINE_MAX_SIZE - 3)
          / (1 + STATE_SIZE + TRANSITION_RATIO * (1 + TRANSITION_SIZE));

constexpr size_t NAIVE_NUM_TRANSITIONS = MAX_NUM_STATES * TRANSITION_RATIO;
constexpr size_t DERIVED_NUM_TRANSITIONS
        = (STATE_MACHINE_MAX_SIZE - 3 - MAX_NUM_STATES * (STATE_SIZE + 1))
          / (1 + TRANSITION_SIZE);
constexpr size_t MAX_NUM_TRANSITIONS
        = DERIVED_NUM_TRANSITIONS > NAIVE_NUM_TRANSITIONS
                  ? DERIVED_NUM_TRANSITIONS
                  : NAIVE_NUM_TRANSITIONS;

template<size_t MaxNumStates = MAX_NUM_STATES,
         size_t MaxNumTransitions = MAX_NUM_TRANSITIONS>
class StateMachine {
public:
    // Highest value an index will ever be is MaxNum+1
    // For u8 the ValueMax is 256, so to ensure no index ever overflows,
    // MaxNum needs to be one less than ValueMax: 255
    static_assert(MaxNumStates <= 255);
    static_assert(MaxNumTransitions <= 255);
    using StateIndex = uint8_t;
    using TransitionIndex = uint8_t;

    StateMachine() = default;
    ~StateMachine() = default;
    StateMachine(const StateMachine&) = delete;
    StateMachine(StateMachine&&) = default;
    StateMachine& operator=(const StateMachine&) = delete;
    StateMachine& operator=(StateMachine&&) = default;

    void doWork() { states[currentState](); }

    bool triggerTransitions()
    {
        for (TransitionIndex i = stateTransitionsStartIndices[currentState];
             i <= transitionEndIndexOf(currentState); ++i) {
            if (!transitionConditions[i]()) continue;
            currentState = transitionTargets[i];
            return true;
        }
        return false;
    }

    void run()
    {
        triggerTransitions();
        doWork();
    }

private:
    TransitionIndex transitionEndIndexOf(const StateIndex stateIndex)
    {
        if (stateIndex >= numStates)
            throw std::out_of_range("State index out of range");
        if (stateIndex == numStates - 1) return numTransitions - 1;
        return stateTransitionsStartIndices[stateIndex + 1] - 1;
    }

    friend class StateMachineBuilder<MaxNumStates, MaxNumTransitions>;
    // Let S = MaxNumStates and T=MaxNumTransitions
    std::function<bool()> transitionConditions[MaxNumTransitions]; // Size = 32T
    std::function<void()> states[MaxNumStates]; // Size = 32S
    StateIndex currentState = 0; // Size = 1
    StateIndex numStates = 0; // Size = 1
    StateIndex transitionTargets[MaxNumTransitions] = {}; // Size = T
    TransitionIndex numTransitions = 0; // Size = 1
    TransitionIndex stateTransitionsStartIndices[MaxNumStates] = {}; // Size = S
    // Total Size = 3 + 33S + 33T
};

static_assert(sizeof(StateMachine<>) <= STATE_MACHINE_MAX_SIZE);
constexpr std::size_t MIN_FUNCTION_SIZE
        = STATE_SIZE < TRANSITION_SIZE ? STATE_SIZE : TRANSITION_SIZE;
static_assert(sizeof(StateMachine<>) + MIN_FUNCTION_SIZE
              >= STATE_MACHINE_MAX_SIZE);

template<size_t MaxNumStates = 24, size_t MaxNumTransitions = 100>
class StateMachineBuilder {
    using StateMachineType = StateMachine<MaxNumStates, MaxNumTransitions>;
    using StateIndex = typename StateMachineType::StateIndex;
    using TransitionIndex = typename StateMachineType::TransitionIndex;

public:
    struct State {
        std::function<void()> work;

        State() = delete;
        ~State() = default;
        State(const State&) = delete;
        State(State&&) = default;
        State& operator=(const State&) = delete;
        State& operator=(State&&) = delete;
        void* operator new(size_t) = delete;
        void* operator new[](size_t) = delete;

    private:
        friend class StateMachineBuilder;
        explicit State(std::function<void()> work): work(std::move(work)) {}
    };
    struct Transition {
        const State& from;
        const State& to;
        std::function<bool()> condition;

        Transition() = delete;
        ~Transition() = default;
        Transition(const Transition&) = delete;
        Transition(Transition&&) = default;
        Transition& operator=(const Transition&) = delete;
        Transition& operator=(Transition&&) = delete;

    private:
        friend class StateMachineBuilder;
        explicit Transition(const State& from, const State& to,
                            std::function<bool()> condition)
            : from(from), to(to), condition(std::move(condition))
        {}
    };

    StateMachineBuilder() = delete;
    explicit StateMachineBuilder(
            StateMachine<MaxNumStates, MaxNumTransitions>& stateMachine)
        : stateMachine(stateMachine)
    {
        states.reserve(MaxNumStates);
        transitions.reserve(MaxNumTransitions);
    }
    ~StateMachineBuilder() = default;
    StateMachineBuilder(const StateMachineBuilder&) = default;
    StateMachineBuilder(StateMachineBuilder&&) = default;
    StateMachineBuilder& operator=(const StateMachineBuilder&) = default;
    StateMachineBuilder& operator=(StateMachineBuilder&&) = default;

    const State& createState(std::function<void()> work)
    {
        states.emplace_back(State{std::move(work)});
        return states.back();
    }

    void setInitialState(const State& state) { initialState = &state; }

    const Transition& createTransition(const State& from, const State& to,
                                       std::function<bool()> condition)
    {
        transitions.push_back(Transition{from, to, std::move(condition)});
        return transitions.back();
    }

    StateMachineType& build()
    {
        assertValidity();
        stateMachine.numStates = static_cast<StateIndex>(states.size());
        stateMachine.numTransitions
                = static_cast<TransitionIndex>(transitions.size());
        TransitionIndex currentTransitionIndex = 0;
        StateIndex currentStateIndex = 0;
        for (; currentStateIndex < states.size(); ++currentStateIndex) {
            const auto& state = states.at(currentStateIndex);
            if (initialState == &state)
                stateMachine.currentState = currentStateIndex;
            stateMachine.stateTransitionsStartIndices[currentStateIndex]
                    = currentTransitionIndex;
            setTransitionsFor(state, currentTransitionIndex);
            stateMachine.states[currentStateIndex] = std::move(state.work);
        }

        return stateMachine;
    }

private:
    static std::string vectorRepresentation(const std::vector<size_t>& vector)
    {
        std::string repr = "[";
        if (vector.empty()) return repr + "]";
        repr += std::to_string(vector.front());
        if (vector.size() == 1) return repr + "]";
        for (size_t i = 1; i < vector.size(); i++) {
            repr += ", " + std::to_string(vector[i]);
        }
        return repr + "]";
    }

    bool isReachable(const State& state) const
    {
        if (initialState == &state) return true;
        // Raw loop instead of std::any_of because it is more portable
        for (const auto& transition: transitions) {
            if (&transition.to == &state) return true;
        }
        return false;
    }

    void assertValidity() const
    {
        if (states.size() > MaxNumStates) {
            throw std::range_error(
                    "Given state machine does not have enough space for "
                    "registered states!\n"
                    "Amount reserved: "
                    + std::to_string(MaxNumStates)
                    + "\n"
                      "Amount registered: "
                    + std::to_string(states.size())
                    + "\n"
                      "Try allocating a bigger state machine, like:\n"
                      "StateMachine<"
                    + std::to_string(states.size()) + ", "
                    + std::to_string(transitions.size()) + ">\n");
        }
        if (transitions.size() > MaxNumTransitions) {
            throw std::range_error(
                    "Given state machine does not have enough space for "
                    "registered states!\n"
                    "Amount reserved: "
                    + std::to_string(MaxNumTransitions)
                    + "\n"
                      "Amount registered: "
                    + std::to_string(transitions.size())
                    + "\n"
                      "Try allocating a bigger state machine, like:\n"
                      "StateMachine<"
                    + std::to_string(states.size()) + ", "
                    + std::to_string(transitions.size()) + ">\n");
        }
        if (states.empty()) {
            throw std::invalid_argument(
                    "No states were registered! Make sure to use "
                    "createState(...) to add states to the state machine.");
        }
        if (transitions.empty()) {
            throw std::invalid_argument(
                    "No transitions were registered! Make sure to use "
                    "createTransition(...) to add transitions to the state "
                    "machine.");
        }
        if (initialState == nullptr) {
            throw std::invalid_argument(
                    "Initial state was not set! Make sure to call "
                    "setInitialState(...) before calling build().");
        }
        std::vector<size_t> unreachableStates;
        for (size_t i = 0; i < states.size(); i++) {
            if (!isReachable(states.at(i))) { unreachableStates.push_back(i); }
        }
        if (!unreachableStates.empty()) {
            throw std::invalid_argument(
                    "State machine has unreachable states! State(s) with "
                    "the following indices cannot be reached"
                    + vectorRepresentation(unreachableStates)
                    + " (indices start at 0 and are assigned in "
                      "chronological order).\n"
                      "Consider removing the state(s) or adding "
                      "transition(s).");
        }
    }

    void setTransitionsFor(const State& state,
                           TransitionIndex& currentTransitionIndex)
    {
        for (auto& transition: transitions) {
            if (&transition.from != &state) continue;
            stateMachine.transitionConditions[currentTransitionIndex]
                    = std::move(transition.condition);
            stateMachine.transitionTargets[currentTransitionIndex]
                    = GetIndexOf(transition.to);
            ++currentTransitionIndex;
        }
    }

    StateIndex GetIndexOf(const State& state) const
    {
        for (StateIndex i = 0; i < states.size(); ++i) {
            if (&states.at(i) == &state) { return i; }
        }
        throw std::invalid_argument("State cannot be found.");
    }

    StateMachine<MaxNumStates, MaxNumTransitions>& stateMachine;
    std::vector<State> states;
    std::vector<Transition> transitions;
    const State* initialState = nullptr;
};

} // namespace SpaceMachine

#endif // SPACEMACHINE_SPACEMACHINE_HPP
