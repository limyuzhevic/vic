#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include <iomanip>
#include <unordered_map>
#include <memory>

namespace nlm {
namespace test {

// Test result structure
struct TestResult {
    std::string name;
    bool passed;
    double executionTime;
    std::string errorMessage;
    
    TestResult(const std::string& testName, bool success, double time, const std::string& error = "")
        : name(testName), passed(success), executionTime(time), errorMessage(error) {}
};

// Test function signature
using TestFunction = std::function<void()>;

// Test category for organization
enum class TestCategory {
    CORE,
    BRAIN,
    NEURON,
    SYNAPSES,
    PLASTICITY,
    DYNAMICS,
    MEMORY,
    COGNITION,
    ADVANCED
};

// Test case class for better organization
class TestCase {
public:
    TestCase(const std::string& name, TestCategory category, TestFunction testFunc)
        : name(name), category(category), testFunc(testFunc) {}
    
    const std::string& getName() const { return name; }
    TestCategory getCategory() const { return category; }
    void run() { testFunc(); }
    
private:
    std::string name;
    TestCategory category;
    TestFunction testFunc;
};

// Test framework class
class TestFramework {
public:
    static TestFramework& getInstance() {
        static TestFramework instance;
        return instance;
    }
    
    // Register a test case
    void registerTest(const std::string& name, TestCategory category, TestFunction testFunc) {
        tests.push_back(TestCase(name, category, testFunc));
    }
    
    // Run all tests
    std::vector<TestResult> runAllTests() {
        std::vector<TestResult> results;
        results.reserve(tests.size());
        
        std::cout << "=== Enhanced NLM Test Suite ===" << std::endl;
        std::cout << "Running " << tests.size() << " test cases..." << std::endl;
        std::cout << std::endl;
        
        // Run tests by category
        for (const auto& category : {TestCategory::CORE, TestCategory::BRAIN, TestCategory::NEURON,
                                     TestCategory::SYNAPSES, TestCategory::PLASTICITY,
                                     TestCategory::DYNAMICS, TestCategory::MEMORY,
                                     TestCategory::COGNITION, TestCategory::ADVANCED}) {
            runTestsByCategory(category, results);
        }
        
        return results;
    }
    
    // Run tests for a specific category
    std::vector<TestResult> runTestsByCategory(TestCategory category) {
        std::vector<TestResult> results;
        runTestsByCategory(category, results);
        return results;
    }
    
    // Print test results
    static void printResults(const std::vector<TestResult>& results) {
        std::cout << "=== Test Results Summary ===" << std::endl;
        std::cout << std::endl;
        
        int passed = 0, failed = 0;
        double totalTime = 0.0;
        
        for (const auto& result : results) {
            std::cout << (result.passed ? "✓ " : "✗ ") << result.name;
            if (!result.passed) {
                std::cout << " - " << result.errorMessage;
                failed++;
            } else {
                passed++;
            }
            
            // Format execution time
            std::cout << " (" << std::fixed << std::setprecision(4) << result.executionTime << "s)";
            std::cout << std::endl;
            
            totalTime += result.executionTime;
        }
        
        std::cout << std::endl;
        std::cout << "=== Summary ===" << std::endl;
        std::cout << "  Total tests: " << results.size() << std::endl;
        std::cout << "  Passed: " << passed << std::endl;
        std::cout << "  Failed: " << failed << std::endl;
        std::cout << "  Total time: " << std::fixed << std::setprecision(4) << totalTime << "s" << std::endl;
        std::cout << std::endl;
        
        if (failed > 0) {
            std::cout << "=== Some tests failed ===" << std::endl;
            return 1;
        } else {
            std::cout << "=== All tests passed ===" << std::endl;
            return 0;
        }
    }
    
private:
    TestFramework() {}
    ~TestFramework() {}
    TestFramework(const TestFramework&) = delete;
    TestFramework& operator=(const TestFramework&) = delete;
    
    void runTestsByCategory(TestCategory category, std::vector<TestResult>& results) {
        std::cout << "--- " << getCategoryName(category) << " Tests ---" << std::endl;
        
        for (const auto& testCase : tests) {
            if (testCase.getCategory() == category) {
                runSingleTest(testCase, results);
            }
        }
        
        std::cout << std::endl;
    }
    
    void runSingleTest(const TestCase& testCase, std::vector<TestResult>& results) {
        auto startTime = std::chrono::high_resolution_clock::now();
        bool passed = false;
        std::string errorMessage = "";
        
        try {
            testCase.run();
            passed = true;
        } catch (const std::exception& e) {
            errorMessage = e.what();
            passed = false;
        } catch (...) {
            errorMessage = "Unknown error occurred";
            passed = false;
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - startTime).count();
        
        results.emplace_back(testCase.getName(), passed, duration, errorMessage);
    }
    
    std::string getCategoryName(TestCategory category) {
        switch (category) {
            case TestCategory::CORE: return "Core Tests";
            case TestCategory::BRAIN: return "Brain Tests";
            case TestCategory::NEURON: return "Neuron Tests";
            case TestCategory::SYNAPSES: return "Synapse Tests";
            case TestCategory::PLASTICITY: return "Plasticity Tests";
            case TestCategory::DYNAMICS: return "Dynamics Tests";
            case TestCategory::MEMORY: return "Memory Tests";
            case TestCategory::COGNITION: return "Cognition Tests";
            case TestCategory::ADVANCED: return "Advanced Tests";
            default: return "Unknown";
        }
    }
    
    std::vector<TestCase> tests;
};

// Macro for registering tests
#define REGISTER_TEST(name, category, testFunc) \
    namespace { \
        class Register##name { \
        public: \
            Register##name() { \
                TestFramework::getInstance().registerTest(name, category, testFunc); \
            } \
        }; \
        Register##name register##name; \
    }

// Helper macros for test assertions
#define ASSERT_TRUE(condition) \
    if (!(condition)) { \
        throw std::runtime_error("Assertion failed: " #condition); \
    }

#define ASSERT_FALSE(condition) \
    if (condition) { \
        throw std::runtime_error("Assertion failed: !" #condition); \
    }

#define ASSERT_EQUAL(expected, actual) \
    if ((expected) != (actual)) { \
        throw std::runtime_error("Assertion failed: " #expected " != " #actual " (" + \
                                 std::to_string(expected) + " != " + std::to_string(actual) + ")"); \
    }

#define ASSERT_NOT_EQUAL(expected, actual) \
    if ((expected) == (actual)) { \
        throw std::runtime_error("Assertion failed: " #expected " == " #actual " (" + \
                                 std::to_string(expected) + " == " + std::to_string(actual) + ")"); \
    }

#define ASSERT_GREATER(expected, actual) \
    if ((expected) >= (actual)) { \
        throw std::runtime_error("Assertion failed: " #expected " >= " #actual " (" + \
                                 std::to_string(expected) + " >= " + std::to_string(actual) + ")"); \
    }

#define ASSERT_LESS(expected, actual) \
    if ((expected) <= (actual)) { \
        throw std::runtime_error("Assertion failed: " #expected " <= " #actual " (" + \
                                 std::to_string(expected) + " <= " + std::to_string(actual) + ")"); \
    }

// Benchmark utilities
namespace benchmark {
    class Benchmark {
    public:
        Benchmark(const std::string& name) : name(name) {
            startTime = std::chrono::high_resolution_clock::now();
        }
        
        ~Benchmark() {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double, std::milli>(endTime - startTime).count();
            std::cout << "[BENCHMARK] " << name << ": " << std::fixed << std::setprecision(3) 
                      << duration << " ms" << std::endl;
        }
        
    private:
        std::string name;
        std::chrono::high_resolution_clock::time_point startTime;
    };
}

} // namespace test
} // namespace nlm
