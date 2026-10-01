// NLM Test Infrastructure - Command Pattern Implementation
// Provides a clean framework for running neural learning machine tests

#ifndef NLM_TEST_RUNNER_HPP
#define NLM_TEST_RUNNER_HPP

#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <chrono>

#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"

namespace nlm {

class TestResult {
public:
    enum class Status { PASS, FAIL, SKIP };
    
    std::string testName;
    Status status;
    double executionTime;
    std::string errorMessage;
    int neuronCount;
    int synapseCount;
    
    TestResult(const std::string& name) 
        : testName(name), status(Status::SKIP), executionTime(0.0), 
          neuronCount(0), synapseCount(0) {}
};

class TestRunner {
private:
    std::shared_ptr<Brain> brain;
    std::map<std::string, std::function<bool()>> testRegistry;
    std::vector<TestResult> results;
    bool brainInitialized;
    
public:
    TestRunner(std::shared_ptr<Brain> b) 
        : brain(b), brainInitialized(false) {}
    
    void registerTest(const std::string& name, std::function<bool()> testFunc) {
        testRegistry[name] = testFunc;
        NLM_LOG_DEBUG("Registered test: " + name);
    }
    
    bool runTest(const std::string& testName) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        if (testRegistry.find(testName) == testRegistry.end()) {
            NLM_LOG_ERROR("Test not found: " + testName);
            return false;
        }
        
        NLM_LOG_INFO("=== Running Test: " + testName + " ===");
        
        // Ensure brain is initialized
        if (!brainInitialized) {
            if (!brain->initialize()) {
                NLM_LOG_ERROR("Brain initialization failed for test: " + testName);
                return false;
            }
            brainInitialized = true;
        }
        
        bool testPassed = testRegistry[testName]();
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        TestResult result(testName);
        result.status = testPassed ? TestResult::Status::PASS : TestResult::Status::FAIL;
        result.executionTime = duration.count() / 1000.0;
        result.neuronCount = brain->getTotalNeuronCount();
        result.synapseCount = brain->getTotalSynapseCount();
        
        if (testPassed) {
            NLM_LOG_INFO("✓ Test PASSED: " + testName + " (" + std::to_string(result.executionTime) + "s)");
        } else {
            NLM_LOG_ERROR("✗ Test FAILED: " + testName + " (" + std::to_string(result.executionTime) + "s)");
        }
        
        results.push_back(result);
        return testPassed;
    }
    
    void runAllTests() {
        NLM_LOG_INFO("=== Running All Registered Tests ===");
        for (const auto& pair : testRegistry) {
            runTest(pair.first);
            // Reset brain for next test to ensure isolation
            brain->reset();
        }
        
        NLM_LOG_INFO("=== Test Summary ===");
        int passed = 0, failed = 0, skipped = 0;
        for (const auto& result : results) {
            switch (result.status) {
                case TestResult::Status::PASS: passed++; break;
                case TestResult::Status::FAIL: failed++; break;
                case TestResult::Status::SKIP: skipped++; break;
            }
        }
        
        NLM_LOG_INFO("Total tests: " + std::to_string(results.size()));
        NLM_LOG_INFO("Passed: " + std::to_string(passed));
        NLM_LOG_INFO("Failed: " + std::to_string(failed));
        NLM_LOG_INFO("Skipped: " + std::to_string(skipped));
        
        if (failed > 0) {
            NLM_LOG_ERROR("Some tests failed! Check logs for details.");
        }
    }
    
    const std::vector<TestResult>& getResults() const { return results; }
    
    void reset() {
        brainInitialized = false;
        results.clear();
        NLM_LOG_DEBUG("TestRunner reset completed");
    }
};

} // namespace nlm

#endif // NLM_TEST_RUNNER_HPP