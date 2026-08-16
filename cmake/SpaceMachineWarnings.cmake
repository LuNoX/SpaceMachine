include_guard(GLOBAL)

function (spacemachine_enable_warnings target)
    set(gnu_warnings
        -Werror
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion
        -Wsign-conversion
        -Wnull-dereference
        -Wdouble-promotion
        -Wformat=2
        -Wimplicit-fallthrough
        -Woverloaded-virtual
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Wcast-align
        -Wmisleading-indentation
    )

    set(msvc_warnings
        /W4
        /permissive-
        /WX
        /w14242 # conversion possible loss of data
        /w14254 # operator precedence
        /w14263 # member function hides virtual
        /w14265 # class has virtual functions but destructor is not virtual
        /w14287 # unsigned/negative constant mismatch
        /we4289 # nonstandard extension used : 'var' loop control variable
        /w14296 # expression is always true/false
        /w14311 # pointer truncation
        /w14545 # expression before comma has no effect
        /w14546 # function call before comma missing argument list
        /w14547 # operator precedence causes unexpected result
        /w14549 # operator before comma has no effect
        /w14555 # expression has no effect
        /w14619 # pragma warning invalid
        /w14640 # thread safety warning
        /w14826 # conversion between types
        /w14905 # string literal to char*
        /w14906 # string literal conversion mismatch
        /w14928 # illegal copy initialization
    )

    set(clang_cl_warnings
        ${gnu_warnings}
        # Wall expands to Weverything on clang-cl, so we need to disable some
        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic
        -Wno-padded
        /permissive-
    )

    if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        set(warnings ${msvc_warnings})
    elseif (CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
        set(warnings ${clang_cl_warnings})
    else ()
        set(warnings ${gnu_warnings})
    endif ()

    target_compile_options(${target} PRIVATE ${warnings})
endfunction ()
