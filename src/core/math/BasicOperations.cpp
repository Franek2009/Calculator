#include "BasicOperations.h"

#include <cmath>
#include <stdexcept>

namespace Calculator::BasicOperations
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

    double sine(double radians)
    {
        return std::sin(radians);
    }

    double cosine(double radians)
    {
        return std::cos(radians);
    }

    double tangent(double radians)
    {
        return std::tan(radians);
    }

    double absoluteValue(double number)
    {
        return std::abs(number);
    }

    double naturalLogarithm(double number)
    {
        if (number <= 0)
        {
            throw std::invalid_argument("Natural logarithm of a non-positive number");
        }

        return std::log(number);
    }

    double base10Logarithm(double number)
    {
        if (number <= 0)
        {
            throw std::invalid_argument("Base-10 logarithm of a non-positive number");
        }

        return std::log10(number);
    }

    double logarithm(double base, double value)
    {
        if (base <= 0)
        {
            throw std::invalid_argument("Logarithm base must be positive");
        }

        if (base == 1)
        {
            throw std::invalid_argument("Logarithm base must not equal 1");
        }

        if (value <= 0)
        {
            throw std::invalid_argument("Logarithm value must be positive");
        }

        return std::log(value) / std::log(base);
    }
}
