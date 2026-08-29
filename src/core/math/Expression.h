#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <memory>
#include <cstddef>

namespace Calculator
{
    enum class ExpressionType
    {
        Number,
        Constant,
        BinaryOperation,
        UnaryOperation,
        FunctionCall
    };

    enum class Operator
    {
        Add,
        Subtract,
        Multiply,
        Divide,
        Power
    };

    enum class UnaryOperator
    {
        Negate
    };

    enum class Function
    {
        SquareRoot,
        Sine,
        Cosine,
        Tangent,
        AbsoluteValue,
        NaturalLogarithm,
        Base10Logarithm
    };

    enum class Constant
    {
        Pi
    };

    struct Expression
    {
        ExpressionType type;
        double value;

        Operator operation;

        std::unique_ptr<Expression> left;
        std::unique_ptr<Expression> right;

        UnaryOperator unaryOperation;
        Function function;
        std::unique_ptr<Expression> operand;

        std::size_t position;
        Constant constant;
    };
}

#endif
