// Test framework utilities header
// Common utilities for the NLM testing framework

#ifndef TEST_FRAMEWORK_UTILITIES_H
#define TEST_FRAMEWORK_UTILITIES_H

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <cassert>
#include <iomanip>
#include <sstream>

namespace nlm {
namespace test {

// Test result structure
struct TestResult {
    std::string name;
    std::string status; // "PASSED", "FAILED", "SKIPPED"
    double duration_ms;
    std::string details;
    std::unordered_map<std::string, double> metrics;
    std::chrono::time_point<std::chrono::high_resolution_clock> timestamp;
    
    TestResult(const std::string& test_name) : name(test_name), status("UNKNOWN"), 
        duration_ms(0.0), timestamp(std::chrono::high_resolution_clock::now()) {}
};

// Test framework base class
class TestFramework {
protected:
    std::vector<TestResult> test_results;
    std::string output_format;
    
public:
    TestFramework(const std::string& format = "console") : output_format(format) {}
    
    virtual ~TestFramework() = default;
    
    void addTestResult(const TestResult& result) {
        test_results.push_back(result);
    }
    
    virtual void runAllTests() = 0;
    
    void generateReport() {
        if (output_format == "console") {
            generateConsoleReport();
        } else if (output_format == "json") {
            generateJSONReport();
        } else if (output_format == "xml") {
            generateXMLReport();
        }
    }
    
protected:
    virtual void generateConsoleReport() {
        std::cout << "=== Test Framework Report ===" << std::endl;
        int passed = 0, failed = 0, skipped = 0;
        
        for (const auto& result : test_results) {
            std::cout << "Test: " << result.name << std::endl;
            std::cout << "  Status: " << result.status << std::endl;
            std::cout << "  Duration: " << result.duration_ms << "ms" << std::endl;
            if (!result.details.empty()) {
                std::cout << "  Details: " << result.details << std::endl;
            }
            if (!result.metrics.empty()) {
                std::cout << "  Metrics:" << std::endl;
                for (const auto& metric : result.metrics) {
                    std::cout << "    " << metric.first << ": " << metric.second << std::endl;
                }
            }
            std::cout << std::endl;
            
            if (result.status == "PASSED") passed++;
            else if (result.status == "FAILED") failed++;
            else if (result.status == "SKIPPED") skipped++;
        }
        
        std::cout << "=== Summary ===" << std::endl;
        std::cout << "Total tests: " << test_results.size() << std::endl;
        std::cout << "Passed: " << passed << std::endl;
        std::cout << "Failed: " << failed << std::endl;
        std::cout << "Skipped: " << skipped << std::endl;
    }
    
    virtual void generateJSONReport() {
        std::cout << "{" << std::endl;
        std::cout << "  \"test_results\": [" << std::endl;
        
        for (size_t i = 0; i < test_results.size(); ++i) {
            const auto& result = test_results[i];
            std::cout << "    {" << std::endl;
            std::cout << "      \"name\": \"" << result.name << "\"," << std::endl;
            std::cout << "      \"status\": \"" << result.status << "\"," << std::endl;
            std::cout << "      \"duration_ms\": " << result.duration_ms << "," << std::endl;
            std::cout << "      \"details\": \"" << result.details << "\"" << std::endl;
            
            if (!result.metrics.empty()) {
                std::cout << "      \"metrics\": {" << std::endl;
                bool first_metric = true;
                for (const auto& metric : result.metrics) {
                    if (!first_metric) std::cout << "," << std::endl;
                    std::cout << "        \"" << metric.first << "\": " << metric.second;
                    first_metric = false;
                }
                std::cout << std::endl << "      }" << std::endl;
            }
            
            if (i < test_results.size() - 1) std::cout << "    }," << std::endl;
            else std::cout << "    }" << std::endl;
        }
        
        std::cout << "  ]" << std::endl;
        std::cout << "}" << std::endl;
    }
    
    virtual void generateXMLReport() {
        std::cout << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
        std::cout << "<test_results>" << std::endl;
        
        for (const auto& result : test_results) {
            std::cout << "  <test>" << std::endl;
            std::cout << "    <name>" << result.name << "</name>" << std::endl;
            std::cout << "    <status>" << result.status << "</status>" << std::endl;
            std::cout << "    <duration_ms>" << result.duration_ms << "</duration_ms>" << std::endl;
            if (!result.details.empty()) {
                std::cout << "    <details>" << result.details << "</details>" << std::endl;
            }
            if (!result.metrics.empty()) {
                std::cout << "    <metrics>" << std::endl;
                for (const auto& metric : result.metrics) {
                    std::cout << "      <metric name=\"" << metric.first << "\">" 
                             << metric.second << "</metric>" << std::endl;
                }
                std::cout << "    </metrics>" << std::endl;
            }
            std::cout << "  </test>" << std::endl;
        }
        
        std::cout << "</test_results>" << std::endl;
    }
    
    size_t getTestCount() const { return test_results.size(); }
    int getPassedCount() const {
        return std::count_if(test_results.begin(), test_results.end(),
                           [](const TestResult& r) { return r.status == "PASSED"; });
    }
    int getFailedCount() const {
        return std::count_if(test_results.begin(), test_results.end(),
                           [](const TestResult& r) { return r.status == "FAILED"; });
    }
    int getSkippedCount() const {
        return std::count_if(test_results.begin(), test_results.end(),
                           [](const TestResult& r) { return r.status == "SKIPPED"; });
    }
};

// Automated test suite runner
class AutomatedTestSuite : public TestFramework {
private:
    std::vector<std::function<void()>> test_cases;
    
public:
    AutomatedTestSuite(const std::string& name, const std::string& output_format = "console")
        : TestFramework(output_format) {
        suite_name = name;
    }
    
    void addTestCase(const std::string& test_name, std::function<void()> test_func) {
        test_cases.push_back([this, test_name, test_func]() {
            auto start_time = std::chrono::high_resolution_clock::now();
            
            TestResult result(test_name);
            
            try {
                test_func();
                result.status = "PASSED";
                result.details = "Test completed successfully";
            } catch (const std::exception& e) {
                result.status = "FAILED";
                result.details = std::string("Exception: ") + e.what();
            } catch (...) {
                result.status = "FAILED";
                result.details = "Unknown exception occurred";
            }
            
            auto end_time = std::chrono::high_resolution_clock::now();
            result.duration_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
            
            addTestResult(result);
        });
    }
    
    void runAllTests() override {
        std::cout << "=== Running Test Suite: " << suite_name << " ===" << std::endl;
        
        for (const auto& test_case : test_cases) {
            test_case();
        }
        
        generateReport();
    }
    
private:
    std::string suite_name;
};

// Performance test runner
class PerformanceTestRunner : public TestFramework {
public:
    struct PerformanceTestConfig {
        std::string test_name;
        std::string test_category;
        std::function<bool()> test_function;
        double timeout_ms;
    };
    
    void addPerformanceTest(const std::string& name, const std::string& category,
                           std::function<bool()> test_func, double timeout = 60000.0) {
        performance_tests.push_back({name, category, test_func, timeout});
    }
    
    void runAllTests() override {
        std::cout << "=== Running Performance Tests ===" << std::endl;
        
        for (const auto& test : performance_tests) {
            TestResult result(test.test_name);
            
            auto start_time = std::chrono::high_resolution_clock::now();
            
            try {
                // Set timeout
                auto timeout_time = start_time + std::chrono::milliseconds(static_cast<long long>(test.timeout_ms));
                
                // Run test with timeout
                bool test_passed = test.test_function();
                
                auto end_time = std::chrono::high_resolution_clock::now();
                result.duration_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
                
                if (test_passed) {
                    result.status = "PASSED";
                    result.details = "Performance test completed successfully";
                } else {
                    result.status = "FAILED";
                    result.details = "Performance test failed to meet criteria";
                }
            } catch (const std::exception& e) {
                auto end_time = std::chrono::high_resolution_clock::now();
                result.duration_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
                result.status = "FAILED";
                result.details = std::string("Exception: ") + e.what();
            }
            
            addTestResult(result);
        }
        
        generateReport();
    }
    
private:
    std::vector<PerformanceTestConfig> performance_tests;
};

// Regression detection utilities
namespace regression {

struct RegressionResult {
    std::string test_name;
    std::string regression_type; // "PERFORMANCE", "MEMORY", "THROUGHPUT"
    double baseline_value;
    double current_value;
    double regression_percentage;
    bool has_regression;
    std::string details;
};

class RegressionDetector {
private:
    std::vector<RegressionResult> regression_results;
    double performance_threshold;
    double memory_threshold;
    double throughput_threshold;
    
public:
    RegressionDetector(double perf_threshold = 0.10, 
                      double mem_threshold = 0.15, 
                      double throughput_threshold = 0.10)
        : performance_threshold(perf_threshold),
          memory_threshold(mem_threshold),
          throughput_threshold(throughput_threshold) {}
    
    void addRegression(const RegressionResult& result) {
        regression_results.push_back(result);
    }
    
    bool hasPerformanceRegression(const std::string& test_name) const {
        auto it = std::find_if(regression_results.begin(), regression_results.end(),
                              [&test_name](const RegressionResult& r) {
                                  return r.test_name == test_name && r.regression_type == "PERFORMANCE";
                              });
        return (it != regression_results.end()) && it->has_regression;
    }
    
    bool hasMemoryRegression(const std::string& test_name) const {
        auto it = std::find_if(regression_results.begin(), regression_results.end(),
                              [&test_name](const RegressionResult& r) {
                                  return r.test_name == test_name && r.regression_type == "MEMORY";
                              });
        return (it != regression_results.end()) && it->has_regression;
    }
    
    bool hasThroughputRegression(const std::string& test_name) const {
        auto it = std::find_if(regression_results.begin(), regression_results.end(),
                              [&test_name](const RegressionResult& r) {
                                  return r.test_name == test_name && r.regression_type == "THROUGHPUT";
                              });
        return (it != regression_results.end()) && it->has_regression;
    }
    
    std::vector<RegressionResult> getAllRegressions() const {
        std::vector<RegressionResult> regressions;
        for (const auto& result : regression_results) {
            if (result.has_regression) {
                regressions.push_back(result);
            }
        }
        return regressions;
    }
    
    void printRegressionReport() const {
        std::cout << "=== Regression Detection Report ===" << std::endl;
        
        auto regressions = getAllRegressions();
        if (regressions.empty()) {
            std::cout << "No regressions detected." << std::endl;
            return;
        }
        
        std::cout << "Regressions detected:" << std::endl;
        for (const auto& result : regressions) {
            std::cout << "  Test: " << result.test_name << std::endl;
            std::cout << "    Type: " << result.regression_type << std::endl;
            std::cout << "    Baseline: " << result.baseline_value << std::endl;
            std::cout << "    Current: " << result.current_value << std::endl;
            std::cout << "    Regression: " << (result.regression_percentage * 100) << "%" << std::endl;
            std::cout << "    Details: " << result.details << std::endl;
            std::cout << std::endl;
        }
    }
    
    void clear() {
        regression_results.clear();
}

// Test utility functions
namespace utils {
    
    // Execute a test with timing and error handling
    template<typename TestFunc>
    TestResult executeTest(const std::string& test_name, TestFunc test_func) {
        TestResult result(test_name);
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        try {
            test_func();
            result.status = "PASSED";
            result.details = "Test completed successfully";
        } catch (const std::exception& e) {
            result.status = "FAILED";
            result.details = std::string("Exception: ") + e.what();
        } catch (...) {
            result.status = "FAILED";
            result.details = "Unknown exception occurred";
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        result.duration_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
        
        return result;
    }
    
    // Compare two performance results
    bool comparePerformance(const std::vector<double>& baseline,
                           const std::vector<double>& current,
                           double threshold = 0.10) {
        if (baseline.size() != current.size() || baseline.empty()) {
            return false;
        }
        
        for (size_t i = 0; i < baseline.size(); ++i) {
            if (baseline[i] == 0.0) continue; // Skip zero division
            
            double regression = (current[i] - baseline[i]) / baseline[i];
            if (regression > threshold) {
                return false;
            }
        }
        
        return true;
    }
    
    // Format double with specified precision
    std::string formatDouble(double value, int precision = 2) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(precision) << value;
        return ss.str();
    }
    
    // Format time duration
    std::string formatDuration(double milliseconds) {
        if (milliseconds < 1000.0) {
            return formatDouble(milliseconds, 0) + "ms";
        } else {
            double seconds = milliseconds / 1000.0;
            if (seconds < 60.0) {
                return formatDouble(seconds, 2) + "s";
            } else {
                double minutes = seconds / 60.0;
                return formatDouble(minutes, 1) + "m";
            }
        }
    }
    
    // Generate unique test identifier
    std::string generateTestId(const std::string& prefix = "test") {
        static int counter = 0;
        return prefix + "_" + std::to_string(++counter);
    }
    
} // namespace utils

} // namespace test
} // namespace nlm

#endif // TEST_FRAMEWORK_UTILITIES_H

