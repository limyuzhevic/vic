#ifndef NLM_TEST_BASE_HPP
#define NLM_TEST_BASE_HPP

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <string>
#include <vector>
#include <functional>
#include <chrono>

namespace nlm {

struct TestResult {
    std::string name;
    bool success;
    double duration;
};

class TestBase {
public:
    TestBase(const std::string& name) : testName(name) {
        NLM_LOG_INFO("Test suite initialized: " + name);
    }
    
    virtual ~TestBase() {
        NLM_LOG_INFO("Test suite cleanup: " + testName);
    }
    
    virtual void setup() {}
    virtual void teardown() {}
    virtual void runTests() = 0;
    
protected:
    std::string testName;
    std::vector<TestResult> testResults;
    Config testConfig;
    
    void setTestCategories(const std::vector<std::string>& categories) {
        testCategories = categories;
    }
    
    const std::vector<std::string>& getTestCategories() const {
        return testCategories;
    }
    
    void addTestResult(const TestResult& result) {
        testResults.push_back(result);
    }
    
    void runTestCase(const std::string& name, std::function<bool()> testFunc) {
        NLM_LOG_INFO("Running test: " + name);
        
        auto startTime = std::chrono::high_resolution_clock::now();
        bool result = testFunc();
        auto endTime = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double, std::milli>(endTime - startTime).count() / 1000.0;
        
        TestResult testResult;
        testResult.name = name;
        testResult.success = result;
        testResult.duration = duration;
        addTestResult(testResult);
        
        if (result) {
            NLM_LOG_INFO("  ✓ Test PASSED: " + name + " (" + std::to_string(duration) + "s)");
        } else {
            NLM_LOG_ERROR("  ✗ Test FAILED: " + name + " (" + std::to_string(duration) + "s)");
        }
    }
    
    bool containsTestCategory(const std::string& category) const {
        return std::find(testCategories.begin(), testCategories.end(), category) != testCategories.end();
    }
    
    void printTestReport() const {
        NLM_LOG_INFO("=== Test Report ===");
        
        size_t totalTests = testResults.size();
        size_t passedTests = 0;
        double totalDuration = 0.0;
        
        for (const auto& result : testResults) {
            if (result.success) {
                passedTests++;
            }
            totalDuration += result.duration;
        }
        
        NLM_LOG_INFO("Total Tests: " + std::to_string(totalTests));
        NLM_LOG_INFO("Passed: " + std::to_string(passedTests));
        NLM_LOG_INFO("Failed: " + std::to_string(totalTests - passedTests));
        NLM_LOG_INFO("Success Rate: " + std::to_string((passedTests * 100.0) / totalTests) + "%");
        NLM_LOG_INFO("Total Duration: " + std::to_string(totalDuration) + "s");
        NLM_LOG_INFO("Average Duration: " + std::to_string(totalDuration / totalTests) + "s");
        
        NLM_LOG_INFO("==================");
    }
    
private:
    std::vector<std::string> testCategories;
};

} // namespace nlm

#endif // NLM_TEST_BASE_HPP
