#include "Parser.h"

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
            throw std::invalid_argument("Expected an expression");
        }

        return parseExpression();
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

            Expression right = parseTerm();

            left = {
                ExpressionType::BinaryOperation,
                0,
                operation,
                std::make_unique<Expression>(std::move(left)),
                std::make_unique<Expression>(std::move(right))
            };
        }

        return left;
    }

    Expression Parser::parseTerm()
    {
        Expression left = parsePower();

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

            Expression right = parsePower();

            left = {
                ExpressionType::BinaryOperation,
                0,
                operation,
                std::make_unique<Expression>(std::move(left)),
                std::make_unique<Expression>(std::move(right))
            };
        }

        return left;
    }

    Expression Parser::parsePower()
    {
        Expression left = parsePrimary();

        if (current < tokens.size() &&
            tokens[current].type == TokenType::Power)
        {
            current++;

            Expression right = parsePower();

            left = {
                ExpressionType::BinaryOperation,
                0,
                Operator::Power,
                std::make_unique<Expression>(std::move(left)),
                std::make_unique<Expression>(std::move(right))
            };
        }

        return left;
    }

    Expression Parser::parsePrimary()
    {
        if (current >= tokens.size())
        {
            throw std::invalid_argument("Expected a number or '('");
        }

        if (tokens[current].type == TokenType::Number)
        {
            Expression result{
                ExpressionType::Number,
                std::stod(tokens[current].value)
            };

            current++;

            return result;
        }

        if (tokens[current].type == TokenType::LeftParenthesis)
        {
            current++;

            Expression result = parseExpression();

            if (current >= tokens.size() ||
                tokens[current].type != TokenType::RightParenthesis)
            {
                throw std::invalid_argument("Expected ')'");
            }

            current++;

            return result;
        }

        throw std::invalid_argument("Expected a number or '('");
    }
}
