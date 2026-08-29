#include "Evaluator.h"

#include "BasicOperations.h"
#include "CalculatorError.h"

#include <numbers>
#include <stdexcept>
#include <string>
#include <utility>

namespace Calculator
{
    namespace
    {
        std::size_t expectedArgumentCount(Function function)
        {
            switch (function)
            {
                case Function::SquareRoot:
                case Function::Sine:
                case Function::Cosine:
                case Function::Tangent:
                case Function::AbsoluteValue:
                case Function::NaturalLogarithm:
                case Function::Base10Logarithm:
                    return 1;

                case Function::Logarithm:
                    return 2;
            }

            return 0;
        }
    }

    Evaluator::Evaluator()
        : Evaluator(EvaluationContext{})
    {
    }

    Evaluator::Evaluator(AngleMode angleMode)
        : Evaluator(EvaluationContext{angleMode, std::nullopt})
    {
    }

    Evaluator::Evaluator(EvaluationContext context)
        : context(std::move(context))
    {
    }

    double Evaluator::evaluate(const Expression& expression) const
    {
        try
        {
            switch (expression.type)
            {
                case ExpressionType::Number:
                    return expression.value;

                case ExpressionType::Constant:
                    switch (expression.constant)
                    {
                        case Constant::Pi:
                            return std::numbers::pi;

                        case Constant::E:
                            return std::numbers::e;

                        case Constant::Ans:
                            if (!context.answer)
                            {
                                throw CalculatorError(
                                    ErrorCategory::Evaluation,
                                    expression.position,
                                    "Ans is not available"
                                );
                            }

                            return *context.answer;
                    }

                    throw CalculatorError(
                        ErrorCategory::Evaluation,
                        expression.position,
                        "Unsupported constant"
                    );

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
                    const std::size_t expectedCount = expectedArgumentCount(expression.function);

                    if (expression.arguments.size() != expectedCount)
                    {
                        throw CalculatorError(
                            ErrorCategory::Evaluation,
                            expression.position,
                            "Function call requires " + std::to_string(expectedCount) +
                                (expectedCount == 1 ? " argument" : " arguments")
                        );
                    }

                    switch (expression.function)
                    {
                        case Function::SquareRoot:
                            return BasicOperations::squareRoot(evaluate(expression.arguments[0]));

                        case Function::Sine:
                            return BasicOperations::sine(
                                angleInRadians(evaluate(expression.arguments[0]))
                            );

                        case Function::Cosine:
                            return BasicOperations::cosine(
                                angleInRadians(evaluate(expression.arguments[0]))
                            );

                        case Function::Tangent:
                            return BasicOperations::tangent(
                                angleInRadians(evaluate(expression.arguments[0]))
                            );

                        case Function::AbsoluteValue:
                            return BasicOperations::absoluteValue(evaluate(expression.arguments[0]));

                        case Function::NaturalLogarithm:
                            return BasicOperations::naturalLogarithm(
                                evaluate(expression.arguments[0])
                            );

                        case Function::Base10Logarithm:
                            return BasicOperations::base10Logarithm(
                                evaluate(expression.arguments[0])
                            );

                        case Function::Logarithm:
                            return BasicOperations::logarithm(
                                evaluate(expression.arguments[0]),
                                evaluate(expression.arguments[1])
                            );
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

    double Evaluator::angleInRadians(double angle) const
    {
        if (context.angleMode == AngleMode::Degrees)
        {
            return angle * std::numbers::pi / 180.0;
        }

        return angle;
    }
}
