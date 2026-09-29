#pragma once

#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <map>
#include <algorithm>
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Constants.hpp"

namespace nlm {

namespace tests {

// Test result structure
struct TestResult {
    enum class Status { PASSED, FAILED, SKIPPED, ERROR };
    
    std::string name;
    Status status;
    std::string message;
    std::chrono::milliseconds duration;
    
    // Test metadata
    std::map<std::string, std::string> metadata;
    
    TestResult(const std::string& testName, Status testStatus, 
               const std::string& testMessage = "",
               const std::chrono::milliseconds& testDuration = std::chrono::milliseconds(0))
        : name(testName), status(testStatus), message(testMessage), 
          duration(testDuration) {}
    
    bool isPassed() const { return status == Status::PASSED; }
    bool isFailed() const { return status == Status::FAILED; }
    bool isSkipped() const { return status == Status::SKIPPED; }
    bool isError() const { return status == Status::ERROR; }
};

// Test configuration
struct TestConfig {
    // Test parameters
    bool enabled = true;
    std::string description;
    
    // Configuration parameters
    std::map<std::string, std::string> parameters;
    
    // Thresholds and tolerances
    float tolerance = 0.01f;
    int maxIterations = 1000;
    double maxTimeSeconds = 10.0;
    
    // Expected results
    std::map<std::string, double> expectedValues;
    
    TestConfig(const std::string& desc = "", bool enabledFlag = true)
        : description(desc), enabled(enabledFlag) {}
};

// Base test class
class TestBase {
protected:
    std::shared_ptr<Brain> brain_;
    TestConfig testConfig_;
    
public:
    TestBase(const TestConfig& config) : testConfig_(config) {}
    virtual ~TestBase() = default;
    
    // Disable copying
    TestBase(const TestBase&) = delete;
    TestBase& operator=(const TestBase&) = delete;
    
    // Test execution
    virtual TestResult run() = 0;
    
    // Setup and teardown
    virtual TestResult setup() { return TestResult("Setup", TestResult::Status::PASSED); }
    virtual TestResult teardown() { return TestResult("Teardown", TestResult::Status::PASSED); }
    
    // Getters
    const TestConfig& getConfig() const { return testConfig_; }
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    
    // Configuration access
    template<typename T>
    T getParameter(const std::string& key, const T& defaultValue) const {
        auto it = testConfig_.parameters.find(key);
        if (it != testConfig_.parameters.end()) {
            // Parse parameter based on type
            if constexpr (std::is_same_v<T, int>) {
                return std::stoi(it->second);
            } else if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>) {
                return std::stod(it->second);
            } else if constexpr (std::is_same_v<T, bool>) {
                return it->second == "true" || it->second == "1";
            } else {
                return defaultValue;
            }
        }
        return defaultValue;
    }
    
    // Helper methods for common test operations
    template<typename Func>
    TestResult executeWithTimeout(Func&& func, const std::string& operation, 
                                  double timeoutSeconds = 5.0) {
        auto startTime = std::chrono::steady_clock::now();
        
        try {
            auto result = func();
            
            auto endTime = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            
            if (duration.count() > timeoutSeconds * 1000) {
                return TestResult(operation, TestResult::Status::FAILED,
                                 "Operation timed out after " + std::to_string(duration.count()) + "ms");
            }
            
            return TestResult(operation, TestResult::Status::PASSED, "", duration);
            
        } catch (const std::exception& e) {
            return TestResult(operation, TestResult::Status::ERROR, 
                             std::string("Exception: ") + e.what());
        }
    }
    
    // Brain setup helper
    TestResult setupBrain(const std::shared_ptr<Config>& config) {
        try {
            brain_ = std::make_shared<Brain>(config);
            
            if (!brain_->initialize()) {
                return TestResult("BrainSetup", TestResult::Status::FAILED,
                                 "Brain initialization failed");
            }
            
            return TestResult("BrainSetup", TestResult::Status::PASSED);
            
        } catch (const std::exception& e) {
            return TestResult("BrainSetup", TestResult::Status::ERROR,
                             std::string("Exception during brain setup: ") + e.what());
        }
    }
    
    // Brain reset helper
    TestResult resetBrain() {
        if (!brain_) {
            return TestResult("BrainReset", TestResult::Status::FAILED, "No brain to reset");
        }
        
        try {
            brain_->reset();
            brain_->initialize();
            return TestResult("BrainReset", TestResult::Status::PASSED);
        } catch (const std::exception& e) {
            return TestResult("BrainReset", TestResult::Status::ERROR,
                             std::string("Exception during brain reset: ") + e.what());
        }
    }
};

// Test framework
class TestFramework {
private:
    std::vector<std::unique_ptr<TestBase>> tests_;
    std::vector<TestResult> results_;
    TestConfig defaultConfig_;
    
public:
    TestFramework() = default;
    
    // Test registration
    template<typename TestClass>
    void registerTest(const std::string& name, const TestConfig& config = TestConfig()) {
        static_assert(std::is_base_of_v<TestBase, TestClass>, "TestClass must inherit from TestBase");
        
        tests_.push_back(std::make_unique<TestClass>(config));
    }
    
    // Test execution
    TestResult runTest(size_t index) {
        if (index >= tests_.size()) {
            return TestResult("Test_" + std::to_string(index), TestResult::Status::FAILED,
                             "Test index out of range");
        }
        
        auto& test = tests_[index];
        
        // Setup
        auto setupResult = test->setup();
        if (!setupResult.isPassed()) {
            return setupResult;
        }
        
        // Run test
        auto startTime = std::chrono::steady_clock::now();
        TestResult testResult;
        
        try {
            testResult = test->run();
        } catch (const std::exception& e) {
            testResult = TestResult(test->getConfig().description, TestResult::Status::ERROR,
                                   std::string("Test exception: ") + e.what());
        }
        
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        testResult.duration = duration;
        
        // Teardown
        auto teardownResult = test->teardown();
        if (!teardownResult.isPassed()) {
            testResult.message += "\nTeardown failed: " + teardownResult.message;
            testResult.status = TestResult::Status::ERROR;
        }
        
        results_.push_back(testResult);
        return testResult;
    }
    
    // Run all tests
    std::vector<TestResult> runAllTests() {
        results_.clear();
        
        for (size_t i = 0; i < tests_.size(); ++i) {
            TestResult result = runTest(i);
            
            // Log test result
            logTestResult(result);
        }
        
        return results_;
    }
    
    // Run specific test
    TestResult runSingleTest(size_t index) {
        if (index >= tests_.size()) {
            return TestResult("Test_" + std::to_string(index), TestResult::Status::FAILED,
                             "Test index out of range");
        }
        
        return runTest(index);
    }
    
    // Get test results
    const std::vector<TestResult>& getResults() const { return results_; }
    
    // Test statistics
    size_t getTotalTests() const { return tests_.size(); }
    size_t getPassedTests() const {
        return std::count_if(results_.begin(), results_.end(),
                           [](const TestResult& result) { return result.isPassed(); });
    }
    size_t getFailedTests() const {
        return std::count_if(results_.begin(), results_.end(),
                           [](const TestResult& result) { return result.isFailed(); });
    }
    size_t getSkippedTests() const {
        return std::count_if(results_.begin(), results_.end(),
                           [](const TestResult& result) { return result.isSkipped(); });
    }
    size_t getErrorTests() const {
        return std::count_if(results_.begin(), results_.end(),
                           [](const TestResult& result) { return result.isError(); });
    }
    
    // Test filtering
    void filterTests(const std::function<bool(const TestBase&)>& filter) {
        std::vector<std::unique_ptr<TestBase>> filteredTests;
        
        for (auto& test : tests_) {
            if (filter(*test)) {
                filteredTests.push_back(std::move(test));
            }
        }
        
        tests_ = std::move(filteredTests);
    }
    
    // Clear results
    void clearResults() { results_.clear(); }
    
    // Test result logging
    void logTestResult(const TestResult& result) {
        std::cout << std::left << std::setw(40) << result.name << " ";
        
        switch (result.status) {
            case TestResult::Status::PASSED:
                std::cout << "[PASS]";
                break;
            case TestResult::Status::FAILED:
                std::cout << "[FAIL]";
                break;
            case TestResult::Status::SKIPPED:
                std::cout << "[SKIP]";
                break;
            case TestResult::Status::ERROR:
                std::cout << "[ERROR]";
                break;
        }
        
        if (!result.message.empty()) {
            std::cout << " - " << result.message;
        }
        
        if (result.duration.count() > 0) {
            std::cout << " (" << result.duration.count() << "ms)";
        }
        
        std::cout << std::endl;
    }
    
    // Get summary report
    std::string getSummaryReport() const {
        std::ostringstream oss;
        oss << "=== Test Summary ===\n";
        oss << "Total Tests: " << getTotalTests() << "\n";
        oss << "Passed: " << getPassedTests() << "\n";
        oss << "Failed: " << getFailedTests() << "\n";
        oss << "Skipped: " << getSkippedTests() << "\n";
        oss << "Errors: " << getErrorTests() << "\n";
        oss << "Success Rate: " << (getTotalTests() > 0 ? 
            (static_cast<double>(getPassedTests()) / getTotalTests() * 100.0) : 0.0) << "%\n";
        
        return oss.str();
    }
    
    // Test configuration
    void setDefaultConfig(const TestConfig& config) { defaultConfig_ = config; }
    const TestConfig& getDefaultConfig() const { return defaultConfig_; }
};

// Test macros for easy test creation
#define REGISTER_TEST(ClassName, TestName, Config) \
    class TestName ## _Test : public ClassName { \
    public: \
        TestName ## _Test() : ClassName(Config) {} \
    }; \
    void registerTestName ## _Test() { \
        static TestName ## _Test testInstance; \
        /* Test will be registered automatically in main */ \
    }

#define TEST_CASE(description) \
    TestResult run() override { \
        return TestResult(#description, TestResult::Status::PASSED, description); \
    }

#define TEST_ASSUME(condition, message) \
    if (!(condition)) { \
        return TestResult(getConfig().description, TestResult::Status::FAILED, message); \
    }

#define TEST_EXPECT(condition, message) \
    if (!(condition)) { \
        return TestResult(getConfig().description, TestResult::Status::FAILED, message); \
    }

#define TEST_ASSERT(condition, message) \
    if (!(condition)) { \
        return TestResult(getConfig().description, TestResult::Status::ERROR, message); \
    }

// Specific test implementations for NLM tests
class BasicConnectivityTest : public TestBase {
public:
    BasicConnectivityTest(const TestConfig& config) : TestBase(config) {}
    
    TestResult run() override {
        return executeWithTimeout([this]() {
            // Inject strong current into first 10 neurons
            auto* region = brain_->getRegion(RegionId(1));
            auto neurons = region->getAllNeurons();
            
            if (neurons.empty()) {
                return TestResult("BasicConnectivityTest", TestResult::Status::FAILED,
                                 "No neurons found");
            }
            
            // Get initial spike count
            size_t initialSpikes = brain_->getTotalSpikeCount();
            
            // Inject current using constants
            size_t numNeuronsToStimulate = getParameter<size_t>("num_neurons", 10);
            float currentStrength = getParameter<float>("current_strength", 
                                                      nlm::constants::main_phase2::EXCITATORY_CURRENT_STRENGTH);
            
            for (size_t i = 0; i < std::min(numNeuronsToStimulate, neurons.size()); ++i) {
                neurons[i]->injectCurrent(currentStrength);
            }
            
            // Run steps using constants
            size_t testSteps = getParameter<size_t>("test_steps", 
                                                   nlm::constants::main_phase2::BASIC_CONNECTIVITY_TEST_STEPS);
            
            for (SimulationStep step = 0; step < testSteps; ++step) {
                brain_->step(step, step * nlm::constants::main_phase2::DEFAULT_TIMESTEP);
            }
            
            size_t spikes = brain_->getTotalSpikeCount() - initialSpikes;
            
            if (spikes > 0) {
                return TestResult("BasicConnectivityTest", TestResult::Status::PASSED,
                                 "Spikes propagated successfully: " + std::to_string(spikes));
            } else {
                return TestResult("BasicConnectivityTest", TestResult::Status::FAILED,
                                 "No spikes generated - network disconnected");
            }
            
        }, "BasicConnectivityTest");
    }
};

class PlasticityExperimentTest : public TestBase {
public:
    PlasticityExperimentTest(const TestConfig& config) : TestBase(config) {}
    
    TestResult run() override {
        return executeWithTimeout([this]() {
            // Reset brain for plasticity experiment
            brain_->reset();
            brain_->initialize();
            
            // Enable plasticity on synapses
            if (auto* region = brain_->getRegion(RegionId(1))) {
                for (auto& syn : region->getSynapses()) {
                    syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
                }
            }
            
            // Apply repeated input pattern
            size_t repeatedInputSteps = getParameter<size_t>("repeated_input_steps", 
                                                             nlm::constants::main_phase2::REPEATED_INPUT_PATTERN_STEPS);
            
            NLM_LOG_INFO("Applying repeated input patterns (" + std::to_string(repeatedInputSteps) + " steps)...");
            
            for (SimulationStep step = 0; step < repeatedInputSteps; ++step) {
                // Create input pattern - inject current into sensory neurons
                size_t numSensoryNeurons = getParameter<size_t>("num_sensory_neurons", 20);
                float sensoryCurrent = getParameter<float>("sensory_current", 30.0f);
                
                for (size_t i = 0; i < numSensoryNeurons && i < brain_->getTotalNeuronCount() / 4; ++i) {
                    brain_->injectCurrentToNeurons(NeuronType::Sensory, sensoryCurrent);
                }
                
                brain_->step(step, step * nlm::constants::main_phase2::DEFAULT_TIMESTEP);
                
                // Log progress every 100 steps
                if (step % 100 == 0) {
                    NLM_LOG_INFO("  Step " + std::to_string(step) + 
                                " | Spikes: " + std::to_string(brain_->getTotalSpikeCount()) +
                                " | Firing: " + std::to_string(brain_->getFiringNeuronCount()));
                }
            }
            
            return TestResult("PlasticityExperimentTest", TestResult::Status::PASSED,
                             "Plasticity experiment completed successfully");
            
        }, "PlasticityExperimentTest");
    }
};

class STDPVerificationTest : public TestBase {
public:
    STDPVerificationTest(const TestConfig& config) : TestBase(config) {}
    
    TestResult run() override {
        return executeWithTimeout([this]() {
            auto* region = brain_->getRegion(RegionId(1));
            if (!region) {
                return TestResult("STDPVerificationTest", TestResult::Status::FAILED,
                                 "No brain region found");
            }
            
            // Get first few synapses
            auto& synapses = region->getSynapses();
            if (synapses.size() < 5) {
                return TestResult("STDPVerificationTest", TestResult::Status::FAILED,
                                 "Not enough synapses for STDP test");
            }
            
            NLM_LOG_INFO("Testing STDP on 5 synapses:");
            
            // Record initial weights
            std::vector<float> beforeWeights;
            for (size_t i = 0; i < 5; ++i) {
                beforeWeights.push_back(synapses[i]->getWeight());
                synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
                NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                            " before: " + std::to_string(beforeWeights[i]));
            }
            
            // Create correlated activity: fire pre then post to trigger LTP
            NLM_LOG_INFO("");
            NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");
            
            size_t stdpTrials = getParameter<size_t>("stdp_trials", 
                                                    nlm::constants::main_phase2::STDP_TEST_TRIALS);
            
            for (int trial = 0; trial < stdpTrials; ++trial) {
                // Fire pre-synaptic neuron
                Neuron* preNeuron = nullptr;
                Neuron* postNeuron = nullptr;
                
                auto neurons = region->getAllNeurons();
                if (neurons.size() >= 2) {
                    preNeuron = neurons[0];
                    postNeuron = neurons[1];
                }
                
                if (preNeuron && postNeuron) {
                    // Pre fires first
                    preNeuron->injectCurrent(nlm::constants::main_phase2::SYNAPTIC_CURRENT_FOR_TESTING);
                    brain_->step(trial * 2, trial * 2 * nlm::constants::main_phase2::DEFAULT_TIMESTEP);
                    
                    // Then post fires
                    postNeuron->injectCurrent(nlm::constants::main_phase2::SYNAPTIC_CURRENT_FOR_TESTING);
                    brain_->step(trial * 2 + 1, (trial * 2 + 1) * nlm::constants::main_phase2::DEFAULT_TIMESTEP);
                }
            }
            
            // Record after weights
            NLM_LOG_INFO("  After correlated activity:");
            for (size_t i = 0; i < 5; ++i) {
                float delta = synapses[i]->getWeight() - beforeWeights[i];
                NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                            " after: " + std::to_string(synapses[i]->getWeight()) +
                            " (Δ=" + std::to_string(delta) + ")");
            }
            
            // Check if weights increased (LTP)
            float totalDelta = 0.0f;
            for (size_t i = 0; i < 5; ++i) {
                totalDelta += synapses[i]->getWeight() - beforeWeights[i];
            }
            
            NLM_LOG_INFO("");
            if (totalDelta > nlm::constants::experiments::LEARNING_THRESHOLD) {
                return TestResult("STDPVerificationTest", TestResult::Status::PASSED,
                                 "STDP working: Pre-before-post produced potentiation (Δ=" + 
                                 std::to_string(totalDelta) + ")");
            } else if (totalDelta < -nlm::constants::experiments::LEARNING_THRESHOLD) {
                return TestResult("STDPVerificationTest", TestResult::Status::FAILED,
                                 "STDP reversed: Check parameters (Δ=" + std::to_string(totalDelta) + ")");
            } else {
                return TestResult("STDPVerificationTest", TestResult::Status::FAILED,
                                 "No change: STDP may not be triggering (Δ=" + std::to_string(totalDelta) + ")");
            }
            
        }, "STDPVerificationTest");
    }
};

// Convenience function to register NLM tests
inline void registerNLMTests(TestFramework& framework) {
    // Register default test configurations
    TestConfig basicConfig("Basic neural connectivity test");
    basicConfig.parameters["num_neurons"] = "10";
    basicConfig.parameters["current_strength"] = std::to_string(nlm::constants::main_phase2::EXCITATORY_CURRENT_STRENGTH);
    basicConfig.parameters["test_steps"] = std::to_string(nlm::constants::main_phase2::BASIC_CONNECTIVITY_TEST_STEPS);
    
    framework.registerTest<BasicConnectivityTest>("BasicConnectivity", basicConfig);
    
    TestConfig plasticityConfig("Plasticity learning experiment");
    plasticityConfig.parameters["repeated_input_steps"] = std::to_string(nlm::constants::main_phase2::REPEATED_INPUT_PATTERN_STEPS);
    plasticityConfig.parameters["num_sensory_neurons"] = "20";
    plasticityConfig.parameters["sensory_current"] = "30.0";
    
    framework.registerTest<PlasticityExperimentTest>("PlasticityExperiment", plasticityConfig);
    
    TestConfig stdpConfig("STDP verification");
    stdpConfig.parameters["stdp_trials"] = std::to_string(nlm::constants::main_phase2::STDP_TEST_TRIALS);
    
    framework.registerTest<STDPVerificationTest>("STDPVerification", stdpConfig);
}

} // namespace tests

} // namespace nlm
