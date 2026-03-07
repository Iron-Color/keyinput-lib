cmake_minimum_required(VERSION 3.16)
project(KeyInputLib VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(keyinput STATIC
    src/KeyInput.cpp
    src/KeyboardDeviceFinder.cpp
    src/KeyboardDeviceSelector.cpp
)

target_include_directories(keyinput
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(keyinput_example
    examples/main.cpp
)

target_link_libraries(keyinput_example
    PRIVATE
        keyinput
)