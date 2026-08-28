#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <stdexcept>
#include <utility>

#include "../src/core/math/Evaluator.h"

namespace
{
    Calculator::Expression number(double value)
    {
        return {
            Calculator::ExpressionType::Number,
            value,
            Calculator::Operator::Add,
            nullptr,
            nullptr
        };
    }

    Calculator::Expression binary(Calculator::Operator operation,
                                  Calculator::Expression left,
                                  Calculator::Expression right)
    {
        return {
            Calculator::ExpressionType::BinaryOperation,
            0,
            operation,
            std::make_unique<Calculator::Expression>(std::move(left)),
            std::make_unique<Calculator::Expression>(std::move(right))
        };
    }
}

TEST_CASE("Evaluator returns the value of a number expression")
{
    Calculator::Evaluator evaluator;
    const auto expression = number(42.5);

    REQUIRE(evaluator.evaluate(expression) == 42.5);
}

TEST_CASE("Evaluator evaluates every binary operator")
{
    Calculator::Evaluator evaluator;

    REQUIRE(evaluator.evaluate(binary(Calculator::Operator::Add, number(2), number(3))) == 5);
    REQUIRE(evaluator.evaluate(binary(Calculator::Operator::Subtract, number(5), number(3))) == 2);
    REQUIRE(evaluator.evaluate(binary(Calculator::Operator::Multiply, number(2), number(3))) == 6);
    REQUIRE(evaluator.evaluate(binary(Calculator::Operator::Divide, number(8), number(2))) == 4);
    REQUIRE(evaluator.evaluate(binary(Calculator::Operator::Power, number(2), number(3))) == 8);
}

TEST_CASE("Evaluator recursively evaluates nested expressions")
{
    Calculator::Evaluator evaluator;
    const auto expression = binary(
        Calculator::Operator::Add,
        number(2),
        binary(Calculator::Operator::Multiply, number(3), number(4))
    );

    REQUIRE(evaluator.evaluate(expression) == 14);
}

TEST_CASE("Evaluator evaluates right-associative exponentiation")
{
    Calculator::Evaluator evaluator;
    const auto expression = binary(
        Calculator::Operator::Power,
        number(2),
        binary(Calculator::Operator::Power, number(3), number(2))
    );

    REQUIRE(evaluator.evaluate(expression) == 512);
}

TEST_CASE("Evaluator propagates division by zero")
{
    Calculator::Evaluator evaluator;
    const auto expression = binary(Calculator::Operator::Divide, number(1), number(0));

    REQUIRE_THROWS_AS(evaluator.evaluate(expression), std::invalid_argument);
}

TEST_CASE("Evaluator rejects binary expressions with a missing operand")
{
    Calculator::Evaluator evaluator;
    const Calculator::Expression expression{
        Calculator::ExpressionType::BinaryOperation,
        0,
        Calculator::Operator::Add,
        nullptr,
        nullptr
    };

    REQUIRE_THROWS_AS(evaluator.evaluate(expression), std::invalid_argument);
}
