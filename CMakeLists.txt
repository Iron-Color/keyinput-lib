cmake_minimum_required(VERSION 3.16)
project(KeyInputLib VERSION 1.0.0 LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(keyinput STATIC
    src/KeyInput.cpp
    src/KeyboardDeviceFinder.cpp
)

add_library(KeyInputLib::keyinput ALIAS keyinput)

target_include_directories(keyinput
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_features(keyinput PUBLIC cxx_std_17)

add_executable(keyinput_example
    examples/main.cpp
    examples/ConsoleDeviceSelector.cpp
)

target_include_directories(keyinput_example
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/examples
)

target_link_libraries(keyinput_example
    PRIVATE
        keyinput
)

install(
    TARGETS keyinput
    EXPORT KeyInputLibTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/KeyInputLib
)

install(
    EXPORT KeyInputLibTargets
    FILE KeyInputLibTargets.cmake
    NAMESPACE KeyInputLib::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/KeyInputLib
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/KeyInputLibConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/KeyInputLibConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/KeyInputLib
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/KeyInputLibConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/KeyInputLibConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/KeyInputLibConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/KeyInputLib
)
