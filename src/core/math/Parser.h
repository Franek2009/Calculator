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
        std::vector<Expression> parseArgumentList();
        Function parseFunction(const Token& token) const;
        static std::size_t expectedArgumentCount(Function function);
        static void validateFunctionArity(Function function,
                                          const Token& token,
                                          std::size_t actualCount);

        std::size_t errorPosition() const;
        [[noreturn]] void throwSyntaxError(const std::string& description) const;
    };

}

#endif
