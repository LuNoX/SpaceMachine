list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(SpaceMachineCompileCommands)
include(SpaceMachineHeaderStubs)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# Report links to missing targets at configure time instead of link time.
set(CMAKE_LINK_LIBRARIES_ONLY_TARGETS ON)
spacemachine_link_compile_commands()

spacemachine_collect_headers(SPACEMACHINE_HEADERS)
spacemachine_generate_header_stubs(
    "${SPACEMACHINE_HEADERS}" SPACEMACHINE_HEADER_STUBS
)

add_subdirectory(checkindividualheadercompilation)
add_subdirectory(assertodr)
add_subdirectory(checkincludes)
add_subdirectory(demo)
add_subdirectory(checkpolyfills)

add_dependencies(SpaceMachineDemo ${check_targets})
