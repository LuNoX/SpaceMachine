include_guard(GLOBAL)

option(SPACEMACHINE_REQUIRE_DEV_TOOLS
       "Fail configuration when a dev analysis tool is missing" OFF
)

function (spacemachine_report_missing_tool tool target consequence)
    if (SPACEMACHINE_REQUIRE_DEV_TOOLS)
        message(
            FATAL_ERROR
                "Could not find ${tool}! "
                "SPACEMACHINE_REQUIRE_DEV_TOOLS is enabled, so it is required "
                "for ${target}. Please make sure ${tool} is in PATH. "
                "${consequence}"
        )
        return()
    endif ()

    message(STATUS "Could not find ${tool}! ${consequence}")
endfunction ()

function (spacemachine_enable_clang_tidy target)
    find_program(SPACEMACHINE_CLANG_TIDY_EXE NAMES clang-tidy)
    if (NOT SPACEMACHINE_CLANG_TIDY_EXE)
        spacemachine_report_missing_tool(
            clang-tidy ${target} "clang-tidy will be skipped for ${target}."
        )
        return()
    endif ()

    set(clang_tidy_command ${SPACEMACHINE_CLANG_TIDY_EXE} --quiet
                           --config-file=${PROJECT_SOURCE_DIR}/.clang-tidy
    )
    set_target_properties(
        ${target} PROPERTIES CXX_CLANG_TIDY "${clang_tidy_command}"
    )
endfunction ()

function (spacemachine_enable_cppcheck target)
    find_program(SPACEMACHINE_CPPCHECK_EXE NAMES cppcheck)
    if (NOT SPACEMACHINE_CPPCHECK_EXE)
        spacemachine_report_missing_tool(
            cppcheck ${target} "cppcheck will be skipped for ${target}."
        )
        return()
    endif ()

    set(cppcheck_command
        ${SPACEMACHINE_CPPCHECK_EXE} --enable=all --inconclusive --std=c++17
        --suppress=missingIncludeSystem --suppress=unusedFunction
        --suppress=checkersReport --suppress=unmatchedSuppression
        --error-exitcode=1
    )
    set_target_properties(
        ${target} PROPERTIES CXX_CPPCHECK "${cppcheck_command}"
    )
endfunction ()
