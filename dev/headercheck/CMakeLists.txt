include(SpaceMachineChecks)
include(SpaceMachineWarnings)

# The demo build only produces a single translation unit.
# To get better compile-time diagnostics, we build an additional target.
# This target adds .cpp stubs for every header, then compiles each.

file(
    GLOB_RECURSE
    SPACEMACHINE_HEADERS
    CONFIGURE_DEPENDS
    ${PROJECT_SOURCE_DIR}/include/*.hpp
)

set(SPACEMACHINE_HEADER_STUBS)
foreach (header IN LISTS SPACEMACHINE_HEADERS)
    file(
        RELATIVE_PATH
        header_rel
        ${PROJECT_SOURCE_DIR}/include
        ${header}
    )
    set(stub ${CMAKE_CURRENT_BINARY_DIR}/header_checks/${header_rel}.cpp)
    file(GENERATE OUTPUT ${stub}
         CONTENT "#include \"${header_rel}\" // NOLINT(misc-include-cleaner)\n"
    )
    list(APPEND SPACEMACHINE_HEADER_STUBS ${stub})
endforeach ()

add_library(SpaceMachineHeaderChecks OBJECT ${SPACEMACHINE_HEADER_STUBS})
target_link_libraries(SpaceMachineHeaderChecks PRIVATE SpaceMachine)
spacemachine_enable_warnings(SpaceMachineHeaderChecks)
spacemachine_register_check(SpaceMachineHeaderChecks)

# clang-tidy and clangd behave differently when it comes to header includes.
# clangd has better detection for transitive includes.
# So in addition to clang-tidy, we use the python library clangD-tidy.
# It runs clangd over each header, in order to get the best of both worlds.

find_program(UV_EXE NAMES uv)

if (UV_EXE)
    cmake_host_system_information(
        RESULT SPACEMACHINE_LINT_JOBS QUERY NUMBER_OF_LOGICAL_CORES
    )

    add_custom_target(
        SpaceMachineClangdHeaders
        COMMAND
            ${UV_EXE} run python ${PROJECT_SOURCE_DIR}/tools/lint_headers.py -p
            ${PROJECT_BINARY_DIR} -j ${SPACEMACHINE_LINT_JOBS} --color never
            --fail-on-severity hint ${SPACEMACHINE_HEADERS}
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        COMMENT "Linting headers (clangd-tidy)"
        VERBATIM
    )
    spacemachine_register_check(SpaceMachineClangdHeaders)
else ()
    message(
        WARNING
            "uv not found -- skipping clangd header lint. Install uv to catch "
            "missing includes that clang-tidy cannot see."
    )
endif ()
