// Custom GoogleTest Matchers
// Advanced matchers for testing NLM components

#ifndef TEST_MATCHERS_HPP
#define TEST_MATCHERS_HPP

#include <gtest/gtest.h>
#include <cmath>
#include <string>
#include <vector>
#include <type_traits>

// Float matcher with tolerance
MATCHER_P(IsClose, expected, 
          "is close to " + ::testing::PrintToString(expected)) {
    return std::abs(arg - expected) <= static_cast<decltype(arg)>(1e-6);
}

// Double matcher with tolerance
MATCHER_P2(IsCloseDouble, expected, tolerance,
           "is close to " + ::testing::PrintToString(expected) + 
           " within tolerance " + ::testing::PrintToString(tolerance)) {
    return std::abs(arg - expected) <= tolerance;
}

// Integer in range matcher
MATCHER_P2(IsInRange, min, max,
           "is in range [" + ::testing::PrintToString(min) + ", " +
           ::testing::PrintToString(max) + "]") {
    return arg >= min && arg <= max;
}

// Vector matcher for standard containers
MATCHER_P2(VectorEq, expected, tolerance,
           "matches expected vector within tolerance") {
    if (arg.size() != expected.size()) {
        return false;
    }
    for (size_t i = 0; i < arg.size(); ++i) {
        if (std::abs(arg[i] - expected[i]) > tolerance) {
            return false;
        }
    }
    return true;
}

// String contains matcher
MATCHER_P(StringContains, substr,
          "contains substring '" + ::testing::PrintToString(substr) + "'") {
    return arg.find(substr) != std::string::npos;
}

// Point matcher for (x, y) pairs
MATCHER_P2(PointNear, x, y, tolerance,
           "is near point (" + ::testing::PrintToString(x) + ", " +
           ::testing::PrintToString(y) + ") within tolerance " +
           ::testing::PrintToString(tolerance)) {
    return std::abs(arg.first - x) <= tolerance &&
           std::abs(arg.second - y) <= tolerance;
}

// Pair matcher for two values
MATCHER_P2(PairEq, first_expected, second_expected,
           "is equal to (" + ::testing::PrintToString(first_expected) + ", " +
           ::testing::PrintToString(second_expected) + ")") {
    return arg.first == first_expected && arg.second == second_expected;
}

// Enumerated value matcher
MATCHER_P(IsEnum, expected,
          "is equal to " + ::testing::PrintToString(expected)) {
    return arg == expected;
}

// Optional value matcher
MATCHER_P(HasValue, expected,
          "has value " + ::testing::PrintToString(expected)) {
    if (!arg.has_value()) {
        return false;
    }
    return arg.value() == expected;
}

// Non-empty container matcher
MATCHER(NonEmpty,
        "is not empty") {
    return !arg.empty();
}

// Vector contains value matcher
MATCHER_P(VectorContains, value,
          "contains value " + ::testing::PrintToString(value)) {
    return std::find(arg.begin(), arg.end(), value) != arg.end();
}

// Vector size matcher
MATCHER_P(VectorSize, expected_size,
          "has size " + ::testing::PrintToString(expected_size)) {
    return arg.size() == expected_size;
}

// Floating point vector matcher
MATCHER_P(VectorFloatEq, expected,
          "matches expected float vector within tolerance") {
    if (arg.size() != expected.size()) {
        return false;
    }
    for (size_t i = 0; i < arg.size(); ++i) {
        if (std::abs(static_cast<float>(arg[i]) - static_cast<float>(expected[i])) > 1e-6f) {
            return false;
        }
    }
    return true;
}

#endif // TEST_MATCHERS_HPP