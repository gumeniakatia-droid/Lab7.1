cmake_minimum_required(VERSION 3.21)
project(RootFindingLab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Static library
add_library(RootFindingLib STATIC
    src/RootFindingTask.cpp
)

target_include_directories(RootFindingLib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Demo program
add_executable(RootFindingDemo
    demo/main.cpp
)

target_link_libraries(RootFindingDemo
    PRIVATE RootFindingLib
)

# Antipattern demonstration
add_executable(AntipatternDemo
    antipattern/main.cpp
)

# Google Test
find_package(GTest CONFIG REQUIRED)

enable_testing()

add_executable(RootFindingTests
    tests/RootFindingTests.cpp
)

target_link_libraries(RootFindingTests
    PRIVATE
        RootFindingLib
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(RootFindingTests)
