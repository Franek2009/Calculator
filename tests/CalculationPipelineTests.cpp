#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <stdexcept>
#include <string>

#include "../src/core/math/Evaluator.h"
#include "../src/core/math/Lexer.h"
#include "../src/core/math/Parser.h"

namespace
{
    double calculate(const std::string& input)
    {
        Calculator::Lexer lexer(input);
        const auto tokens = lexer.tokenize();
        Calculator::Parser parser(tokens);
        const auto expression = parser.parse();
        Calculator::Evaluator evaluator;

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
}

TEST_CASE("Calculation pipeline evaluates square root function calls")
{
    REQUIRE(calculate("sqrt(9)") == 3);
    REQUIRE(calculate("sqrt(2.25)") == Catch::Approx(1.5));
    REQUIRE(calculate("sqrt(4+5)") == 3);
    REQUIRE(calculate("2*sqrt(9)+1") == 7);
}

TEST_CASE("Calculation pipeline gives exponentiation precedence over unary negation")
{
    REQUIRE(calculate("-2^2") == -4);
    REQUIRE(calculate("2^-2") == Catch::Approx(0.25));
    REQUIRE(calculate("sqrt(9)^2") == 9);
}

TEST_CASE("Calculation pipeline reports function errors")
{
    REQUIRE_THROWS_AS(calculate("sqrt(-1)"), std::invalid_argument);
    REQUIRE_THROWS_AS(calculate("sin(1)"), std::invalid_argument);
    REQUIRE_THROWS_AS(calculate("sqrt(9"), std::invalid_argument);
}
