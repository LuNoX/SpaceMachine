list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(SpaceMachineCompileCommands)
include(SpaceMachineHeaderStubs)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_EXTENSIONS OFF)
spacemachine_link_compile_commands()

spacemachine_collect_headers(SPACEMACHINE_HEADERS)
spacemachine_generate_header_stubs(
    "${SPACEMACHINE_HEADERS}" SPACEMACHINE_HEADER_STUBS
)

add_subdirectory(checkindividualheadercompilation)
add_subdirectory(assertodr)
add_subdirectory(checkincludes)
add_subdirectory(demo)

add_dependencies(SpaceMachineDemo ${check_targets})
