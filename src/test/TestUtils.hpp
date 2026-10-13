// Test utilities for NLM Phase 2
// Common functionality for neural network testing

#pragma once

#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "core/Config/Config.hpp"
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include <sstream>
#include <cmath>

namespace nlm {
namespace test {

// Test result structure for reporting
struct TestResult {
    std::string name;
    bool passed;
    std::string message;
    double executionTime;
    
    TestResult(const std::string& testName, bool success, const std::string& msg = "")
        : name(testName), passed(success), message(msg), executionTime(0.0) {}
};

// Test runner base class
class TestBase {
public:
    virtual ~TestBase() = default;
    virtual TestResult run(std::shared_ptr<Brain> brain) = 0;
    virtual std::string getName() const = 0;
};

// Utility functions
namespace utils {
    // Measure execution time
    template<typename Func, typename... Args>
    double measureTime(Func&& func, Args&&... args) {
        auto start = std::chrono::high_resolution_clock::now();
        func(std::forward<Args>(args)...);
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(end - start).count() / 1000.0;
    }
    
    // Validate brain state
    bool validateBrainHealth(std::shared_ptr<Brain> brain);
    
    // Get neuron information
    std::string formatNeuronInfo(std::shared_ptr<Brain> brain);
    
    // Get synapse information
    std::string formatSynapseInfo(std::shared_ptr<Brain> brain);
    
    // Calculate statistics
    struct BrainStats {
        size_t totalNeurons;
        size_t totalSynapses;
        size_t activeNeurons;
        size_t firingNeurons;
        float averageFiringRate;
        float excitationInhibitionRatio;
    };
    
    BrainStats calculateBrainStats(std::shared_ptr<Brain> brain);
}

// Macro for easy test registration
#define REGISTER_TEST(TestClass) \
    class TestClass##Wrapper : public TestBase { \
    public: \
        TestResult run(std::shared_ptr<Brain> brain) override { \
            TestClass test; \
            return test.run(brain); \
        } \
        std::string getName() const override { return #TestClass; } \
    }; \
    static TestClass##Wrapper testClass##WrapperInstance;

// Simple test class template
class SimpleTest : public TestBase {
protected:
    std::string testName;
    bool setup(std::shared_ptr<Brain> brain) { return true; }
    bool teardown(std::shared_ptr<Brain> brain) { return true; }
    
public:
    explicit SimpleTest(const std::string& name) : testName(name) {}
    
    TestResult run(std::shared_ptr<Brain> brain) override {
        if (!setup(brain)) {
            return TestResult(testName, false, "Setup failed");
        }
        
        bool success = executeTest(brain);
        
        if (!teardown(brain)) {
            return TestResult(testName, false, "Teardown failed");
        }
        
        return TestResult(testName, success, success ? "Test passed" : "Test failed");
    }
    
    virtual bool executeTest(std::shared_ptr<Brain> brain) = 0;
    std::string getName() const override { return testName; }
};

// Test runner that manages multiple tests
class TestRunner {
private:
    std::vector<std::unique_ptr<TestBase>> tests;
    bool verbose;
    
public:
    explicit TestRunner(bool verboseOutput = false) : verbose(verboseOutput) {}
    
    void addTest(std::unique_ptr<TestBase> test) {
        tests.push_back(std::move(test));
    }
    
    std::vector<TestResult> runAll(std::shared_ptr<Brain> brain) {
        std::vector<TestResult> results;
        
        for (auto& test : tests) {
            if (verbose) {
                NLM_LOG_INFO("Running test: " + test->getName());
            }
            
            TestResult result = test->run(brain);
            results.push_back(result);
            
            if (verbose) {
                std::string status = result.passed ? "PASSED" : "FAILED";
                NLM_LOG_INFO("Test " + test->getName() + ": " + status);
                if (!result.message.empty()) {
                    NLM_LOG_INFO("  " + result.message);
                }
            }
        }
        
        return results;
    }
    
    bool runTest(std::shared_ptr<Brain> brain, const std::string& testName) {
        for (auto& test : tests) {
            if (test->getName() == testName) {
                TestResult result = test->run(brain);
                return result.passed;
            }
        }
        return false;
    }
};

}} // namespace nlm::test