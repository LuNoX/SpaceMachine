include_guard(GLOBAL)

function (spacemachine_link_compile_commands)
    get_filename_component(compile_commands_dir ${CMAKE_BINARY_DIR} DIRECTORY)

    if ("${compile_commands_dir}" STREQUAL "${PROJECT_SOURCE_DIR}")
        return()
    endif ()

    file(CREATE_LINK ${CMAKE_BINARY_DIR}/compile_commands.json
         ${compile_commands_dir}/compile_commands.json SYMBOLIC
         RESULT link_result
    )

    if (link_result)
        message(
            STATUS
                "Could not link compile_commands.json into "
                "${compile_commands_dir} (${link_result})! "
                "Point CompilationDatabase in .clangd at ${CMAKE_BINARY_DIR}, "
                "if clangd cannot find it."
        )
    endif ()
endfunction ()
