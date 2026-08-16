include_guard(GLOBAL)

function (spacemachine_enable_clang_tidy target)
    find_program(CLANG_TIDY_EXE NAMES clang-tidy)

    if (CLANG_TIDY_EXE)
        set_target_properties(
            ${target}
            PROPERTIES
                CXX_CLANG_TIDY
                "${CLANG_TIDY_EXE};--quiet;--config-file=${PROJECT_SOURCE_DIR}/.clang-tidy"
        )
    endif ()
endfunction ()

function (spacemachine_enable_cppcheck target)
    find_program(CPPCHECK_EXE NAMES cppcheck)

    if (CPPCHECK_EXE)
        set_target_properties(
            ${target}
            PROPERTIES
                CXX_CPPCHECK
                "${CPPCHECK_EXE};
                --enable=all;
                --inconclusive;
                --std=c++17;
                --suppress=missingIncludeSystem;
                --suppress=unusedFunction;
                --suppress=checkersReport"
        )
    endif ()
endfunction ()
