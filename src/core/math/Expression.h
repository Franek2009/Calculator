#ifndef EXPRESSION_H
#define EXPRESSION_H

namespace Calculator
{
    enum class ExpressionType
    {
        Number
    };

    struct Expression
    {
        ExpressionType type;
        double value;
    };
}

#endif
