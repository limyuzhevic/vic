// Assert.cpp
// Implementation of custom assertion framework for NLM

#include "Assert.hpp"
#include <iostream>

namespace nlm {

bool Assert::check(bool condition, const std::string& message, const std::string& file, int line) {
    if (!condition) {
        std::cerr << "Assertion failed: " << message << " at " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}

bool Assert::isTrue(bool condition, const std::string& message, const std::string& file, int line) {
    return check(condition, message, file, line);
}

bool Assert::isFalse(bool condition, const std::string& message, const std::string& file, int line) {
    return check(!condition, message, file, line);
}

bool Assert::equals(int a, int b, const std::string& message, const std::string& file, int line) {
    return check(a == b, message, file, line);
}

bool Assert::notEquals(int a, int b, const std::string& message, const std::string& file, int line) {
    return check(a != b, message, file, line);
}

bool Assert::equals(double a, double b, double epsilon, const std::string& message, 
                   const std::string& file, int line) {
    return check(std::abs(a - b) <= epsilon, message, file, line);
}

bool Assert::notEquals(double a, double b, double epsilon, const std::string& message,
                      const std::string& file, int line) {
    return check(std::abs(a - b) > epsilon, message, file, line);
}

bool Assert::equals(const std::string& a, const std::string& b, const std::string& message,
                   const std::string& file, int line) {
    return check(a == b, message, file, line);
}

bool Assert::notEquals(const std::string& a, const std::string& b, const std::string& message,
                      const std::string& file, int line) {
    return check(a != b, message, file, line);
}

bool Assert::isNull(void* ptr, const std::string& message, const std::string& file, int line) {
    return check(ptr == nullptr, message, file, line);
}

bool Assert::isNotNull(void* ptr, const std::string& message, const std::string& file, int line) {
    return check(ptr != nullptr, message, file, line);
}

bool Assert::isGreaterThan(int a, int b, const std::string& message, const std::string& file, int line) {
    return check(a > b, message, file, line);
}

bool Assert::isGreaterThanOrEqual(int a, int b, const std::string& message, 
                                  const std::string& file, int line) {
    return check(a >= b, message, file, line);
}

bool Assert::isLessThan(int a, int b, const std::string& message, const std::string& file, int line) {
    return check(a < b, message, file, line);
}

bool Assert::isLessThanOrEqual(int a, int b, const std::string& message, 
                               const std::string& file, int line) {
    return check(a <= b, message, file, line);
}

}} // namespace nlm