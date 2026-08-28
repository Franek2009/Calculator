#include <catch2/catch_test_macros.hpp>

#include <stdexcept>
#include <string>

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
TEST_CASE("Parser respects operator precedence")
{
    Calculator::Lexer lexer("2+3*4");

    auto tokens = lexer.tokenize();

    Calculator::Parser parser(tokens);

    auto result = parser.parse();

    REQUIRE(result.type == Calculator::ExpressionType::BinaryOperation);
    REQUIRE(result.operation == Calculator::Operator::Add);

    REQUIRE(result.left->type == Calculator::ExpressionType::Number);
    REQUIRE(result.left->value == 2);

    REQUIRE(result.right->type == Calculator::ExpressionType::BinaryOperation);
    REQUIRE(result.right->operation == Calculator::Operator::Multiply);

    REQUIRE(result.right->left->value == 3);
    REQUIRE(result.right->right->value == 4);
}
TEST_CASE("Parser builds correct AST for mixed precedence")
{
    Calculator::Lexer lexer("2+3*4");

    auto tokens = lexer.tokenize();

    Calculator::Parser parser(tokens);

    auto result = parser.parse();

    REQUIRE(result.type == Calculator::ExpressionType::BinaryOperation);
    REQUIRE(result.operation == Calculator::Operator::Add);

    REQUIRE(result.left->type == Calculator::ExpressionType::Number);
    REQUIRE(result.left->value == 2);

    REQUIRE(result.right->type == Calculator::ExpressionType::BinaryOperation);
    REQUIRE(result.right->operation == Calculator::Operator::Multiply);

    REQUIRE(result.right->left->type == Calculator::ExpressionType::Number);
    REQUIRE(result.right->left->value == 3);

    REQUIRE(result.right->right->type == Calculator::ExpressionType::Number);
    REQUIRE(result.right->right->value == 4);
}
TEST_CASE("Parser respects parentheses")
{
    Calculator::Lexer lexer("(2+3)*4");

    auto tokens = lexer.tokenize();

    Calculator::Parser parser(tokens);

    auto result = parser.parse();

    REQUIRE(result.type == Calculator::ExpressionType::BinaryOperation);
    REQUIRE(result.operation == Calculator::Operator::Multiply);

    REQUIRE(result.left->type == Calculator::ExpressionType::BinaryOperation);
    REQUIRE(result.left->operation == Calculator::Operator::Add);

    REQUIRE(result.left->left->type == Calculator::ExpressionType::Number);
    REQUIRE(result.left->left->value == 2);

    REQUIRE(result.left->right->type == Calculator::ExpressionType::Number);
    REQUIRE(result.left->right->value == 3);

    REQUIRE(result.right->type == Calculator::ExpressionType::Number);
    REQUIRE(result.right->value == 4);
}

TEST_CASE("Parser recognizes square root function calls")
{
    Calculator::Lexer lexer("sqrt(16)");
    const auto tokens = lexer.tokenize();
    Calculator::Parser parser(tokens);

    const auto result = parser.parse();

    REQUIRE(result.type == Calculator::ExpressionType::FunctionCall);
    REQUIRE(result.function == Calculator::Function::SquareRoot);
    REQUIRE(result.operand->type == Calculator::ExpressionType::Number);
    REQUIRE(result.operand->value == 16);
}

TEST_CASE("Parser allows an expression as a function argument")
{
    Calculator::Lexer lexer("sqrt(4+5)");
    const auto tokens = lexer.tokenize();
    Calculator::Parser parser(tokens);

    const auto result = parser.parse();

    REQUIRE(result.type == Calculator::ExpressionType::FunctionCall);
    REQUIRE(result.operand->type == Calculator::ExpressionType::BinaryOperation);
    REQUIRE(result.operand->operation == Calculator::Operator::Add);
}

TEST_CASE("Parser recognizes unary negation with exponentiation precedence")
{
    Calculator::Lexer lexer("-2^2");
    const auto tokens = lexer.tokenize();
    Calculator::Parser parser(tokens);

    const auto result = parser.parse();

    REQUIRE(result.type == Calculator::ExpressionType::UnaryOperation);
    REQUIRE(result.unaryOperation == Calculator::UnaryOperator::Negate);
    REQUIRE(result.operand->type == Calculator::ExpressionType::BinaryOperation);
    REQUIRE(result.operand->operation == Calculator::Operator::Power);
}

TEST_CASE("Parser accepts unary negation as an exponent")
{
    Calculator::Lexer lexer("2^-2");
    const auto tokens = lexer.tokenize();
    Calculator::Parser parser(tokens);

    const auto result = parser.parse();

    REQUIRE(result.type == Calculator::ExpressionType::BinaryOperation);
    REQUIRE(result.operation == Calculator::Operator::Power);
    REQUIRE(result.right->type == Calculator::ExpressionType::UnaryOperation);
}

TEST_CASE("Parser rejects unsupported and malformed function calls")
{
    const auto parse = [](const std::string& input)
    {
        Calculator::Lexer lexer(input);
        const auto tokens = lexer.tokenize();
        Calculator::Parser parser(tokens);
        return parser.parse();
    };

    REQUIRE_THROWS_AS(parse("sin(1)"), std::invalid_argument);
    REQUIRE_THROWS_AS(parse("sqrt"), std::invalid_argument);
    REQUIRE_THROWS_AS(parse("sqrt()"), std::invalid_argument);
    REQUIRE_THROWS_AS(parse("sqrt(9"), std::invalid_argument);
    REQUIRE_THROWS_AS(parse("sqrt(4) 2"), std::invalid_argument);
}
