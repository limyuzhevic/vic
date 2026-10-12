// Integration test experiment runner
// 
// Manages and executes comprehensive integration test suites for NLM brain systems.
// Coordinates multiple integration test classes and provides test execution utilities.

#pragma once

#include "IntegrationTest.hpp"
#include "MemoryIntegrationTester.hpp"
#include "NeuromodulationIntegrationTester.hpp"
#include "PredictionIntegrationTester.hpp"
#include "CognitionIntegrationTester.hpp"
#include "DevelopmentIntegrationTester.hpp"
#include <vector>
#include <memory>
#include <string>
#include <chrono>

namespace nlm {
namespace test {

// Integration test experiment runner
class ExperimentRunnerIntegration {
public:
    ExperimentRunnerIntegration();
    ~ExperimentRunnerIntegration();
    
    // Run all integration tests
    void runAllIntegrationTests();
    
    // Run specific integration test suites
    void runMemoryIntegrationTests();
    void runNeuromodulationIntegrationTests();
    void runPredictionIntegrationTests();
    void runCognitionIntegrationTests();
    void runDevelopmentIntegrationTests();
    
    // Run cross-system workflow tests
    void runCrossSystemWorkflowTests();
    
    // Get test results
    struct TestResult {
        std::string testName;
        bool passed;
        double executionTime;
        std::string errorMessage;
    };
    
    const std::vector<TestResult>& getTestResults() const { return testResults_; }
    
    // Get test statistics
    size_t getTotalTests() const { return totalTests_; }
    size_t getPassedTests() const { return passedTests_; }
    size_t getFailedTests() const { return failedTests_; }
    double getTotalExecutionTime() const { return totalExecutionTime_; }
    
    // Clear test results
    void clearResults() { testResults_.clear(); totalTests_ = 0; passedTests_ = 0; failedTests_ = 0; totalExecutionTime_ = 0.0; }
    
private:
    // Test execution helpers
    template<typename TestClass>
    void runTestSuite(std::vector<TestResult>& results, const std::string& suiteName);
    
    // Integration test classes
    std::unique_ptr<MemoryIntegrationTester> memoryTester_;
    std::unique_ptr<NeuromodulationIntegrationTester> neuromodulationTester_;
    std::unique_ptr<PredictionIntegrationTester> predictionTester_;
    std::unique_ptr<CognitionIntegrationTester> cognitionTester_;
    std::unique_ptr<DevelopmentIntegrationTester> developmentTester_;
    
    // Test results
    std::vector<TestResult> testResults_;
    size_t totalTests_;
    size_t passedTests_;
    size_t failedTests_;
    double totalExecutionTime_;
}; // class ExperimentRunnerIntegration

} // namespace test
} // namespace nlm