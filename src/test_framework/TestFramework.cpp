// NLM Test Framework Implementation
// Implements the test framework functionality

#include "test_framework/TestFramework.hpp"
#include <iostream>
#include <algorithm>

namespace nlm {

// TestFramework implementation details

// Private helper methods
void TestFramework::runTest(TestCase& testCase) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    testCase.result.status = TestStatus::RUNNING;
    std::cout << "Running: " << testCase.name << " [" << testCase.category << "]" << std::endl;
    
    try {
        if (testCase.config) {
            // Use test configuration if provided
            testCase.testFunc();
        } else {
            testCase.testFunc();
        }
        
        // Test passed
        testCase.result.status = TestStatus::PASSED;
        testCase.result.message = "Test passed successfully";
        passedTests++;
        
    } catch (const std::runtime_error& e) {
        // Test failed
        testCase.result.status = TestStatus::FAILED;
        testCase.result.message = std::string("Test failed: ") + e.what();
        failedTests++;
        
        std::cerr << "FAILED: " << testCase.name << " [" << testCase.category << "]" 
                 << std::endl << "Error: " << e.what() << std::endl;
        
    } catch (...) {
        // Test failed with unknown exception
        testCase.result.status = TestStatus::FAILED;
        testCase.result.message = "Test failed with unknown exception";
        failedTests++;
        
        std::cerr << "FAILED: " << testCase.name << " [" << testCase.category << "]" 
                 << std::endl << "Error: Unknown exception" << std::endl;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    testCase.result.durationSeconds = duration.count() / 1000000.0;
}

void TestFramework::printSummary(double totalTime) {
    std::cout << std::endl;
    std::cout << "=== Test Summary ===" << std::endl;
    std::cout << getSummary() << std::endl;
    std::cout << "Total time: " << std::fixed << std::setprecision(3) << totalTime << "s" << std::endl;
    
    if (failedTests == 0) {
        std::cout << "✓ All tests passed!" << std::endl;
    } else {
        std::cout << "✗ " << failedTests << " tests failed." << std::endl;
    }
}

} // namespace nlm
