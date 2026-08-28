#ifndef PARSER_H
#define PARSER_H

#include <cstddef>
#include <string>
#include <vector>

#include "Token.h"
#include "Expression.h"

namespace Calculator
{
    class Parser
    {
    public:
        explicit Parser(const std::vector<Token>& tokens);

        Expression parse();

    private:
        std::vector<Token> tokens;
        std::size_t current = 0;

        Expression parseExpression();
        Expression parseTerm();
        Expression parseUnary();
        Expression parsePower();
        Expression parsePrimary();

        std::size_t errorPosition() const;
        [[noreturn]] void throwSyntaxError(const std::string& description) const;
    };

}

#endif
