#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <cmath>
#include <numbers>
#include <stdexcept>
#include <string>

#include "../src/core/math/CalculatorError.h"
#include "../src/core/math/Evaluator.h"
#include "../src/core/math/Lexer.h"
#include "../src/core/math/Parser.h"

namespace
{
    double calculate(const std::string& input,
                     Calculator::AngleMode angleMode = Calculator::AngleMode::Radians)
    {
        Calculator::Lexer lexer(input);
        const auto tokens = lexer.tokenize();
        Calculator::Parser parser(tokens);
        const auto expression = parser.parse();
        Calculator::Evaluator evaluator(angleMode);

        return evaluator.evaluate(expression);
    }
}

TEST_CASE("Calculation pipeline respects multiplication precedence")
{
    REQUIRE(calculate("2+3*4") == 14);
}

TEST_CASE("Calculation pipeline respects parentheses")
{
    REQUIRE(calculate("(2+3)*4") == 20);
}

TEST_CASE("Calculation pipeline evaluates right-associative exponentiation")
{
    REQUIRE(calculate("2^3^2") == 512);
}

TEST_CASE("Calculation pipeline handles combined operators")
{
    REQUIRE(calculate("(2+3)^2/5") == 5);
}

TEST_CASE("Calculation pipeline handles decimal values")
{
    REQUIRE(calculate("1.5+2.5") == Catch::Approx(4.0));
}

TEST_CASE("Calculation pipeline reports division by zero")
{
    REQUIRE_THROWS_AS(calculate("1/(2-2)"), std::invalid_argument);

    try
    {
        calculate("1/(2-2)");
        FAIL("Expected an evaluation error");
    }
    catch (const Calculator::CalculatorError& error)
    {
        REQUIRE(error.category() == Calculator::ErrorCategory::Evaluation);
        REQUIRE(error.position() == 1);
        REQUIRE(std::string(error.what()) ==
                "Evaluation error at position 2: Division by zero");
    }
}

TEST_CASE("Calculation pipeline evaluates square root function calls")
{
    REQUIRE(calculate("sqrt(9)") == 3);
    REQUIRE(calculate("sqrt(2.25)") == Catch::Approx(1.5));
    REQUIRE(calculate("sqrt(4+5)") == 3);
    REQUIRE(calculate("2*sqrt(9)+1") == 7);
}

TEST_CASE("Calculation pipeline evaluates trigonometric functions in radians")
{
    REQUIRE(calculate("sin(0)") == 0);
    REQUIRE(calculate("cos(0)") == 1);
    REQUIRE(calculate("tan(0)") == 0);
    REQUIRE(calculate("tan(0.5)") == Catch::Approx(std::tan(0.5)));
    REQUIRE(calculate("sin(cos(0))") == Catch::Approx(std::sin(1.0)));
    REQUIRE(calculate("sin(-0.5)") == Catch::Approx(std::sin(-0.5)));

    const double identity = calculate("sin(0.5)^2+cos(0.5)^2");
    REQUIRE(identity == Catch::Approx(1.0));
}

TEST_CASE("Calculation pipeline evaluates pi, absolute value, and logarithms")
{
    REQUIRE(calculate("pi") == Catch::Approx(std::numbers::pi));
    REQUIRE(calculate("2*pi") == Catch::Approx(2 * std::numbers::pi));
    REQUIRE(calculate("-pi^2") == Catch::Approx(-std::numbers::pi * std::numbers::pi));
    REQUIRE(calculate("sin(pi/2)") == Catch::Approx(1.0));
    REQUIRE(calculate("cos(pi)") == Catch::Approx(-1.0));
    REQUIRE(calculate("abs(-5)") == 5);
    REQUIRE(calculate("ln(1)") == 0);
    REQUIRE(calculate("log10(1000)") == 3);
    REQUIRE(calculate("ln(abs(-2))") == Catch::Approx(std::log(2.0)));
    REQUIRE(calculate("e") == Catch::Approx(std::numbers::e));
    REQUIRE(calculate("2*e") == Catch::Approx(2 * std::numbers::e));
    REQUIRE(calculate("ln(e)") == Catch::Approx(1.0));
    REQUIRE(calculate("e^2") == Catch::Approx(std::numbers::e * std::numbers::e));
    REQUIRE(calculate("log(2,8)") == Catch::Approx(3.0));
    REQUIRE(calculate("log(10,1000)") == Catch::Approx(3.0));
    REQUIRE(calculate("log(3,81)") == Catch::Approx(4.0));
    REQUIRE(calculate("log(2,sqrt(16))") == Catch::Approx(2.0));
    REQUIRE(calculate("log(2,log(3,81))") == Catch::Approx(2.0));
}

TEST_CASE("Calculation pipeline supports radian and degree angle modes")
{
    REQUIRE(calculate("sin(pi/2)") == Catch::Approx(1.0).epsilon(1e-12));
    REQUIRE(calculate("cos(pi)") == Catch::Approx(-1.0).epsilon(1e-12));
    REQUIRE(calculate("tan(pi/4)") == Catch::Approx(1.0).epsilon(1e-12));

    REQUIRE(calculate("sin(90)", Calculator::AngleMode::Degrees) ==
            Catch::Approx(1.0).epsilon(1e-12));
    REQUIRE(calculate("cos(180)", Calculator::AngleMode::Degrees) ==
            Catch::Approx(-1.0).epsilon(1e-12));
    REQUIRE(calculate("tan(45)", Calculator::AngleMode::Degrees) ==
            Catch::Approx(1.0).epsilon(1e-12));
    REQUIRE(calculate("sin(90)") !=
            Catch::Approx(1.0).epsilon(1e-12));
}

TEST_CASE("Calculation pipeline reports logarithm domain errors")
{
    const auto requireEvaluationError = [](const std::string& input,
                                           const std::string& description)
    {
        try
        {
            calculate(input);
            FAIL("Expected an evaluation error");
        }
        catch (const Calculator::CalculatorError& error)
        {
            REQUIRE(error.category() == Calculator::ErrorCategory::Evaluation);
            REQUIRE(error.position() == 0);
            REQUIRE(std::string(error.what()) ==
                    "Evaluation error at position 1: " + description);
        }
    };

    requireEvaluationError("ln(0)", "Natural logarithm of a non-positive number");
    requireEvaluationError("ln(-1)", "Natural logarithm of a non-positive number");
    requireEvaluationError("log10(0)", "Base-10 logarithm of a non-positive number");
    requireEvaluationError("log10(-1)", "Base-10 logarithm of a non-positive number");
    requireEvaluationError("log(0,8)", "Logarithm base must be positive");
    requireEvaluationError("log(-2,8)", "Logarithm base must be positive");
    requireEvaluationError("log(1,8)", "Logarithm base must not equal 1");
    requireEvaluationError("log(2,0)", "Logarithm value must be positive");
    requireEvaluationError("log(2,-8)", "Logarithm value must be positive");
}

TEST_CASE("Calculation pipeline gives exponentiation precedence over unary negation")
{
    REQUIRE(calculate("--2") == 2);
    REQUIRE(calculate("-2^2") == -4);
    REQUIRE(calculate("2^-2") == Catch::Approx(0.25));
    REQUIRE(calculate("2^-2^2") == Catch::Approx(0.0625));
    REQUIRE(calculate("sqrt(9)^2") == 9);
}

TEST_CASE("Calculation pipeline reports function errors")
{
    REQUIRE_THROWS_AS(calculate("sqrt(-1)"), std::invalid_argument);
    REQUIRE_THROWS_AS(calculate("unknown(1)"), std::invalid_argument);
    REQUIRE_THROWS_AS(calculate("sqrt(9"), std::invalid_argument);

    try
    {
        calculate("sqrt(-1)");
        FAIL("Expected an evaluation error");
    }
    catch (const Calculator::CalculatorError& error)
    {
        REQUIRE(error.category() == Calculator::ErrorCategory::Evaluation);
        REQUIRE(error.position() == 0);
        REQUIRE(std::string(error.what()) ==
                "Evaluation error at position 1: Square root of a negative number");
    }
}
