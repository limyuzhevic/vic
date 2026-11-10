// Comprehensive Testing Framework for NLM
// Enhanced test framework with better validation, reporting, and analysis

#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <cassert>
#include <map>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <functional>
#include <algorithm>
#include <cmath>

// Test framework namespace
namespace nlm_test {
    // Test result structure
    struct TestResult {
        std::string name;
        std::string category;
        bool passed;
        std::string message;
        double executionTime;
        std::map<std::string, std::string> metadata;
        
        TestResult(const std::string& testName, const std::string& testCategory)
            : name(testName), category(testCategory), passed(true), executionTime(0.0) {}
    };
    
    // Test suite class
    class TestSuite {
    private:
        std::vector<TestResult> results;
        std::string name;
        bool verbose;
        
    public:
        TestSuite(const std::string& suiteName, bool verboseOutput = false)
            : name(suiteName), verbose(verboseOutput) {}
        
        void addResult(const TestResult& result) {
            results.push_back(result);
            if (verbose) {
                std::cout << "    " << result.name << ": " << (result.passed ? "PASS" : "FAIL") << std::endl;
                if (!result.passed && !result.message.empty()) {
                    std::cout << "      Error: " << result.message << std::endl;
                }
            }
        }
        
        const std::vector<TestResult>& getResults() const { return results; }
        std::string getName() const { return name; }
        size_t getTestCount() const { return results.size(); }
        size_t getPassedCount() const {
            return std::count_if(results.begin(), results.end(), 
                               [](const TestResult& r) { return r.passed; });
        }
        size_t getFailedCount() const {
            return results.size() - getPassedCount();
        }
        double getPassRate() const {
            return results.empty() ? 0.0 : (getPassedCount() * 100.0) / results.size();
        }
    };
    
    // Enhanced assertion functions
    class Assert {
    public:
        static void equal(bool condition, const std::string& message = "Assertion failed") {
            if (!condition) {
                throw AssertionError(message);
            }
        }
        
        static void equal(double expected, double actual, double tolerance = 1e-6,
                         const std::string& message = "Values are not equal") {
            if (std::abs(expected - actual) > tolerance) {
                std::stringstream ss;
                ss << message << "\n  Expected: " << expected << "\n  Actual: " << actual;
                throw AssertionError(ss.str());
            }
        }
        
        static void notEqual(double value1, double value2, double tolerance = 1e-6,
                           const std::string& message = "Values should not be equal") {
            if (std::abs(value1 - value2) <= tolerance) {
                throw AssertionError(message);
            }
        }
        
        static void greaterThan(double value, double threshold,
                              const std::string& message = "Value should be greater than threshold") {
            if (value <= threshold) {
                throw AssertionError(message);
            }
        }
        
        static void lessThan(double value, double threshold,
                           const std::string& message = "Value should be less than threshold") {
            if (value >= threshold) {
                throw AssertionError(message);
            }
        }
        
        static void inRange(double value, double min, double max,
                          const std::string& message = "Value should be within range") {
            if (value < min || value > max) {
                throw AssertionError(message);
            }
        }
        
        static void notNull(void* ptr, const std::string& message = "Pointer should not be null") {
            if (ptr == nullptr) {
                throw AssertionError(message);
            }
        }
        
        static void empty(const std::string& str, const std::string& message = "String should not be empty") {
            if (!str.empty()) {
                throw AssertionError(message);
            }
        }
        
        static void notEmpty(const std::string& str, const std::string& message = "String should not be empty") {
            if (str.empty()) {
                throw AssertionError(message);
            }
        }
    };
    
    // Performance measurement
    class PerformanceMeter {
    public:
        struct PerformanceResult {
            std::string name;
            double executionTime; // seconds
            double cpuTime;       // seconds
            size_t memoryUsage;   // bytes
            double operationsPerSecond;
        };
        
        static PerformanceResult measure(const std::string& name, std::function<void()> func) {
            auto startTime = std::chrono::high_resolution_clock::now();
            auto cpuStartTime = std::chrono::system_clock::now();
            
            func();
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto cpuEndTime = std::chrono::system_clock::now();
            
            PerformanceResult result;
            result.name = name;
            result.executionTime = std::chrono::duration<double>(endTime - startTime).count();
            result.cpuTime = std::chrono::duration<double>(cpuEndTime - cpuStartTime).count();
            result.memoryUsage = 0; // TODO: Implement memory tracking
            result.operationsPerSecond = 0.0; // TODO: Define operation count
            
            return result;
        }
    };
    
    // Test reporter
    class TestReporter {
    public:
        static void printSummary(const std::vector<TestSuite>& suites) {
            std::cout << std::endl;
            std::cout << "╔════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
            std::cout << "║                             NLM Test Suite Summary                                ║" << std::endl;
            std::cout << "╠════════════════════════════════════════════════════════════════════════════════╣" << std::endl;
            
            size_t totalTests = 0;
            size_t totalPassed = 0;
            
            for (const auto& suite : suites) {
                std::cout << "║ " << std::setw(50) << suite.getName() << "║ " 
                          << std::setw(8) << suite.getTestCount() << " tests, "
                          << std::setw(6) << suite.getPassedCount() << " passed ║" << std::endl;
                totalTests += suite.getTestCount();
                totalPassed += suite.getPassedCount();
            }
            
            std::cout << "╠════════════════════════════════════════════════════════════════════════════════╣" << std::endl;
            std::cout << "║ " << std::setw(50) << "Overall Summary" << "║ "
                      << std::setw(8) << totalTests << " tests, "
                      << std::setw(6) << totalPassed << " passed ║" << std::endl;
            
            if (totalTests > 0) {
                double passRate = (totalPassed * 100.0) / totalTests;
                std::cout << "║ " << std::setw(50) << "Pass Rate: " << std::fixed << std::setprecision(2) << passRate << "%" << "║" << std::endl;
            }
            
            std::cout << "╚════════════════════════════════════════════════════════════════════════════════╝" << std::endl;
            std::cout << std::endl;
            
            if (totalPassed != totalTests) {
                std::cout << "FAILED TESTS:" << std::endl;
                for (const auto& suite : suites) {
                    for (const auto& result : suite.getResults()) {
                        if (!result.passed) {
                            std::cout << "  - " << suite.getName() << "." << result.name << ": " << result.message << std::endl;
                        }
                    }
                }
            }
        }
        
        static void exportResults(const std::vector<TestSuite>& suites, const std::string& filename) {
            std::ofstream file(filename);
            if (!file.is_open()) {
                std::cerr << "ERROR: Cannot open output file: " << filename << std::endl;
                return;
            }
            
            file << "NLM Test Results Export" << std::endl;
            file << "Generated: " << getCurrentTimestamp() << std::endl;
            file << std::endl;
            
            for (const auto& suite : suites) {
                file << "Test Suite: " << suite.getName() << std::endl;
                file << "Tests: " << suite.getTestCount() << " (" << suite.getPassedCount() << " passed, " 
                     << suite.getFailedCount() << " failed)" << std::endl;
                file << "Pass Rate: " << suite.getPassRate() << "%" << std::endl;
                file << std::endl;
                
                for (const auto& result : suite.getResults()) {
                    file << "  - " << result.name << ": " << (result.passed ? "PASS" : "FAIL")
                         << " (" << std::fixed << std::setprecision(6) << result.executionTime << "s)";
                    if (!result.message.empty()) {
                        file << " - " << result.message;
                    }
                    file << std::endl;
                }
                file << std::endl;
            }
            
            file.close();
        }
        
    private:
        static std::string getCurrentTimestamp() {
            auto now = std::chrono::system_clock::now();
            auto in_time_t = std::chrono::system_clock::to_time_t(now);
            
            std::stringstream ss;
            ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
            return ss.str();
        }
    };
    
    // Assertion error
    class AssertionError : public std::runtime_error {
    public:
        AssertionError(const std::string& message) : std::runtime_error(message) {}
    };
} // namespace nlm_test

// Backward compatibility macros for existing tests
#define ASSERT_EQ(expected, actual) assert((expected) == (actual))
#define ASSERT_DOUBLE_EQ(expected, actual, tolerance) assert(std::abs((expected) - (actual)) < tolerance)
#define ASSERT_TRUE(condition) assert(condition)
#define ASSERT_FALSE(condition) assert(!(condition))
#define ASSERT_THROW(expression, exception_type) do { \
    try { \
        expression; \
        assert(false && "Expected " #expression " to throw " #exception_type); \
    } catch (const exception_type&) { \
    } catch (...) { assert(false && "Unexpected exception"); } \
} while(0)