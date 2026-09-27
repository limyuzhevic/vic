// Test Macros and Utilities
// Supporting macros and utility functions for comprehensive testing
// Provides common testing patterns, assertions, and helper functions

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <functional>
#include <sstream>
#include <iomanip>

// Test logging and output utilities
namespace nlm_tests {

// Test severity levels
enum class TestSeverity {
    INFO,
    WARNING,
    ERROR,
    FATAL
};

// Test log entry structure
struct TestLogEntry {
    TestSeverity severity;
    std::string message;
    std::string test_name;
    std::chrono::system_clock::time_point timestamp;
};

// Test logger class
class TestLogger {
public:
    static TestLogger& GetInstance() {
        static TestLogger instance;
        return instance;
    }
    
    void Log(TestSeverity severity, const std::string& message, const std::string& test_name = "") {
        std::lock_guard<std::mutex> lock(mutex_);
        
        TestLogEntry entry;
        entry.severity = severity;
        entry.message = message;
        entry.test_name = test_name;
        entry.timestamp = std::chrono::system_clock::now();
        
        logs_.push_back(entry);
        
        // Output to console
        std::string severity_str;
        switch (severity) {
            case TestSeverity::INFO: severity_str = "INFO"; break;
            case TestSeverity::WARNING: severity_str = "WARNING"; break;
            case TestSeverity::ERROR: severity_str = "ERROR"; break;
            case TestSeverity::FATAL: severity_str = "FATAL"; break;
        }
        
        if (!test_name.empty()) {
            std::cout << "[" << severity_str << "] " << test_name << ": " << message << std::endl;
        } else {
            std::cout << "[" << severity_str << "] " << message << std::endl;
        }
    }
    
    std::vector<TestLogEntry> GetLogs() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return logs_;
    }
    
    void ClearLogs() {
        std::lock_guard<std::mutex> lock(mutex_);
        logs_.clear();
    }
    
    void SetLogLevel(TestSeverity level) {
        std::lock_guard<std::mutex> lock(mutex_);
        log_level_ = level;
    }
    
    bool ShouldLog(TestSeverity severity) const {
        return static_cast<int>(severity) >= static_cast<int>(log_level_);
    }
    
private:
    TestLogger() : log_level_(TestSeverity::INFO) {}
    ~TestLogger() = default;
    TestLogger(const TestLogger&) = delete;
    TestLogger& operator=(const TestLogger&) = delete;
    
    std::vector<TestLogEntry> logs_;
    std::mutex mutex_;
    TestSeverity log_level_;
};

// Test assertion macros
#define ASSERT_NEAR_WITH_TOLERANCE(actual, expected, tolerance, message) \
    do { \
        if (std::abs((actual) - (expected)) > (tolerance)) { \
            std::stringstream ss; \
            ss << "Assertion failed: " << message << "\n" \
               << "  Expected: " << (expected) << "\n" \
               << "  Actual: " << (actual) << "\n" \
               << "  Tolerance: " << (tolerance); \
            TestLogger::GetInstance().Log(TestSeverity::FATAL, ss.str(), "ASSERT_NEAR_WITH_TOLERANCE"); \
            std::exit(1); \
        } \
    } while(0)

#define EXPECT_NEAR_WITH_TOLERANCE(actual, expected, tolerance, message) \
    do { \
        if (std::abs((actual) - (expected)) > (tolerance)) { \
            std::stringstream ss; \
            ss << "Expectation failed: " << message << "\n" \
               << "  Expected: " << (expected) << "\n" \
               << "  Actual: " << (actual) << "\n" \
               << "  Tolerance: " << (tolerance); \
            TestLogger::GetInstance().Log(TestSeverity::ERROR, ss.str(), "EXPECT_NEAR_WITH_TOLERANCE"); \
            throw std::runtime_error(ss.str()); \
        } \
    } while(0)

#define ASSERT_TRUE(condition, message) \
    do { \
        if (!(condition)) { \
            std::stringstream ss; \
            ss << "Assertion failed: " << message << "\n" \
               << "  Condition: " << #condition; \
            TestLogger::GetInstance().Log(TestSeverity::FATAL, ss.str(), "ASSERT_TRUE"); \
            std::exit(1); \
        } \
    } while(0)

#define EXPECT_TRUE(condition, message) \
    do { \
        if (!(condition)) { \
            std::stringstream ss; \
            ss << "Expectation failed: " << message << "\n" \
               << "  Condition: " << #condition; \
            TestLogger::GetInstance().Log(TestSeverity::ERROR, ss.str(), "EXPECT_TRUE"); \
            throw std::runtime_error(ss.str()); \
        } \
    } while(0)

#define ASSERT_FALSE(condition, message) \
    do { \
        if ((condition)) { \
            std::stringstream ss; \
            ss << "Assertion failed: " << message << "\n" \
               << "  Condition: !" << #condition; \
            TestLogger::GetInstance().Log(TestSeverity::FATAL, ss.str(), "ASSERT_FALSE"); \
            std::exit(1); \
        } \
    } while(0)

#define EXPECT_FALSE(condition, message) \
    do { \
        if ((condition)) { \
            std::stringstream ss; \
            ss << "Expectation failed: " << message << "\n" \
               << "  Condition: !" << #condition; \
            TestLogger::GetInstance().Log(TestSeverity::ERROR, ss.str(), "EXPECT_FALSE"); \
            throw std::runtime_error(ss.str()); \
        } \
    } while(0)

#define ASSERT_EQ(actual, expected, message) \
    do { \
        if ((actual) != (expected)) { \
            std::stringstream ss; \
            ss << "Assertion failed: " << message << "\n" \
               << "  Expected: " << (expected) << "\n" \
               << "  Actual: " << (actual); \
            TestLogger::GetInstance().Log(TestSeverity::FATAL, ss.str(), "ASSERT_EQ"); \
            std::exit(1); \
        } \
    } while(0)

#define EXPECT_EQ(actual, expected, message) \
    do { \
        if ((actual) != (expected)) { \
            std::stringstream ss; \
            ss << "Expectation failed: " << message << "\n" \
               << "  Expected: " << (expected) << "\n" \
               << "  Actual: " << (actual); \
            TestLogger::GetInstance().Log(TestSeverity::ERROR, ss.str(), "EXPECT_EQ"); \
            throw std::runtime_error(ss.str()); \
        } \
    } while(0)

#define ASSERT_NE(actual, expected, message) \
    do { \
        if ((actual) == (expected)) { \
            std::stringstream ss; \
            ss << "Assertion failed: " << message << "\n" \
               << "  Expected not equal: " << (expected) << "\n" \
               << "  Actual: " << (actual); \
            TestLogger::GetInstance().Log(TestSeverity::FATAL, ss.str(), "ASSERT_NE"); \
            std::exit(1); \
        } \
    } while(0)

#define EXPECT_NE(actual, expected, message) \
    do { \
        if ((actual) == (expected)) { \
            std::stringstream ss; \
            ss << "Expectation failed: " << message << "\n" \
               << "  Expected not equal: " << (expected) << "\n" \
               << "  Actual: " << (actual); \
            TestLogger::GetInstance().Log(TestSeverity::ERROR, ss.str(), "EXPECT_NE"); \
            throw std::runtime_error(ss.str()); \
        } \
    } while(0)

#define ASSERT_GT(actual, expected, message) \
    do { \
        if (!((actual) > (expected))) { \
            std::stringstream ss; \
            ss << "Assertion failed: " << message << "\n" \
               << "  Expected: " << (expected) << " < actual < infinity\n" \
               << "  Actual: " << (actual); \
            TestLogger::GetInstance().Log(TestSeverity::FATAL, ss.str(), "ASSERT_GT"); \
            std::exit(1); \
        } \
    } while(0)

#define EXPECT_GT(actual, expected, message) \
    do { \
        if (!((actual) > (expected))) { \
            std::stringstream ss; \
            ss << "Expectation failed: " << message << "\n" \
               << "  Expected: " << (expected) << " < actual < infinity\n" \
               << "  Actual: " << (actual); \
            TestLogger::GetInstance().Log(TestSeverity::ERROR, ss.str(), "EXPECT_GT"); \
            throw std::runtime_error(ss.str()); \
        } \
    } while(0)

#define ASSERT_GE(actual, expected, message) \
    do { \
        if (!((actual) >= (expected))) { \
            std::stringstream ss; \
            ss << "Assertion failed: " << message << "\n" \
               << "  Expected: " << (expected) << " <= actual\n" \
               << "  Actual: " << (actual); \
            TestLogger::GetInstance().Log(TestSeverity::FATAL, ss.str(), "ASSERT_GE"); \
            std::exit(1); \
        } \
    } while(0)

#define EXPECT_GE(actual, expected, message) \
    do { \
        if (!((actual) >= (expected))) { \
            std::stringstream ss; \
            ss << "Expectation failed: " << message << "\n" \
               << "  Expected: " << (expected) << " <= actual\n" \
               << "  Actual: " << (actual); \
            TestLogger::GetInstance().Log(TestSeverity::ERROR, ss.str(), "EXPECT_GE"); \
            throw std::runtime_error(ss.str()); \
        } \
    } while(0)

#define ASSERT_LT(actual, expected, message) \
    do { \
        if (!((actual) < (expected))) { \
            std::stringstream ss; \
            ss << "Assertion failed: " << message << "\n" \
               << "  Expected: " << "-infinity < actual" << (expected) << "\n" \
               << "  Actual: " << (actual); \
            TestLogger::GetInstance().Log(TestSeverity::FATAL, ss.str(), "ASSERT_LT"); \
            std::exit(1); \
        } \
    } while(0)

#define EXPECT_LT(actual, expected, message) \
    do { \
        if (!((actual) < (expected))) { \
            std::stringstream ss; \
            ss << "Expectation failed: " << message << "\n" \
               << "  Expected: " << "-infinity < actual" << (expected) << "\n" \
               << "  Actual: " << (actual); \
            TestLogger::GetInstance().Log(TestSeverity::ERROR, ss.str(), "EXPECT_LT"); \
            throw std::runtime_error(ss.str()); \
        } \
    } while(0)

#define ASSERT_LE(actual, expected, message) \
    do { \
        if (!((actual) <= (expected))) { \
            std::stringstream ss; \
            ss << "Assertion failed: " << message << "\n" \
               << "  Expected: actual <= " << (expected) << "\n" \
               << "  Actual: " << (actual); \
            TestLogger::GetInstance().Log(TestSeverity::FATAL, ss.str(), "ASSERT_LE"); \
            std::exit(1); \
        } \
    } while(0)

#define EXPECT_LE(actual, expected, message) \
    do { \
        if (!((actual) <= (expected))) { \
            std::stringstream ss; \
            ss << "Expectation failed: " << message << "\n" \
               << "  Expected: actual <= " << (expected) << "\n" \
               << "  Actual: " << (actual); \
            TestLogger::GetInstance().Log(TestSeverity::ERROR, ss.str(), "EXPECT_LE"); \
            throw std::runtime_error(ss.str()); \
        } \
    } while(0)

// Test benchmark macros
#define BENCHMARK(test_name, iterations) \
    do { \
        std::cout << "=== BENCHMARK: " << (test_name) << " ===" << std::endl; \
        auto start_time = std::chrono::high_resolution_clock::now(); \
        for (int i = 0; i < (iterations); ++i) { \
            /* Test code here */ \
        } \
        auto end_time = std::chrono::high_resolution_clock::now(); \
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time); \
        std::cout << "Execution time: " << duration.count() << " ms" << std::endl; \
        std::cout << "Average per iteration: " << (duration.count() / static_cast<double>(iterations)) << " ms" << std::endl; \
        std::cout << "Operations per second: " << ((iterations) / (duration.count() / 1000.0)) << std::endl; \
        std::cout << "=== END BENCHMARK ===" << std::endl; \
    } while(0)

// Test performance measurement utilities
class TestPerformance {
public:
    struct PerformanceMetrics {
        double execution_time_ms;
        size_t operations_count;
        double throughput_ops_per_sec;
        double efficiency;
    };
    
    static PerformanceMetrics MeasurePerformance(std::function<void()> test_function, size_t iterations = 1) {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < iterations; ++i) {
            test_function();
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        PerformanceMetrics metrics;
        metrics.execution_time_ms = static_cast<double>(duration.count());
        metrics.operations_count = iterations;
        metrics.throughput_ops_per_sec = (iterations / (metrics.execution_time_ms / 1000.0));
        metrics.efficiency = 100.0 / (metrics.execution_time_ms / (1000.0 / iterations)); // Simplified efficiency
        
        return metrics;
    }
    
    static std::string FormatMetrics(const PerformanceMetrics& metrics) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2);
        ss << "Execution time: " << metrics.execution_time_ms << " ms\n";
        ss << "Throughput: " << metrics.throughput_ops_per_sec << " ops/sec\n";
        ss << "Efficiency: " << metrics.efficiency << "%";
        return ss.str();
    }
    
    static void PrintPerformance(const std::string& test_name, const PerformanceMetrics& metrics) {
        std::cout << "=== PERFORMANCE: " << test_name << " ===" << std::endl;
        std::cout << FormatMetrics(metrics) << std::endl;
        std::cout << "=== END PERFORMANCE ===" << std::endl;
    }
};

// Test randomization utilities
class TestRandom {
public:
    TestRandom(unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count())
        : rng_(seed) {}
    
    float UniformFloat(float min, float max) {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(rng_);
    }
    
    int UniformInt(int min, int max) {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(rng_);
    }
    
    float NormalFloat(float mean, float stddev) {
        std::normal_distribution<float> dist(mean, stddev);
        return dist(rng_);
    }
    
    bool Bernoulli(float probability) {
        std::bernoulli_distribution dist(probability);
        return dist(rng_);
    }
    
    std::vector<float> RandomVector(size_t size, float min = 0.0f, float max = 1.0f) {
        std::vector<float> result;
        result.reserve(size);
        for (size_t i = 0; i < size; ++i) {
            result.push_back(UniformFloat(min, max));
        }
        return result;
    }
    
    std::vector<int> RandomPermutation(size_t size) {
        std::vector<int> result(size);
        std::iota(result.begin(), result.end(), 0);
        std::shuffle(result.begin(), result.end(), rng_);
        return result;
    }
    
    template<typename T>
    T RandomChoice(const std::vector<T>& choices) {
        if (choices.empty()) {
            throw std::invalid_argument("Cannot choose from empty vector");
        }
        std::uniform_int_distribution<size_t> dist(0, choices.size() - 1);
        return choices[dist(rng_)];
    }
    
private:
    std::mt19937 rng_;
};

// Test data generators
class TestDataGenerator {
public:
    static std::vector<float> GenerateNormalDistribution(size_t size, float mean = 0.0f, float stddev = 1.0f) {
        std::vector<float> result;
        TestRandom rng;
        for (size_t i = 0; i < size; ++i) {
            result.push_back(rng.NormalFloat(mean, stddev));
        }
        return result;
    }
    
    static std::vector<float> GenerateExponentialDistribution(size_t size, float lambda = 1.0f) {
        std::vector<float> result;
        TestRandom rng;
        for (size_t i = 0; i < size; ++i) {
            result.push_back(-std::log(1.0f - rng.UniformFloat(0.0f, 1.0f)) / lambda);
        }
        return result;
    }
    
    static std::vector<float> GeneratePoissonDistribution(size_t size, float lambda = 1.0f) {
        std::vector<float> result;
        TestRandom rng;
        for (size_t i = 0; i < size; ++i) {
            float x = lambda;
            float p = 1.0f;
            for (int k = 0; k < 100; ++k) { // Approximate Poisson
                p *= (x / (k + 1));
                if (rng.Bernoulli(p)) {
                    result.push_back(static_cast<float>(k));
                    break;
                }
            }
        }
        return result;
    }
    
    static std::vector<std::pair<float, float>> GenerateCorrelationData(size_t size, float correlation) {
        std::vector<std::pair<float, float>> result;
        TestRandom rng;
        
        for (size_t i = 0; i < size; ++i) {
            float x = rng.NormalFloat(0.0f, 1.0f);
            float noise = rng.NormalFloat(0.0f, sqrt(1.0f - correlation * correlation));
            float y = correlation * x + noise;
            result.push_back(std::make_pair(x, y));
        }
        return result;
    }
    
    static std::vector<float> GenerateTimeSeries(size_t size, float trend = 0.0f, float seasonality = 0.0f, float noise = 0.1f) {
        std::vector<float> result;
        TestRandom rng;
        
        for (size_t i = 0; i < size; ++i) {
            float time = static_cast<float>(i);
            float trend_value = trend * time;
            float seasonality_value = seasonality * sin(time * 2.0f * 3.14159f / 12.0f);
            float noise_value = noise * rng.NormalFloat(0.0f, 1.0f);
            result.push_back(trend_value + seasonality_value + noise_value);
        }
        return result;
    }
};

// Test comparison utilities
class TestComparisons {
public:
    static bool IsMonotonicIncreasing(const std::vector<float>& values) {
        return std::is_sorted(values.begin(), values.end());
    }
    
    static bool IsMonotonicDecreasing(const std::vector<float>& values) {
        return std::is_sorted(values.begin(), values.end(), std::greater<float>());
    }
    
    static bool IsSorted(const std::vector<float>& values, bool increasing = true) {
        if (increasing) {
            return std::is_sorted(values.begin(), values.end());
        } else {
            return std::is_sorted(values.begin(), values.end(), std::greater<float>());
        }
    }
    
    static size_t CountGreaterThan(const std::vector<float>& values, float threshold) {
        return std::count_if(values.begin(), values.end(),
                            [threshold](float val) { return val > threshold; });
    }
    
    static size_t CountLessThan(const std::vector<float>& values, float threshold) {
        return std::count_if(values.begin(), values.end(),
                            [threshold](float val) { return val < threshold; });
    }
    
    static size_t CountEqual(const std::vector<float>& values, float target) {
        return std::count(values.begin(), values.end(), target);
    }
    
    static float CalculateMean(const std::vector<float>& values) {
        if (values.empty()) return 0.0f;
        float sum = std::accumulate(values.begin(), values.end(), 0.0f);
        return sum / values.size();
    }
    
    static float CalculateVariance(const std::vector<float>& values) {
        if (values.empty()) return 0.0f;
        float mean = CalculateMean(values);
        float sum_sq = 0.0f;
        for (float val : values) {
            sum_sq += (val - mean) * (val - mean);
        }
        return sum_sq / values.size();
    }
    
    static float CalculateStandardDeviation(const std::vector<float>& values) {
        return std::sqrt(CalculateVariance(values));
    }
    
    static float CalculateMedian(std::vector<float> values) {
        if (values.empty()) return 0.0f;
        std::sort(values.begin(), values.end());
        size_t size = values.size();
        if (size % 2 == 0) {
            return (values[size / 2 - 1] + values[size / 2]) / 2.0f;
        } else {
            return values[size / 2];
        }
    }
    
    static float CalculatePercentile(std::vector<float> values, float percentile) {
        if (values.empty()) return 0.0f;
        std::sort(values.begin(), values.end());
        float index = (percentile / 100.0f) * (values.size() - 1);
        size_t lower_index = static_cast<size_t>(std::floor(index));
        size_t upper_index = static_cast<size_t>(std::ceil(index));
        
        if (lower_index == upper_index) {
            return values[lower_index];
        }
        
        float weight = index - lower_index;
        return values[lower_index] * (1.0f - weight) + values[upper_index] * weight;
    }
};

// Test validation utilities
class TestValidation {
public:
    static bool IsFinite(float value) {
        return std::isfinite(value);
    }
    
    static bool IsPositive(float value) {
        return value > 0.0f;
    }
    
    static bool IsNegative(float value) {
        return value < 0.0f;
    }
    
    static bool IsInRange(float value, float min, float max) {
        return value >= min && value <= max;
    }
    
    static bool IsZero(float value, float epsilon = 1e-6f) {
        return std::abs(value) <= epsilon;
    }
    
    static bool IsInteger(float value) {
        return std::abs(value - std::round(value)) <= 1e-6f;
    }
    
    static bool IsOdd(int value) {
        return value % 2 != 0;
    }
    
    static bool IsEven(int value) {
        return value % 2 == 0;
    }
    
    static bool IsPrime(int value) {
        if (value <= 1) return false;
        if (value == 2) return true;
        if (value % 2 == 0) return false;
        
        for (int i = 3; i * i <= value; i += 2) {
            if (value % i == 0) return false;
        }
        return true;
    }
    
    static bool IsPerfectSquare(float value) {
        float sqrt_val = std::sqrt(value);
        return std::abs(sqrt_val - std::round(sqrt_val)) <= 1e-6f;
    }
    
    static bool IsPowerOfTwo(int value) {
        return value > 0 && (value & (value - 1)) == 0;
    }
    
    static bool IsPalindrome(const std::string& str) {
        std::string reversed = str;
        std::reverse(reversed.begin(), reversed.end());
        return str == reversed;
    }
    
    static bool HasUniqueElements(const std::vector<float>& values) {
        return std::unique(values.begin(), values.end()) == values.end();
    }
    
    static bool IsSorted(const std::vector<float>& values, bool increasing = true) {
        if (increasing) {
            return std::is_sorted(values.begin(), values.end());
        } else {
            return std::is_sorted(values.begin(), values.end(), std::greater<float>());
        }
    }
};

// Test memory utilities
class TestMemory {
public:
    static size_t GetAvailableMemory() {
        // Implementation would depend on platform
        return 0; // Placeholder
    }
    
    static size_t GetUsedMemory() {
        // Implementation would depend on platform
        return 0; // Placeholder
    }
    
    static size_t GetTotalMemory() {
        // Implementation would depend on platform
        return 0; // Placeholder
    }
    
    static bool HasEnoughMemory(size_t required_bytes) {
        return GetAvailableMemory() >= required_bytes;
    }
    
    static void PrintMemoryUsage() {
        std::cout << "Memory Usage:" << std::endl;
        std::cout << "  Total: " << GetTotalMemory() / (1024 * 1024) << " MB" << std::endl;
        std::cout << "  Used: " << GetUsedMemory() / (1024 * 1024) << " MB" << std::endl;
        std::cout << "  Available: " << GetAvailableMemory() / (1024 * 1024) << " MB" << std::endl;
    }
    
    template<typename T>
    static std::vector<T> AllocateMemory(size_t count, T value = T()) {
        std::vector<T> result(count, value);
        return result;
    }
    
    template<typename T>
    static T* AllocateHeapMemory(size_t count, T value = T()) {
        T* result = new T[count];
        for (size_t i = 0; i < count; ++i) {
            result[i] = value;
        }
        return result;
    }
    
    template<typename T>
    static void FreeHeapMemory(T* ptr) {
        delete[] ptr;
    }
};

// Test timing utilities
class TestTiming {
public:
    static std::chrono::high_resolution_clock::time_point Now() {
        return std::chrono::high_resolution_clock::now();
    }
    
    static double ToMilliseconds(const std::chrono::high_resolution_clock::time_point& start,
                                   const std::chrono::high_resolution_clock::time_point& end) {
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        return static_cast<double>(duration.count());
    }
    
    static double ToMicroseconds(const std::chrono::high_resolution_clock::time_point& start,
                                  const std::chrono::high_resolution_clock::time_point& end) {
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        return static_cast<double>(duration.count());
    }
    
    static double ToSeconds(const std::chrono::high_resolution_clock::time_point& start,
                             const std::chrono::high_resolution_clock::time_point& end) {
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        return duration.count();
    }
    
    class Timer {
    public:
        Timer() : start_time_(std::chrono::high_resolution_clock::now()) {}
        
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
        
        double ElapsedSeconds() const {
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(end_time - start_time_);
            return duration.count();
        }
        
        void Reset() {
            start_time_ = std::chrono::high_resolution_clock::now();
        }
        
    private:
        std::chrono::high_resolution_clock::time_point start_time_;
    };
};

// Test output formatting
class TestFormatter {
public:
    static std::string FormatNumber(float number, int precision = 2) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(precision) << number;
        return ss.str();
    }
    
    static std::string FormatPercentage(float value, int precision = 1) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(precision) << (value * 100.0f) << "%";
        return ss.str();
    }
    
    static std::string FormatTime(double seconds) {
        std::stringstream ss;
        if (seconds < 60.0) {
            ss << std::fixed << std::setprecision(3) << seconds << " s";
        } else if (seconds < 3600.0) {
            ss << std::fixed << std::setprecision(3) << (seconds / 60.0) << " min";
        } else {
            ss << std::fixed << std::setprecision(3) << (seconds / 3600.0) << " h";
        }
        return ss.str();
    }
    
    static std::string FormatBytes(size_t bytes) {
        if (bytes < 1024) {
            return std::to_string(bytes) + " B";
        } else if (bytes < 1024 * 1024) {
            return std::to_string(bytes / 1024.0) + " KB";
        } else if (bytes < 1024 * 1024 * 1024) {
            return std::to_string(bytes / (1024.0 * 1024.0)) + " MB";
        } else {
            return std::to_string(bytes / (1024.0 * 1024.0 * 1024.0)) + " GB";
        }
    }
    
    static std::string FormatVector(const std::vector<float>& values, const std::string& delimiter = ", ") {
        std::stringstream ss;
        for (size_t i = 0; i < values.size(); ++i) {
            if (i > 0) ss << delimiter;
            ss << values[i];
        }
        return ss.str();
    }
    
    static std::string FormatMap(const std::map<std::string, float>& data, const std::string& delimiter = "\n") {
        std::stringstream ss;
        for (const auto& pair : data) {
            ss << pair.first << ": " << pair.second << delimiter;
        }
        return ss.str();
    }
    
    static std::string CenterString(const std::string& str, size_t width) {
        if (str.length() >= width) {
            return str;
        }
        size_t pad_left = (width - str.length()) / 2;
        size_t pad_right = width - str.length() - pad_left;
        return std::string(pad_left, ' ') + str + std::string(pad_right, ' ');
    }
    
    static std::string RightAlignString(const std::string& str, size_t width) {
        if (str.length() >= width) {
            return str;
        }
        return std::string(width - str.length(), ' ') + str;
    }
    
    static std::string LeftAlignString(const std::string& str, size_t width) {
        if (str.length() >= width) {
            return str;
        }
        return str + std::string(width - str.length(), ' ');
    }
    
    static std::string RepeatString(const std::string& str, size_t count) {
        std::stringstream ss;
        for (size_t i = 0; i < count; ++i) {
            ss << str;
        }
        return ss.str();
    }
    
    static std::string GetBoxedText(const std::string& text, size_t width = 80) {
        std::stringstream ss;
        ss << std::string(width, '=') << std::endl;
        ss << "| " << std::left << std::setw(static_cast<int>(width - 4)) << text << " |" << std::endl;
        ss << std::string(width, '=');
        return ss.str();
    }
    
    static std::string GetDashedLine(size_t width = 80) {
        return std::string(width, '-');
    }
    
    static std::string GetIndentedText(const std::string& text, size_t indent = 2) {
        std::stringstream ss;
        std::istringstream iss(text);
        std::string line;
        while (std::getline(iss, line)) {
            ss << std::string(indent, ' ') << line << std::endl;
        }
        return ss.str();
    }
};}}

// Test environment setup and teardown
class TestEnvironment {
public:
    static void SetUpEnvironment() {
        std::cout << "=== Test Environment Setup ===" << std::endl;
        std::cout << "Setting up test environment..." << std::endl;
        
        // Initialize test logger
        TestLogger& logger = TestLogger::GetInstance();
        logger.SetLogLevel(TestSeverity::INFO);
        
        // Log environment information
        logger.Log(TestSeverity::INFO, "Test environment initialized", "Setup");
        logger.Log(TestSeverity::INFO, "Test framework ready", "Setup");
        
        std::cout << "Test environment setup completed successfully." << std::endl;
        std::cout << "=== Environment Setup Complete ===" << std::endl;
    }
    
    static void TearDownEnvironment() {
        std::cout << "=== Test Environment Teardown ===" << std::endl;
        std::cout << "Cleaning up test environment..." << std::endl;
        
        // Clear test logs
        TestLogger::GetInstance().ClearLogs();
        
        // Print test summary
        std::cout << "Test environment cleaned up successfully." << std::endl;
        std::cout << "=== Environment Teardown Complete ===" << std::endl;
    }
    
    static void PrintTestSummary() {
        const auto& logger = TestLogger::GetInstance();
        auto logs = logger.GetLogs();
        
        std::cout << "=== Test Summary ===" << std::endl;
        std::cout << "Total log entries: " << logs.size() << std::endl;
        
        std::map<TestSeverity, size_t> severity_counts;
        for (const auto& entry : logs) {
            severity_counts[entry.severity]++;
        }
        
        for (const auto& pair : severity_counts) {
            std::string severity_str;
            switch (pair.first) {
                case TestSeverity::INFO: severity_str = "INFO"; break;
                case TestSeverity::WARNING: severity_str = "WARNING"; break;
                case TestSeverity::ERROR: severity_str = "ERROR"; break;
                case TestSeverity::FATAL: severity_str = "FATAL"; break;
            }
            std::cout << severity_str << ": " << pair.second << std::endl;
        }
        
        std::cout << "=== End Test Summary ===" << std::endl;
    }
};

// Test main function
inline int RunAllTests() {
    TestEnvironment::SetUpEnvironment();
    
    int result = 0;
    try {
        result = RUN_ALL_TESTS();
    } catch (const std::exception& e) {
        std::cerr << "Test execution failed with exception: " << e.what() << std::endl;
        result = 1;
    }
    
    TestEnvironment::PrintTestSummary();
    TestEnvironment::TearDownEnvironment();
    
    return result;
}

// Test registration utilities
class TestRegistration {
public:
    template<typename TestCase>
    static void RegisterTest(const std::string& name, TestCase test_function) {
        // In a real implementation, this would register the test with the test framework
        std::cout << "Registering test: " << name << std::endl;
    }
    
    static void RegisterTestSuite(const std::string& name) {
        std::cout << "Registering test suite: " << name << std::endl;
    }
    
    static void RegisterFixture(const std::string& name) {
        std::cout << "Registering fixture: " << name << std::endl;
    }
};

// Test configuration
struct TestConfig {
    bool enable_benchmarks;
    bool enable_performance_tests;
    bool enable_stress_tests;
    bool enable_memory_tests;
    std::string output_format;
    std::string log_level;
    size_t timeout_ms;
    size_t max_memory_mb;
    
    TestConfig() 
        : enable_benchmarks(true)
        , enable_performance_tests(true)
        , enable_stress_tests(true)
        , enable_memory_tests(true)
        , output_format("text")
        , log_level("INFO")
        , timeout_ms(30000)
        , max_memory_mb(1024) {}
};

// Global test configuration
static TestConfig g_test_config;

// Test configuration management
class TestConfigManager {
public:
    static void SetConfig(const TestConfig& config) {
        g_test_config = config;
    }
    
    static const TestConfig& GetConfig() {
        return g_test_config;
    }
    
    static void PrintConfig() {
        std::cout << "=== Test Configuration ===" << std::endl;
        std::cout << "Benchmarks: " << (g_test_config.enable_benchmarks ? "ENABLED" : "DISABLED") << std::endl;
        std::cout << "Performance tests: " << (g_test_config.enable_performance_tests ? "ENABLED" : "DISABLED") << std::endl;
        std::cout << "Stress tests: " << (g_test_config.enable_stress_tests ? "ENABLED" : "DISABLED") << std::endl;
        std::cout << "Memory tests: " << (g_test_config.enable_memory_tests ? "ENABLED" : "DISABLED") << std::endl;
        std::cout << "Output format: " << g_test_config.output_format << std::endl;
        std::cout << "Log level: " << g_test_config.log_level << std::endl;
        std::cout << "Timeout: " << g_test_config.timeout_ms << " ms" << std::endl;
        std::cout << "Max memory: " << g_test_config.max_memory_mb << " MB" << std::endl;
        std::cout << "=== End Test Configuration ===" << std::endl;
    }
};

// Test main macro
#define TEST_MAIN() \
    int main(int argc, char** argv) { \
        TestEnvironment::SetUpEnvironment(); \
        TestConfigManager::PrintConfig(); \
        \
        int result = RUN_ALL_TESTS(); \
        \
        TestEnvironment::PrintTestSummary(); \
        TestEnvironment::TearDownEnvironment(); \
        \
        return result; \
    }