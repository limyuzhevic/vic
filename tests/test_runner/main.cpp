#include "framework/test_base.hpp"
#include "CoverageReport.hpp"
#include <vector>
#include <string>
#include <algorithm>
#include <thread>
#include <future>
#include <map>

class TestSuite;

class TestRunner {
public:
    TestRunner() : totalTests(0), passedTests(0), failedTests(0), skippedTests(0) {
        coverage = &CoverageReporter::getInstance().getCoverage();
    }
    
    void registerTestSuite(TestSuite* suite) {
        testSuites.push_back(suite);
    }
    
    void runAllTests() {
        std::cout << "=" << std::string(60, '=') << std::endl;
        std::cout << "NLM TEST RUNNER" << std::endl;
        std::cout << "=" << std::string(60, '=') << std::endl;
        std::cout << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (auto suite : testSuites) {
            std::cout << "Running test suite: " << suite->getName() << std::endl;
            try {
                suite->runTests();
            } catch (const std::exception& e) {
                std::cerr << "Test suite " << suite->getName() << " failed with exception: " 
                          << e.what() << std::endl;
                failedTests++;
            } catch (...) {
                std::cerr << "Test suite " << suite->getName() << " failed with unknown exception" << std::endl;
                failedTests++;
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        std::cout << std::endl << "=" << std::string(60, '=') << std::endl;
        std::cout << "TEST RESULTS SUMMARY:" << std::endl;
        std::cout << "  Total Test Suites: " << testSuites.size() << std::endl;
        std::cout << "  Total Tests: " << totalTests << std::endl;
        std::cout << "  Passed: " << passedTests << std::endl;
        std::cout << "  Failed: " << failedTests << std::endl;
        std::cout << "  Skipped: " << skippedTests << std::endl;
        std::cout << "  Execution Time: " << duration.count() << " ms" << std::endl;
        std::cout << "=" << std::string(60, '=') << std::endl;
        
        MemoryTracker::reportLeaks();
        
        CoverageReporter::getInstance().generateReport();
    }
    
    void setCoverageStats(size_t totalFunctions, size_t coveredFunctions, size_t totalLines, size_t coveredLines) {
        coverage->setOverallStats(totalFunctions, coveredFunctions, totalLines, coveredLines);
    }
    
    void addTestResult(const std::string& testName, bool passed, bool failed = false, bool skipped = false) {
        totalTests++;
        if (skipped) {
            skippedTests++;
        } else if (failed) {
            failedTests++;
        } else {
            passedTests++;
        }
    }
    
    size_t getPassedTests() const { return passedTests; }
    size_t getFailedTests() const { return failedTests; }
    
private:
    std::vector<TestSuite*> testSuites;
    CoverageInfo* coverage;
    size_t totalTests;
    size_t passedTests;
    size_t failedTests;
    size_t skippedTests;
};

TestRunner& getTestRunner() {
    static TestRunner runner;
    return runner;
}

TestSuite::TestSuite() {}

TestSuite::~TestSuite() {}

void initializeTestRunner() {
    // Global test registration initialization
}

void finalizeTestRunner() {
    MemoryTracker::reportLeaks();
}