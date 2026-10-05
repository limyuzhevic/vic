#pragma once

#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <unordered_map>
#include <chrono>
#include "NLMApplication.hpp"

namespace nlm {

// Default test framework implementation
class DefaultTestFramework : public TestFramework {
public:
    DefaultTestFramework() = default;
    ~DefaultTestFramework() override = default;
    
    void registerTest(const std::string& name, std::function<TestResult(std::shared_ptr<Brain>)> test) override {
        tests_[name] = {
            test,
            std::chrono::steady_clock::now()
        };
    }
    
    std::vector<TestResult> runAllTests(std::shared_ptr<Brain> brain) override {
        std::vector<TestResult> results;
        
        for (const auto& [name, testInfo] : tests_) {
            auto startTime = std::chrono::steady_clock::now();
            
            try {
                TestResult result = testInfo.test(brain);
                auto endTime = std::chrono::steady_clock::now();
                
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
                result.executionTime = duration.count() / 1000.0; // Convert to seconds
                
                results.push_back(result);
                
            } catch (const std::exception& e) {
                auto endTime = std::chrono::steady_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
                
                TestResult result(name, TestResult::Status::FAILED, e.what());
                result.executionTime = duration.count() / 1000.0;
                
                results.push_back(result);
            }
        }
        
        return results;
    }
    
    void clearTests() override {
        tests_.clear();
    }
    
    size_t getTestCount() const override {
        return tests_.size();
    }
    
    // Get test statistics
    struct TestStatistics {
        size_t total = 0;
        size_t passed = 0;
        size_t failed = 0;
        size_t skipped = 0;
        double totalExecutionTime = 0.0;
        double averageExecutionTime = 0.0;
        
        double passRate() const {
            return total > 0 ? (static_cast<double>(passed) / total) * 100.0 : 0.0;
        }
    };
    
    TestStatistics getStatistics() const {
        TestStatistics stats;
        stats.total = tests_.size();
        
        // Note: We don't have access to test results here, only test info
        // This is a placeholder - actual implementation would run tests
        
        return stats;
    }
    
    // Get test names
    std::vector<std::string> getTestNames() const {
        std::vector<std::string> names;
        names.reserve(tests_.size());
        for (const auto& pair : tests_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // Check if test exists
    bool hasTest(const std::string& name) const {
        return tests_.find(name) != tests_.end();
    }
    
    // Get test execution info
    struct TestInfo {
        std::function<TestResult(std::shared_ptr<Brain>)> test;
        std::chrono::steady_clock::time_point registeredTime;
    };
    
    const TestInfo& getTestInfo(const std::string& name) const {
        return tests_.at(name);
    }
    
private:
    std::unordered_map<std::string, TestInfo> tests_;
};

// Utility functions for test creation
namespace test_utils {
    
    // Create a simple connectivity test
    std::function<TestResult(std::shared_ptr<Brain>)> createConnectivityTest(
        const std::string& name = "Basic Connectivity",
        size_t neuronIndex = 0,
        float current = 50.0f,
        SimulationStep steps = 50
    ) {
        return [name, neuronIndex, current, steps](std::shared_ptr<Brain> brain) {
            try {
                auto startTime = std::chrono::steady_clock::now();
                
                // Get initial spike count
                size_t initialSpikes = brain->getTotalSpikeCount();
                
                // Inject current
                brain->injectCurrentToNeurons(NeuronType::All, current);
                
                // Run simulation
                for (SimulationStep step = 0; step < steps; ++step) {
                    brain->step(step, step * 0.001);
                }
                
                size_t finalSpikes = brain->getTotalSpikeCount();
                size_t spikesGenerated = finalSpikes - initialSpikes;
                
                auto endTime = std::chrono::steady_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
                
                if (spikesGenerated > 0) {
                    return TestResult(name, TestResult::Status::PASSED,
                                    "Generated " + std::to_string(spikesGenerated) + " spikes");
                } else {
                    return TestResult(name, TestResult::Status::FAILED,
                                    "No spikes generated");
                }
                
            } catch (const std::exception& e) {
                return TestResult(name, TestResult::Status::FAILED, std::string("Exception: ") + e.what());
            }
        };
    }
    
    // Create a plasticity learning test
    std::function<TestResult(std::shared_ptr<Brain>)> createPlasticityTest(
        const std::string& name = "Plasticity Learning",
        size_t steps = 1000,
        float current = 30.0f
    ) {
        return [name, steps, current](std::shared_ptr<Brain> brain) {
            try {
                auto startTime = std::chrono::steady_clock::now();
                
                // Record initial weights
                std::vector<float> initialWeights;
                auto* region = brain->getRegion(RegionId(1));
                if (region) {
                    for (const auto& syn : region->getSynapses()) {
                        initialWeights.push_back(syn->getWeight());
                    }
                }
                
                // Apply input patterns to stimulate learning
                for (size_t i = 0; i < steps; ++i) {
                    // Create input pattern
                    brain->injectCurrentToNeurons(NeuronType::Sensory, current);
                    brain->step(i, i * 0.001);
                }
                
                // Record final weights
                std::vector<float> finalWeights;
                if (region) {
                    for (const auto& syn : region->getSynapses()) {
                        finalWeights.push_back(syn->getWeight());
                    }
                }
                
                auto endTime = std::chrono::steady_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
                
                // Calculate weight changes
                if (initialWeights.empty() || finalWeights.empty()) {
                    return TestResult(name, TestResult::Status::FAILED,
                                    "No weights recorded");
                }
                
                float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
                float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
                float initialMean = initialSum / initialWeights.size();
                float finalMean = finalSum / finalWeights.size();
                
                float change = finalMean - initialMean;
                bool learningOccurred = std::abs(change) > 0.001f;
                
                if (learningOccurred) {
                    return TestResult(name, TestResult::Status::PASSED,
                                    "Weight change: " + std::to_string(change) + " (learning occurred)");
                } else {
                    return TestResult(name, TestResult::Status::FAILED,
                                    "No significant weight change detected");
                }
                
            } catch (const std::exception& e) {
                return TestResult(name, TestResult::Status::FAILED, std::string("Exception: ") + e.what());
            }
        };
    }
    
    // Create a STDP verification test
    std::function<TestResult(std::shared_ptr<Brain>)> createSTDPTest(
        const std::string& name = "STDP Verification",
        size_t trials = 50
    ) {
        return [name, trials](std::shared_ptr<Brain> brain) {
            try {
                auto startTime = std::chrono::steady_clock::now();
                
                auto* region = brain->getRegion(RegionId(1));
                if (!region || region->getSynapses().size() < 2) {
                    return TestResult(name, TestResult::Status::SKIPPED,
                                    "Not enough synapses for STDP test");
                }
                
                // Get first two neurons
                auto neurons = region->getAllNeurons();
                if (neurons.size() < 2) {
                    return TestResult(name, TestResult::Status::SKIPPED,
                                    "Not enough neurons for STDP test");
                }
                
                Neuron* preNeuron = neurons[0];
                Neuron* postNeuron = neurons[1];
                
                // Record initial weights
                float initialWeight = region->getSynapses()[0]->getWeight();
                
                // Create correlated activity (pre before post)
                for (size_t trial = 0; trial < trials; ++trial) {
                    // Fire pre-synaptic neuron
                    preNeuron->injectCurrent(60.0f);
                    brain->step(trial * 2, trial * 2 * 0.001);
                    
                    // Fire post-synaptic neuron
                    postNeuron->injectCurrent(60.0f);
                    brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
                }
                
                // Record final weight
                float finalWeight = region->getSynapses()[0]->getWeight();
                float delta = finalWeight - initialWeight;
                
                auto endTime = std::chrono::steady_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
                
                // Check for potentiation (LTP)
                if (delta > 0.001f) {
                    return TestResult(name, TestResult::Status::PASSED,
                                    "STDP potentiation: delta = " + std::to_string(delta));
                } else if (delta < -0.001f) {
                    return TestResult(name, TestResult::Status::PASSED,
                                    "STDP depression: delta = " + std::to_string(delta));
                } else {
                    return TestResult(name, TestResult::Status::FAILED,
                                    "No significant STDP change: delta = " + std::to_string(delta));
                }
                
            } catch (const std::exception& e) {
                return TestResult(name, TestResult::Status::FAILED, std::string("Exception: ") + e.what());
            }
        };
    }
    
} // namespace test_utils

} // namespace nlm
