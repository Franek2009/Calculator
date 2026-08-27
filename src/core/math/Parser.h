#ifndef PARSER_H
#define PARSER_H

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
    };
}

#endif
