include(SpaceMachineChecks)

set(SPACEMACHINE_CHECKS_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(headercheck)
add_subdirectory(demo)

spacemachine_attach_checks(SpaceMachineDemo)
