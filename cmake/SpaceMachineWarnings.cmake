include_guard(GLOBAL)

function (spacemachine_enable_warnings target)
    set(gnu_clang_warnings
        -Werror
        -Wall
        -Wextra
        -Wpedantic
        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic
        -Wno-padded
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

    target_compile_options(
        ${target}
        PRIVATE
            $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:${gnu_clang_warnings}>
            $<$<CXX_COMPILER_ID:MSVC>:${msvc_warnings}>
    )
endfunction ()
