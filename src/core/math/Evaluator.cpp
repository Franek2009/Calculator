#include "Evaluator.h"

#include "BasicOperations.h"

#include <stdexcept>

namespace Calculator
{
    double Evaluator::evaluate(const Expression& expression) const
    {
        switch (expression.type)
        {
            case ExpressionType::Number:
                return expression.value;

            case ExpressionType::BinaryOperation:
                if (!expression.left || !expression.right)
                {
                    throw std::invalid_argument("Binary operation requires two operands");
                }

                const double leftValue = evaluate(*expression.left);
                const double rightValue = evaluate(*expression.right);

                switch (expression.operation)
                {
                    case Operator::Add:
                        return BasicOperations::add(leftValue, rightValue);

                    case Operator::Subtract:
                        return BasicOperations::subtract(leftValue, rightValue);

                    case Operator::Multiply:
                        return BasicOperations::multiply(leftValue, rightValue);

                    case Operator::Divide:
                        return BasicOperations::divide(leftValue, rightValue);

                    case Operator::Power:
                        return BasicOperations::power(leftValue, rightValue);
                }

                throw std::invalid_argument("Unsupported operator");
        }

        throw std::invalid_argument("Unsupported expression type");
    }
}
