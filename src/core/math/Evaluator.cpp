#include "Evaluator.h"

#include "BasicOperations.h"
#include "CalculatorError.h"

#include <stdexcept>

namespace Calculator
{
    double Evaluator::evaluate(const Expression& expression) const
    {
        try
        {
            switch (expression.type)
            {
                case ExpressionType::Number:
                    return expression.value;

                case ExpressionType::BinaryOperation:
                {
                    if (!expression.left || !expression.right)
                    {
                        throw CalculatorError(
                            ErrorCategory::Evaluation,
                            expression.position,
                            "Binary operation requires two operands"
                        );
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

                    throw CalculatorError(
                        ErrorCategory::Evaluation,
                        expression.position,
                        "Unsupported operator"
                    );
                }

                case ExpressionType::UnaryOperation:
                {
                    if (!expression.operand)
                    {
                        throw CalculatorError(
                            ErrorCategory::Evaluation,
                            expression.position,
                            "Unary operation requires an operand"
                        );
                    }

                    switch (expression.unaryOperation)
                    {
                        case UnaryOperator::Negate:
                            return -evaluate(*expression.operand);
                    }

                    throw CalculatorError(
                        ErrorCategory::Evaluation,
                        expression.position,
                        "Unsupported unary operator"
                    );
                }

                case ExpressionType::FunctionCall:
                {
                    if (!expression.operand)
                    {
                        throw CalculatorError(
                            ErrorCategory::Evaluation,
                            expression.position,
                            "Function call requires an argument"
                        );
                    }

                    switch (expression.function)
                    {
                        case Function::SquareRoot:
                            return BasicOperations::squareRoot(evaluate(*expression.operand));
                    }

                    throw CalculatorError(
                        ErrorCategory::Evaluation,
                        expression.position,
                        "Unsupported function"
                    );
                }
            }

            throw CalculatorError(
                ErrorCategory::Evaluation,
                expression.position,
                "Unsupported expression type"
            );
        }
        catch (const CalculatorError&)
        {
            throw;
        }
        catch (const std::invalid_argument& error)
        {
            throw CalculatorError(ErrorCategory::Evaluation,
                                  expression.position,
                                  error.what());
        }
    }
}
