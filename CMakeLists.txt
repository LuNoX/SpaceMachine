cmake_minimum_required(VERSION 3.21)

project(
    SpaceMachine
    VERSION 0.1.0
    DESCRIPTION "Header-only compile-time finite state machine"
    LANGUAGES CXX
)

include(GNUInstallDirs)

add_library(SpaceMachine INTERFACE)
add_library(SpaceMachine::SpaceMachine ALIAS SpaceMachine)

target_compile_features(SpaceMachine INTERFACE cxx_std_17)
target_include_directories(
    SpaceMachine
    INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
              $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

option(SPACEMACHINE_DEV_MODE "SpaceMachine Development Settings" OFF)
option(SPACEMACHINE_INSTALL "Generate SpaceMachine install rules"
       ${PROJECT_IS_TOP_LEVEL}
)

if (SPACEMACHINE_DEV_MODE AND PROJECT_IS_TOP_LEVEL)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
    add_subdirectory(dev)
endif ()

if (SPACEMACHINE_INSTALL)
    include(CMakePackageConfigHelpers)

    set(SPACEMACHINE_INSTALL_CMAKEDIR
        ${CMAKE_INSTALL_LIBDIR}/cmake/SpaceMachine
    )

    install(TARGETS SpaceMachine EXPORT SpaceMachineTargets)
    install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
    install(
        EXPORT SpaceMachineTargets
        FILE SpaceMachineTargets.cmake
        NAMESPACE SpaceMachine::
        DESTINATION ${SPACEMACHINE_INSTALL_CMAKEDIR}
    )

    configure_package_config_file(
        cmake/SpaceMachineConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/SpaceMachineConfig.cmake
        INSTALL_DESTINATION ${SPACEMACHINE_INSTALL_CMAKEDIR}
    )
    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/SpaceMachineConfigVersion.cmake
        COMPATIBILITY SameMajorVersion ARCH_INDEPENDENT
    )
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/SpaceMachineConfig.cmake
                  ${CMAKE_CURRENT_BINARY_DIR}/SpaceMachineConfigVersion.cmake
            DESTINATION ${SPACEMACHINE_INSTALL_CMAKEDIR}
    )
endif ()
