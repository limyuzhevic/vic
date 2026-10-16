// Test Expectations Helper
// Common test expectations and validation helpers

#ifndef TEST_EXPECTATIONS_HPP
#define TEST_EXPECTATIONS_HPP

#include <gtest/gtest.h>
#include <cmath>
#include <stdexcept>
#include <functional>

namespace test_expectations {

// Expects that a condition holds true, similar to assert
inline void expects(bool condition, const std::string &message = "Condition failed") {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

// Expects that a value is within bounds
template <typename T>
void expectsInRange(T value, T min, T max, const std::string &message = "Value out of range") {
    expects(value >= min && value <= max, message);
}

// Expects that a function throws an exception
template <typename Func, typename... Args>
void expectsThrows(Func func, Args&&... args, const std::string &message = "Expected exception not thrown") {
    bool threw = false;
    try {
        func(std::forward<Args>(args)...);
    } catch (...) {
        threw = true;
    }
    expects(threw, message);
}

// Expects that a condition is close to expected value (floating point)
template <typename T>
void expectsClose(T actual, T expected, T tolerance,
                 const std::string &message = "Values not close enough") {
    expects(std::abs(actual - expected) <= tolerance, message);
}

// Expects that a string contains substring
template <typename... Args>
void expectsContains(const std::string &str, Args&&... substrs) {
    std::string temp = str;
    bool all_found = (std::string(temp).find(std::forward<Args>(substrs)) != std::string::npos) && ...;
    expects(all_found, "String does not contain all substrings");
}

// Expects that a value is greater than another
template <typename T, typename U>
void expectsGreater(T value, U other, const std::string &message = "Value not greater") {
    expects(value > other, message);
}

// Expects that a value is less than another
template <typename T, typename U>
void expectsLess(T value, U other, const std::string &message = "Value not less") {
    expects(value < other, message);
}

// Expects that a value is non-negative
template <typename T>
void expectsNonNegative(T value, const std::string &message = "Value is negative") {
    expects(value >= T(0), message);
}

// Expects that a value is positive
template <typename T>
void expectsPositive(T value, const std::string &message = "Value is not positive") {
    expects(value > T(0), message);
}

// Expects that a value is negative
template <typename T>
void expectsNegative(T value, const std::string &message = "Value is not negative") {
    expects(value < T(0), message);
}

// Expects that a value is within valid bounds for a given type
template <typename T>
void expectsValid(T value, const std::string &message = "Value is invalid") {
    expects(!(std::isinf(value) || std::isnan(value)), message);
}

// Expects that a value is finite
template <typename T>
void expectsFinite(T value, const std::string &message = "Value is not finite") {
    expects(std::isfinite(value), message);
}

// Expects that a vector is not empty
void expectsNotEmpty(const auto &container, const std::string &message = "Container is empty") {
    expects(!container.empty(), message);
}

// Expects that a vector has expected size
template <typename Container>
void expectsSize(const Container &container, typename Container::size_type expectedSize,
                 const std::string &message = "Container has unexpected size") {
    expects(container.size() == expectedSize, message);
}

// Expects that a value equals another
template <typename T, typename U>
void expectsEq(T actual, U expected, const std::string &message = "Values not equal") {
    expects(actual == expected, message);
}

// Expects that a value is approximately equal (for floating point)
template <typename T>
void expectsApproxEq(T actual, T expected, T tolerance,
                     const std::string &message = "Values not approximately equal") {
    expects(std::abs(actual - expected) <= tolerance, message);
}

} // namespace test_expectations

#endif // TEST_EXPECTATIONS_HPP