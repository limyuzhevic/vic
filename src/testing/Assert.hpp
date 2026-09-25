// Assert.hpp
// Custom assertion framework for NLM

#pragma once

#include <string>

namespace nlm {

class Assert {
public:
    static bool check(bool condition, const std::string& message, const std::string& file, int line);
    static bool isTrue(bool condition, const std::string& message, const std::string& file, int line);
    static bool isFalse(bool condition, const std::string& message, const std::string& file, int line);
    static bool equals(int a, int b, const std::string& message, const std::string& file, int line);
    static bool notEquals(int a, int b, const std::string& message, const std::string& file, int line);
    static bool equals(double a, double b, double epsilon, const std::string& message, const std::string& file, int line);
    static bool notEquals(double a, double b, double epsilon, const std::string& message, const std::string& file, int line);
    static bool equals(const std::string& a, const std::string& b, const std::string& message, const std::string& file, int line);
    static bool notEquals(const std::string& a, const std::string& b, const std::string& message, const std::string& file, int line);
    static bool isNull(void* ptr, const std::string& message, const std::string& file, int line);
    static bool isNotNull(void* ptr, const std::string& message, const std::string& file, int line);
    static bool isGreaterThan(int a, int b, const std::string& message, const std::string& file, int line);
    static bool isGreaterThanOrEqual(int a, int b, const std::string& message, const std::string& file, int line);
    static bool isLessThan(int a, int b, const std::string& message, const std::string& file, int line);
    static bool isLessThanOrEqual(int a, int b, const std::string& message, const std::string& file, int line);
};

}} // namespace nlm