#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <cmath>
#include <limits>
#include <numbers>
#include <stdexcept>
#include <string>

#include "../src/core/math/BasicOperations.h"

TEST_CASE("Addition works correctly")
{
    REQUIRE(Calculator::BasicOperations::add(2, 3) == 5);
}

TEST_CASE("Subtraction works correctly")
{
    REQUIRE(Calculator::BasicOperations::subtract(5, 3) == 2);
}

TEST_CASE("Multiplication works correctly")
{
    REQUIRE(Calculator::BasicOperations::multiply(2, 3) == 6);
}

TEST_CASE("Division works correctly")
{
    REQUIRE(Calculator::BasicOperations::divide(6, 3) == 2);
}

TEST_CASE("Power works correctly")
{
    REQUIRE(Calculator::BasicOperations::power(2, 3) == 8);
}

TEST_CASE("Square root works correctly")
{
    REQUIRE(Calculator::BasicOperations::squareRoot(4) == 2);
}

TEST_CASE("Division by zero throws an exception")
{
    REQUIRE_THROWS_AS(Calculator::BasicOperations::divide(6, 0), std::invalid_argument);
}

TEST_CASE("Square root of a negative number throws an exception")
{
    REQUIRE_THROWS_AS(Calculator::BasicOperations::squareRoot(-1), std::invalid_argument);
}

TEST_CASE("Trigonometric operations work with radians")
{
    REQUIRE(Calculator::BasicOperations::sine(0) == 0);
    REQUIRE(Calculator::BasicOperations::cosine(0) == 1);
    REQUIRE(Calculator::BasicOperations::tangent(0) == 0);

    REQUIRE(Calculator::BasicOperations::sine(0.5) == Catch::Approx(std::sin(0.5)));
    REQUIRE(Calculator::BasicOperations::cosine(0.5) == Catch::Approx(std::cos(0.5)));
    REQUIRE(Calculator::BasicOperations::tangent(0.5) == Catch::Approx(std::tan(0.5)));
}

TEST_CASE("Inverse trigonometric operations return radians")
{
    using namespace Calculator::BasicOperations;

    REQUIRE(arcSine(0) == 0);
    REQUIRE(arcSine(1) == Catch::Approx(std::numbers::pi / 2.0));
    REQUIRE(arcSine(-1) == Catch::Approx(-std::numbers::pi / 2.0));
    REQUIRE(arcCosine(0) == Catch::Approx(std::numbers::pi / 2.0));
    REQUIRE(arcCosine(1) == 0);
    REQUIRE(arcCosine(-1) == Catch::Approx(std::numbers::pi));
    REQUIRE(arcTangent(0) == 0);
    REQUIRE(arcTangent(1) == Catch::Approx(std::numbers::pi / 4.0));
    REQUIRE(arcTangent(-1) == Catch::Approx(-std::numbers::pi / 4.0));
}

TEST_CASE("Inverse trigonometric operations validate their domains")
{
    const auto requireDomainError = [](const auto& operation,
                                       const std::string& description)
    {
        try
        {
            operation();
            FAIL("Expected a domain error");
        }
        catch (const std::invalid_argument& error)
        {
            REQUIRE(std::string(error.what()) == description);
        }
    };
    const double infinity = std::numeric_limits<double>::infinity();
    const double nan = std::numeric_limits<double>::quiet_NaN();

    requireDomainError([]() { Calculator::BasicOperations::arcSine(1.01); },
                       "Arc sine argument must be between -1 and 1");
    requireDomainError([]() { Calculator::BasicOperations::arcCosine(-1.01); },
                       "Arc cosine argument must be between -1 and 1");
    requireDomainError([infinity]() { Calculator::BasicOperations::arcSine(infinity); },
                       "Arc sine argument must be between -1 and 1");
    requireDomainError([nan]() { Calculator::BasicOperations::arcCosine(nan); },
                       "Arc cosine argument must be between -1 and 1");
    requireDomainError([infinity]() { Calculator::BasicOperations::arcTangent(infinity); },
                       "Arc tangent argument must be finite");
    requireDomainError([nan]() { Calculator::BasicOperations::arcTangent(nan); },
                       "Arc tangent argument must be finite");
}

TEST_CASE("Absolute value and logarithms work correctly")
{
    REQUIRE(Calculator::BasicOperations::absoluteValue(-5) == 5);
    REQUIRE(Calculator::BasicOperations::naturalLogarithm(1) == 0);
    REQUIRE(Calculator::BasicOperations::base10Logarithm(100) == 2);
    REQUIRE(Calculator::BasicOperations::logarithm(2, 8) == 3);
    REQUIRE(Calculator::BasicOperations::logarithm(3, 81) == 4);
}

TEST_CASE("Arbitrary-base logarithms validate their domain")
{
    const auto requireDomainError = [](double base,
                                       double value,
                                       const std::string& description)
    {
        try
        {
            Calculator::BasicOperations::logarithm(base, value);
            FAIL("Expected a domain error");
        }
        catch (const std::invalid_argument& error)
        {
            REQUIRE(std::string(error.what()) == description);
        }
    };

    requireDomainError(0, 8, "Logarithm base must be positive");
    requireDomainError(-2, 8, "Logarithm base must be positive");
    requireDomainError(1, 8, "Logarithm base must not equal 1");
    requireDomainError(2, 0, "Logarithm value must be positive");
    requireDomainError(2, -8, "Logarithm value must be positive");
}

TEST_CASE("Logarithms reject non-positive arguments")
{
    const auto requireDomainError = [](const auto& operation,
                                       const std::string& description)
    {
        try
        {
            operation();
            FAIL("Expected a domain error");
        }
        catch (const std::invalid_argument& error)
        {
            REQUIRE(std::string(error.what()) == description);
        }
    };

    requireDomainError(
        []() { Calculator::BasicOperations::naturalLogarithm(0); },
        "Natural logarithm of a non-positive number"
    );
    requireDomainError(
        []() { Calculator::BasicOperations::naturalLogarithm(-1); },
        "Natural logarithm of a non-positive number"
    );
    requireDomainError(
        []() { Calculator::BasicOperations::base10Logarithm(0); },
        "Base-10 logarithm of a non-positive number"
    );
    requireDomainError(
        []() { Calculator::BasicOperations::base10Logarithm(-1); },
        "Base-10 logarithm of a non-positive number"
    );
}
