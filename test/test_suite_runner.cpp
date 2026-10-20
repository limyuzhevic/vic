// Test suite runner implementation
#include "test_suite_runner.hpp"

namespace nlm {
namespace integration_test {

void TestSuiteRunner::runAllTests() {
    std::vector<TestMetrics> results;
    
    std::cout << "=== NLM Integration Test Suite ===" << std::endl;
    std::cout << std::endl;
    
    for (size_t i = 0; i < tests_.size(); ++i) {
        auto& test = tests_[i];
        std::cout << "Running test " << (i + 1) << "/" << tests_.size() << ": " << test->getFullTestName() << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        TestMetrics metrics = test->run();
        auto endTime = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        metrics.executionTimeMs = duration;
        
        results.push_back(metrics);
        
        if (metrics.passed) {
            std::cout << "  PASSED (" << duration << " ms)" << std::endl;
        } else {
            std::cout << "  FAILED (" << duration << " ms)" << std::endl;
            if (!metrics.errorMessage.empty()) {
                std::cout << "    Error: " << metrics.errorMessage << std::endl;
            }
        }
        std::cout << std::endl;
    }
    
    printSummary(results);
}

void TestSuiteRunner::printSummary(const std::vector<TestMetrics>& results) {
    int totalTests = results.size();
    int passedTests = 0;
    double totalTime = 0.0;
    
    std::cout << "=== Test Summary ===" << std::endl;
    for (const auto& result : results) {
        if (result.passed) {
            passedTests++;
        }
        totalTime += result.executionTimeMs;
    }
    
    std::cout << "Total tests: " << totalTests << std::endl;
    std::cout << "Passed: " << passedTests << std::endl;
    std::cout << "Failed: " << (totalTests - passedTests) << std::endl;
    std::cout << "Total time: " << totalTime << " ms" << std::endl;
    std::cout << "Success rate: " << (totalTests > 0 ? (passedTests * 100.0 / totalTests) : 0.0) << "%" << std::endl;
}

} // namespace integration_test
} // namespace nlm