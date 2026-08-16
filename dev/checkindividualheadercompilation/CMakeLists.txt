# Check whether each header can be compiled in an individual translation unit

include(SpaceMachineWarnings)

spacemachine_require_header_stubs()

add_library(
    SpaceMachineCheckIndividualHeaderCompilation OBJECT
    ${SPACEMACHINE_HEADER_STUBS}
)
target_link_libraries(
    SpaceMachineCheckIndividualHeaderCompilation
    PRIVATE SpaceMachine::SpaceMachine
)
spacemachine_enable_warnings(SpaceMachineCheckIndividualHeaderCompilation)

list(APPEND check_targets SpaceMachineCheckIndividualHeaderCompilation)
set(check_targets ${check_targets} PARENT_SCOPE)
