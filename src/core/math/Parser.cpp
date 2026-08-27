#include "Parser.h"

#include <stdexcept>

namespace Calculator
{
    Parser::Parser(const std::vector<Token>& tokens)
        : tokens(tokens)
    {
    }

    Expression Parser::parse()
    {
        if (tokens.size() != 1)
        {
            throw std::invalid_argument("Expected a single number");
        }

        if (tokens[0].type != TokenType::Number)
        {
            throw std::invalid_argument("Expected a number");
        }

        return {
            ExpressionType::Number,
            std::stod(tokens[0].value)
        };
    }
}
