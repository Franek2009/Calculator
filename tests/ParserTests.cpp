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

TEST_CASE("Parser recognizes addition")
{
    Calculator::Lexer lexer("2+3");

    auto tokens = lexer.tokenize();

    Calculator::Parser parser(tokens);

    auto result = parser.parse();

    REQUIRE(result.type == Calculator::ExpressionType::BinaryOperation);
    REQUIRE(result.operation == Calculator::Operator::Add);

    REQUIRE(result.left->type == Calculator::ExpressionType::Number);
    REQUIRE(result.left->value == 2);

    REQUIRE(result.right->type == Calculator::ExpressionType::Number);
    REQUIRE(result.right->value == 3);
}
TEST_CASE("Parser recognizes subtraction")
{
    Calculator::Lexer lexer("5-2");

    auto tokens = lexer.tokenize();

    Calculator::Parser parser(tokens);

    auto result = parser.parse();

    REQUIRE(result.type == Calculator::ExpressionType::BinaryOperation);
    REQUIRE(result.operation == Calculator::Operator::Subtract);

    REQUIRE(result.left->type == Calculator::ExpressionType::Number);
    REQUIRE(result.left->value == 5);

    REQUIRE(result.right->type == Calculator::ExpressionType::Number);
    REQUIRE(result.right->value == 2);
}
TEST_CASE("Parser recognizes multiplication"){
    Calculator::Lexer lexer("3*4");

    auto tokens = lexer.tokenize();

    Calculator::Parser parser(tokens);

    auto result = parser.parse();

    REQUIRE(result.type == Calculator::ExpressionType::BinaryOperation);
    REQUIRE(result.operation == Calculator::Operator::Multiply);

    REQUIRE(result.left->type == Calculator::ExpressionType::Number);
    REQUIRE(result.left->value == 3);

    REQUIRE(result.right->type == Calculator::ExpressionType::Number);
    REQUIRE(result.right->value == 4);
}
TEST_CASE("Parser recognizes division")
{
    Calculator::Lexer lexer("10/2");
    auto tokens = lexer.tokenize();

    Calculator::Parser parser(tokens);

    auto result = parser.parse();

    REQUIRE(result.type == Calculator::ExpressionType::BinaryOperation);
    REQUIRE(result.operation == Calculator::Operator::Divide);

    REQUIRE(result.left->type == Calculator::ExpressionType::Number);
    REQUIRE(result.left->value == 10);

    REQUIRE(result.right->type == Calculator::ExpressionType::Number);
    REQUIRE(result.right->value == 2);
}
TEST_CASE("Parser recognizes exponentiation")
{
    Calculator::Lexer lexer("2^3");

    auto tokens = lexer.tokenize();

    Calculator::Parser parser(tokens);

    auto result = parser.parse();

    REQUIRE(result.type == Calculator::ExpressionType::BinaryOperation);
    REQUIRE(result.operation == Calculator::Operator::Power);

    REQUIRE(result.left->type == Calculator::ExpressionType::Number);
    REQUIRE(result.left->value == 2);

    REQUIRE(result.right->type == Calculator::ExpressionType::Number);
    REQUIRE(result.right->value == 3);
}
