#ifndef TOKEN_H
#define TOKEN_H

#include <string>

namespace Calculator
{
    enum class TokenType
    {
        Number,

        Plus,
        Minus,
        Multiply,
        Divide,
        Power,

        LeftParenthesis,
        RightParenthesis,

        Function
    };

    struct Token
    {
        TokenType type;
        std::string value;
    };
}

#endif
