#include <catch2/catch_test_macros.hpp>

#include "../src/core/math/Lexer.h"
#include "../src/core/math/Parser.h"

TEST_CASE("Parser recognizes a single number")
{
    Calculator::Lexer lexer("42");

    auto tokens = lexer.tokenize();

    Calculator::Parser parser(tokens);

    auto result = parser.parse();

    REQUIRE(result.type == Calculator::ExpressionType::Number);
    REQUIRE(result.value == 42);
}
