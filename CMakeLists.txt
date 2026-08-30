cmake_minimum_required(VERSION 3.21)

project(Calculator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)
include(GNUInstallDirs)

option(CALCULATOR_DEPLOY_QT
    "Install Qt runtime libraries and plugins with the application"
    OFF
)

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

add_library(CalculatorUi STATIC
    src/ui/CalculatorWindow.cpp
)

target_link_libraries(CalculatorUi PUBLIC CalculatorCore Qt6::Widgets)

add_executable(Calculator
    src/main.cpp
)

target_link_libraries(Calculator PRIVATE CalculatorUi)

set_target_properties(Calculator PROPERTIES
    WIN32_EXECUTABLE TRUE
)

install(TARGETS Calculator
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES
    LICENSE
    README.md
    DESTINATION .
)

if(CALCULATOR_DEPLOY_QT)
    if(Qt6_VERSION VERSION_LESS 6.5)
        message(FATAL_ERROR
            "CALCULATOR_DEPLOY_QT requires Qt 6.5 or newer"
        )
    endif()

    qt_finalize_target(Calculator)
    qt_generate_deploy_app_script(
        TARGET Calculator
        OUTPUT_SCRIPT calculator_deploy_script
        NO_TRANSLATIONS
    )
    install(SCRIPT ${calculator_deploy_script})
endif()

if(BUILD_TESTING)
    find_package(Qt6 REQUIRED COMPONENTS Test)
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

    add_executable(CalculatorUiTests
        tests/CalculatorWindowTests.cpp
    )

    target_link_libraries(CalculatorUiTests
        PRIVATE
            CalculatorUi
            Qt6::Test
    )

    set_target_properties(CalculatorUiTests PROPERTIES AUTOMOC ON)

    add_test(NAME CalculatorUiTests COMMAND CalculatorUiTests)
    set_tests_properties(CalculatorUiTests PROPERTIES
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen;QT_QPA_PLATFORMTHEME="
    )
endif()
