#include <catch2/catch_test_macros.hpp>

#include "../src/core/math/Lexer.h"

TEST_CASE("Lexer recognizes basic arithmetic")
{
    Calculator::Lexer lexer("2+3");

    auto tokens = lexer.tokenize();

    REQUIRE(tokens.size() == 3);

    REQUIRE(tokens[0].type == Calculator::TokenType::Number);
    REQUIRE(tokens[0].value == "2");

    REQUIRE(tokens[1].type == Calculator::TokenType::Plus);
    REQUIRE(tokens[1].value == "+");

    REQUIRE(tokens[2].type == Calculator::TokenType::Number);
    REQUIRE(tokens[2].value == "3");
}
