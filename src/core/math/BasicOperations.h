#ifndef BASIC_OPERATIONS_H
#define BASIC_OPERATIONS_H

namespace Calculator::BasicOperations
{
    double add(double a, double b);
    double subtract(double a, double b);
    double multiply(double a, double b);
    double divide(double a, double b);
    double power(double base, double exponent);
    double squareRoot(double number);
    double sine(double radians);
    double cosine(double radians);
    double tangent(double radians);
    double arcSine(double value);
    double arcCosine(double value);
    double arcTangent(double value);
    double absoluteValue(double number);
    double naturalLogarithm(double number);
    double base10Logarithm(double number);
    double logarithm(double base, double value);
}

#endif
