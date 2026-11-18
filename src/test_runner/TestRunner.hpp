#pragma once

#include <string>
#include <vector>
#include <memory>
#include "src/core/Config/Config.hpp"
#include "src/brain/Brain.hpp"

namespace nlm {

// Test result structure for structured reporting
struct TestResult {
    std::string name;
    bool passed;
    std::string message;
    std::string details;
    float executionTime;
};

// Test interface
class Test {
public:
    virtual ~Test() = default;
    virtual std::string getName() const = 0;
    virtual TestResult run() = 0;
    virtual bool setup() { return true; }
    virtual bool cleanup() { return true; }
};

// Basic connectivity test
class BasicConnectivityTest : public Test {
public:
    std::string getName() const override { return "Basic Connectivity Test"; }
    
    TestResult run() override {
        TestResult result;
        result.name = getName();
        result.executionTime = 0.0f;
        
        if (!brain) {
            result.passed = false;
            result.message = "Brain not initialized";
            return result;
        }
        
        // Get initial spike count
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject current into neurons
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            result.passed = false;
            result.message = "No brain region found";
            return result;
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            result.passed = false;
            result.message = "No neurons found";
            return result;
        }
        
        // Inject current into first 10 neurons
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);
        }
        
        // Run simulation
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        
        if (spikes > 0) {
            result.passed = true;
            result.message = "Spikes propagated successfully";
            result.details = "Generated " + std::to_string(spikes) + " spikes";
        } else {
            result.passed = false;
            result.message = "No spikes generated";
            result.details = "Neuron potentials: V=" + std::to_string(neurons[0]->getMembranePotential()) + 
                          ", thresh=" + std::to_string(neurons[0]->getThreshold());
        }
        
        return result;
    }
    
    bool setup(std::shared_ptr<Brain> b) {
        brain = b;
        return brain != nullptr;
    }
    
private:
    std::shared_ptr<Brain> brain;
};

// Plasticity learning experiment test
class PlasticityExperimentTest : public Test {
public:
    std::string getName() const override { return "Plasticity Learning Experiment"; }
    
    TestResult run() override {
        TestResult result;
        result.name = getName();
        result.executionTime = 0.0f;
        
        if (!brain) {
            result.passed = false;
            result.message = "Brain not initialized";
            return result;
        }
        
        // Record initial state
        float initialMeanWeight = recordInitialState();
        
        // Enable plasticity
        enablePlasticity(true, true, false);
        
        // Apply repeated input pattern
        applyInputPattern(1000);
        
        // Record final state
        float finalMeanWeight = recordFinalState();
        
        // Calculate weight change
        float weightChange = finalMeanWeight - initialMeanWeight;
        
        if (std::abs(weightChange) > 0.001f || (strengthened > 0 || weakened > 0)) {
            result.passed = true;
            result.message = "Learning detected through synaptic changes";
            result.details = "Weight change: " + std::to_string(weightChange) + ", " +
                          "Strengthened: " + std::to_string(strengthened) + ", " +
                          "Weakened: " + std::to_string(weakened);
        } else {
            result.passed = false;
            result.message = "No significant learning detected";
            result.details = "Weight change too small: " + std::to_string(weightChange);
        }
        
        return result;
    }
    
    bool setup(std::shared_ptr<Brain> b) {
        brain = b;
        return brain != nullptr;
    }
    
private:
    std::shared_ptr<Brain> brain;
    std::vector<float> initialWeights;
    std::vector<float> finalWeights;
    size_t strengthened = 0;
    size_t weakened = 0;
    
    float recordInitialState() {
        initialWeights.clear();
        
        auto* region = brain->getRegion(RegionId(1));
        if (region) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights.push_back(syn->getWeight());
            }
        }
        
        if (initialWeights.empty()) return 0.0f;
        
        float sum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
        return sum / initialWeights.size();
    }
    
    float recordFinalState() {
        finalWeights.clear();
        
        auto* region = brain->getRegion(RegionId(1));
        if (region) {
            for (const auto& syn : region->getSynapses()) {
                finalWeights.push_back(syn->getWeight());
            }
        }
        
        if (finalWeights.empty()) return 0.0f;
        
        float sum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
        return sum / finalWeights.size();
    }
    
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
        auto* region = brain->getRegion(RegionId(1));
        if (region) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(hebbian, stdp, rewardModulated);
            }
        }
    }
    
    void applyInputPattern(int steps) {
        std::vector<NeuronId> patternNeurons;
        auto* region = brain->getRegion(RegionId(1));
        
        if (region) {
            for (auto* neuron : region->getAllNeurons()) {
                if (neuron->getType() == NeuronType::Sensory && patternNeurons.size() < 20) {
                    patternNeurons.push_back(neuron->getId());
                }
            }
        }
        
        for (int step = 0; step < steps; ++step) {
            for (NeuronId id : patternNeurons) {
                brain->injectCurrent(id, 30.0f);
            }
            brain->step(step, step * 0.001);
        }
        
        // Count synaptic changes
        size_t minSize = std::min(initialWeights.size(), finalWeights.size());
        for (size_t i = 0; i < minSize; ++i) {
            float delta = finalWeights[i] - initialWeights[i];
            if (delta > 0.01f) ++strengthened;
            else if (delta < -0.01f) ++weakened;
        }
    }
};

// STDP verification test
class StdpVerificationTest : public Test {
public:
    std::string getName() const override { return "STDP Verification Test"; }
    
    TestResult run() override {
        TestResult result;
        result.name = getName();
        result.executionTime = 0.0f;
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region || region->getSynapses().empty()) {
            result.passed = false;
            result.message = "No synapses available for STDP test";
            return result;
        }
        
        // Use first synapse for testing
        Synapse* syn = region->getSynapses()[0].get();
        syn->enablePlasticity(false, true, false);  // STDP only
        float beforeWeight = syn->getWeight();
        
        // Create correlated pre->post activity
        float totalDelta = runStdpTrials(syn, beforeWeight);
        
        if (totalDelta > 0.001f) {
            result.passed = true;
            result.message = "STDP potentiation working correctly";
            result.details = "Weight increased by " + std::to_string(totalDelta);
        } else if (totalDelta < -0.001f) {
            result.passed = true;
            result.message = "STDP depression working correctly";
            result.details = "Weight decreased by " + std::to_string(-totalDelta);
        } else {
            result.passed = false;
            result.message = "STDP not triggering (no significant change)";
            result.details = "Weight change: " + std::to_string(totalDelta);
        }
        
        return result;
    }
    
    bool setup(std::shared_ptr<Brain> b) {
        brain = b;
        return brain != nullptr;
    }
    
private:
    std::shared_ptr<Brain> brain;
    
    float runStdpTrials(Synapse* syn, float beforeWeight) {
        float currentWeight = beforeWeight;
        
        for (int trial = 0; trial < 50; ++trial) {
            brain->injectCurrent(syn->getSourceNeuron(), 60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            brain->injectCurrent(syn->getDestinationNeuron(), 60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
            
            currentWeight = syn->getWeight();
        }
        
        return currentWeight - beforeWeight;
    }
};

// Test runner class
class TestRunner {
public:
    TestRunner(std::shared_ptr<Config> config) : config(config) {}
    
    void addTest(std::unique_ptr<Test> test) {
        tests.push_back(std::move(test));
    }
    
    // Accessor for tests (needed for demo)
    const std::vector<std::unique_ptr<Test>>& getTests() const {
        return tests;
    }
    
    std::vector<TestResult> runAllTests() {
        std::vector<TestResult> results;
        
        // Setup all tests
        for (auto& test : tests) {
            if (!test->setup()) {
                TestResult result;
                result.name = test->getName();
                result.passed = false;
                result.message = "Test setup failed";
                results.push_back(result);
            }
        }
        
        // Run all tests
        for (auto& test : tests) {
            results.push_back(test->run());
            test->cleanup();
        }
        
        return results;
    }
    
    void printResults(const std::vector<TestResult>& results) {
        int passed = 0;
        int failed = 0;
        
        std::cout << "\n" << std::string(80, '=') << "\n";
        std::cout << "NLM TEST RESULTS" << std::string(80 - 16, '=') << "\n\n";
        
        for (const auto& result : results) {
            std::cout << "Test: " << result.name << "\n";
            std::cout << "Status: " << (result.passed ? "✓ PASSED" : "✗ FAILED") << "\n";
            std::cout << "Message: " << result.message << "\n";
            if (!result.details.empty()) {
                std::cout << "Details: " << result.details << "\n";
            }
            std::cout << std::string(80, '-') << "\n\n";
            
            if (result.passed) passed++;
            else failed++;
        }
        
        std::cout << "SUMMARY: " << passed << " passed, " << failed << " failed\n";
        std::cout << std::string(80, '=') << "\n\n";
        
        if (failed > 0) {
            std::cout << "FAILED TESTS: ";
            for (const auto& result : results) {
                if (!result.passed) {
                    std::cout << result.name << " ";
                }
            }
            std::cout << "\n";
        }
    }
    
private:
    std::shared_ptr<Config> config;
    std::vector<std::unique_ptr<Test>> tests;
};

} // namespace nlm
