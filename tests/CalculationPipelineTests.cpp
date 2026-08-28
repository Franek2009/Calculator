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
