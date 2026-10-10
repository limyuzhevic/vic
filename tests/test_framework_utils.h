// Test framework utilities for comprehensive testing
#pragma once

#include <chrono>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <random>
#include <cassert>
#include <iomanip>
#include <sstream>

namespace test_utils {

// Performance measurement utilities
struct PerformanceResult {
    double timeMs;
    uint64_t memoryBytes;
    uint64_t operationsPerSecond;
};

class PerformanceTimer {
private:
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    
public:
    void start() {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    void stop() {
        endTime = std::chrono::high_resolution_clock::now();
    }
    
    double getElapsedMs() const {
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        return duration.count() / 1000.0;
    }
    
    double getElapsedSeconds() const {
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        return duration.count() / 1'000'000.0;
    }
};

// Validation utilities
class ValidationUtils {
public:
    // Check that a value is within expected range
    template<typename T>
    static bool withinRange(T value, T min, T max) {
        return value >= min && value <= max;
    }
    
    // Check that a vector has expected size
    template<typename T>
    static bool hasExpectedSize(const std::vector<T>& vec, size_t expectedSize) {
        return vec.size() == expectedSize;
    }
    
    // Check that a vector contains only positive values
    template<typename T>
    static bool allPositive(const std::vector<T>& vec) {
        return std::all_of(vec.begin(), vec.end(), [](T x) { return x > 0; });
    }
    
    // Check that a vector contains only non-negative values
    template<typename T>
    static bool allNonNegative(const std::vector<T>& vec) {
        return std::all_of(vec.begin(), vec.end(), [](T x) { return x >= 0; });
    }
    
    // Check that a vector contains only values in [0,1] range
    template<typename T>
    static bool normalized(const std::vector<T>& vec) {
        return std::all_of(vec.begin(), vec.end(), [](T x) { return x >= 0 && x <= 1; });
    }
    
    // Compute mean of vector
    template<typename T>
    static double computeMean(const std::vector<T>& vec) {
        if (vec.empty()) return 0.0;
        double sum = std::accumulate(vec.begin(), vec.end(), 0.0);
        return sum / vec.size();
    }
    
    // Compute standard deviation
    template<typename T>
    static double computeStdDev(const std::vector<T>& vec) {
        if (vec.size() < 2) return 0.0;
        double mean = computeMean(vec);
        double sumSq = 0.0;
        for (T val : vec) {
            sumSq += (val - mean) * (val - mean);
        }
        return std::sqrt(sumSq / (vec.size() - 1));
    }
    
    // Check if values are monotonically increasing
    template<typename T>
    static bool isMonotonicIncreasing(const std::vector<T>& vec) {
        return std::is_sorted(vec.begin(), vec.end());
    }
    
    // Check if values are monotonically decreasing
    template<typename T>
    static bool isMonotonicDecreasing(const std::vector<T>& vec) {
        return std::is_sorted(vec.begin(), vec.end(), std::greater<T>());
    }
    
    // Generate random test data
    static std::vector<float> generateRandomData(size_t size, float min = 0.0f, float max = 1.0f) {
        static std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
        std::uniform_real_distribution<float> dist(min, max);
        
        std::vector<float> data(size);
        for (size_t i = 0; i < size; ++i) {
            data[i] = dist(rng);
        }
        return data;
    }
    
    // Test data generator for specific module types
    static std::vector<float> generateSpikeTimes(size_t count, float min = 0.0f, float max = 100.0f) {
        auto times = generateRandomData(count, min, max);
        std::sort(times.begin(), times.end());
        return times;
    }
    
    static std::vector<float> generateWeightValues(size_t count, float min = -1.0f, float max = 1.0f) {
        return generateRandomData(count, min, max);
    }
    
    static std::vector<float> generateStateVectors(size_t numVectors, size_t vectorSize) {
        std::vector<float> states;
        for (size_t i = 0; i < numVectors; ++i) {
            auto vec = generateRandomData(vectorSize, 0.0f, 1.0f);
            states.insert(states.end(), vec.begin(), vec.end());
        }
        return states;
    }
};

// Benchmarking utilities
class BenchmarkUtils {
public:
    // Benchmark a test function
    template<typename Func, typename... Args>
    static PerformanceResult benchmark(Func&& func, Args&&... args) {
        PerformanceTimer timer;
        timer.start();
        
        // Run the function
        func(std::forward<Args>(args)...);
        
        timer.stop();
        
        PerformanceResult result;
        result.timeMs = timer.getElapsedMs();
        
        // Estimate operations per second (simplified)
        double elapsedSeconds = timer.getElapsedSeconds();
        result.operationsPerSecond = static_cast<uint64_t>(1.0 / elapsedSeconds);
        
        return result;
    }
    
    // Benchmark with multiple iterations
    template<typename Func, typename... Args>
    static PerformanceResult benchmarkMultiple(Func&& func, size_t iterations, Args&&... args) {
        PerformanceTimer timer;
        timer.start();
        
        for (size_t i = 0; i < iterations; ++i) {
            func(std::forward<Args>(args)...);
        }
        
        timer.stop();
        
        PerformanceResult result;
        result.timeMs = timer.getElapsedMs() / iterations;
        
        double elapsedSeconds = timer.getElapsedSeconds();
        result.operationsPerSecond = static_cast<uint64_t>(iterations / elapsedSeconds);
        
        return result;
    }
    
    // Memory usage benchmark (simplified - in real implementation would use system calls)
    static PerformanceResult benchmarkMemory(Func&& func, Args&&... args) {
        PerformanceResult result = benchmark(std::forward<Func>(func), std::forward<Args>(args)...);
        // Simplified memory estimation
        result.memoryBytes = static_cast<uint64_t>(result.operationsPerSecond * 0.1); // Rough estimate
        return result;
    }
};

// Error condition testing utilities
class ErrorConditionUtils {
public:
    // Test file I/O error conditions
    static bool testFileNotFound(const std::string& filename) {
        // This would test if the file doesn't exist
        // Implementation would depend on the actual file system usage
        return false; // Placeholder
    }
    
    // Test memory allocation failure (would need to be implemented with custom allocators)
    static bool testMemoryAllocationFailure() {
        // This is difficult to test reliably
        return false; // Placeholder
    }
    
    // Test invalid configuration scenarios
    static bool testInvalidConfigScenarios() {
        // Test with invalid configs
        return false; // Placeholder
    }
    
    // Test edge cases for numerical stability
    static bool testNumericalStability() {
        // Test with extreme values, zero, infinity, etc.
        return false; // Placeholder
    }
};

// Test result reporting
class TestReporter {
public:
    static void printHeader(const std::string& testName) {
        std::cout << "=== " << testName << " ===" << std::endl;
    }
    
    static void printResult(const std::string& testName, bool passed, double timeMs = 0.0) {
        std::cout << (passed ? "PASS" : "FAIL") << ": " << testName;
        if (timeMs > 0.0) {
            std::cout << " (" << std::fixed << std::setprecision(3) << timeMs << " ms)";
        }
        std::cout << std::endl;
    }
    
    static void printPerformance(const PerformanceResult& result, const std::string& testName) {
        std::cout << "Performance: " << testName << std::endl;
        std::cout << "  Time: " << std::fixed << std::setprecision(3) << result.timeMs << " ms" << std::endl;
        std::cout << "  Operations/sec: " << result.operationsPerSecond << std::endl;
        std::cout << "  Memory: " << result.memoryBytes << " bytes" << std::endl;
    }
    
    static void printSection(const std::string& sectionName) {
        std::cout << std::endl << "--- " << sectionName << " ---" << std::endl;
    }
    
    static void printError(const std::string& testName, const std::string& error) {
        std::cout << "ERROR in " << testName << ": " << error << std::endl;
    }
};

// Test framework utilities
void runAll();

} // namespace test_utils

