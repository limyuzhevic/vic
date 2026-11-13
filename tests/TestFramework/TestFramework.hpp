#ifndef NLM_TEST_FRAMEWORK_HPP
#define NLM_TEST_FRAMEWORK_HPP

#include <string>
#include <iostream>
#include <vector>
#include <functional>
#include <stdexcept>
#include <memory>
#include "core/Exceptions/NLMExceptions.hpp"

namespace nlm {
namespace test {

// Test result structure
struct TestResult {
    std::string name;
    bool passed;
    std::string message;
    std::string category;
    
    TestResult(const std::string& testName, bool isPassed, 
               const std::string& testMessage = "",
               const std::string& testCategory = "General")
        : name(testName), passed(isPassed), message(testMessage), category(testCategory) {}
};

// Test category base class
class TestCategory {
public:
    virtual ~TestCategory() = default;
    virtual void runAll() = 0;
    virtual std::string getName() const = 0;
    virtual std::vector<TestResult> getResults() const = 0;
protected:
    std::vector<TestResult> results;
};

// Simple test framework implementation
class TestFramework {
public:
    TestFramework() {
        std::cout << "=== NLM Test Framework ===" << std::endl;
    }
    
    void addCategory(std::shared_ptr<TestCategory> category) {
        categories.push_back(category);
    }
    
    void runAllTests() {
        totalTests = 0;
        passedTests = 0;
        failedTests = 0;
        
        for (const auto& category : categories) {
            std::cout << std::endl << "Running " << category->getName() << " tests..." << std::endl;
            try {
                category->runAll();
                auto results = category->getResults();
                
                for (const auto& result : results) {
                    totalTests++;
                    if (result.passed) {
                        passedTests++;
                        std::cout << "  ✓ " << result.name << " (" << result.category << ")" << std::endl;
                    } else {
                        failedTests++;
                        std::cout << "  ✗ " << result.name << " (" << result.category << ") - " 
                                 << result.message << std::endl;
                    }
                }
            } catch (const std::exception& e) {
                std::cout << "  ✗ " << category->getName() << " tests crashed: " << e.what() << std::endl;
                failedTests++;
            }
        }
        
        printSummary();
    }
    
    void printSummary() const {
        std::cout << std::endl << "=== Test Summary ===" << std::endl;
        std::cout << "Total tests: " << totalTests << std::endl;
        std::cout << "Passed: " << passedTests << std::endl;
        std::cout << "Failed: " << failedTests << std::endl;
        
        if (failedTests == 0) {
            std::cout << std::endl << "✓ All tests passed!" << std::endl;
            return 0;
        } else {
            std::cout << std::endl << "✗ " << failedTests << " tests failed." << std::endl;
            return 1;
        }
    }
    
    int getFailureCount() const {
        return failedTests;
    }
    
private:
    std::vector<std::shared_ptr<TestCategory>> categories;
    int totalTests = 0;
    int passedTests = 0;
    int failedTests = 0;
};

// Convenience macros for test assertions
#define TEST_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            throw nlm::test::TestAssertionException(std::string("Assertion failed: ") + message + " at " + __FILE__ + ":" + std::to_string(__LINE__)); \
        } \
    } while(0)

#define TEST_THROW(statement, exceptionType) \
    do { \
        bool caught = false; \
        try { statement; } catch (const exceptionType&) { caught = true; } catch (...) {} \
        if (!caught) { \
            throw nlm::test::TestAssertionException(std::string("Expected ") + #exceptionType + " to be thrown"); \
        } \
    } while(0)

// Test assertion exception
class TestAssertionException : public nlm::NLMException {
public:
    TestAssertionException(const std::string& message) 
        : nlm::NLMException(std::string("Test Assertion: ") + message, "Test framework") {}
    
    virtual std::string getErrorCode() const override { return "NLM_TEST_ASSERT_"; }
};

// Basic test category implementation
class BasicTestCategory : public TestCategory {
public:
    BasicTestCategory(const std::string& name) : categoryName(name) {}
    
    void addTest(const std::string& testName, 
                std::function<void()> testFunction,
                const std::string& testCategory = "General") {
        testFunctions.push_back(std::make_pair(testName, testFunction));
        testCategories.push_back(testCategory);
    }
    
    void runAll() override {
        for (size_t i = 0; i < testFunctions.size(); ++i) {
            try {
                testFunctions[i].second();
                results.push_back(TestResult(testFunctions[i].first, true, "", testCategories[i]));
            } catch (const std::exception& e) {
                results.push_back(TestResult(testFunctions[i].first, false, e.what(), testCategories[i]));
            }
        }
    }
    
    std::string getName() const override {
        return categoryName;
    }
    
    std::vector<TestResult> getResults() const override {
        return results;
    }
    
private:
    std::string categoryName;
    std::vector<std::pair<std::string, std::function<void()>>> testFunctions;
    std::vector<std::string> testCategories;
};

} // namespace test
} // namespace nlm

#endif // NLM_TEST_FRAMEWORK_HPP
