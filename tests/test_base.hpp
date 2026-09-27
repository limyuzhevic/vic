// Test Base Framework
// Foundation for all comprehensive tests
// Provides common test utilities, assertions, and base classes

#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <memory>
#include <map>
#include <set>

namespace nlm_tests {

// Type aliases for test convenience
using TestString = std::string;
using TestFloat = float;
using TestInt = int;
using TestDouble = double;
using TestBool = bool;
using TestVector = std::vector<TestFloat>;
using TestMap = std::map<std::string, TestFloat>;
using TestSet = std::set<TestString>;

// Test assertion utilities
class TestAssertions {
public:
    // Enhanced floating-point comparison
    template<typename T>
    static bool AreEqual(T a, T b, T epsilon = 1e-6f) {
        return std::abs(a - b) <= epsilon;
    }
    
    // Range checking
    template<typename T>
    static bool AreInRange(T value, T min, T max) {
        return value >= min && value <= max;
    }
    
    // Vector equality
    template<typename T>
    static bool AreVectorsEqual(const std::vector<T>& a, const std::vector<T>& b, T epsilon = 1e-6f) {
        if (a.size() != b.size()) return false;
        for (size_t i = 0; i < a.size(); ++i) {
            if (!AreEqual(a[i], b[i], epsilon)) return false;
        }
        return true;
    }
    
    // String contains
    static bool StringContains(const TestString& str, const TestString& substr) {
        return str.find(substr) != TestString::npos;
    }
};

// Test fixture base class
class TestBase : public ::testing::Test {
protected:
    void SetUp() override {
        // Common setup for all tests
        test_info_ = "Default Test Information";
    }
    
    void TearDown() override {
        // Common cleanup for all tests
    }
    
    // Helper methods for test utilities
    template<typename T>
    void AssertWithinTolerance(T actual, T expected, T tolerance, const TestString& message = "") {
        if (!TestAssertions::AreEqual(actual, expected, tolerance)) {
            FAIL() << "Tolerance assertion failed: " << message
                   << " (expected: " << expected << ", actual: " << actual << ")";
        }
    }
    
    template<typename T>
    void AssertInRange(T value, T min, T max, const TestString& message = "") {
        if (!TestAssertions::AreInRange(value, min, max)) {
            FAIL() << "Range assertion failed: " << message
                   << " (expected in range [" << min << ", " << max << "], actual: " << value << ")";
        }
    }
    
    TestString GetTestInfo() const { return test_info_; }
    
    void SetTestInfo(const TestString& info) { test_info_ = info; }
    
    // Performance measurement utilities
    class PerformanceTimer {
    public:
        PerformanceTimer() : start_time_(std::chrono::high_resolution_clock::now()) {}
        
        double ElapsedMilliseconds() const {
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time_);
            return static_cast<double>(duration.count());
        }
        
        double ElapsedMicroseconds() const {
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time_);
            return static_cast<double>(duration.count());
        }
        
    private:
        std::chrono::high_resolution_clock::time_point start_time_;
    };
    
    // Random number generation for tests
    class TestRandom {
    public:
        TestRandom(unsigned int seed = 42) : rng_(seed) {}
        
        TestFloat UniformReal(TestFloat min, TestFloat max) {
            return std::uniform_real_distribution<TestFloat>(min, max)(rng_);
        }
        
        TestInt UniformInt(TestInt min, TestInt max) {
            return std::uniform_int_distribution<TestInt>(min, max)(rng_);
        }
        
        TestFloat Gaussian(TestFloat mean, TestFloat stddev) {
            std::normal_distribution<TestFloat> dist(mean, stddev);
            return dist(rng_);
        }
        
    private:
        std::mt19937 rng_;
    };
    
    // Test data generators
    class TestDataGenerator {
    public:
        static TestVector GenerateRandomVector(size_t size, TestFloat min = 0.0f, TestFloat max = 1.0f) {
            TestVector data;
            TestRandom rng;
            for (size_t i = 0; i < size; ++i) {
                data.push_back(rng.UniformReal(min, max));
            }
            return data;
        }
        
        static TestMap GenerateRandomMap(size_t size, TestFloat min = 0.0f, TestFloat max = 1.0f) {
            TestMap data;
            TestRandom rng;
            for (size_t i = 0; i < size; ++i) {
                TestString key = "key_" + std::to_string(i);
                data[key] = rng.UniformReal(min, max);
            }
            return data;
        }
        
        static TestVector GenerateSortedVector(size_t size, TestFloat min = 0.0f, TestFloat max = 1.0f) {
            TestVector data = GenerateRandomVector(size, min, max);
            std::sort(data.begin(), data.end());
            return data;
        }
        
        static TestVector GenerateIncreasingVector(size_t size, TestFloat min = 0.0f, TestFloat max = 1.0f) {
            TestVector data;
            TestFloat step = (max - min) / (size - 1);
            for (size_t i = 0; i < size; ++i) {
                data.push_back(min + i * step);
            }
            return data;
        }
    };
    
private:
    TestString test_info_;
};

// Test macros for common patterns
#define EXPECT_NEAR_TOLERANCE(actual, expected, tolerance) \
    ASSERT_TRUE(TestAssertions::AreEqual((actual), (expected), (tolerance)))

#define EXPECT_VECTOR_EQUAL(actual, expected, tolerance) \
    ASSERT_TRUE(TestAssertions::AreVectorsEqual((actual), (expected), (tolerance)))

#define EXPECT_STRING_CONTAINS(str, substr) \
    ASSERT_TRUE(TestAssertions::StringContains((str), (substr)))

// Test parameter structures
struct TestParameters {
    TestFloat learning_rate;
    TestFloat time_constant;
    TestFloat threshold;
    TestInt population_size;
    TestBool use_plasticity;
    TestString name;
    
    TestParameters() : learning_rate(0.01f), time_constant(20.0f), threshold(-55.0f),
                       population_size(100), use_plasticity(true), name("default") {}
    
    TestParameters(TestFloat lr, TestFloat tc, TestFloat th, TestInt ps, TestBool up, const TestString& n)
        : learning_rate(lr), time_constant(tc), threshold(th), population_size(ps),
          use_plasticity(up), name(n) {}
};

// Test utilities for common operations
class TestUtils {
public:
    // Statistical utilities
    static TestFloat CalculateMean(const TestVector& values) {
        if (values.empty()) return 0.0f;
        TestFloat sum = std::accumulate(values.begin(), values.end(), 0.0f);
        return sum / values.size();
    }
    
    static TestFloat CalculateStandardDeviation(const TestVector& values) {
        if (values.empty()) return 0.0f;
        TestFloat mean = CalculateMean(values);
        TestFloat sum_sq = 0.0f;
        for (TestFloat val : values) {
            sum_sq += (val - mean) * (val - mean);
        }
        return std::sqrt(sum_sq / values.size());
    }
    
    static TestFloat CalculateVariance(const TestVector& values) {
        if (values.empty()) return 0.0f;
        TestFloat mean = CalculateMean(values);
        TestFloat sum_sq = 0.0f;
        for (TestFloat val : values) {
            sum_sq += (val - mean) * (val - mean);
        }
        return sum_sq / values.size();
    }
    
    // Vector operations
    static TestVector AddVectors(const TestVector& a, const TestVector& b) {
        if (a.size() != b.size()) {
            throw std::invalid_argument("Vectors must have the same size");
        }
        TestVector result;
        result.reserve(a.size());
        for (size_t i = 0; i < a.size(); ++i) {
            result.push_back(a[i] + b[i]);
        }
        return result;
    }
    
    static TestVector MultiplyVectorByScalar(const TestVector& vec, TestFloat scalar) {
        TestVector result;
        result.reserve(vec.size());
        for (TestFloat val : vec) {
            result.push_back(val * scalar);
        }
        return result;
    }
    
    static TestFloat DotProduct(const TestVector& a, const TestVector& b) {
        if (a.size() != b.size()) {
            throw std::invalid_argument("Vectors must have the same size");
        }
        TestFloat result = 0.0f;
        for (size_t i = 0; i < a.size(); ++i) {
            result += a[i] * b[i];
        }
        return result;
    }
    
    // String utilities
    static TestString ToString(TestFloat value) {
        std::stringstream ss;
        ss << value;
        return ss.str();
    }
    
    static TestString FormatString(const TestString& format, ...) {
        va_list args;
        va_start(args, format);
        
        char buffer[1024];
        vsnprintf(buffer, sizeof(buffer), format.c_str(), args);
        va_end(args);
        
        return TestString(buffer);
    }
    
    // Time utilities
    static std::string GetCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S")
           << "." << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }
    
    // Memory utilities
    static size_t GetMemoryUsage() {
        // Simplified memory usage estimation
        // In a real implementation, this would use system-specific calls
        return 0; // Placeholder
    }
    
    // Performance utilities
    static double CalculateThroughput(size_t operations, double time_ms) {
        return time_ms > 0.0 ? (operations / time_ms) * 1000.0 : 0.0;
    }
    
    static double CalculateEfficiency(double work_done, double time_ms, double max_possible_work) {
        return time_ms > 0.0 ? (work_done / max_possible_work) / (time_ms / 1000.0) : 0.0;
    }
};

// Test result collection
class TestResults {
public:
    struct TestResult {
        TestString test_name;
        bool passed;
        double execution_time_ms;
        TestString error_message;
    };
    
    void AddResult(const TestResult& result) {
        results_.push_back(result);
    }
    
    size_t GetPassedCount() const {
        size_t count = 0;
        for (const auto& result : results_) {
            if (result.passed) count++;
        }
        return count;
    }
    
    size_t GetFailedCount() const {
        size_t count = 0;
        for (const auto& result : results_) {
            if (!result.passed) count++;
        }
        return count;
    }
    
    double GetAverageExecutionTime() const {
        if (results_.empty()) return 0.0;
        double total_time = 0.0;
        for (const auto& result : results_) {
            total_time += result.execution_time_ms;
        }
        return total_time / results_.size();
    }
    
    const std::vector<TestResult>& GetAllResults() const {
        return results_;
    }
    
    void Clear() {
        results_.clear();
    }
    
private:
    std::vector<TestResult> results_;
};

// Global test results collector
static TestResults g_test_results;

// RAII test result reporter
class TestResultReporter {
public:
    TestResultReporter(const TestString& test_name, double start_time) 
        : test_name_(test_name), start_time_(start_time) {}
    
    ~TestResultReporter() {
        double end_time = TestUtils::GetCurrentTimestamp(); // This would need to be measured properly
        double execution_time = end_time - start_time_;
        
        TestResults::TestResult result;
        result.test_name = test_name_;
        result.passed = true; // Assume passed unless exception thrown
        result.execution_time_ms = execution_time;
        result.error_message = "";
        
        g_test_results.AddResult(result);
    }
    
    void SetFailure(const TestString& message) {
        TestResults::TestResult result;
        result.test_name = test_name_;
        result.passed = false;
        result.execution_time_ms = 0.0;
        result.error_message = message;
        
        g_test_results.AddResult(result);
    }
    
private:
    TestString test_name_;
    double start_time_;
};

} // namespace nlm_tests

// Convenience macros for test result reporting
#define TEST_REPORT(test_name) \
    nlm_tests::TestResultReporter test_result_reporter(test_name, nlm_tests::TestUtils::GetCurrentTimestamp())

#define TEST_REPORT_FAILURE(message) \
    test_result_reporter.SetFailure(message)