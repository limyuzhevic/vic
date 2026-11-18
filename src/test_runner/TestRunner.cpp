#include "TestRunner.hpp"
#include "src/core/Logger/Logger.hpp"
#include <chrono>
#include <iostream>

namespace nlm {

// TestRunner implementation

TestRunner::TestRunner(std::shared_ptr<Config> config) : config(config) {}

void TestRunner::addTest(std::unique_ptr<Test> test) {
    tests.push_back(std::move(test));
}

std::vector<TestResult> TestRunner::runAllTests() {
    std::vector<TestResult> results;
    
    // Setup all tests
    for (auto& test : tests) {
        if (!test->setup()) {
            TestResult result;
            result.name = test->getName();
            result.passed = false;
            result.message = "Test setup failed";
            results.push_back(result);
        }
    }
    
    // Run all tests
    for (auto& test : tests) {
        auto startTime = std::chrono::high_resolution_clock::now();
        results.push_back(test->run());
        auto endTime = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        results.back().executionTime = duration.count() / 1000.0f;
        
        test->cleanup();
    }
    
    return results;
}

void TestRunner::printResults(const std::vector<TestResult>& results) {
    int passed = 0;
    int failed = 0;
    
    std::cout << "\n" << std::string(80, '=') << "\n";
    std::cout << "NLM TEST RESULTS" << std::string(80 - 16, '=') << "\n\n";
    
    for (const auto& result : results) {
        std::cout << "Test: " << result.name << "\n";
        std::cout << "Status: " << (result.passed ? "✓ PASSED" : "✗ FAILED") << "\n";
        std::cout << "Message: " << result.message << "\n";
        if (!result.details.empty()) {
            std::cout << "Details: " << result.details << "\n";
        }
        std::cout << std::string(80, '-') << "\n\n";
        
        if (result.passed) passed++;
        else failed++;
    }
    
    std::cout << "SUMMARY: " << passed << " passed, " << failed << " failed\n";
    std::cout << std::string(80, '=') << "\n\n";
    
    if (failed > 0) {
        std::cout << "FAILED TESTS: ";
        for (const auto& result : results) {
            if (!result.passed) {
                std::cout << result.name << " ";
            }
        }
        std::cout << "\n";
    }
}

} // namespace nlm
