#include "Parser.h"

#include "CalculatorError.h"

#include <memory>
#include <stdexcept>

namespace Calculator
{
    Parser::Parser(const std::vector<Token>& tokens)
        : tokens(tokens)
    {
    }

    Expression Parser::parse()
    {
        if (tokens.empty())
        {
            throwSyntaxError("Expected expression");
        }

        Expression result = parseExpression();

        if (current != tokens.size())
        {
            throwSyntaxError("Unexpected token '" + tokens[current].value +
                             "' after expression");
        }

        return result;
    }

    Expression Parser::parseExpression()
    {
        Expression left = parseTerm();

        while (current < tokens.size())
        {
            Operator operation;

            if (tokens[current].type == TokenType::Plus)
            {
                operation = Operator::Add;
            }
            else if (tokens[current].type == TokenType::Minus)
            {
                operation = Operator::Subtract;
            }
            else
            {
                break;
            }

            current++;

            const std::size_t position = tokens[current - 1].position;

            Expression right = parseTerm();

            left = {
                ExpressionType::BinaryOperation,
                0,
                operation,
                std::make_unique<Expression>(std::move(left)),
                std::make_unique<Expression>(std::move(right)),
                UnaryOperator::Negate,
                Function::SquareRoot,
                nullptr,
                position
            };
        }

        return left;
    }

    Expression Parser::parseTerm()
    {
        Expression left = parseUnary();

        while (current < tokens.size())
        {
            Operator operation;

            if (tokens[current].type == TokenType::Multiply)
            {
                operation = Operator::Multiply;
            }
            else if (tokens[current].type == TokenType::Divide)
            {
                operation = Operator::Divide;
            }
            else
            {
                break;
            }

            current++;

            const std::size_t position = tokens[current - 1].position;

            Expression right = parseUnary();

            left = {
                ExpressionType::BinaryOperation,
                0,
                operation,
                std::make_unique<Expression>(std::move(left)),
                std::make_unique<Expression>(std::move(right)),
                UnaryOperator::Negate,
                Function::SquareRoot,
                nullptr,
                position
            };
        }

        return left;
    }

    Expression Parser::parseUnary()
    {
        if (current < tokens.size() && tokens[current].type == TokenType::Minus)
        {
            const std::size_t position = tokens[current].position;
            current++;

            Expression operand = parseUnary();

            return {
                ExpressionType::UnaryOperation,
                0,
                Operator::Add,
                nullptr,
                nullptr,
                UnaryOperator::Negate,
                Function::SquareRoot,
                std::make_unique<Expression>(std::move(operand)),
                position
            };
        }

        return parsePower();
    }

    Expression Parser::parsePower()
    {
        Expression left = parsePrimary();

        if (current < tokens.size() &&
            tokens[current].type == TokenType::Power)
        {
            const std::size_t position = tokens[current].position;
            current++;

            Expression right = parseUnary();

            left = {
                ExpressionType::BinaryOperation,
                0,
                Operator::Power,
                std::make_unique<Expression>(std::move(left)),
                std::make_unique<Expression>(std::move(right)),
                UnaryOperator::Negate,
                Function::SquareRoot,
                nullptr,
                position
            };
        }

        return left;
    }

    Expression Parser::parsePrimary()
    {
        if (current >= tokens.size())
        {
            throwSyntaxError("Expected expression");
        }

        if (tokens[current].type == TokenType::Number)
        {
            const std::size_t position = tokens[current].position;
            Expression result{
                ExpressionType::Number,
                std::stod(tokens[current].value),
                Operator::Add,
                nullptr,
                nullptr,
                UnaryOperator::Negate,
                Function::SquareRoot,
                nullptr,
                position
            };

            current++;

            return result;
        }

        if (tokens[current].type == TokenType::Function)
        {
            const Token& functionToken = tokens[current];
            const std::size_t position = functionToken.position;
            const Function function = parseFunction(functionToken);
            current++;

            if (current >= tokens.size() ||
                tokens[current].type != TokenType::LeftParenthesis)
            {
                throwSyntaxError("Expected '(' after function name");
            }

            current++;

            Expression operand = parseExpression();

            if (current >= tokens.size() ||
                tokens[current].type != TokenType::RightParenthesis)
            {
                throwSyntaxError("Expected ')'");
            }

            current++;

            return {
                ExpressionType::FunctionCall,
                0,
                Operator::Add,
                nullptr,
                nullptr,
                UnaryOperator::Negate,
                function,
                std::make_unique<Expression>(std::move(operand)),
                position
            };
        }

        if (tokens[current].type == TokenType::LeftParenthesis)
        {
            current++;

            Expression result = parseExpression();

            if (current >= tokens.size() ||
                tokens[current].type != TokenType::RightParenthesis)
            {
                throwSyntaxError("Expected ')'");
            }

            current++;

            return result;
        }

        throwSyntaxError("Expected expression");
    }

    Function Parser::parseFunction(const Token& token) const
    {
        if (token.value == "sqrt")
        {
            return Function::SquareRoot;
        }

        if (token.value == "sin")
        {
            return Function::Sine;
        }

        if (token.value == "cos")
        {
            return Function::Cosine;
        }

        if (token.value == "tan")
        {
            return Function::Tangent;
        }

        throw CalculatorError(
            ErrorCategory::Syntax,
            token.position,
            "Unsupported function '" + token.value + "'"
        );
    }

    std::size_t Parser::errorPosition() const
    {
        if (current < tokens.size())
        {
            return tokens[current].position;
        }

        if (tokens.empty())
        {
            return 0;
        }

        const Token& lastToken = tokens.back();
        return lastToken.position + lastToken.value.length();
    }

    void Parser::throwSyntaxError(const std::string& description) const
    {
        throw CalculatorError(ErrorCategory::Syntax, errorPosition(), description);
    }
}
