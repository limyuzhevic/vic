// TestFramework.cpp - Implementation of core testing framework
#include "TestingFramework.hpp"

namespace nlm::test {

// TestCase implementation
void TestCase::assertEqual(const std::string& actual, const std::string& expected, 
                          const std::string& message) {
    if (actual != expected) {
        throw std::runtime_error("Assertion failed: " + message +
                               " (expected: '" + expected + "', got: '" + actual + "')");
    }
}

void TestCase::assertNotEqual(const std::string& actual, const std::string& expected,
                             const std::string& message) {
    if (actual == expected) {
        throw std::runtime_error("Assertion failed: " + message +
                               " (unexpected: '" + expected + "')");
    }
}

// TestSuite implementation
std::vector<TestInfo> TestSuite::run() {
    std::vector<TestInfo> results;
    
    // Run each test
    for (auto& testCase : testCases) {
        TestInfo info(testCase->getName(), testCase->getCategory(), "");
        info.fileName = "test_framework_base.cpp";
        
        try {
            if (testCase->setupTestCase()) {
                auto start = std::chrono::high_resolution_clock::now();
                testCase->setup();
                testCase->run();
                testCase->teardown();
                auto end = std::chrono::high_resolution_clock::now();
                
                info.duration = end - start;
                info.result = TestResult::PASSED;
            } else {
                info.result = TestResult::SKIPPED;
                info.errorMessage = "Test case setup failed";
            }
        } catch (const std::exception& e) {
            auto end = std::chrono::high_resolution_clock::now();
            info.duration = end - start;
            info.result = TestResult::FAILED;
            info.errorMessage = e.what();
        }
        
        results.push_back(info);
    }
    
    return results;
}

} // namespace nlm::test
