// Comprehensive Test Suite Runner
// This file runs all available tests for the NLM system

#include <iostream>
#include <chrono>
#include <vector>
#include <string>

// Test includes
#include "tests/test_config.cpp"
#include "tests/test_brain.cpp"

namespace nlm_test {

struct TestResult {
    std::string name;
    std::string status;
    double duration;  // in seconds
    std::string message;
};

class TestRunner {
public:
    std::vector<TestResult> results;
    
    void runAllTests() {
        std::cout << "========================================" << std::endl;
        std::cout << "NLM Comprehensive Test Suite" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << std::endl;
        
        // Run configuration tests
        std::cout << "Running Configuration Tests..." << std::endl;
        runConfigTests();
        
        // Run brain tests  
        std::cout << "Running Brain Tests..." << std::endl;
        runBrainTests();
        
        // Summary
        std::cout << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "Test Summary" << std::endl;
        std::cout << "========================================" << std::endl;
        
        int passed = 0;
        int failed = 0;
        
        for (const auto& result : results) {
            std::cout << result.name << ": " << result.status << " (" << result.duration << "s)" << std::endl;
            if (result.status == "PASSED") {
                passed++;
            } else {
                failed++;
                std::cout << "  Error: " << result.message << std::endl;
            }
        }
        
        std::cout << std::endl;
        std::cout << "Total Tests: " << results.size() << std::endl;
        std::cout << "Passed: " << passed << std::endl;
        std::cout << "Failed: " << failed << std::endl;
        
        if (failed == 0) {
            std::cout << "All tests PASSED!" << std::endl;
        } else {
            std::cout << "Tests FAILED! " << failed << " test(s) failed." << std::endl;
        }
    }
    
private:
    void runConfigTests() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        test_config::testConfigCreation();
        test_config::testConfigSetAndGet();
        test_config::testConfigGetOr();
        test_config::testConfigRemove();
        test_config::testConfigKeys();
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double>(endTime - startTime).count();
        
        std::cout << "Config tests completed in " << duration << "s" << std::endl;
    }
    
    void runBrainTests() {
        auto startTime = std::chrono::high_resolution_clock::auto() = std::chrono::high_resolution_clock::now();
        
        test_brain::testBrainCreation();
        test_brain::testBrainInitialization();
        test_brain::testBrainStep();
        test_brain::testBrainReset();
        test_brain::testBrainRegions();
        test_brain::testBrainActionProduction();
        test_brain::testBrainStatus();
        test_brain::testBrainCheckpointRoundTrip();
        test_brain::testPhase6TextEncoding();
        test_brain::testPhase6ConceptSimilarity();
        test_brain::testPhase6WorkingMemoryTraces();
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double>(endTime - startTime).count();
        
        std::cout << "Brain tests completed in " << duration << "s" << std::endl;
    }
};

} // namespace nlm_test

int main() {
    nlm_test::TestRunner runner;
    runner.runAllTests();
    return 0;
}