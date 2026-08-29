#include <catch2/catch_test_macros.hpp>

#include <stdexcept>
#include <string>

#include "../src/core/math/CalculatorError.h"
#include "../src/core/math/Lexer.h"
#include "../src/core/math/Parser.h"

namespace
{
    void requireSyntaxError(const std::string& input,
                            std::size_t expectedPosition,
                            const std::string& description)
    {
        try
        {
            Calculator::Lexer lexer(input);
            const auto tokens = lexer.tokenize();
            Calculator::Parser parser(tokens);
            parser.parse();
            FAIL("Expected a syntax error");
        }
        catch (const Calculator::CalculatorError& error)
        {
            REQUIRE(error.category() == Calculator::ErrorCategory::Syntax);
            REQUIRE(error.position() == expectedPosition);
            REQUIRE(std::string(error.what()) ==
                    "Syntax error at position " +
                    std::to_string(expectedPosition + 1) + ": " + description);
        }
    }
}

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
    REQUIRE(result.position == 0);
    REQUIRE(result.operand == nullptr);
    REQUIRE(result.arguments.size() == 1);
    REQUIRE(result.arguments[0].type == Calculator::ExpressionType::Number);
    REQUIRE(result.arguments[0].value == 16);
}

TEST_CASE("Parser allows an expression as a function argument")
{
    Calculator::Lexer lexer("sqrt(4+5)");
    const auto tokens = lexer.tokenize();
    Calculator::Parser parser(tokens);

    const auto result = parser.parse();

    REQUIRE(result.type == Calculator::ExpressionType::FunctionCall);
    REQUIRE(result.arguments[0].type == Calculator::ExpressionType::BinaryOperation);
    REQUIRE(result.arguments[0].operation == Calculator::Operator::Add);
}

TEST_CASE("Parser recognizes trigonometric function calls")
{
    const auto requireFunction = [](const std::string& input,
                                    Calculator::Function expectedFunction)
    {
        Calculator::Lexer lexer(input);
        const auto tokens = lexer.tokenize();
        Calculator::Parser parser(tokens);
        const auto result = parser.parse();

        REQUIRE(result.type == Calculator::ExpressionType::FunctionCall);
        REQUIRE(result.function == expectedFunction);
        REQUIRE(result.operand == nullptr);
        REQUIRE(result.arguments.size() == 1);
        REQUIRE(result.arguments[0].type == Calculator::ExpressionType::Number);
    };

    requireFunction("sin(0)", Calculator::Function::Sine);
    requireFunction("cos(0)", Calculator::Function::Cosine);
    requireFunction("tan(0)", Calculator::Function::Tangent);
    requireFunction("abs(0)", Calculator::Function::AbsoluteValue);
    requireFunction("ln(1)", Calculator::Function::NaturalLogarithm);
    requireFunction("log10(1)", Calculator::Function::Base10Logarithm);
}

TEST_CASE("Parser nests trigonometric function calls")
{
    Calculator::Lexer lexer("sin(cos(0))");
    const auto tokens = lexer.tokenize();
    Calculator::Parser parser(tokens);
    const auto result = parser.parse();

    REQUIRE(result.function == Calculator::Function::Sine);
    REQUIRE(result.arguments[0].type == Calculator::ExpressionType::FunctionCall);
    REQUIRE(result.arguments[0].function == Calculator::Function::Cosine);
}

TEST_CASE("Parser recognizes symbolic constants")
{
    Calculator::Lexer lexer("pi");
    const auto tokens = lexer.tokenize();
    Calculator::Parser parser(tokens);
    const auto result = parser.parse();

    REQUIRE(result.type == Calculator::ExpressionType::Constant);
    REQUIRE(result.constant == Calculator::Constant::Pi);
    REQUIRE(result.position == 0);

    Calculator::Lexer eLexer("e");
    Calculator::Parser eParser(eLexer.tokenize());
    const auto eResult = eParser.parse();
    REQUIRE(eResult.type == Calculator::ExpressionType::Constant);
    REQUIRE(eResult.constant == Calculator::Constant::E);
}

TEST_CASE("Parser nests logarithm and absolute value calls")
{
    Calculator::Lexer lexer("ln(abs(-2))");
    const auto tokens = lexer.tokenize();
    Calculator::Parser parser(tokens);
    const auto result = parser.parse();

    REQUIRE(result.function == Calculator::Function::NaturalLogarithm);
    REQUIRE(result.arguments[0].type == Calculator::ExpressionType::FunctionCall);
    REQUIRE(result.arguments[0].function == Calculator::Function::AbsoluteValue);
    REQUIRE(result.arguments[0].arguments[0].type == Calculator::ExpressionType::UnaryOperation);
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
    requireSyntaxError("unknown(1)", 0, "Unknown identifier 'unknown'");
    requireSyntaxError("sqrt", 4, "Expected '(' after function name");
    requireSyntaxError("sqrt()", 0, "Function 'sqrt' expects 1 argument");
    requireSyntaxError("sqrt(9", 6, "Expected ')'");
    requireSyntaxError("sqrt(4) 2", 8, "Unexpected token '2' after expression");
}

TEST_CASE("Parser reports positions for invalid operator and parenthesis usage")
{
    requireSyntaxError("2**3", 2, "Expected expression");
    requireSyntaxError("2+", 2, "Expected expression");
    requireSyntaxError("2^-", 3, "Expected expression");
    requireSyntaxError("(2+3", 4, "Expected ')'");
    requireSyntaxError("2+3)", 3, "Unexpected token ')' after expression");
    requireSyntaxError("()", 1, "Expected expression");
    requireSyntaxError("2 3", 2, "Unexpected token '3' after expression");
    requireSyntaxError("+2", 0, "Expected expression");
    requireSyntaxError("++2", 0, "Expected expression");
}

TEST_CASE("Parser rejects constants without explicit operators")
{
    requireSyntaxError("pi()", 2, "Unexpected token '(' after expression");
    requireSyntaxError("pi2", 0, "Unknown identifier 'pi2'");
    requireSyntaxError("2pi", 1, "Unexpected token 'pi' after expression");
    requireSyntaxError("e2", 0, "Unknown identifier 'e2'");
    requireSyntaxError("2e", 1, "Unexpected token 'e' after expression");
}

TEST_CASE("Parser builds function calls with general argument lists")
{
    Calculator::Lexer lexer("log(2,sqrt(16))");
    Calculator::Parser parser(lexer.tokenize());
    const auto result = parser.parse();

    REQUIRE(result.type == Calculator::ExpressionType::FunctionCall);
    REQUIRE(result.function == Calculator::Function::Logarithm);
    REQUIRE(result.operand == nullptr);
    REQUIRE(result.arguments.size() == 2);
    REQUIRE(result.arguments[0].type == Calculator::ExpressionType::Number);
    REQUIRE(result.arguments[0].value == 2);
    REQUIRE(result.arguments[1].type == Calculator::ExpressionType::FunctionCall);
    REQUIRE(result.arguments[1].function == Calculator::Function::SquareRoot);
    REQUIRE(result.arguments[1].arguments.size() == 1);

    Calculator::Lexer nestedLexer("log(2,log(3,81))");
    Calculator::Parser nestedParser(nestedLexer.tokenize());
    const auto nested = nestedParser.parse();
    REQUIRE(nested.arguments[1].function == Calculator::Function::Logarithm);
    REQUIRE(nested.arguments[1].arguments.size() == 2);
}

TEST_CASE("Parser reports function arity errors")
{
    requireSyntaxError("log()", 0, "Function 'log' expects 2 arguments");
    requireSyntaxError("log(2)", 0, "Function 'log' expects 2 arguments");
    requireSyntaxError("log(2,8,16)", 0, "Function 'log' expects 2 arguments");
    requireSyntaxError("sin(1,2)", 0, "Function 'sin' expects 1 argument");
}

TEST_CASE("Parser reports malformed function argument lists")
{
    requireSyntaxError("log(,8)", 4, "Expected expression");
    requireSyntaxError("log(2,)", 6, "Expected expression");
    requireSyntaxError("log(2,,8)", 6, "Expected expression");
    requireSyntaxError("log(2 8)", 6, "Expected ',' or ')' after function argument");
    requireSyntaxError("log(2,8", 7, "Expected ')'");
    requireSyntaxError("2,3", 1, "Unexpected token ',' after expression");
}
