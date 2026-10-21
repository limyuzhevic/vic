// IntegrationTestRunner implementation
// Coordinates all integration test components

#include "experiments/Phase6IntegratedExperimentIntegration.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>

namespace nlm {

struct IntegrationTestRunner::Impl {
    // Test runner state
    bool testsRunning;
    int testCount;
    int passedTests;
    int failedTests;
    std::string currentTestName;
    
    Impl() : testsRunning(false), testCount(0), passedTests(0), failedTests(0) {}
    
    void logTestResult(const std::string& testName, bool passed) {
        testCount++;
        if (passed) {
            passedTests++;
            std::cout << "[PASS] " << testName << std::endl;
        } else {
            failedTests++;
            std::cout << "[FAIL] " << testName << std::endl;
        }
    }
    
    void printTestSummary() {
        std::cout << "\n=== Integration Test Summary ===" << std::endl;
        std::cout << "Total tests: " << testCount << std::endl;
        std::cout << "Passed: " << passedTests << std::endl;
        std::cout << "Failed: " << failedTests << std::endl;
        std::cout << "Success rate: " << (testCount > 0 ? (passedTests * 100 / testCount) : 0) << "%" << std::endl;
    }
};

IntegrationTestRunner::IntegrationTestRunner() : pImpl(std::make_unique<Impl>()) {}

IntegrationTestRunner::~IntegrationTestRunner() = default;

bool IntegrationTestRunner::runAllTests() {
    pImpl->testsRunning = true;
    pImpl->testCount = 0;
    pImpl->passedTests = 0;
    pImpl->failedTests = 0;
    
    std::cout << "=== Phase 6 Integration Test Suite ===" << std::endl;
    
    bool allPassed = true;
    
    // Integration verification
    pImpl->currentTestName = "Integration Verification";
    if (!experiment.verifyIntegration()) {
        allPassed = false;
        pImpl->logTestResult(pImpl->currentTestName, false);
    } else {
        pImpl->logTestResult(pImpl->currentTestName, true);
    }
    
    // Memory integration tests
    pImpl->currentTestName = "Memory Integration Tests";
    if (!BrainIntegrationTests::testBrainMemorySystems()) {
        allPassed = false;
        pImpl->logTestResult(pImpl->currentTestName, false);
    } else {
        pImpl->logTestResult(pImpl->currentTestName, true);
    }
    
    // Neuromodulation tests
    pImpl->currentTestName = "Neuromodulation Integration Tests";
    if (!NeuromodulationIntegrationTests::testDopamineSystem() ||
        !NeuromodulationIntegrationTests::testCuriositySystem() ||
        !NeuromodulationIntegrationTests::testPredictionErrorSystem() ||
        !NeuromodulationIntegrationTests::testNoveltyDetection()) {
        allPassed = false;
        pImpl->logTestResult(pImpl->currentTestName, false);
    } else {
        pImpl->logTestResult(pImpl->currentTestName, true);
    }
    
    // Brain functionality tests
    pImpl->currentTestName = "Brain Functionality Tests";
    if (!BrainIntegrationTests::testBasicBrainFunctionality() ||
        !BrainIntegrationTests::testBrainPlasticity() ||
        !BrainIntegrationTests::testBrainDevelopment()) {
        allPassed = false;
        pImpl->logTestResult(pImpl->currentTestName, false);
    } else {
        pImpl->logTestResult(pImpl->currentTestName, true);
    }
    
    // Checkpoint tests
    pImpl->currentTestName = "Checkpoint Integration Tests";
    if (!CheckpointIntegrationTests::testCheckpointSaveLoad()) {
        allPassed = false;
        pImpl->logTestResult(pImpl->currentTestName, false);
    } else {
        pImpl->logTestResult(pImpl->currentTestName, true);
    }
    
    // Replay tests
    pImpl->currentTestName = "Replay Integration Tests";
    if (!ReplayIntegrationTests::testMemoryReplay()) {
        allPassed = false;
        pImpl->logTestResult(pImpl->currentTestName, false);
    } else {
        pImpl->logTestResult(pImpl->currentTestName, true);
    }
    
    pImpl->testsRunning = false;
    pImpl->printTestSummary();
    
    return allPassed;
}

} // namespace nlm