include_guard(GLOBAL)

function (spacemachine_register_check target)
    set_property(
        DIRECTORY ${SPACEMACHINE_CHECKS_DIR} APPEND PROPERTY SPACEMACHINE_CHECKS
                                                             ${target}
    )
endfunction ()

# Builds run through CMake Tools via cmake.launchTargetPath, which builds only
# the launch target. Hanging the checks off it is what makes them run on an
# ordinary build.
function (spacemachine_attach_checks target)
    if (NOT SPACEMACHINE_CHECKS_DIR)
        message(FATAL_ERROR "SPACEMACHINE_CHECKS_DIR is not set")
    endif ()

    get_property(
        checks DIRECTORY ${SPACEMACHINE_CHECKS_DIR}
        PROPERTY SPACEMACHINE_CHECKS
    )

    if (checks)
        add_dependencies(${target} ${checks})
    endif ()
endfunction ()
