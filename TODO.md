# TODOs

- [ ] add mode selection for
    - [ ] work then transition checks vs transition checks then work
    - [ ] only do one transition per cycle vs trigger transitions until state is
          stable vs set limit on transitions triggered
- [ ] add tests, covering at least these StateMachine::Step() behaviours
    - [ ] a taken transition does not cascade: the new state's transitions are
          not evaluated in the same step (guards the snapshot in StepImpl)
    - [ ] the first triggering transition wins and later conditions are not
          evaluated
    - [ ] a taken transition runs the target's work, not the source's
    - [ ] with no triggering transition the state stays and runs its own work
    - [ ] a self transition runs the state's own work and leaves it active
    - [ ] only the active state's conditions and work are evaluated
    - [ ] IsActive() reports the initial state before the first step
    - [ ] non void work results are discarded, including [[nodiscard]] types
    - [ ] all of the above with every SPACEMACHINE_POLYFILL_NO_STD_* defined
- [ ] add benchmarks
- [ ] add polyfill presets for common embedded standard libraries
- [ ] add automatic copyright attributions (created by, last edited) on commit
- [ ] add compiler explorer API calls to ensure compliance
- [ ] add codegen checks via compiler explorer, asserting StateMachine::Step()
      compiles to one bounds check plus a jump table, inlines all work and
      conditions, and runs a taken transition's target work without a second
      dispatch
- [ ] add lint for TODO items
- [ ] add lint for git conflict tags
- [ ] add check that dev/demo/main.cpp matches the README example, minus NOLINTs
- [ ] noexcept everything
    - [ ] StateMachine::Step() should be noexcept exactly when all work and
          conditions are (needs is_nothrow_invocable_v in the polyfill)
- [ ] constexpr everything
- [ ] [[nodiscard]] everything whose result must not be ignored
- [ ] support states without transitions (TypeList<> cannot be constructed)
- [ ] fix MakeStateMachine's "One or more arguments are not states!" static_assert
    - [ ] non state arguments fail with "no type named 'ID'" and an ambiguous call
    - [ ] other overloads' enable_if conditions instantiate traits on the argument
- [ ] use one central StateMachine::StateIndex type for every state index, later
      the smallest unsigned type that fits the number of states
    - [ ] add integer_sequence and make_integer_sequence to the polyfill and make
          index_sequence an alias of integer_sequence<size_t, ...>; once
          StateIndex is narrower than size_t, a StateIndex... pack can only be
          deduced from integer_sequence<StateIndex, ...>
    - [ ] rename the integral_constant alias StateMachine::StateIndex to
          StateIndexConstant, freeing the name
    - [ ] add StateMachine::StateIndex (polyfill::size_t for now) and use it for
          m_activeStateIndex, indexOf, stateIndices, StateIndexConstant and every
          state index template parameter; transition indices stay size_t
    - [ ] pick the smallest unsigned type that fits sizeof...(States) at compile
          time (needs conditional_t exported from the polyfill)
- [ ] add StateMachine::VisitActiveState(visitor), calling the visitor with the
      active state as const& (ID via State::ID) for logging, telemetry and mode
      specific behaviour
    - [ ] support visitors that return a value (Step()'s comma fold only handles
          void)
    - [ ] share the runtime to compile time index dispatch with Step(), so the
          m_activeStateIndex snapshot lives in one place
- [ ] require work to return exactly void, so a returned status cannot be dropped
      silently
    - [ ] make is_valid_work_v check that the return type is exactly void
          (is_invocable_r_v<void, Fn&> accepts any return type; needs declval or
          invoke_result_t exported from the polyfill)
    - [ ] change the error message in Work.hpp and State.hpp to "Work must be
          callable with zero arguments and return void!"
    - [ ] change the test reminder "non void work results are discarded,
          including [[nodiscard]] types" to "non void work is rejected at compile
          time"
- [ ] add vcpkg and conan distribution
- [ ] add release tarball to gh actions
- [ ] add gh action checking for dead links in all md files, and later the docs
- [ ] add maximum confirmed version to cmake
- [ ] add validation for c++ 20+
- [ ] add DO-178C compliance
- [ ] add MISRA C++:2023 compliance checks
- [ ] add generated docs hosted on github pages
