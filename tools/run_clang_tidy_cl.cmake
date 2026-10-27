# Runs clang-tidy on an MSVC-style compile command without losing its /-style
# flags.
#
# Usage: cmake -P run_clang_tidy_cl.cmake -- <clang-tidy> <args>... -- <compile
# command>...
#
# clang-tidy parses the compile command after its -- in GCC mode, before
# applying CMake's --extra-arg-before=--driver-mode=cl, and so drops every
# /-style flag. Losing /EHsc turns off all exception analysis. Naming the
# driver mode in the compile command itself keeps them, see
# https://gitlab.kitware.com/cmake/cmake/-/issues/20512.

set(command)
set(in_command FALSE)
set(in_compile_command FALSE)
math(EXPR last_index "${CMAKE_ARGC} - 1")

foreach (index RANGE ${last_index})
    set(arg "${CMAKE_ARGV${index}}")

    if (NOT in_command)
        if (arg STREQUAL "--")
            set(in_command TRUE)
        endif ()
        continue()
    endif ()

    list(APPEND command "${arg}")
    if (NOT in_compile_command AND arg STREQUAL "--")
        list(APPEND command --driver-mode=cl)
        set(in_compile_command TRUE)
    endif ()
endforeach ()

execute_process(COMMAND ${command} RESULT_VARIABLE result)
if (NOT result EQUAL 0)
    message(FATAL_ERROR "clang-tidy failed with exit code ${result}.")
endif ()
