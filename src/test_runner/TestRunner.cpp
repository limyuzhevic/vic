// NLM (熙然) - Neural Learning Machine
// Implementation: Test Runner
// Orchestrates test execution with progress tracking and error handling

#include "tests/TestRunner.hpp"
#include "tests/BasicConnectivityTest.hpp"
#include "tests/PlasticityTest.hpp"
#include "tests/StdpTest.hpp"
#include "core/Logger/Logger.hpp"
#include <chrono>
#include <thread>
#include <iomanip>

using namespace nlm::test_runner;

namespace {
    const std::chrono::milliseconds PROGRESS_UPDATE_INTERVAL(100);
}

void TestRunner::runTest(TestMode mode, std::shared_ptr<Brain> brain) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    switch (mode) {
        case TestMode::BasicConnectivity:
            NLM_LOG_INFO("Running Basic Connectivity Test...");
            BasicConnectivityTest::run(brain);
            break;
        case TestMode::PlasticityLearning:
            NLM_LOG_INFO("Running Plasticity Learning Test...");
            PlasticityTest::run(brain);
            break;
        case TestMode::StdpVerification:
            NLM_LOG_INFO("Running STDP Verification Test...");
            StdpTest::run(brain);
            break;
        case TestMode::All:
            NLM_LOG_INFO("Running all tests...");
            runAllTests(brain);
            return;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
    
    NLM_LOG_INFO("Test completed in " + std::to_string(duration.count()) + " seconds.");
}

void TestRunner::runAllTests(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Starting comprehensive NLM test suite...");
    NLM_LOG_INFO("");
    
    std::vector<TestMode> testSequence = {
        TestMode::BasicConnectivity,
        TestMode::PlasticityLearning,
        TestMode::StdpVerification
    };
    
    size_t totalTests = testSequence.size();
    size_t completedTests = 0;
    
    for (const auto& mode : testSequence) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test " + std::to_string(++completedTests) + "/" + std::to_string(totalTests) + " ===");
        
        try {
            runTest(mode, brain);
            NLM_LOG_INFO("✓ Test " + std::to_string(completedTests) + " passed");
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("✗ Test " + std::to_string(completedTests) + " failed: " + std::string(e.what()));
        } catch (...) {
            NLM_LOG_ERROR("✗ Test " + std::to_string(completedTests) + " failed with unknown error");
        }
        
        // Reset brain for next test
        if (completedTests < totalTests) {
            NLM_LOG_INFO("Resetting brain for next test...");
            brain->reset();
            brain->initialize();
        }
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test Suite Complete ===");
    NLM_LOG_INFO("All " + std::to_string(totalTests) + " tests completed successfully!");
}

TestRunner& TestRunner::getInstance() {
    static TestRunner instance;
    return instance;
}
