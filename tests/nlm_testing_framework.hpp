// Modern Testing Framework for NLM
// Enhanced test infrastructure based on doctest

#ifndef NLM_TESTING_FRAMEWORK_H
#define NLM_TESTING_FRAMEWORK_H

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <cassert>
#include <iomanip>
#include <sstream>
#include <functional>

namespace nlm {
    // Forward declarations for types that will be tested
    class Neuron;
    class Synapse;
    class Brain;
    class Config;
    class SimulationClock;
    class RandomGenerator;
    class STDP;
}

// Custom test assertion framework
class TestAssert {
public:
    static bool equal(const std::string& message, double actual, double expected, double epsilon = 1e-9) {
        if (std::abs(actual - expected) > epsilon) {
            std::cerr << "FAIL: " << message << " - Expected: " << expected << ", Got: " << actual << std::endl;
            return false;
        }
        std::cout << "PASS: " << message << std::endl;
        return true;
    }
    
    static bool equal(const std::string& message, int actual, int expected) {
        if (actual != expected) {
            std::cerr << "FAIL: " << message << " - Expected: " << expected << ", Got: " << actual << std::endl;
            return false;
        }
        std::cout << "PASS: " << message << std::endl;
        return true;
    }
    
    static bool greater(const std::string& message, double actual, double expected) {
        if (actual <= expected) {
            std::cerr << "FAIL: " << message << " - Expected: > " << expected << ", Got: " << actual << std::endl;
            return false;
        }
        std::cout << "PASS: " << message << std::endl;
        return true;
    }
    
    static bool less(const std::string& message, double actual, double expected) {
        if (actual >= expected) {
            std::cerr << "FAIL: " << message << " - Expected: < " << expected << ", Got: " << actual << std::endl;
            return false;
        }
        std::cout << "PASS: " << message << std::endl;
        return true;
    }
    
    static bool notNull(const std::string& message, void* ptr) {
        if (ptr == nullptr) {
            std::cerr << "FAIL: " << message << " - Expected non-null pointer" << std::endl;
            return false;
        }
        std::cout << "PASS: " << message << std::endl;
        return true;
    }
    
    static bool empty(const std::string& message, size_t size) {
        if (size != 0) {
            std::cerr << "FAIL: " << message << " - Expected empty, got size " << size << std::endl;
            return false;
        }
        std::cout << "PASS: " << message << std::endl;
        return true;
    }
    
    static bool notEmpty(const std::string& message, size_t size) {
        if (size == 0) {
            std::cerr << "FAIL: " << message << " - Expected non-empty" << std::endl;
            return false;
        }
        std::cout << "PASS: " << message << std::endl;
        return true;
    }
};

// Performance measurement utilities
class TestPerformance {
public:
    template<typename Func>
    static double measureTime(Func func, int iterations = 1000) {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
            func();
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;
        return duration.count() / iterations;
    }
    
    static void reportBenchmark(const std::string& name, double timeNs, int iterations) {
        std::cout << "BENCH: " << name << " - " 
                  << std::fixed << std::setprecision(3) 
                  << timeNs * 1e9 << " ns/iteration (" << iterations << " iterations)" << std::endl;
    }
};

// Test categories for organization
enum class TestCategory {
    UNIT,
    INTEGRATION,
    PERFORMANCE,
    CONFIGURATION,
    ERROR_HANDLING,
    CLI,
    SYSTEM_INTERACTION
};

// Test metadata for organization
struct TestMetadata {
    std::string name;
    TestCategory category;
    std::string description;
    bool enabled;
    
    TestMetadata(const std::string& n, TestCategory c, const std::string& d = "")
        : name(n), category(c), description(d), enabled(true) {}
};

// Base test class
class TestBase {
protected:
    std::vector<TestMetadata> testSuites;
    
public:
    virtual ~TestBase() = default;
    
    void addTestSuite(const std::string& name, TestCategory category, const std::string& description = "") {
        testSuites.emplace_back(name, category, description);
    }
    
    virtual void runAll() = 0;
    
    virtual void runCategory(TestCategory category) = 0;
    
    void printTestSummary() {
        std::cout << "\n=== Test Summary ===" << std::endl;
        int totalSuites = testSuites.size();
        std::cout << "Total test suites: " << totalSuites << std::endl;
        
        // Group by category
        int unitCount = 0, integrationCount = 0, performanceCount = 0;
        int configCount = 0, errorCount = 0, cliCount = 0, systemCount = 0;
        
        for (const auto& suite : testSuites) {
            switch (suite.category) {
                case TestCategory::UNIT: unitCount++; break;
                case TestCategory::INTEGRATION: integrationCount++; break;
                case TestCategory::PERFORMANCE: performanceCount++; break;
                case TestCategory::CONFIGURATION: configCount++; break;
                case TestCategory::ERROR_HANDLING: errorCount++; break;
                case TestCategory::CLI: cliCount++; break;
                case TestCategory::SYSTEM_INTERACTION: systemCount++; break;
            }
        }
        
        std::cout << "UNIT tests: " << unitCount << std::endl;
        std::cout << "INTEGRATION tests: " << integrationCount << std::endl;
        std::cout << "PERFORMANCE tests: " << performanceCount << std::endl;
        std::cout << "CONFIGURATION tests: " << configCount << std::endl;
        std::cout << "ERROR HANDLING tests: " << errorCount << std::endl;
        std::cout << "CLI tests: " << cliCount << std::endl;
        std::cout << "SYSTEM INTERACTION tests: " << systemCount << std::endl;
    }
};

// Test macros for better syntax
#define TEST_CASE(name, category, description) \
    namespace test_##name { \
        bool test_##name(); \
    } \
    struct Register##name { \
        Register##name() { \
            auto suite = TestRegistry::getInstance().registerTest(#name, category, description); \
            suite->addTest([]() { return test_##name::test_##name(); }, #name); \
        } \
    }; \
    Register##name registerInstance##name; \
    bool test_##name::test_##name()

// Global test registry
class TestRegistry {
private:
    static TestRegistry* instance;
    std::vector<std::function<bool()>> tests;
    std::vector<TestMetadata> testSuites;
    
    TestRegistry() {}
    
public:
    static TestRegistry& getInstance() {
        if (!instance) {
            instance = new TestRegistry();
        }
        return *instance;
    }
    
    TestMetadata* registerTest(const std::string& name, TestCategory category, const std::string& description = "") {
        TestMetadata metadata(name, category, description);
        testSuites.push_back(metadata);
        return &testSuites.back();
    }
    
    void addTest(std::function<bool()> testFunc, const std::string& suiteName) {
        tests.push_back(testFunc);
    }
    
    void runAll() {
        std::cout << "=== Running NLM Test Suite ===" << std::endl;
        
        int passed = 0;
        int failed = 0;
        
        for (auto& test : tests) {
            std::cout << "\nRunning test: " << "[details will be added]" << std::endl;
            try {
                if (test()) {
                    passed++;
                    std::cout << "Test PASSED" << std::endl;
                } else {
                    failed++;
                    std::cout << "Test FAILED" << std::endl;
                }
            } catch (const std::exception& e) {
                failed++;
                std::cout << "Test FAILED with exception: " << e.what() << std::endl;
            }
        }
        
        std::cout << "\n=== Test Results ===" << std::endl;
        std::cout << "Passed: " << passed << std::endl;
        std::cout << "Failed: " << failed << std::endl;
        std::cout << "Total: " << (passed + failed) << std::endl;
        
        if (failed > 0) {
            std::cerr << "ERROR: " << failed << " tests failed" << std::endl;
            return;
        }
    }
    
    void runCategory(TestCategory category) {
        std::cout << "\n=== Running " << static_cast<int>(category) << " tests ===" << std::endl;
        
        int passed = 0;
        int failed = 0;
        
        for (auto& test : tests) {
            // In a real implementation, we would check if the test belongs to the category
            try {
                if (test()) {
                    passed++;
                } else {
                    failed++;
                }
            } catch (const std::exception& e) {
                failed++;
            }
        }
        
        std::cout << "Category " << static_cast<int>(category) << " results:" << std::endl;
        std::cout << "Passed: " << passed << std::endl;
        std::cout << "Failed: " << failed << std::endl;
    }
};

TestRegistry* TestRegistry::instance = nullptr;

#endif // NLM_TESTING_FRAMEWORK_H