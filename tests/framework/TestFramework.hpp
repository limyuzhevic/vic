// NLM Testing Framework
// Framework Interface

#ifndef TEST_FRAMEWORK_HPP
#define TEST_FRAMEWORK_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <cassert>
#include <cmath>
#include <iomanip>
#include <sstream>

typedef long long Timestamp;

timestamp() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
}

enum class TestResult {
    PASSED,
    FAILED,
    SKIPPED,
    ERROR
};

enum class TestType {
    UNIT,
    INTEGRATION,
    PERFORMANCE,
    STRESS,
    COMPATIBILITY,
    REGRESSION
};

enum class MetricType {
    TIME,
    MEMORY,
    CPU,
    NETWORK,
    ACCURACY
};

struct PerformanceMetrics {
    std::string test_name;
    Timestamp duration;           // milliseconds
    uint64_t cpu_cycles;           // CPU cycles
    size_t memory_usage;          // bytes
    double gflops;               // GFLOPs
    double accuracy;             // percentage 0-100
    Timestamp timestamp;
    
    PerformanceMetrics() : duration(0), cpu_cycles(0), memory_usage(0), 
                          gflops(0.0), accuracy(0.0), timestamp(timestamp()) {}
};

struct TestCase {
    std::string name;
    std::string description;
    TestType type;
    TestResult result;
    std::string failure_message;
    std::vector<PerformanceMetrics> metrics;
    
    TestCase(const std::string& test_name, const std::string& test_desc, TestType test_type)
        : name(test_name), description(test_desc), type(test_type), 
          result(TestResult::SKIPPED), failure_message("") {}
};

class TestFramework {
private:
    std::vector<TestCase> registered_tests;
    std::map<std::string, std::vector<PerformanceMetrics>> test_results;
    size_t passed_tests = 0;
    size_t failed_tests = 0;
    size_t skipped_tests = 0;
    size_t total_tests = 0;
    
public:
    TestFramework() {
        registerTestFramework();
    }
    
    void registerTestFramework() {
        // Register core framework tests
        TestCase framework_test("Framework Initialization", "Test framework core functionality", TestType::UNIT);
        framework_test.result = TestResult::PASSED;
        registered_tests.push_back(framework_test);
        
        TestCase timer_test("Timer Accuracy", "Test timestamp and timing functionality", TestType::UNIT);
        timer_test.result = TestResult::PASSED;
        registered_tests.push_back(timer_test);
    }
    
    template<typename TestClass>
    void registerTest(const std::string& name, const std::string& description) {
        TestCase test(name, description, TestClass::getTestType());
        registered_tests.push_back(test);
    }
    
    template<typename TestClass>
    void executeTest(TestClass& test_instance) {
        total_tests++;
        
        try {
            test_instance.runTest();
            test_instance.getResult();
            
            // Find and update the test case
            for (auto& test : registered_tests) {
                if (test.name == test_instance.getName()) {
                    test.result = test_instance.getTestResult();
                    test.metrics = test_instance.getMetrics();
                    if (test.result == TestResult::PASSED) {
                        passed_tests++;
                    } else if (test.result == TestResult::FAILED) {
                        failed_tests++;
                    } else if (test.result == TestResult::SKIPPED) {
                        skipped_tests++;
                    }
                    break;
                }
            }
        } catch (const std::exception& e) {
            // Handle test exceptions
            for (auto& test : registered_tests) {
                if (test.name == test_instance.getName()) {
                    test.result = TestResult::ERROR;
                    test.failure_message = std::string("Exception: ") + e.what();
                    failed_tests++;
                    break;
                }
            }
        }
    }
    
    void reportResults() {
        std::cout << "\n=== NLM Test Framework Results ===" << std::endl;
        std::cout << "Total Tests: " << total_tests << std::endl;
        std::cout << "Passed: " << passed_tests << std::endl;
        std::cout << "Failed: " << failed_tests << std::endl;
        std::cout << "Skipped: " << skipped_tests << std::endl;
        
        if (failed_tests > 0) {
            std::cout << "\n=== FAILED TESTS ===" << std::endl;
            for (const auto& test : registered_tests) {
                if (test.result == TestResult::FAILED || test.result == TestResult::ERROR) {
                    std::cout << "FAILED: " << test.name << std::endl;
                    if (!test.failure_message.empty()) {
                        std::cout << "  Reason: " << test.failure_message << std::endl;
                    }
                }
            }
        }
        
        double success_rate = (passed_tests * 100.0) / total_tests;
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "Success Rate: " << success_rate << "%" << std::endl;
        
        // Generate detailed report
        generateReport();
    }
    
    void generateReport() {
        // Generate HTML, JSON, or text report
        std::cout << "Report generation not yet implemented" << std::endl;
    }
    
    void clearResults() {
        registered_tests.clear();
        test_results.clear();
        passed_tests = 0;
        failed_tests = 0;
        skipped_tests = 0;
        total_tests = 0;
    }
    
    size_t getPassedTests() const { return passed_tests; }
    size_t getFailedTests() const { return failed_tests; }
    size_t getSkippedTests() const { return skipped_tests; }
    size_t getTotalTests() const { return total_tests; }
};

#endif // TEST_FRAMEWORK_HPP