include_guard(GLOBAL)

function (spacemachine_collect_polyfill_defines headers out_defines)
    set(defines)

    foreach (header IN LISTS headers)
        if (NOT header MATCHES "/spacemachine/polyfill/([^/]+)\\.(h|hpp)$")
            continue()
        endif ()

        set(std_header ${CMAKE_MATCH_1})
        set(extension ${CMAKE_MATCH_2})
        string(TOUPPER ${std_header} name)
        set(define SPACEMACHINE_POLYFILL_NO_STD_${name})

        set_property(
            DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${header}
        )
        file(STRINGS ${header} define_uses REGEX ${define})

        if (NOT define_uses)
            message(
                FATAL_ERROR
                    "${header} never checks ${define}!\n"
                    "Each polyfill header needs an additional define guard to "
                    "allow for explicit manual compilation toggling.\n"
                    "Please add something to the effect of:\n"
                    "#if __has_include(<${std_header}>) "
                    "&& !defined(${define})\n"
                    "\t#include <${std_header}>\n"
                    "#else\n"
                    "\t#include \"spacemachine/polyfill/detail/"
                    "${std_header}.${extension}\"\n"
                    "#endif\n"
                    "to ${header}."
            )
        endif ()

        list(APPEND defines ${define})
    endforeach ()

    if (NOT defines)
        message(
            FATAL_ERROR "Found no polyfill headers in spacemachine/polyfill!"
        )
    endif ()

    set(${out_defines} ${defines} PARENT_SCOPE)
endfunction ()
