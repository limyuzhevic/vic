# Test Framework
// Provides a base Test class and TestRunner for managing and executing tests

#ifndef NLM_TEST_FRAMEWORK
#define NLM_TEST_FRAMEWORK

#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <memory>
#include <functional>

class TestBase {
public:
    virtual ~TestBase() = default;
    virtual std::string getName() const = 0;
    virtual void run() = 0;
    virtual bool shouldRun() const { return true; }
};

class TestRunner {
private:
    struct TestInfo {
        std::string name;
        std::unique_ptr<TestBase> test;
    };
    
    std::vector<TestInfo> tests;
    std::vector<std::string> failedTests;
    int totalRun = 0;
    int totalPassed = 0;
    int totalFailed = 0;
public:
    TestRunner() = default;
    
    void registerTest(std::unique_ptr<TestBase> test) {
        tests.push_back({
            test->getName(),
            std::move(test)
        });
    }
    
    template<typename T>
    void registerTest() {
        registerTest(std::make_unique<T>());
    }
    
    void runAll() {
        std::cout << "Running all registered tests..." << std::endl;
        totalRun = 0;
        totalPassed = 0;
        totalFailed = 0;
        
        for (auto& testInfo : tests) {
            if (!testInfo.test->shouldRun()) {
                std::cout << "  Skipping " << testInfo.name << std::endl;
                continue;
            }
            
            std::cout << "Running " << testInfo.name << "..." << std::endl;
            totalRun++;
            
            try {
                testInfo.test->run();
                std::cout << "  PASSED" << std::endl;
                totalPassed++;
            } catch (const std::exception& e) {
                std::cout << "  FAILED: " << e.what() << std::endl;
                totalFailed++;
                failedTests.push_back(testInfo.name);
            }
        }
        
        std::cout << std::endl;
        std::cout << "=== Test Results ===" << std::endl;
        std::cout << "Total: " << tests.size() << std::endl;
        std::cout << "Run: " << totalRun << std::endl;
        std::cout << "Passed: " << totalPassed << std::endl;
        std::cout << "Failed: " << totalFailed << std::endl;
        
        if (totalFailed > 0) {
            std::cout << "Failed tests:" << std::endl;
            for (const auto& failed : failedTests) {
                std::cout << "  - " << failed << std::endl;
            }
            throw std::runtime_error("Some tests failed");
        }
    }
    
    void runSingleTest(const std::string& testName) {
        for (auto& testInfo : tests) {
            if (testInfo.name == testName) {
                std::cout << "Running single test: " << testName << std::endl;
                try {
                    testInfo.test->run();
                    std::cout << "  PASSED" << std::endl;
                    return;
                } catch (const std::exception& e) {
                    std::cout << "  FAILED: " << e.what() << std::endl;
                    throw;
                }
            }
        }
        throw std::runtime_error("Test not found: " + testName);
    }
    
    bool hasTest(const std::string& testName) const {
        for (const auto& testInfo : tests) {
            if (testInfo.name == testName) {
                return true;
            }
        }
        return false;
    }
    
    int getTestCount() const { return static_cast<int>(tests.size()); }
    
    const std::vector<std::string>& getFailedTests() const { return failedTests; }
};

#endif // NLM_TEST_FRAMEWORK