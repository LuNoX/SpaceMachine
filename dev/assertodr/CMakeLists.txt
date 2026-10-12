# Check that each header can be compiled to a second translation unit.
# Thus asserting that the One Definition Rule holds.

include(SpaceMachineWarnings)

spacemachine_require_header_stubs()

file(GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/main.cpp
     CONTENT "int main() { return 0; }\n"
)

add_executable(
    SpaceMachineAssertOneDefinitionRule ${SPACEMACHINE_HEADER_STUBS}
                                        ${CMAKE_CURRENT_BINARY_DIR}/main.cpp
)
target_link_libraries(
    SpaceMachineAssertOneDefinitionRule
    PRIVATE SpaceMachineCheckIndividualHeaderCompilation
            SpaceMachine::SpaceMachine
)
spacemachine_enable_warnings(SpaceMachineAssertOneDefinitionRule)

list(APPEND check_targets SpaceMachineAssertOneDefinitionRule)
set(check_targets ${check_targets} PARENT_SCOPE)
