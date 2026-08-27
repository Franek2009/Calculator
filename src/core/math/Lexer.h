#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>

#include "Token.h"

namespace Calculator
{
    class Lexer
    {
    public:
        explicit Lexer(const std::string& input);

        std::vector<Token> tokenize();

    private:
        std::string input;
    };
}

#endif
