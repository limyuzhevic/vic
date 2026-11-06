// Test runner for NLM Phase 2 neural computation tests
// Provides modular execution of individual or combined tests with proper sequencing

#pragma once

#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "experiments/LearningExperiment.hpp"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <functional>

namespace nlm {

// Test result structure
struct TestResult {
    std::string testName;
    double wallClockTime;
    bool success;
    std::string errorMessage;
    size_t spikeCount;
    size_t totalNeuronCount;
    size_t totalSynapseCount;
    float avgFiringRate;
    float meanWeightChange;
    bool learningDetected;
    
    TestResult() : wallClockTime(0.0), success(false), spikeCount(0), 
                   totalNeuronCount(0), totalSynapseCount(0), avgFiringRate(0.0f),
                   meanWeightChange(0.0f), learningDetected(false) {}
};

// Test configuration
struct TestConfig {
    std::string name;
    std::string description;
    std::function<void(std::shared_ptr<Brain>, const TestConfig&)> testFunction;
    bool requiresReset = true; // Whether to reset brain before this test
    bool enabled = true;
};

// High precision timer
class TestTimer {
private:
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::duration<double> elapsedTime;
    
public:
    void start() {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    void stop() {
        auto endTime = std::chrono::high_resolution_clock::now();
        elapsedTime = endTime - startTime;
    }
    
    double getSeconds() const {
        return elapsedTime.count();
    }
    
    long long getMilliseconds() const {
        return std::chrono::duration_cast<std::chrono::milliseconds>(elapsedTime).count();
    }
    
    long long getMicroseconds() const {
        return std::chrono::duration_cast<std::chrono::microseconds>(elapsedTime).count();
    }
};

// Main test runner class
class TestRunner {
private:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<Config> config;
    std::vector<TestResult> results;
    TestTimer timer;
    
    // Test configurations
    std::map<std::string, TestConfig> testConfigs;
    
public:
    TestRunner() {
        initializeTestConfigurations();
    }
    
    ~TestRunner() = default;
    
    // Initialize test configurations
    void initializeTestConfigurations() {
        // Test 1: Basic Connectivity
        testConfigs["test1"] = {
            "test1",
            "Basic neural connectivity - test spike propagation",
            [this](std::shared_ptr<Brain> b, const TestConfig&) {
                runBasicConnectivityTest(b);
            },
            true,  // requires reset
            true
        };
        
        // Test 2: Plasticity Learning
        testConfigs["test2"] = {
            "test2", 
            "Plasticity learning experiment - Hebbian and STDP",
            [this](std::shared_ptr<Brain> b, const TestConfig&) {
                runPlasticityExperiment(b);
            },
            true,  // requires reset
            true
        };
        
        // Test 3: STDP Verification
        testConfigs["test3"] = {
            "test3",
            "STDP verification - pre-before-post potentiation",
            [this](std::shared_ptr<Brain> b, const TestConfig&) {
                runStdpVerificationTest(b);
            },
            true,  // requires reset
            true
        };
        
        // Test 4: Advanced STDP
        testConfigs["stp"] = {
            "stp",
            "Advanced STDP experiment - multiple synapse testing",
            [this](std::shared_ptr<Brain> b, const TestConfig&) {
                runAdvancedStdpTest(b);
            },
            true,  // requires reset
            true
        };
        
        // Test 5: General Learning Experiment
        testConfigs["experiment"] = {
            "experiment",
            "General learning experiment - comprehensive plasticity",
            [this](std::shared_ptr<Brain> b, const TestConfig&) {
                runLearningExperiment(b);
            },
            true,  // requires reset
            true
        };
        
        // Test 6: All Tests (special case - runs all individually)
        testConfigs["all"] = {
            "all",
            "Run all tests sequentially",
            [this](std::shared_ptr<Brain> b, const TestConfig&) {
                runAllTestsSequentially(b);
            },
            false,  // no reset - tests manage their own resets
            true
        };
    }
    
    // Set up brain and configuration
    void setupBrain(const std::shared_ptr<Config>& cfg) {
        config = cfg;
        brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        
        NLM_LOG_INFO("Brain initialized for testing");
    }
    
    // Run a single test
    TestResult runTest(const std::string& testName) {
        TestResult result;
        result.testName = testName;
        
        if (testConfigs.find(testName) == testConfigs.end()) {
            result.success = false;
            result.errorMessage = "Unknown test: " + testName;
            results.push_back(result);
            return result;
        }
        
        const TestConfig& testConfig = testConfigs[testName];
        
        if (!testConfig.enabled) {
            result.success = false;
            result.errorMessage = "Test disabled: " + testName;
            results.push_back(result);
            return result;
        }
        
        try {
            // Reset brain if required
            if (testConfig.requiresReset) {
                brain->reset();
                brain->initialize();
                NLM_LOG_INFO("Brain reset for test: " + testName);
            }
            
            // Start timing
            timer.start();
            
            // Run the test
            testConfig.testFunction(brain, testConfig);
            
            // Stop timing
            timer.stop();
            
            // Collect results
            result.wallClockTime = timer.getSeconds();
            result.success = true;
            result.spikeCount = brain->getTotalSpikeCount();
            result.totalNeuronCount = brain->getTotalNeuronCount();
            result.totalSynapseCount = brain->getTotalSynapseCount();
            result.avgFiringRate = brain->getAverageFiringRate();
            
            NLM_LOG_INFO("Test " + testName + " completed successfully in " + 
                        std::to_string(result.wallClockTime) + "s");
            
        } catch (const std::exception& e) {
            timer.stop();
            result.success = false;
            result.errorMessage = std::string("Exception: ") + e.what();
            NLM_LOG_ERROR("Test " + testName + " failed: " + result.errorMessage);
        }
        
        results.push_back(result);
        return result;
    }
    
    // Run all enabled tests
    std::vector<TestResult> runAllTests() {
        NLM_LOG_INFO("Running all enabled tests...");
        
        for (auto& pair : testConfigs) {
            const std::string& testName = pair.first;
            const TestConfig& testConfig = pair.second;
            
            if (testConfig.enabled) {
                NLM_LOG_INFO("--- Running test: " + testName + " ---");
                runTest(testName);
            }
        }
        
        return results;
    }
    
    // Run specific tests (comma-separated list)
    std::vector<TestResult> runTests(const std::string& testList) {
        std::vector<std::string> testNames;
        size_t start = 0;
        size_t end = testList.find(',');
        
        while (end != std::string::npos) {
            testNames.push_back(testList.substr(start, end - start));
            start = end + 1;
            end = testList.find(',', start);
        }
        testNames.push_back(testList.substr(start));
        
        NLM_LOG_INFO("Running specific tests: " + testList);
        
        for (const std::string& testName : testNames) {
            // Trim whitespace
            std::string name = testName;
            name.erase(0, name.find_first_not_of(" \t\n\r"));
            name.erase(name.find_last_not_of(" \t\n\r") + 1);
            
            if (!name.empty()) {
                runTest(name);
            }
        }
        
        return results;
    }
    
    // Get all test results
    const std::vector<TestResult>& getResults() const {
        return results;
    }
    
    // Print test results summary
    void printResultsSummary() const {
        if (results.empty()) {
            std::cout << "No tests were run." << std::endl;
            return;
        }
        
        std::cout << std::endl;
        std::cout << "=== TEST RESULTS SUMMARY ===" << std::endl;
        std::cout << std::endl;
        
        size_t passed = 0;
        size_t failed = 0;
        double totalTime = 0.0;
        
        for (const auto& result : results) {
            std::cout << "Test: " << result.testName << std::endl;
            std::cout << "  Status: " << (result.success ? "PASS" : "FAIL") << std::endl;
            if (!result.success && !result.errorMessage.empty()) {
                std::cout << "  Error: " << result.errorMessage << std::endl;
            }
            std::cout << "  Wall time: " << std::fixed << std::setprecision(3) << result.wallClockTime << "s" << std::endl;
            std::cout << "  Spikes: " << result.spikeCount << std::endl;
            std::cout << "  Neurons: " << result.totalNeuronCount << std::endl;
            std::cout << "  Synapses: " << result.totalSynapseCount << std::endl;
            std::cout << "  Avg firing rate: " << std::fixed << std::setprecision(3) << result.avgFiringRate << " Hz" << std::endl;
            
            if (result.success) {
                passed++;
                totalTime += result.wallClockTime;
            } else {
                failed++;
            }
            std::cout << std::endl;
        }
        
        std::cout << "=== SUMMARY ===" << std::endl;
        std::cout << "Total tests: " << results.size() << std::endl;
        std::cout << "Passed: " << passed << std::endl;
        std::cout << "Failed: " << failed << std::endl;
        std::cout << "Total time: " << std::fixed << std::setprecision(3) << totalTime << "s" << std::endl;
        
        if (failed > 0) {
            std::cout << std::endl;
            std::cout << "FAILED TESTS:" << std::endl;
            for (const auto& result : results) {
                if (!result.success) {
                    std::cout << "  " << result.testName << ": " << result.errorMessage << std::endl;
                }
            }
        }
    }
    
    // Enable/disable specific tests
    void enableTest(const std::string& testName, bool enabled = true) {
        if (testConfigs.find(testName) != testConfigs.end()) {
            testConfigs[testName].enabled = enabled;
        }
    }
    
    // Check if test is enabled
    bool isTestEnabled(const std::string& testName) const {
        auto it = testConfigs.find(testName);
        return it != testConfigs.end() && it->second.enabled;
    }
    
    // Get list of available tests
    std::vector<std::string> getAvailableTests() const {
        std::vector<std::string> testNames;
        for (const auto& pair : testConfigs) {
            testNames.push_back(pair.first);
        }
        return testNames;
    }
    
    // Get test description
    std::string getTestDescription(const std::string& testName) const {
        auto it = testConfigs.find(testName);
        if (it != testConfigs.end()) {
            return it->second.description;
        }
        return "Unknown test";
    }
    
private:
    // Individual test implementations
    void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
        
        // Inject current into a few neurons and see if spikes propagate
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            NLM_LOG_ERROR("ERROR: No region found!");
            return;
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            NLM_LOG_INFO("No neurons found!");
            return;
        }
        
        // Get initial spike count
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject strong current into first 10 neurons
        NLM_LOG_INFO("Injecting current into " + std::to_string(std::min(size_t(10), neurons.size())) + " neurons...");
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
        }
        
        // Run a few steps
        NLM_LOG_INFO("Running 50 steps...");
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        NLM_LOG_INFO("Spikes generated: " + std::to_string(spikes));
        
        if (spikes > 0) {
            NLM_LOG_INFO("✓ Spikes propagate through network");
        } else {
            NLM_LOG_INFO("! No spikes - checking neuron parameters...");
            for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
                NLM_LOG_INFO("  Neuron " + std::to_string(i) + 
                            " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                            " thresh=" + std::to_string(neurons[i]->getThreshold()));
            }
        }
    }
    
    void runPlasticityExperiment(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
        
        LearningExperiment experiment(brain, 42);
        
        // Record initial state
        experiment.recordInitialState();
        
        // Enable plasticity on synapses
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
            }
        }
        
        // Apply repeated input pattern to stimulate learning
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Applying repeated input patterns (1000 steps)...");
        
        std::vector<NeuronId> patternNeurons;
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto* neuron : region->getAllNeurons()) {
                if (neuron->getType() == NeuronType::Sensory && patternNeurons.size() < 20) {
                    patternNeurons.push_back(neuron->getId());
                }
            }
        }
        NLM_LOG_INFO("Selected " + std::to_string(patternNeurons.size()) + " pattern neurons");
        
        for (SimulationStep step = 0; step < 1000; ++step) {
            for (NeuronId id : patternNeurons) {
                brain->injectCurrent(id, 30.0f);
            }
            
            brain->step(step, step * 0.001);
            
            // Log progress every 200 steps
            if (step % 200 == 0) {
                NLM_LOG_INFO("  Step " + std::to_string(step) + 
                            " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                            " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
            }
        }
        
        // Record final state
        experiment.recordFinalState();
        
        // Compute and display statistics
        experiment.computeStatistics();
    }
    
    void runStdpVerificationTest(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 3: STDP Verification ===");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            NLM_LOG_ERROR("ERROR: No region found!");
            return;
        }
        
        auto& synapses = region->getSynapses();
        if (synapses.empty()) {
            NLM_LOG_INFO("No synapses for STDP test");
            return;
        }
        
        Synapse* syn = synapses[0].get();
        syn->enablePlasticity(false, true, false);  // STDP only
        float beforeWeight = syn->getWeight();
        NLM_LOG_INFO("Testing STDP on 1 synapse:");
        NLM_LOG_INFO("  Synapse before: " + std::to_string(beforeWeight) +
                     " (pre=" + std::to_string(syn->getSourceNeuron().index()) +
                     ", post=" + std::to_string(syn->getDestinationNeuron().index()) + ")");
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Creating correlated pre->post activity (potentiation)...");
        
        for (int trial = 0; trial < 50; ++trial) {
            brain->injectCurrent(syn->getSourceNeuron(), 60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            brain->injectCurrent(syn->getDestinationNeuron(), 60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
        
        float delta = syn->getWeight() - beforeWeight;
        NLM_LOG_INFO("  After correlated activity:");
        NLM_LOG_INFO("    Synapse after: " + std::to_string(syn->getWeight()) +
                     " (Δ=" + std::to_string(delta) + ")");
        
        NLM_LOG_INFO("");
        if (delta > 0.001f) {
            NLM_LOG_INFO("✓ STDP WORKING: Pre-before-post produced potentiation");
        } else if (delta < -0.001f) {
            NLM_LOG_INFO("! STDP reversed: Check parameters");
        } else {
            NLM_LOG_INFO("! No change: STDP may not be triggering");
        }
    }
    
    void runAdvancedStdpTest(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 4: Advanced STDP Experiment ===");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            NLM_LOG_ERROR("ERROR: No region found!");
            return;
        }
        
        auto& synapses = region->getSynapses();
        if (synapses.empty()) {
            NLM_LOG_INFO("No synapses for STDP test");
            return;
        }
        
        size_t testCount = std::min(size_t(10), synapses.size());
        NLM_LOG_INFO("Testing " + std::to_string(testCount) + " synapses with correlated activity");
        
        float totalDelta = 0.0f;
        size_t successfulTests = 0;
        
        for (size_t testIdx = 0; testIdx < testCount; ++testIdx) {
            Synapse* syn = synapses[testIdx].get();
            syn->enablePlasticity(false, true, false);  // STDP only
            float beforeWeight = syn->getWeight();
            
            // Create correlated activity
            for (int trial = 0; trial < 10; ++trial) {
                brain->injectCurrent(syn->getSourceNeuron(), 60.0f);
                brain->step(trial * 2, trial * 2 * 0.001);
                brain->injectCurrent(syn->getDestinationNeuron(), 60.0f);
                brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
            }
            
            float delta = syn->getWeight() - beforeWeight;
            totalDelta += delta;
            
            if (std::abs(delta) > 0.001f) {
                successfulTests++;
            }
            
            if (testIdx < 3) {  // Only show first 3 tests
                NLM_LOG_INFO("  Synapse " + std::to_string(testIdx) + ": Δ=" + std::to_string(delta));
            }
        }
        
        NLM_LOG_INFO("Results: " + std::to_string(successfulTests) + "/" + std::to_string(testCount) + " synapses showed significant change");
        NLM_LOG_INFO("Average Δ: " + std::to_string(totalDelta / testCount));
        
        if (successfulTests > testCount * 0.7f) {
            NLM_LOG_INFO("✓ STDP robust: Most synapses responded correctly");
        } else {
            NLM_LOG_INFO("! STDP variable: Response rate lower than expected");
        }
    }
    
    void runLearningExperiment(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test: Learning Experiment ===");
        
        LearningExperiment experiment(brain, 42);
        
        // Enable all plasticity
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, true);
            }
        }
        
        // Record initial state
        experiment.recordInitialState();
        
        // Apply a more sophisticated pattern
        NLM_LOG_INFO("Applying sophisticated learning pattern...");
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return;
        
        std::vector<NeuronId> patternNeurons;
        for (auto* neuron : region->getAllNeurons()) {
            if (neuron->getType() == NeuronType::Sensory && patternNeurons.size() < 15) {
                patternNeurons.push_back(neuron->getId());
            }
        }
        
        // Two-phase learning
        NLM_LOG_INFO("  Phase 1: Pattern A (500 steps)");
        for (SimulationStep step = 0; step < 500; ++step) {
            // Alternate between pattern A and pattern B every 50 steps
            if (step < 250) {
                for (NeuronId id : patternNeurons) {
                    brain->injectCurrent(id, 25.0f);
                }
            } else {
                for (NeuronId id : patternNeurons) {
                    brain->injectCurrent(id, -15.0f);  // Inhibit
                }
            }
            
            brain->step(step, step * 0.001);
            
            if (step % 100 == 0) {
                NLM_LOG_INFO("    Step " + std::to_string(step) + 
                            " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                            " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
            }
        }
        
        // Record final state
        experiment.recordFinalState();
        
        // Compute statistics
        experiment.computeStatistics();
    }
    
    void runAllTestsSequentially(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("Running all tests sequentially...");
        
        // Run test 1
        runBasicConnectivityTest(brain);
        brain->reset();
        brain->initialize();
        
        // Run test 2
        runPlasticityExperiment(brain);
        brain->reset();
        brain->initialize();
        
        // Run test 3
        runStdpVerificationTest(brain);
        brain->reset();
        brain->initialize();
        
        // Run test 4
        runAdvancedStdpTest(brain);
        brain->reset();
        brain->initialize();
        
        // Run test 5
        runLearningExperiment(brain);
    }
};

} // namespace nlm