cmake_minimum_required(VERSION 3.21)

project(Calculator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_library(CalculatorCore STATIC
    src/core/math/BasicOperations.cpp
    src/core/math/CalculatorError.cpp
    src/core/math/Lexer.cpp
    src/core/math/Parser.cpp
    src/core/math/Evaluator.cpp
)

target_include_directories(CalculatorCore
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/core/math
)

add_executable(Calculator
    src/main.cpp
)

target_link_libraries(Calculator PRIVATE CalculatorCore Qt6::Widgets)

include(CTest)
enable_testing()

find_package(Catch2 3 REQUIRED)

add_executable(CalculatorTests
    tests/BasicOperationsTests.cpp
    tests/LexerTests.cpp
    tests/ParserTests.cpp
    tests/EvaluatorTests.cpp
    tests/CalculationPipelineTests.cpp
)

target_link_libraries(CalculatorTests
    PRIVATE
        CalculatorCore
        Catch2::Catch2WithMain
)

include(Catch)
catch_discover_tests(CalculatorTests)
