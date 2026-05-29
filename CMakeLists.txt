cmake_minimum_required(VERSION 3.20)

project(SpaceMachine LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(SpaceMachine INTERFACE)

target_compile_features(SpaceMachine INTERFACE
    cxx_std_17
)

target_include_directories(SpaceMachine INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(SpaceMachine INTERFACE cxx_std_17)

add_executable(SpaceMachineDemo
    main.cpp
)

set(SPACEMACHINE_GNU_CLANG_WARNINGS
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
    -Wuseless-cast
    -Wduplicated-cond
    -Wduplicated-branches
    -Wlogical-op
    -Wmisleading-indentation
)

set(SPACEMACHINE_MSVC_WARNINGS
    /W4
    /permissive-
    /WX

    # More strict MSVC diagnostics (equivalent intent to GCC/Clang warnings)
    /w14242   # conversion possible loss of data
    /w14254   # operator precedence
    /w14263   # member function hides virtual
    /w14265   # class has virtual functions but destructor is not virtual
    /w14287   # unsigned/negative constant mismatch
    /we4289   # nonstandard extension used : 'var' loop control variable
    /w14296   # expression is always true/false
    /w14311   # pointer truncation
    /w14545   # expression before comma has no effect
    /w14546   # function call before comma missing argument list
    /w14547   # operator precedence causes unexpected result
    /w14549   # operator before comma has no effect
    /w14555   # expression has no effect
    /w14619   # pragma warning invalid
    /w14640   # thread safety warning
    /w14826   # conversion between types
    /w14905   # string literal to char*
    /w14906   # string literal conversion mismatch
    /w14928   # illegal copy initialization
)

target_compile_options(SpaceMachineDemo PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:${SPACEMACHINE_GNU_CLANG_WARNINGS}>
    $<$<CXX_COMPILER_ID:MSVC>:${SPACEMACHINE_MSVC_WARNINGS}>
)

target_link_libraries(SpaceMachineDemo PRIVATE SpaceMachine)