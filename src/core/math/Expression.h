#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <memory>

namespace Calculator
{
    enum class ExpressionType
    {
        Number,
        BinaryOperation
    };

    enum class Operator
    {
        Add,
        Subtract,
        Multiply,
        Divide,
        Power
    };

    struct Expression
    {
        ExpressionType type;
        double value;

        Operator operation;

        std::unique_ptr<Expression> left;
        std::unique_ptr<Expression> right;
    };
}

#endif
