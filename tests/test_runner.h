// Comprehensive Test Runner for NLM Brain Simulation
// Main test execution controller that coordinates all test suites

#pragma once

#include "test_suite_infrastructure.h"
#include <iostream>
#include <chrono>
#include <vector>
#include <string>

namespace nlm {
namespace test {

// Test execution result structure
struct TestExecutionResult {
    std::string testName;
    bool passed;
    double executionTime;
    std::string details;
};

// Comprehensive test runner with detailed reporting
class TestRunner {
public:
    TestRunner() : totalTests(0), passedTests(0), failedTests(0) {}
    
    void runAllTestSuites() {
        std::cout << std::endl;
        std::cout << "=== Executing NLM Brain Simulation Test Suites ===" << std::endl;
        std::cout << std::endl;
        
        // Run all test suites
        runPlasticityRuleSuites();
        runNeuralDynamicsSuites();
        runIntegrationSuites();
        runPerformanceSuites();
        
        // Generate final report
        generateTestReport();
    }
    
private:
    void runPlasticityRuleSuites() {
        std::cout << "Phase 1: Plasticity Rule Tests" << std::endl;
        std::cout << std::string(50, '-') << std::endl;
        
        // STDP Test Suite
        STDPTestSuite stdpSuite;
        auto stdpResults = executeTestSuite(stdpSuite, "STDP Plasticity");
        storeResults(stdpResults);
        
        // Hebbian Test Suite
        HebbianTestSuite hebbianSuite;
        auto hebbianResults = executeTestSuite(hebbianSuite, "Hebbian Plasticity");
        storeResults(hebbianResults);
        
        // Structural Plasticity Test Suite
        StructuralPlasticityTestSuite structuralSuite;
        auto structuralResults = executeTestSuite(structuralSuite, "Structural Plasticity");
        storeResults(structuralResults);
        
        // Plasticity Rule Base Class Test Suite
        PlasticityRuleTestSuite ruleSuite;
        auto ruleResults = executeTestSuite(ruleSuite, "Plasticity Rule Base Class");
        storeResults(ruleResults);
    }
    
    void runNeuralDynamicsSuites() {
        std::cout << "\nPhase 2: Neural Dynamics Tests" << std::endl;
        std::cout << std::string(50, '-') << std::endl;
        
        // Neuron Test Suite
        NeuronTestSuite neuronSuite;
        auto neuronResults = executeTestSuite(neuronSuite, "Neural Dynamics (Neuron)");
        storeResults(neuronResults);
        
        // Synapse Test Suite
        SynapseTestSuite synapseSuite;
        auto synapseResults = executeTestSuite(synapseSuite, "Neural Dynamics (Synapse)");
        storeResults(synapseResults);
        
        // Neural Population Test Suite
        NeuralPopulationTestSuite populationSuite;
        auto populationResults = executeTestSuite(populationSuite, "Neural Dynamics (Population)");
        storeResults(populationResults);
        
        // Neural Region Test Suite
        NeuralRegionTestSuite regionSuite;
        auto regionResults = executeTestSuite(regionSuite, "Neural Dynamics (Region)");
        storeResults(regionResults);
        
        // Brain Test Suite
        BrainTestSuite brainSuite;
        auto brainResults = executeTestSuite(brainSuite, "Neural Dynamics (Brain)");
        storeResults(brainResults);
    }
    
    void runIntegrationSuites() {
        std::cout << "\nPhase 3: Integration Tests" << std::endl;
        std::cout << std::string(50, '-') << std::endl;
        
        // Plasticity-Neural Dynamics Integration
        IntegrationPlasticityNeuralTestSuite integration1;
        auto integration1Results = executeTestSuite(integration1, "Plasticity-Neural Dynamics Integration");
        storeResults(integration1Results);
        
        // Memory System Integration
        IntegrationMemoryPlasticityTestSuite integration2;
        auto integration2Results = executeTestSuite(integration2, "Memory System Integration");
        storeResults(integration2Results);
        
        // Neuromodulation Effects
        IntegrationNeuromodulationTestSuite integration3;
        auto integration3Results = executeTestSuite(integration3, "Neuromodulation Effects");
        storeResults(integration3Results);
    }
    
    void runPerformanceSuites() {
        std::cout << "\nPhase 4: Performance Tests" << std::endl;
        std::cout << std::string(50, '-') << std::endl;
        
        // Performance Test Suite
        PerformanceTestSuite performanceSuite;
        auto performanceResults = executeTestSuite(performanceSuite, "Performance Validation");
        storeResults(performanceResults);
    }
    
    template<typename T>
    std::vector<TestExecutionResult> executeTestSuite(T& testSuite, const std::string& suiteName) {
        std::vector<TestExecutionResult> results;
        
        std::cout << "Executing " << suiteName << "..." << std::endl;
        
        // We need a way to extract individual test results from the suite
        // This is a simplified version - in practice, the test suite would provide detailed results
        double startTime = getCurrentTime();
        
        try {
            testSuite.run();
            
            double endTime = getCurrentTime();
            double duration = endTime - startTime;
            
            TestExecutionResult result;
            result.testName = suiteName;
            result.passed = true;
            result.executionTime = duration;
            result.details = "All tests passed successfully";
            
            results.push_back(result);
            
            std::cout << "  ✓ " << suiteName << " completed in " 
                      << duration << " seconds" << std::endl;
            
        } catch (const std::exception& e) {
            double endTime = getCurrentTime();
            double duration = endTime - startTime;
            
            TestExecutionResult result;
            result.testName = suiteName;
            result.passed = false;
            result.executionTime = duration;
            result.details = std::string("Failed: ") + e.what();
            
            results.push_back(result);
            
            std::cout << "  ✗ " << suiteName << " failed: " << e.what() << std::endl;
        }
        
        return results;
    }
    
    void storeResults(const std::vector<TestExecutionResult>& results) {
        for (const auto& result : results) {
            executionResults.push_back(result);
            
            if (result.passed) {
                passedTests++;
            } else {
                failedTests++;
            }
            
            totalTests++;
        }
    }
    
    void generateTestReport() {
        std::cout << std::endl;
        std::cout << "=" << std::string(60, '=') << std::endl;
        std::cout << "                    NLM BRAIN SIMULATION TEST REPORT" << std::endl;
        std::cout << "=" << std::string(60, '=') << std::endl;
        std::cout << std::endl;
        
        // Executive summary
        std::cout << "EXECUTIVE SUMMARY:" << std::endl;
        std::cout << "  Total Test Cases Executed: " << totalTests << std::endl;
        std::cout << "  Total Tests Passed:        " << passedTests << std::endl;
        std::cout << "  Total Tests Failed:        " << failedTests << std::endl;
        std::cout << "  Success Rate:              " << (totalTests > 0 ? (passedTests * 100.0 / totalTests) : 0.0) << "%" << std::endl;
        std::cout << "  Total Execution Time:      " << getTotalExecutionTime() << " seconds" << std::endl;
        std::cout << std::endl;
        
        // Detailed results by category
        std::cout << "DETAILED RESULTS BY CATEGORY:" << std::endl;
        std::cout << std::string(60, '-') << std::endl;
        
        std::string currentCategory = "";
        int categoryPassed = 0, categoryFailed = 0;
        
        for (const auto& result : executionResults) {
            if (result.testName != currentCategory) {
                if (!currentCategory.empty()) {
                    std::cout << "  " << currentCategory << ": " 
                              << categoryPassed << " passed, " 
                              << categoryFailed << " failed" << std::endl;
                }
                
                currentCategory = result.testName;
                categoryPassed = 0;
                categoryFailed = 0;
            }
            
            if (result.passed) {
                categoryPassed++;
            } else {
                categoryFailed++;
            }
        }
        
        if (!currentCategory.empty()) {
            std::cout << "  " << currentCategory << ": " 
                      << categoryPassed << " passed, " 
                      << categoryFailed << " failed" << std::endl;
        }
        
        std::cout << std::endl;
        
        // Performance metrics
        std::cout << "PERFORMANCE METRICS:" << std::endl;
        std::cout << std::string(60, '-') << std::endl;
        
        double avgTestTime = totalTests > 0 ? getTotalExecutionTime() / totalTests : 0.0;
        double throughput = totalTests > 0 ? totalTests / getTotalExecutionTime() : 0.0;
        
        std::cout << "  Average Time per Test:    " << avgTestTime << " seconds" << std::endl;
        std::cout << "  Tests per Second:         " << throughput << " tests/sec" << std::endl;
        std::cout << "  Total Categories Tested:   " << getUniqueCategories().size() << std::endl;
        std::cout << std::endl;
        
        // Status
        if (failedTests == 0) {
            std::cout << "STATUS: ALL TESTS PASSED SUCCESSFULLY!" << std::endl;
            std::cout << "The NLM brain simulation test suite demonstrates comprehensive" << std::endl;
            std::cout << "coverage of plasticity rules, neural dynamics, integration points, " << std::endl;
            std::cout << "and performance characteristics." << std::endl;
        } else {
            std::cout << "STATUS: TESTS COMPLETED WITH FAILURES" << std::endl;
            std::cout << "Please review the failed tests and address any issues before proceeding." << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << "=" << std::string(60, '=') << std::endl;
    }
    
    double getCurrentTime() {
        return std::chrono::duration<double>(
            std::chrono::high_resolution_clock::now().time_since_epoch()
        ).count();
    }
    
    double getTotalExecutionTime() {
        double totalTime = 0.0;
        for (const auto& result : executionResults) {
            totalTime += result.executionTime;
        }
        return totalTime;
    }
    
    std::set<std::string> getUniqueCategories() {
        std::set<std::string> categories;
        for (const auto& result : executionResults) {
            categories.insert(result.testName);
        }
        return categories;
    }
    
    std::vector<TestExecutionResult> executionResults;
    int totalTests;
    int passedTests;
    int failedTests;
};

// Global test runner instance
TestRunner& getGlobalTestRunner() {
    static TestRunner runner;
    return runner;
}

}}} // namespace nlm::test
