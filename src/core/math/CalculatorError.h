#ifndef CALCULATOR_ERROR_H
#define CALCULATOR_ERROR_H

#include <cstddef>
#include <stdexcept>
#include <string>

namespace Calculator
{
    enum class ErrorCategory
    {
        Lexical,
        Syntax,
        Evaluation
    };

    class CalculatorError : public std::invalid_argument
    {
    public:
        CalculatorError(ErrorCategory category,
                        std::size_t position,
                        const std::string& description);

        ErrorCategory category() const noexcept;
        std::size_t position() const noexcept;

    private:
        ErrorCategory errorCategory;
        std::size_t errorPosition;
    };
}

#endif
