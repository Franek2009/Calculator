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
        if (tokens.size() == 1)
        {
            if (tokens[0].type != TokenType::Number)
            {
                throw std::invalid_argument("Expected a number");
            }

            return {
                ExpressionType::Number,
                std::stod(tokens[0].value)
            };
        }

        if (tokens.size() == 3)
        {
            if (tokens[0].type != TokenType::Number ||
                tokens[2].type != TokenType::Number)
            {
                throw std::invalid_argument("Expected numbers");
            }

            Operator operation;

            if (tokens[1].type == TokenType::Plus)
            {
                operation = Operator::Add;
            }
            else if (tokens[1].type == TokenType::Minus)
            {
                operation = Operator::Subtract;
            }
            else if (tokens[1].type == TokenType::Multiply)
            {
                operation = Operator::Multiply;
            }
            else if (tokens[1].type == TokenType::Divide)
            {
                operation = Operator::Divide;
            }
            else if (tokens[1].type == TokenType::Power)
            {
                operation = Operator::Power;
            }
            else
            {
                throw std::invalid_argument("Expected an operator");
            }

            return {
                ExpressionType::BinaryOperation,
                0,
                operation,
                std::make_unique<Expression>(
                    Expression{
                        ExpressionType::Number,
                        std::stod(tokens[0].value)
                    }
                ),
                std::make_unique<Expression>(
                    Expression{
                        ExpressionType::Number,
                        std::stod(tokens[2].value)
                    }
                )
            };
        }

        throw std::invalid_argument("Unsupported expression");
    }
}
