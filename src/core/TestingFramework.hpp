#ifndef NLM_TESTING_FRAMEWORK_H
#define NLM_TESTING_FRAMEWORK_H

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <cassert>
#include <algorithm>
#include <iomanip>
#include <sstream>

namespace nlm {
namespace testing {

// Test result structure
struct TestResult {
    std::string name;
    bool passed;
    std::string message;
    double executionTime;
    
    TestResult(const std::string& testName, bool success, const std::string& testMessage, double time)
        : name(testName), passed(success), message(testMessage), executionTime(time) {}
};

// Base test class
class Test {
public:
    virtual ~Test() = default;
    virtual std::string getName() const = 0;
    virtual bool run() = 0;
    virtual void setup() {}
    virtual void teardown() {}
};

// Test framework class
class TestFramework {
public:
    TestFramework() : totalTests(0), passedTests(0), failedTests(0) {}
    
    void addTest(std::unique_ptr<Test> test) {
        tests.push_back(std::move(test));
    }
    
    const std::vector<std::unique_ptr<Test>>& getTests() const {
        return tests;
    }
    
    size_t getTestCount() const {
        return tests.size();
    }
    
    std::vector<TestResult> runAllTests() {
        totalTests = passedTests = failedTests = 0;
        results.clear();
        
        for (auto& test : tests) {
            std::cout << "Running test: " << test->getName() << "..." << std::endl;
            
            // Setup
            test->setup();
            
            // Run test and measure time
            auto startTime = std::chrono::high_resolution_clock::now();
            bool success = test->run();
            auto endTime = std::chrono::high_resolution_clock::now();
            
            auto duration = std::chrono::duration<double>(endTime - startTime).count();
            
            // Teardown
            test->teardown();
            
            // Record result
            totalTests++;
            if (success) {
                passedTests++;
                results.emplace_back(test->getName(), true, "Test passed", duration);
                std::cout << "  ✓ PASSED (" << std::fixed << std::setprecision(3) << duration << "s)" << std::endl;
            } else {
                failedTests++;
                results.emplace_back(test->getName(), false, "Test failed", duration);
                std::cout << "  ✗ FAILED" << std::endl;
            }
        }
        
        return results;
    }
    
    void printResults() const {
        std::cout << "\n" << std::string(60, '=') << std::endl;
        std::cout << "TEST RESULTS SUMMARY" << std::endl;
        std::cout << std::string(60, '=') << std::endl;
        std::cout << "Total tests: " << totalTests << std::endl;
        std::cout << "Passed:      " << passedTests << std::endl;
        std::cout << "Failed:      " << failedTests << std::endl;
        std::cout << "Success rate: " << (totalTests > 0 ? (passedTests * 100.0 / totalTests) : 0) << "%" << std::endl;
        std::cout << std::string(60, '=') << std::endl;
        
        if (failedTests > 0) {
            std::cout << "\nFAILED TESTS:" << std::endl;
            std::cout << std::string(60, '-') << std::endl;
            for (const auto& result : results) {
                if (!result.passed) {
                    std::cout << "  " << result.name << " - " << result.message << std::endl;
                }
            }
        }
    }
    
    bool allTestsPassed() const {
        return failedTests == 0;
    }
    
    const std::vector<TestResult>& getResults() const {
        return results;
    }
    
private:
    std::vector<std::unique_ptr<Test>> tests;
    std::vector<TestResult> results;
    int totalTests;
    int passedTests;
    int failedTests;
};

// Macro for easy test creation
#define TEST_CLASS(Name) \
class Name : public Test { \
public: \
    std::string getName() const override { return "Test " #Name; } \

#define TEST_CASE(description) \
    public: \
    bool run() override { \
        std::cout << "  " << description << "..."; \

#define ASSERT_TRUE(condition) \
        if (!(condition)) { \
            std::cout << "FAILED" << std::endl; \
            return false; \
        } \

#define ASSERT_FALSE(condition) \
        if (condition) { \
            std::cout << "FAILED" << std::endl; \
            return false; \
        } \

#define ASSERT_EQUAL(expected, actual) \
        if ((expected) != (actual)) { \
            std::cout << "FAILED: expected " << (expected) << " but got " << (actual) << std::endl; \
            return false; \
        } \

#define ASSERT_APPROX_EQUAL(expected, actual, tolerance) \
        if (std::abs((expected) - (actual)) > (tolerance)) { \
            std::cout << "FAILED: expected " << (expected) << " but got " << (actual) << " (tolerance: " << (tolerance) << ")" << std::endl; \
            return false; \
        } \

// Neural network test utilities
class NeuralTestUtils {
public:
    // Create a standard test configuration
    static std::shared_ptr<Config> createTestConfig() {
        auto config = std::make_shared<Config>();
        
        // Basic configuration
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        
        // Plasticity parameters
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        config->set("stdp_tau", 20.0f, ConfigSource::Default);
        
        // Structural plasticity
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config->set("pruning_rate", 0.00001f, ConfigSource::Default);
        
        return config;
    }
    
    // Initialize a standard test brain
    static std::shared_ptr<Brain> createTestBrain() {
        auto config = createTestConfig();
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize test brain");
        }
        
        return brain;
    }
    
    // Reset brain to clean state
    static void resetBrain(std::shared_ptr<Brain> brain) {
        if (brain) {
            brain->reset();
            brain->initialize();
        }
    }
    
    // Get average membrane potential
    static double getAverageMembranePotential(std::shared_ptr<Brain> brain) {
        if (!brain) return 0.0;
        
        double totalPotential = 0.0;
        size_t neuronCount = 0;
        
        for (const auto& region : brain->getRegions()) {
            for (const auto& neuron : region->getAllNeurons()) {
                totalPotential += neuron->getMembranePotential();
                neuronCount++;
            }
        }
        
        return neuronCount > 0 ? totalPotential / neuronCount : 0.0;
    }
    
    // Get spike rate
    static double getSpikeRate(std::shared_ptr<Brain> brain) {
        if (!brain) return 0.0;
        
        size_t totalSpikes = brain->getTotalSpikeCount();
        double timestep = brain->getTimestep();
        
        return totalSpikes * timestep;
    }
    
    // Check if weights are within expected bounds
    static bool checkWeightBounds(std::shared_ptr<Brain> brain, float minWeight, float maxWeight) {
        if (!brain) return false;
        
        for (const auto& region : brain->getRegions()) {
            for (const auto& synapse : region->getSynapses()) {
                float weight = synapse->getWeight();
                if (weight < minWeight || weight > maxWeight) {
                    return false;
                }
            }
        }
        return true;
    }
};

} // namespace testing
} // namespace nlm

#endif // NLM_TESTING_FRAMEWORK_H
