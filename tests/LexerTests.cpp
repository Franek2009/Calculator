#include <catch2/catch_test_macros.hpp>
#include <stdexcept>

#include "../src/core/math/CalculatorError.h"
#include "../src/core/math/Lexer.h"

TEST_CASE("Lexer recognizes basic arithmetic")
{
    Calculator::Lexer lexer("2+3");

    auto tokens = lexer.tokenize();

    REQUIRE(tokens.size() == 3);

    REQUIRE(tokens[0].type == Calculator::TokenType::Number);
    REQUIRE(tokens[0].value == "2");
    REQUIRE(tokens[0].position == 0);

    REQUIRE(tokens[1].type == Calculator::TokenType::Plus);
    REQUIRE(tokens[1].value == "+");
    REQUIRE(tokens[1].position == 1);

    REQUIRE(tokens[2].type == Calculator::TokenType::Number);
    REQUIRE(tokens[2].value == "3");
    REQUIRE(tokens[2].position == 2);
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
TEST_CASE("Lexer recognizes a function")
{
    Calculator::Lexer lexer("sqrt(16)");

    auto tokens = lexer.tokenize();

    REQUIRE(tokens.size() == 4);

    REQUIRE(tokens[0].type == Calculator::TokenType::Identifier);
    REQUIRE(tokens[0].value == "sqrt");
    REQUIRE(tokens[0].position == 0);

    REQUIRE(tokens[1].type == Calculator::TokenType::LeftParenthesis);
    REQUIRE(tokens[1].value == "(");
    REQUIRE(tokens[1].position == 4);

    REQUIRE(tokens[2].type == Calculator::TokenType::Number);
    REQUIRE(tokens[2].value == "16");
    REQUIRE(tokens[2].position == 5);

    REQUIRE(tokens[3].type == Calculator::TokenType::RightParenthesis);
    REQUIRE(tokens[3].value == ")");
    REQUIRE(tokens[3].position == 7);
}

TEST_CASE("Lexer reports the position of an unknown character")
{
    Calculator::Lexer lexer("2@3");

    try
    {
        lexer.tokenize();
        FAIL("Expected a lexical error");
    }
    catch (const Calculator::CalculatorError& error)
    {
        REQUIRE(error.category() == Calculator::ErrorCategory::Lexical);
        REQUIRE(error.position() == 1);
        REQUIRE(std::string(error.what()) ==
                "Lexical error at position 2: Unknown character '@'");
    }
}

TEST_CASE("Lexer recognizes trigonometric function names and positions")
{
    Calculator::Lexer lexer("sin(0)+cos(0)+tan(0)");
    const auto tokens = lexer.tokenize();

    REQUIRE(tokens.size() == 14);

    REQUIRE(tokens[0].type == Calculator::TokenType::Identifier);
    REQUIRE(tokens[0].value == "sin");
    REQUIRE(tokens[0].position == 0);

    REQUIRE(tokens[5].type == Calculator::TokenType::Identifier);
    REQUIRE(tokens[5].value == "cos");
    REQUIRE(tokens[5].position == 7);

    REQUIRE(tokens[10].type == Calculator::TokenType::Identifier);
    REQUIRE(tokens[10].value == "tan");
    REQUIRE(tokens[10].position == 14);
}

TEST_CASE("Lexer recognizes constants and alphanumeric identifiers")
{
    Calculator::Lexer lexer("pi log10 pi2 2pi");
    const auto tokens = lexer.tokenize();

    REQUIRE(tokens.size() == 5);

    REQUIRE(tokens[0].type == Calculator::TokenType::Identifier);
    REQUIRE(tokens[0].value == "pi");
    REQUIRE(tokens[0].position == 0);

    REQUIRE(tokens[1].type == Calculator::TokenType::Identifier);
    REQUIRE(tokens[1].value == "log10");
    REQUIRE(tokens[1].position == 3);

    REQUIRE(tokens[2].type == Calculator::TokenType::Identifier);
    REQUIRE(tokens[2].value == "pi2");
    REQUIRE(tokens[2].position == 9);

    REQUIRE(tokens[3].type == Calculator::TokenType::Number);
    REQUIRE(tokens[3].value == "2");
    REQUIRE(tokens[3].position == 13);

    REQUIRE(tokens[4].type == Calculator::TokenType::Identifier);
    REQUIRE(tokens[4].value == "pi");
    REQUIRE(tokens[4].position == 14);
}

TEST_CASE("Lexer recognizes e, log, and argument separators")
{
    Calculator::Lexer lexer("e log(2,8)");
    const auto tokens = lexer.tokenize();

    REQUIRE(tokens.size() == 7);
    REQUIRE(tokens[0].type == Calculator::TokenType::Identifier);
    REQUIRE(tokens[0].value == "e");
    REQUIRE(tokens[0].position == 0);
    REQUIRE(tokens[1].type == Calculator::TokenType::Identifier);
    REQUIRE(tokens[1].value == "log");
    REQUIRE(tokens[1].position == 2);
    REQUIRE(tokens[4].type == Calculator::TokenType::Comma);
    REQUIRE(tokens[4].value == ",");
    REQUIRE(tokens[4].position == 7);
}

TEST_CASE("Lexer recognizes Ans as an identifier")
{
    Calculator::Lexer lexer("Ans+2");
    const auto tokens = lexer.tokenize();

    REQUIRE(tokens.size() == 3);
    REQUIRE(tokens[0].type == Calculator::TokenType::Identifier);
    REQUIRE(tokens[0].value == "Ans");
    REQUIRE(tokens[0].position == 0);
}

TEST_CASE("Lexer recognizes inverse trigonometric names as identifiers")
{
    Calculator::Lexer lexer("asin(1)+acos(0)+atan(1)");
    const auto tokens = lexer.tokenize();

    REQUIRE(tokens[0].type == Calculator::TokenType::Identifier);
    REQUIRE(tokens[0].value == "asin");
    REQUIRE(tokens[0].position == 0);
    REQUIRE(tokens[5].type == Calculator::TokenType::Identifier);
    REQUIRE(tokens[5].value == "acos");
    REQUIRE(tokens[5].position == 8);
    REQUIRE(tokens[10].type == Calculator::TokenType::Identifier);
    REQUIRE(tokens[10].value == "atan");
    REQUIRE(tokens[10].position == 16);
}
