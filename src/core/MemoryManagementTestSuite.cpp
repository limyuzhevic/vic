#include "MemoryManagementTestSuite.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <thread>

namespace nlm {

MemoryManagementTestSuite& MemoryManagementTestSuite::getInstance() {
    static MemoryManagementTestSuite instance;
    return instance;
}

MemoryManagementTestSuite::MemoryManagementTestSuite()
    : parallelExecution_(false), verboseOutput_(true), failFast_(false) {}

void MemoryManagementTestSuite::addTest(std::unique_ptr<MemoryManagementTest> test) {
    if (test) {
        tests_.push_back(std::move(test));
    }
}

bool MemoryManagementTestSuite::runAllTests() {
    results_.clear();
    
    for (const auto& test : tests_) {
        runTest(*test);
    }
    
    return getFailedCount() == 0;
}

bool MemoryManagementTestSuite::runSelectedTests(const std::vector<std::string>& testNames) {
    results_.clear();
    
    for (const auto& test : tests_) {
        bool found = false;
        for (const auto& name : testNames) {
            if (test->getName() == name) {
                found = true;
                break;
            }
        }
        
        if (found) {
            runTest(*test);
            if (failFast_ && !test->getResults().empty() && !test->getResults().back().passed) {
                break;
            }
        }
    }
    
    return getFailedCount() == 0;
}

bool MemoryManagementTestSuite::runTestsWithFilter(const std::function<bool(const MemoryManagementTest&)>& filter) {
    results_.clear();
    
    for (const auto& test : tests_) {
        if (filter(*test)) {
            runTest(*test);
            if (failFast_ && !test->getResults().empty() && !test->getResults().back().passed) {
                break;
            }
        }
    }
    
    return getFailedCount() == 0;
}

void MemoryManagementTestSuite::runTest(MemoryManagementTest& test) {
    auto startTime = std::chrono::system_clock::now();
    size_t startMemory = getCurrentMemoryUsage();
    
    TestResult result = test.run();
    
    auto endTime = std::chrono::system_clock::now();
    size_t endMemory = getCurrentMemoryUsage();
    
    double executionTime = std::chrono::duration<double>(endTime - startTime).count();
    size_t memoryUsed = endMemory - startMemory;
    
    result.setPerformance(executionTime, memoryUsed);
    
    if (verboseOutput_) {
        logTestResult(result);
    }
    
    results_.push_back(std::move(result));
}

void MemoryManagementTestSuite::logTestResult(const TestResult& result) const {
    std::cout << "Test: " << result.getName() << std::endl;
    std::cout << "  Status: " << (result.passed ? "PASSED" : "FAILED") << std::endl;
    
    if (!result.errorMessage.empty()) {
        std::cout << "  Error: " << result.errorMessage << std::endl;
    }
    
    std::cout << "  Time: " << std::fixed << std::setprecision(3) << result.executionTime << "s" << std::endl;
    std::cout << "  Memory: " << result.memoryUsed << " bytes" << std::endl;
    std::cout << std::endl;
}

void MemoryManagementTestSuite::generateSummaryReport() const {
    std::cout << "=== Memory Management Test Summary ===" << std::endl;
    std::cout << "Total tests: " << results_.size() << std::endl;
    std::cout << "Passed: " << getPassedCount() << std::endl;
    std::cout << "Failed: " << getFailedCount() << std::endl;
    std::cout << "Success rate: " << std::fixed << std::setprecision(1) << getSuccessRate() << "%" << std::endl;
    std::cout << std::endl;
    
    for (const auto& result : results_) {
        std::cout << result.getSummary() << std::endl;
    }
}

std::string MemoryManagementTestSuite::getReport() const {
    std::ostringstream oss;
    oss << generateMemoryManagementReport(results_);
    return oss.str();
}

bool MemoryManagementTestSuite::exportReport(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << getReport() << std::endl;
        file.close();
        return true;
    } catch (...) {
        return false;
    }
}

MemoryManagementTestSuite& MemoryManagementTestSuite::getInstance() {
    static MemoryManagementTestSuite instance;
    return instance;
}

std::vector<std::string> MemoryManagementTestFactory::getAvailableTestTypes() {
    std::vector<std::string> types;
    for (const auto& pair : testRegistry_) {
        types.push_back(pair.first);
    }
    return types;
}

std::unique_ptr<MemoryManagementTest> MemoryManagementTestFactory::createTest(const std::string& testType) {
    auto it = testRegistry_.find(testType);
    if (it != testRegistry_.end()) {
        return it->second();
    }
    return nullptr;
}

} // namespace nlm
