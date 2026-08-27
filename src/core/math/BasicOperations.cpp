#include "BasicOperations.h"

#include <cmath>
#include <stdexcept>

namespace BasicOperations
{
    double add(double a, double b)
    {
        return a + b;
    }

    double subtract(double a, double b)
    {
        return a - b;
    }

    double multiply(double a, double b)
    {
        return a * b;
    }

    double divide(double a, double b)
    {
        if (b == 0)
        {
            throw std::invalid_argument("Division by zero");
        }

        return a / b;
    }

    double power(double base, double exponent)
    {
        return std::pow(base, exponent);
    }

    double squareRoot(double number)
    {
        if (number < 0)
        {
            throw std::invalid_argument("Square root of a negative number");
        }

        return std::sqrt(number);
    }
}
