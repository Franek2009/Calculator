#include <catch2/catch_test_macros.hpp>
#include <stdexcept>
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
TEST_CASE("Lexer recognizes decimal numbers"){
    Calculator::Lexer lexer("2.5+3.7");
    auto tokens = lexer.tokenize();

    REQUIRE(tokens.size() == 3);

    REQUIRE(tokens[0].type == Calculator::TokenType::Number);
    REQUIRE(tokens[0].value == "2.5");

    REQUIRE(tokens[1].type == Calculator::TokenType::Plus);
    REQUIRE(tokens[1].value == "+");

    REQUIRE(tokens[2].type == Calculator::TokenType::Number);
    REQUIRE(tokens[2].value == "3.7");
}
TEST_CASE("Lexer rejects multiple decimal points")
{
    Calculator::Lexer lexer("2.5.7");

    REQUIRE_THROWS_AS(lexer.tokenize(), std::invalid_argument);
}

TEST_CASE("Lexer rejects a standalone decimal point")
{
    Calculator::Lexer lexer(".");

    REQUIRE_THROWS_AS(lexer.tokenize(), std::invalid_argument);
}

TEST_CASE("Lexer rejects decimal point without a number")
{
    Calculator::Lexer lexer("2+.");

    REQUIRE_THROWS_AS(lexer.tokenize(), std::invalid_argument);
}
