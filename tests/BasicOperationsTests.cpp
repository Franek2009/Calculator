#include <catch2/catch_test_macros.hpp>
#include <stdexcept>

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
