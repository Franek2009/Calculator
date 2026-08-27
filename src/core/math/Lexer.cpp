#include "Lexer.h"

#include <cctype>
#include <stdexcept>

namespace Calculator
{
    Lexer::Lexer(const std::string& input)
        : input(input)
    {
    }
    std::vector<Token> Lexer::tokenize()
    {
        std::vector<Token> tokens;

        for (std::size_t i = 0; i < input.length(); ++i)
        {
            char current = input[i];

            if (std::isspace(current))
            {
                continue;
            }

            if (std::isdigit(current))
            {
                std::string number;
                bool hasDecimalPoint = false;

                while (i < input.length())
                {
                    char character = input[i];

                    if (std::isdigit(character))
                    {
                        number += character;
                    }
                    else if (character == '.' && !hasDecimalPoint)
                    {
                        number += character;
                        hasDecimalPoint = true;
                    }
                    else
                    {
                        break;
                    }

                    ++i;
                }

                --i;

                tokens.push_back({TokenType::Number, number});
                continue;
            }

            switch (current)
            {
                case '+':
                    tokens.push_back({TokenType::Plus, "+"});
                    break;

                case '-':
                    tokens.push_back({TokenType::Minus, "-"});
                    break;

                case '*':
                    tokens.push_back({TokenType::Multiply, "*"});
                    break;

                case '/':
                    tokens.push_back({TokenType::Divide, "/"});
                    break;

                case '^':
                    tokens.push_back({TokenType::Power, "^"});
                    break;

                case '(':
                    tokens.push_back({TokenType::LeftParenthesis, "("});
                    break;

                case ')':
                    tokens.push_back({TokenType::RightParenthesis, ")"});
                    break;

                default:
                    throw std::invalid_argument("Unknown character");
            }
        }

        return tokens;
    }

}
