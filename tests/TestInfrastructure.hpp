#pragma once

#include <chrono>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <cassert>
#include <atomic>
#include <stdexcept>
#include <random>

namespace nlm_test {

// Test result tracking
struct TestResult {
    std::string name;
    bool passed;
    double duration_ms;
    std::string errorMessage;
    
    TestResult(const std::string& testName) 
        : name(testName), passed(false), duration_ms(0.0), errorMessage("") {}
};

// Enhanced assertion macros with timing and error reporting
#define TEST_ASSERT(condition, message) \
    do { \
        bool condition_result = (condition); \
        if (!condition_result) { \
            std::cerr << "FAILED: " << message << std::endl; \
            throw std::runtime_error(std::string("Assertion failed: ") + message); \
        } \
    } while(0)

// Test runner with timing and result collection
class TestRunner {
private:
    std::vector<TestResult> results;
    bool verbose;
    
public:
    TestRunner(bool verboseOutput = false) : verbose(verboseOutput) {}
    
    template<typename TestFunc>
    void runTest(const std::string& name, TestFunc testFunc) {
        auto start_time = std::chrono::high_resolution_clock::now();
        TestResult result(name);
        
        try {
            testFunc();
            result.passed = true;
            if (verbose) {
                std::cout << "    " << name << " PASSED" << std::endl;
            }
        } catch (const std::exception& e) {
            result.passed = false;
            result.errorMessage = e.what();
            if (verbose) {
                std::cout << "    " << name << " FAILED: " << e.what() << std::endl;
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        result.duration_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
        results.push_back(result);
    }
    
    void runAll() {
        // Print summary
        int passed = 0;
        int failed = 0;
        double total_time = 0.0;
        
        for (const auto& result : results) {
            total_time += result.duration_ms;
            if (result.passed) {
                passed++;
            } else {
                failed++;
            }
        }
        
        std::cout << std::endl;
        std::cout << "=== Test Summary ===" << std::endl;
        std::cout << "Total tests: " << results.size() << std::endl;
        std::cout << "Passed: " << passed << std::endl;
        std::cout << "Failed: " << failed << std::endl;
        std::cout << "Total time: " << std::fixed << std::setprecision(2) << total_time << " ms" << std::endl;
        
        if (failed > 0) {
            std::cout << std::endl << "=== Failed Tests ===" << std::endl;
            for (const auto& result : results) {
                if (!result.passed) {
                    std::cout << result.name << ": " << result.errorMessage << std::endl;
                }
            }
        }
        
        if (failed > 0) {
            throw std::runtime_error("Some tests failed");
        }
    }
    
    const std::vector<TestResult>& getResults() const { return results; }
    double getTotalTime() const {
        double total = 0.0;
        for (const auto& result : results) {
            total += result.duration_ms;
        }
        return total;
    }
};

// Common test utilities
namespace utils {
    // Generate unique neuron IDs for testing
    template<typename IDType>
    IDType generateId() {
        static std::atomic<uint64_t> counter{0};
        return IDType(counter++);
    }
    
    // Format double for comparison in tests
    std::string formatDouble(double value, int precision = 6) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(precision) << value;
        return ss.str();
    }
    
    // Check if two floats are approximately equal
    bool approxEqual(float a, float b, float epsilon = 1e-6f) {
        return std::abs(a - b) < epsilon;
    }
    
    // Check if two doubles are approximately equal
    bool approxEqual(double a, double b, double epsilon = 1e-6) {
        return std::abs(a - b) < epsilon;
    }
    
    // Generate random values with seed for reproducibility
    class TestRandom {
    private:
        std::mt19937 rng;
        
    public:
        TestRandom(uint32_t seed = 42) : rng(seed) {}
        
        float uniformFloat(float min = 0.0f, float max = 1.0f) {
            std::uniform_real_distribution<float> dist(min, max);
            return dist(rng);
        }
        
        double uniformDouble(double min = 0.0, double max = 1.0) {
            std::uniform_real_distribution<double> dist(min, max);
            return dist(rng);
        }
        
        int uniformInt(int min, int max) {
            std::uniform_int_distribution<int> dist(min, max);
            return dist(rng);
        }
        
        bool bernoulli(float p) {
            std::bernoulli_distribution dist(p);
            return dist(rng);
        }
    };
}

} // namespace nlm_test
