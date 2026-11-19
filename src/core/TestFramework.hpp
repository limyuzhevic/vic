#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cctype>
#include <functional>
#include <chrono>
#include <memory>

namespace nlm {

class TestFramework {
public:
    enum class TestResult {
        PASSED,
        FAILED,
        SKIPPED
    };
    
    struct TestInfo {
        std::string name;
        std::string category;
        std::function<bool()> test_function;
        std::string description;
    };
    
    TestFramework() = default;
    
    // Add a test
    void addTest(const std::string& name, std::function<bool()> test_function,
                 const std::string& description = "", const std::string& category = "General") {
        TestInfo test;
        test.name = name;
        test.category = category;
        test.test_function = test_function;
        test.description = description;
        
        tests_.push_back(test);
        tests_by_category_[category].push_back(test);
    }
    
    // Run all tests
    void runAllTests() {
        std::cout << "=== NLM Test Suite ===" << std::endl;
        std::cout << std::endl;
        
        int total_tests = 0;
        int passed_tests = 0;
        int failed_tests = 0;
        int skipped_tests = 0;
        
        // Run tests by category
        for (const auto& category_pair : tests_by_category_) {
            const std::string& category = category_pair.first;
            const std::vector<TestInfo>& category_tests = category_pair.second;
            
            if (!category_tests.empty()) {
                std::cout << "=== " << category << " ===" << std::endl;
                
                for (const TestInfo& test : category_tests) {
                    total_tests++;
                    
                    std::cout << "Running: " << test.name << "...";
                    
                    try {
                        bool result = test.test_function();
                        if (result) {
                            std::cout << " PASSED" << std::endl;
                            passed_tests++;
                            test_results_[test.name] = TestResult::PASSED;
                        } else {
                            std::cout << " FAILED" << std::endl;
                            failed_tests++;
                            test_results_[test.name] = TestResult::FAILED;
                        }
                    } catch (const std::exception& e) {
                        std::cout << " ERROR: " << e.what() << " - FAILED" << std::endl;
                        failed_tests++;
                        test_results_[test.name] = TestResult::FAILED;
                    } catch (...) {
                        std::cout << " UNKNOWN ERROR - FAILED" << std::endl;
                        failed_tests++;
                        test_results_[test.name] = TestResult::FAILED;
                    }
                }
                
                std::cout << std::endl;
            }
        }
        
        // Print summary
        printTestSummary(total_tests, passed_tests, failed_tests, skipped_tests);
        
        // Print detailed results
        printDetailedResults();
    }
    
    // Run tests by category
    void runTestsByCategory(const std::string& category) {
        auto it = tests_by_category_.find(category);
        if (it == tests_by_category_.end()) {
            std::cout << "No tests found in category: " << category << std::endl;
            return;
        }
        
        std::cout << "=== Tests in Category: " << category << " ===" << std::endl;
        std::cout << std::endl;
        
        for (const TestInfo& test : it->second) {
            std::cout << "Running: " << test.name << "...";
            
            try {
                bool result = test.test_function();
                if (result) {
                    std::cout << " PASSED" << std::endl;
                } else {
                    std::cout << " FAILED" << std::endl;
                }
            } catch (const std::exception& e) {
                std::cout << " ERROR: " << e.what() << " - FAILED" << std::endl;
            } catch (...) {
                std::cout << " UNKNOWN ERROR - FAILED" << std::endl;
            }
        }
    }
    
    // Get test result
    TestResult getTestResult(const std::string& test_name) const {
        auto it = test_results_.find(test_name);
        if (it != test_results_.end()) {
            return it->second;
        }
        return TestResult::SKIPPED;
    }
    
    // Check if all tests passed
    bool allTestsPassed() const {
        for (const auto& result : test_results_) {
            if (result.second == TestResult::FAILED) {
                return false;
            }
        }
        return true;
    }
    
    // Check if any tests failed
    bool anyTestsFailed() const {
        for (const auto& result : test_results_) {
            if (result.second == TestResult::FAILED) {
                return true;
            }
        }
        return false;
    }
    
    // Get total number of tests
    size_t getTotalTestCount() const {
        return tests_.size();
    }
    
    // Get test names by category
    std::vector<std::string> getTestNamesByCategory(const std::string& category) const {
        std::vector<std::string> test_names;
        auto it = tests_by_category_.find(category);
        if (it != tests_by_category_.end()) {
            for (const TestInfo& test : it->second) {
                test_names.push_back(test.name);
            }
        }
        return test_names;
    }
    
    // Get all categories
    std::vector<std::string> getAllCategories() const {
        std::vector<std::string> categories;
        for (const auto& pair : tests_by_category_) {
            categories.push_back(pair.first);
        }
        return categories;
    }
    
private:
    std::vector<TestInfo> tests_;
    std::map<std::string, std::vector<TestInfo>> tests_by_category_;
    std::map<std::string, TestResult> test_results_;
    
    void printTestSummary(int total_tests, int passed_tests, int failed_tests, int skipped_tests) {
        std::cout << "=== Test Summary ===" << std::endl;
        std::cout << "Total tests: " << total_tests << std::endl;
        std::cout << "Passed: " << passed_tests << std::endl;
        std::cout << "Failed: " << failed_tests << std::endl;
        std::cout << "Skipped: " << skipped_tests << std::endl;
        
        if (total_tests > 0) {
            double pass_rate = (static_cast<double>(passed_tests) / total_tests) * 100.0;
            std::cout << "Pass rate: " << std::fixed << std::setprecision(2) << pass_rate << "%" << std::endl;
        }
        
        if (anyTestsFailed()) {
            std::cout << std::endl;
            std::cout << "!!! TEST SUITE FAILED !!!" << std::endl;
        } else {
            std::cout << std::endl;
            std::cout << "✓ ALL TESTS PASSED" << std::endl;
        }
        
        std::cout << "========================" << std::endl;
    }
    
    void printDetailedResults() {
        std::cout << "=== Detailed Test Results ===" << std::endl;
        std::cout << std::endl;
        
        for (const auto& result_pair : test_results_) {
            const std::string& test_name = result_pair.first;
            TestResult test_result = result_pair.second;
            
            std::string status;
            switch (test_result) {
                case TestResult::PASSED:
                    status = "PASSED";
                    break;
                case TestResult::FAILED:
                    status = "FAILED";
                    break;
                case TestResult::SKIPPED:
                    status = "SKIPPED";
                    break;
            }
            
            std::cout << test_name << ": " << status << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << "===========================" << std::endl;
    }
};

// Global test framework instance
TestFramework& getGlobalTestFramework() {
    static TestFramework test_framework;
    return test_framework;
}

// Convenience functions
void addTest(const std::string& name, std::function<bool()> test_function,
             const std::string& description = "", const std::string& category = "General") {
    getGlobalTestFramework().addTest(name, test_function, description, category);
}

void runAllTests() {
    getGlobalTestFramework().runAllTests();
}

void runTestsByCategory(const std::string& category) {
    getGlobalTestFramework().runTestsByCategory(category);
}

bool allTestsPassed() {
    return getGlobalTestFramework().allTestsPassed();
}

} // namespace nlm