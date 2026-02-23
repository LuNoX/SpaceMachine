cmake_minimum_required(VERSION 4.0)
project(SpaceMachine)

set(CMAKE_CXX_STANDARD 17)

add_executable(SpaceMachine main.cpp
        include/spacemachine/SpaceMachine.hpp
        include/spacemachine/TemplateSpaceMachine.hpp)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(SpaceMachine PRIVATE
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
            -Wuseless-cast
            -Wduplicated-cond
            -Wduplicated-branches
            -Wlogical-op
            -Wmisleading-indentation
            -Werror
    )
endif ()

if (MSVC)
    target_compile_options(SpaceMachine PRIVATE
            /W4
            /permissive-
            /w14242
            /w14254
            /w14263
            /w14265
            /w14287
            /we4289
            /w14296
            /w14311
            /w14545
            /w14546
            /w14547
            /w14549
            /w14555
            /w14619
            /w14640
            /w14826
            /w14905
            /w14906
            /w14928
            /WX
    )
endif ()