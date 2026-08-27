cmake_minimum_required(VERSION 3.21)

project(Calculator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(Calculator
    src/main.cpp
    src/core/math/BasicOperations.cpp
)

target_link_libraries(Calculator PRIVATE Qt6::Widgets)

include(CTest)
enable_testing()

find_package(Catch2 3 REQUIRED)

add_executable(CalculatorTests
    tests/BasicOperationsTests.cpp
    tests/LexerTests.cpp
    src/core/math/BasicOperations.cpp
    src/core/math/Lexer.cpp
    src/core/math/Parser.cpp
)

target_link_libraries(CalculatorTests
    PRIVATE
        Catch2::Catch2WithMain
)

include(Catch)
catch_discover_tests(CalculatorTests)
