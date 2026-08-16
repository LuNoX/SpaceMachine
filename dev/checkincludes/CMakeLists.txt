# Checks that every header includes exactly what it uses.
#
# This is already somewhat covered by the clang-tidy run on the demo target.
# clang-tidy and clangd behave differently when it comes to header includes.
# clangd has better detection for transitive includes.
# So in addition to clang-tidy, we use the python library clangD-tidy.
# It runs clangd over each header, in order to get the best of both worlds.

include(SpaceMachineAnalyzers)

spacemachine_require_headers()

find_program(SPACEMACHINE_UV_EXE NAMES uv)

if (NOT SPACEMACHINE_UV_EXE)
    spacemachine_report_missing_tool(
        uv SpaceMachineCheckIncludes "uv is required to run clangD-tidy! "
        "clangd header include analysis will be skipped."
    )
    return()
endif ()

cmake_host_system_information(
    RESULT SPACEMACHINE_LINT_JOBS QUERY NUMBER_OF_LOGICAL_CORES
)

# Add a stamp to detect wether clangd even needs to run again
set(stamp ${CMAKE_CURRENT_BINARY_DIR}/check_includes.stamp)

add_custom_command(
    OUTPUT ${stamp}
    COMMAND
        ${SPACEMACHINE_UV_EXE} run python
        ${PROJECT_SOURCE_DIR}/tools/lint_headers.py -p ${PROJECT_BINARY_DIR} -j
        ${SPACEMACHINE_LINT_JOBS} --color never --fail-on-severity hint
        ${SPACEMACHINE_HEADERS}
    COMMAND ${CMAKE_COMMAND} -E touch ${stamp}
    DEPENDS ${SPACEMACHINE_HEADERS}
            ${PROJECT_SOURCE_DIR}/tools/lint_headers.py
            ${PROJECT_SOURCE_DIR}/.clang-tidy
            ${PROJECT_SOURCE_DIR}/.clangd
            ${PROJECT_BINARY_DIR}/compile_commands.json
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "Checking header includes (clangd-tidy)"
    VERBATIM
)

add_custom_target(SpaceMachineCheckIncludes ALL DEPENDS ${stamp})

list(APPEND check_targets SpaceMachineCheckIncludes)
set(check_targets ${check_targets} PARENT_SCOPE)
