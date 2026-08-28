#ifndef TOKEN_H
#define TOKEN_H

#include <cstddef>
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
        std::size_t position;
    };
}

#endif
