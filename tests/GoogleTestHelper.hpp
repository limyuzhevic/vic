// GoogleTest Helper Header
// Common utilities and utilities for GoogleTest tests

#ifndef GOOGLETEST_HELPER_HPP
#define GOOGLETEST_HELPER_HPP

#include <cmath>
#include <type_traits>

// Custom GoogleTest matchers and utilities
namespace gtest_helpers {

// Float comparator for assertions with tolerance
template <typename T>
constexpr bool isClose(T a, T b, T epsilon = T(1e-6)) {
    static_assert(std::is_floating_point<T>::value, "T must be floating point type");
    return std::abs(a - b) <= epsilon;
}

// Integer range checker
template <typename T>
constexpr bool isInRange(T val, T min, T max) {
    static_assert(std::is_integral<T>::value, "T must be integral type");
    return val >= min && val <= max;
}

// Vector equality checker for arrays of floats
template <typename T, size_t N>
bool vectorApprox(const T (&a)[N], const T (&b)[N], T epsilon = T(1e-6)) {
    for (size_t i = 0; i < N; ++i) {
        if (!isClose(a[i], b[i], epsilon)) {
            return false;
        }
    }
    return true;
}

// String contains checker
inline bool stringContains(const std::string &str, const std::string &sub) {
    return str.find(sub) != std::string::npos;
}

// Point near checker for floating point comparisons
inline bool pointNear(float x1, float y1, float x2, float y2, float epsilon = 0.001f) {
    return isClose(x1, x2, epsilon) && isClose(y1, y2, epsilon);
}

} // namespace gtest_helpers

#endif // GOOGLETEST_HELPER_HPP