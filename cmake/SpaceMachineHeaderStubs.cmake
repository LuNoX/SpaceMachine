include_guard(GLOBAL)

function (spacemachine_require_headers)
    if (NOT DEFINED SPACEMACHINE_HEADERS)
        message(
            FATAL_ERROR
                "SPACEMACHINE_HEADERS is not set! Please call\n"
                "spacemachine_collect_headers(SPACEMACHINE_HEADER)\n"
                "before reaching this point."
        )
    endif ()
endfunction ()

function (spacemachine_require_header_stubs)
    if (NOT DEFINED SPACEMACHINE_HEADER_STUBS)
        message(
            FATAL_ERROR
                "SPACEMACHINE_HEADERS is not set! Please call\n"
                "spacemachine_require_headers()\n"
                "spacemachine_generate_header_stubs(\n"
                "\t\"\${SPACEMACHINE_HEADERS}\" SPACEMACHINE_HEADER_STUBS\n"
                ")\n"
                "before reaching this point."
        )
    endif ()
endfunction ()

function (spacemachine_collect_headers out_headers)
    file(GLOB_RECURSE headers CONFIGURE_DEPENDS
         ${PROJECT_SOURCE_DIR}/include/*.hpp
    )
    set(${out_headers} ${headers} PARENT_SCOPE)
endfunction ()

function (spacemachine_generate_header_stubs headers out_stubs)
    set(stubs)

    foreach (header IN LISTS headers)
        file(RELATIVE_PATH header_rel ${PROJECT_SOURCE_DIR}/include ${header})
        set(stub ${CMAKE_CURRENT_BINARY_DIR}/stubs/${header_rel}.cpp)

        file(
            CONFIGURE
            OUTPUT
            ${stub}
            CONTENT
            "#include <${header_rel}> // NOLINT(misc-include-cleaner)\n"
            @ONLY
        )

        list(APPEND stubs ${stub})
    endforeach ()

    set(${out_stubs} ${stubs} PARENT_SCOPE)
endfunction ()
