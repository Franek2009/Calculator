#include "CalculatorError.h"

namespace
{
    std::string categoryName(Calculator::ErrorCategory category)
    {
        switch (category)
        {
            case Calculator::ErrorCategory::Lexical:
                return "Lexical";

            case Calculator::ErrorCategory::Syntax:
                return "Syntax";

            case Calculator::ErrorCategory::Evaluation:
                return "Evaluation";
        }

        return "Unknown";
    }

    std::string formatMessage(Calculator::ErrorCategory category,
                              std::size_t position,
                              const std::string& description)
    {
        return categoryName(category) + " error at position " +
               std::to_string(position + 1) + ": " + description;
    }
}

namespace Calculator
{
    CalculatorError::CalculatorError(ErrorCategory category,
                                     std::size_t position,
                                     const std::string& description)
        : std::invalid_argument(formatMessage(category, position, description)),
          errorCategory(category),
          errorPosition(position)
    {
    }

    ErrorCategory CalculatorError::category() const noexcept
    {
        return errorCategory;
    }

    std::size_t CalculatorError::position() const noexcept
    {
        return errorPosition;
    }
}
