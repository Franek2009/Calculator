#include "Lexer.h"

#include "CalculatorError.h"

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

            if (std::isspace(static_cast<unsigned char>(current)))
            {
                continue;
            }

            if (std::isdigit(static_cast<unsigned char>(current)))
            {
                const std::size_t position = i;
                std::string number;
                bool hasDecimalPoint = false;

                while (i < input.length())
                {
                    char character = input[i];

                    if (std::isdigit(static_cast<unsigned char>(character)))
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

                tokens.push_back({TokenType::Number, number, position});
                continue;
            }
            if (std::isalpha(static_cast<unsigned char>(current)))
            {
                const std::size_t position = i;
                std::string function;

                while (i < input.length() &&
                       std::isalpha(static_cast<unsigned char>(input[i])))
                {
                    function += input[i];
                    ++i;
                }

                --i;

                tokens.push_back({TokenType::Function, function, position});
                continue;
            }
            switch (current)
            {
                case '+':
                    tokens.push_back({TokenType::Plus, "+", i});
                    break;

                case '-':
                    tokens.push_back({TokenType::Minus, "-", i});
                    break;

                case '*':
                    tokens.push_back({TokenType::Multiply, "*", i});
                    break;

                case '/':
                    tokens.push_back({TokenType::Divide, "/", i});
                    break;

                case '^':
                    tokens.push_back({TokenType::Power, "^", i});
                    break;

                case '(':
                    tokens.push_back({TokenType::LeftParenthesis, "(", i});
                    break;

                case ')':
                    tokens.push_back({TokenType::RightParenthesis, ")", i});
                    break;

                default:
                    throw CalculatorError(
                        ErrorCategory::Lexical,
                        i,
                        std::string("Unknown character '") + current + "'"
                    );
            }
        }

        return tokens;
    }

}
