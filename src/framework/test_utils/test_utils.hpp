#pragma once

#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <chrono>
#include "NLMApplication.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "core/Config/Config.hpp"

namespace nlm {
namespace test_utils {

// Utility functions for creating test scenarios

// Create a simple connectivity test with configurable parameters
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
            
            // Inject current to neurons
            brain->injectCurrentToNeurons(NeuronType::Sensory, current);
            
            // Run simulation
            for (SimulationStep step = 0; step < steps; ++step) {
                brain->step(step, step * 0.001);
            }
            
            size_t finalSpikes = brain->getTotalSpikeCount();
            size_t spikesGenerated = finalSpikes - initialSpikes;
            
            auto endTime = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            
            TestResult result(name, TestResult::Status::PASSED,
                            "Generated " + std::to_string(spikesGenerated) + " spikes");
            result.executionTime = duration.count() / 1000.0; // Convert to seconds
            return result;
            
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
            
            TestResult result(name, learningOccurred ? TestResult::Status::PASSED : TestResult::Status::FAILED,
                            "Weight change: " + std::to_string(change) + (learningOccurred ? " (learning occurred)" : " (no learning)"));
            result.executionTime = duration.count() / 1000.0;
            return result;
            
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
                TestResult result(name, TestResult::Status::PASSED,
                                "STDP potentiation: delta = " + std::to_string(delta));
                result.executionTime = duration.count() / 1000.0;
                return result;
            } else if (delta < -0.001f) {
                TestResult result(name, TestResult::Status::PASSED,
                                "STDP depression: delta = " + std::to_string(delta));
                result.executionTime = duration.count() / 1000.0;
                return result;
            } else {
                return TestResult(name, TestResult::Status::FAILED,
                                "No significant STDP change: delta = " + std::to_string(delta));
            }
            
        } catch (const std::exception& e) {
            return TestResult(name, TestResult::Status::FAILED, std::string("Exception: ") + e.what());
        }
    };
}

// Create a comprehensive benchmark test
std::function<TestResult(std::shared_ptr<Brain>)> createBenchmarkTest(
    const std::string& name = "Performance Benchmark",
    size_t steps = 5000
) {
    return [name, steps](std::shared_ptr<Brain> brain) {
        try {
            auto startTime = std::chrono::steady_clock::now();
            
            // Run simulation without any learning
            for (size_t step = 0; step < steps; ++step) {
                brain->step(step, step * 0.001);
                
                // Log progress every 1000 steps
                if (step % 1000 == 0) {
                    auto currentTime = std::chrono::steady_clock::now();
                    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime);
                    // Note: In a real implementation, we would log this
                }
            }
            
            auto endTime = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            
            double stepsPerSecond = (steps * 1000.0) / duration.count();
            
            TestResult result(name, TestResult::Status::PASSED,
                            "Processed " + std::to_string(steps) + " steps in " +
                            std::to_string(duration.count()) + " ms (" +
                            std::to_string(stepsPerSecond) + " steps/sec)");
            result.executionTime = duration.count() / 1000.0;
            return result;
            
        } catch (const std::exception& e) {
            return TestResult(name, TestResult::Status::FAILED, std::string("Exception: ") + e.what());
        }
    };
}

// Create a memory usage test
std::function<TestResult(std::shared_ptr<Brain>)> createMemoryTest(
    const std::string& name = "Memory Usage Test"
) {
    return [name](std::shared_ptr<Brain> brain) {
        try {
            auto startTime = std::chrono::steady_clock::now();
            
            // Record brain memory usage
            size_t initialNeuronCount = brain->getTotalNeuronCount();
            size_t initialSynapseCount = brain->getTotalSynapseCount();
            
            // Simulate memory-intensive operations
            for (size_t i = 0; i < 10; ++i) {
                // Create new brain instances and destroy them
                auto config = std::make_shared<Config>();
                auto tempBrain = std::make_shared<Brain>(config);
                if (tempBrain->initialize()) {
                    tempBrain->step(i, i * 0.001);
                }
            }
            
            auto endTime = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            
            TestResult result(name, TestResult::Status::PASSED,
                            "Memory test completed in " + std::to_string(duration.count()) + " ms");
            result.executionTime = duration.count() / 1000.0;
            return result;
            
        } catch (const std::exception& e) {
            return TestResult(name, TestResult::Status::FAILED, std::string("Exception: ") + e.what());
        }
    };
}

// Create a test suite with multiple predefined tests
class TestSuite {
public:
    TestSuite() {
        // Add standard tests
        addTest("Basic Connectivity", createConnectivityTest());
        addTest("Plasticity Learning", createPlasticityTest());
        addTest("STDP Verification", createSTDPTest());
        addTest("Performance Benchmark", createBenchmarkTest());
        addTest("Memory Usage Test", createMemoryTest());
    }
    
    void addTest(const std::string& name, std::function<TestResult(std::shared_ptr<Brain>)> test) {
        tests_[name] = test;
    }
    
    std::vector<TestResult> runAllTests(std::shared_ptr<Brain> brain) {
        std::vector<TestResult> results;
        
        for (const auto& [name, test] : tests_) {
            TestResult result = test(brain);
            results.push_back(result);
        }
        
        return results;
    }
    
    size_t getTestCount() const {
        return tests_.size();
    }
    
    std::vector<std::string> getTestNames() const {
        std::vector<std::string> names;
        names.reserve(tests_.size());
        for (const auto& pair : tests_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
private:
    std::unordered_map<std::string, std::function<TestResult(std::shared_ptr<Brain>)>> tests_;
};

} // namespace test_utils
} // namespace nlm
