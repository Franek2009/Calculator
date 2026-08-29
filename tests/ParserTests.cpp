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
    requireFunction("asin(1)", Calculator::Function::ArcSine);
    requireFunction("acos(0)", Calculator::Function::ArcCosine);
    requireFunction("atan(1)", Calculator::Function::ArcTangent);
    requireFunction("abs(0)", Calculator::Function::AbsoluteValue);
    requireFunction("ln(1)", Calculator::Function::NaturalLogarithm);
    requireFunction("log10(1)", Calculator::Function::Base10Logarithm);
}

TEST_CASE("Parser nests forward and inverse trigonometric calls")
{
    Calculator::Lexer lexer("sin(asin(0.5))");
    Calculator::Parser parser(lexer.tokenize());
    const auto result = parser.parse();

    REQUIRE(result.function == Calculator::Function::Sine);
    REQUIRE(result.arguments[0].function == Calculator::Function::ArcSine);
    REQUIRE(result.arguments[0].arguments[0].value == 0.5);
}

TEST_CASE("Parser validates inverse trigonometric syntax")
{
    requireSyntaxError("asin()", 0, "Function 'asin' expects 1 argument");
    requireSyntaxError("asin(1,2)", 0, "Function 'asin' expects 1 argument");
    requireSyntaxError("asin", 4, "Expected '(' after function name");
    requireSyntaxError("ASIN(1)", 0, "Unknown identifier 'ASIN'");
    requireSyntaxError("Asin(1)", 0, "Unknown identifier 'Asin'");
    requireSyntaxError("2asin(1)", 1, "Unexpected token 'asin' after expression");
    requireSyntaxError("acos(1", 6, "Expected ')'");
}

TEST_CASE("Parser builds postfix expressions with operator positions")
{
    const auto parse = [](const std::string& input)
    {
        Calculator::Lexer lexer(input);
        Calculator::Parser parser(lexer.tokenize());
        return parser.parse();
    };

    const auto factorial = parse("5!");
    REQUIRE(factorial.type == Calculator::ExpressionType::PostfixOperation);
    REQUIRE(factorial.postfixOperation == Calculator::PostfixOperator::Factorial);
    REQUIRE(factorial.position == 1);
    REQUIRE(factorial.operand->value == 5);

    const auto repeated = parse("50%!");
    REQUIRE(repeated.postfixOperation == Calculator::PostfixOperator::Factorial);
    REQUIRE(repeated.position == 3);
    REQUIRE(repeated.operand->postfixOperation == Calculator::PostfixOperator::Percentage);
    REQUIRE(repeated.operand->position == 2);
}

TEST_CASE("Parser reserves consecutive factorial operators")
{
    requireSyntaxError("5!!", 2, "Double factorial is not supported");
    requireSyntaxError("3!!!", 2, "Double factorial is not supported");
    requireSyntaxError("5!!!%", 2, "Double factorial is not supported");
}

TEST_CASE("Parser gives postfix operators precedence over power and unary negation")
{
    Calculator::Lexer lexer("-3!+5!^2+2^3!+50%^2");
    Calculator::Parser parser(lexer.tokenize());
    REQUIRE_NOTHROW(parser.parse());

    Calculator::Lexer groupedLexer("(-3)!");
    Calculator::Parser groupedParser(groupedLexer.tokenize());
    const auto grouped = groupedParser.parse();
    REQUIRE(grouped.type == Calculator::ExpressionType::PostfixOperation);
    REQUIRE(grouped.operand->type == Calculator::ExpressionType::UnaryOperation);
}

TEST_CASE("Parser rejects malformed postfix syntax")
{
    requireSyntaxError("!5", 0, "Expected expression");
    requireSyntaxError("%5", 0, "Expected expression");
    requireSyntaxError("5!2", 2, "Unexpected token '2' after expression");
    requireSyntaxError("5!^", 3, "Expected expression");
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

TEST_CASE("Parser recognizes Ans in expressions and function arguments")
{
    const auto parse = [](const std::string& input)
    {
        Calculator::Lexer lexer(input);
        Calculator::Parser parser(lexer.tokenize());
        return parser.parse();
    };

    const auto answer = parse("Ans");
    REQUIRE(answer.type == Calculator::ExpressionType::Constant);
    REQUIRE(answer.constant == Calculator::Constant::Ans);
    REQUIRE(answer.position == 0);

    const auto sum = parse("Ans+2");
    REQUIRE(sum.left->constant == Calculator::Constant::Ans);

    const auto squareRoot = parse("sqrt(Ans)");
    REQUIRE(squareRoot.arguments[0].constant == Calculator::Constant::Ans);

    const auto logarithm = parse("log(2,Ans)");
    REQUIRE(logarithm.arguments[1].constant == Calculator::Constant::Ans);
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
    requireSyntaxError("2Ans", 1, "Unexpected token 'Ans' after expression");
    requireSyntaxError("Ans()", 3, "Unexpected token '(' after expression");
    requireSyntaxError("ans", 0, "Unknown identifier 'ans'");
    requireSyntaxError("ANS", 0, "Unknown identifier 'ANS'");
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
