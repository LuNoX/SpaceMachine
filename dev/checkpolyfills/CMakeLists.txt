# Check whether polyfill fallback implementations compile.

include(SpaceMachineAnalyzers)
include(SpaceMachinePolyfills)
include(SpaceMachineWarnings)

spacemachine_require_headers()
spacemachine_require_header_stubs()

if (NOT TARGET SpaceMachineDemo)
    message(
        FATAL_ERROR
            "SpaceMachineCheckPolyfillDemo requires target SpaceMachineDemo! "
            "The polyfill demo attempts to build the regular demo with "
            "polyfill implementations.\n"
            "Please add SpaceMachineDemo's directory to your build before "
            "adding SpaceMachineCheckPolyfillDemo."
    )
endif ()

spacemachine_collect_polyfill_defines(
    "${SPACEMACHINE_HEADERS}" polyfill_defines
)

add_library(SpaceMachinePolyfillFallbacks INTERFACE)
target_link_libraries(
    SpaceMachinePolyfillFallbacks INTERFACE SpaceMachine::SpaceMachine
)
target_compile_definitions(
    SpaceMachinePolyfillFallbacks INTERFACE ${polyfill_defines}
)

add_library(
    SpaceMachineCheckPolyfillHeaderCompilation OBJECT
    ${SPACEMACHINE_HEADER_STUBS}
)
target_link_libraries(
    SpaceMachineCheckPolyfillHeaderCompilation
    PRIVATE SpaceMachinePolyfillFallbacks
)
spacemachine_enable_warnings(SpaceMachineCheckPolyfillHeaderCompilation)

get_target_property(demo_dir SpaceMachineDemo SOURCE_DIR)
get_target_property(demo_target_sources SpaceMachineDemo SOURCES)
set(demo_sources)

foreach (source IN LISTS demo_target_sources)
    cmake_path(ABSOLUTE_PATH source BASE_DIRECTORY ${demo_dir})
    list(APPEND demo_sources ${source})
endforeach ()

add_executable(SpaceMachineCheckPolyfillDemo ${demo_sources})
target_link_libraries(
    SpaceMachineCheckPolyfillDemo PRIVATE SpaceMachinePolyfillFallbacks
)
spacemachine_enable_warnings(SpaceMachineCheckPolyfillDemo)
# bugprone-forwarding-reference-overload only accepts std::enable_if as a
# constraint, so it flags constructors constrained by the polyfill's own
# enable_if_t. SpaceMachineDemo still runs it against std::enable_if_t.
spacemachine_enable_clang_tidy(
    SpaceMachineCheckPolyfillDemo
    --checks=-bugprone-forwarding-reference-overload
)

# Keep the forced fallbacks out of compile_commands.json, so clangd and
# SpaceMachineCheckIncludes only ever see the regular configuration.
set_target_properties(
    SpaceMachineCheckPolyfillHeaderCompilation SpaceMachineCheckPolyfillDemo
    PROPERTIES EXPORT_COMPILE_COMMANDS OFF
)

list(APPEND check_targets SpaceMachineCheckPolyfillHeaderCompilation
     SpaceMachineCheckPolyfillDemo
)
set(check_targets ${check_targets} PARENT_SCOPE)
