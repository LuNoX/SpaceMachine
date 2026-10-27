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
- [ ] reword "for aerospace applications" in the README to something like "designed for use in MISRA C++:2023, DO-178C and ECSS projects" until the evidence below exists
- [ ] move the Godbolt links out of COMPLIANCE.md to PORTABILITY.md
- [ ] get the free PVS-Studio open source license (pvs-studio.com)
    - [ ] add the README mention the license requires
    - [ ] run it in CI and mention it in commits that fix its findings
    - [ ] renew it yearly
- [ ] add MISRA C++:2023 compliance, self-declared following MISRA Compliance:2020
    - [ ] read MISRA Compliance:2020 (free from misra.org.uk)
    - [ ] buy the MISRA C++:2023 PDF (£15, single user license, misra.org.uk)
    - [ ] add docs/misra/ with a Guideline Enforcement Plan (each rule enforced by clang-tidy, cppcheck, a compiler warning, PVS-Studio or review), a Guideline Re-categorization Plan, deviation records and a compliance summary per release
    - [ ] only use rule numbers and own wording in docs/misra/, never MISRA's copyrighted rule text
    - [ ] scope the claim to the library headers, C++17, the std and polyfill configurations and the compilers in CI
    - [ ] add deviations for the comma fold in StepImpl and the || fold in TryTransitions, keeping Step()'s design
    - [ ] fix discarded work results through "require work to return exactly void"
    - [ ] add a checklist for the review enforced rules to the PR template
    - [ ] state "MISRA C++:2023 compliant, see docs/misra" in the README
- [ ] attach a reuse evidence package to every release, so DO-178C, ECSS and NASA projects can reuse it instead of redoing it
    - [ ] read ECSS-Q-HB-80-01A (reuse of existing software, free at ecss.nl) and shape the package to answer it
    - [ ] map every ECSS-E-ST-40C Rev.1 and ECSS-Q-ST-80C Rev.2 requirement that applies to reused software to its evidence, or mark it as the integrator's responsibility
    - [ ] do the same for NPR 7150.2D and NASA-STD-8739.8B, including their off the shelf and open source software requirements
    - [ ] lay the package out like an FAA AC 20-148 reusable software component data package, without applying for acceptance
    - [ ] turn the Step() behaviours under "add tests" into requirements with IDs in StrictDoc
    - [ ] trace every test to a requirement ID
    - [ ] add negative compilation tests for every static_assert, checking its message
    - [ ] measure MC/DC coverage with clang's -fcoverage-mcdc and reach 100%
    - [ ] also run the codegen checks on pinned local compilers, since Compiler Explorer output doesn't count as evidence
    - [ ] add gh actions CI with pinned tool versions and attach all reports to each release
- [ ] add a DO-178C objective mapping once aviation users show up or ask for it
    - [ ] buy ED-12C (same text as DO-178C, €250 from eshop.eurocae.net instead of $525 from RTCA)
    - [ ] buy DO-332 ($258 from RTCA, check whether EUROCAE's ED-217 is cheaper)
    - [ ] map every Annex A objective to a repo artifact, or mark it as the integrator's responsibility
    - [ ] justify the per instantiation coverage and the codegen checks with DO-332's template and inlining guidance
- [ ] only if a user wants credit for the MISRA tool results without rerunning them, buy DO-330 ($258) and write TQL-5 tool qualification data
- [ ] add generated docs hosted on github pages
