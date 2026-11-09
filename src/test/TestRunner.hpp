// Test runner framework for NLM neural network simulation
// This module provides a structured approach to running and managing neural network experiments

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <mutex>
#include <thread>
#include "brain/Brain.hpp"

namespace nlm {

// Test execution statistics
struct TestStats {
    std::string name;
    double executionTimeMs = 0.0;
    bool passed = false;
    std::string details;
    size_t neuronsCreated = 0;
    size_t totalSpikes = 0;
    float weightChange = 0.0f;
    std::vector<std::string> logMessages;
};

// Enhanced configuration for test execution
struct TestConfig {
    // Test parameters
    bool enableConnectivityTest = true;
    bool enablePlasticityTest = true;
    bool enableStdpTest = true;
    
    // Test values (configurable via command line or config file)
    float connectivityTestCurrent = 50.0f;
    float plasticityTestCurrent = 30.0f;
    uint32_t plasticityTestSteps = 1000;
    float stdpTestCurrent = 60.0f;
    uint32_t stdpTestTrials = 50;
    
    // Test thresholds
    size_t minSpikesForConnectivity = 1;
    float minWeightChangeForPlasticity = 0.001f;
    float minWeightChangeForStdp = 0.001f;
    
    // Simulation parameters
    uint64_t seed = 42;
    uint64_t neuronCount = 500;
    uint64_t regionCount = 1;
    float connectionProbability = 0.15f;
    
    // Performance parameters
    bool verboseLogging = true;
    bool profilePerformance = true;
    bool enableResultValidation = true;
};

// Test result validation
struct ValidationResult {
    bool isValid = true;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
};

class TestRunner {
public:
    explicit TestRunner(const TestConfig& config) : config_(config) {}
    
    // Run all tests
    std::vector<TestStats> runAllTests(std::shared_ptr<Brain> brain) {
        std::vector<TestStats> results;
        
        // Enable plasticity for plasticity and STDP tests
        enablePlasticity(brain);
        
        if (config_.enableConnectivityTest) {
            results.push_back(runConnectivityTest(brain));
        }
        
        // Reset brain for next test
        resetBrainForNextTest(brain);
        
        if (config_.enablePlasticityTest) {
            results.push_back(runPlasticityTest(brain));
        }
        
        resetBrainForNextTest(brain);
        
        if (config_.enableStdpTest) {
            results.push_back(runStdpTest(brain));
        }
        
        return results;
    }
    
    // Run single test
    TestStats runTest(std::shared_ptr<Brain> brain, const std::string& testName) {
        if (testName == "connectivity") {
            return runConnectivityTest(brain);
        } else if (testName == "plasticity") {
            return runPlasticityTest(brain);
        } else if (testName == "stdp") {
            return runStdpTest(brain);
        } else {
            return createFailedTest(testName, "Unknown test name");
        }
    }
    
    // Validate test results
    ValidationResult validateResults(const std::vector<TestStats>& results) {
        ValidationResult validation;
        
        for (const auto& result : results) {
            if (!result.passed) {
                validation.errors.push_back(f"Test '{result.name}' failed: {result.details}");
            }
        }
        
        return validation;
    }
    
    // Print formatted test results
    void printTestResults(const std::vector<TestStats>& results) {
        std::cout << std::endl;
        std::cout << "=== NLM Test Suite Results ===" << std::endl;
        std::cout << std::endl;
        
        int passedCount = 0;
        int failedCount = 0;
        
        for (const auto& result : results) {
            std::cout << f"Test: {result.name}" << std::endl;
            std::cout << f"  Status: {result.passed ? "PASSED" : "FAILED"}" << std::endl;
            std::cout << f"  Time: {result.executionTimeMs:.2f} ms" << std::endl;
            if (config_.profilePerformance) {
                std::cout << f"  Neurons: {result.neuronsCreated}" << std::endl;
                std::cout << f"  Total Spikes: {result.totalSpikes}" << std::endl;
                std::cout << f"  Weight Change: {result.weightChange:.6f}" << std::endl;
            }
            std::cout << f"  Details: {result.details}" << std::endl;
            
            if (result.passed) {
                passedCount++;
            } else {
                failedCount++;
            }
            
            if (!result.logMessages.empty() && config_.verboseLogging) {
                std::cout << "  Log messages:" << std::endl;
                for (const auto& log : result.logMessages) {
                    std::cout << f"    {log}" << std::endl;
                }
            }
            
            std::cout << std::endl;
        }
        
        std::cout << f"=== Summary ===" << std::endl;
        std::cout << f"Total tests: {results.size()}" << std::endl;
        std::cout << f"Passed: {passedCount}" << std::endl;
        std::cout << f"Failed: {failedCount}" << std::endl;
        std::cout << std::endl;
        
        if (failedCount > 0) {
            std::cout << "FAILURES:" << std::endl;
            for (const auto& result : results) {
                if (!result.passed) {
                    std::cout << f"  - {result.name}: {result.details}" << std::endl;
                }
            }
        }
    }
    
private:
    TestConfig config_;
    
    // Enable plasticity on all synapses in the brain
    void enablePlasticity(std::shared_ptr<Brain> brain) {
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false); // Enable Hebbian and STDP
            }
        }
    }
    
    // Reset brain for next test run
    void resetBrainForNextTest(std::shared_ptr<Brain> brain) {
        brain->reset();
        brain->initialize();
    }
    
    // Create a test result for a failed test
    TestStats createFailedTest(const std::string& name, const std::string& details) {
        TestStats stats;
        stats.name = name;
        stats.passed = false;
        stats.details = details;
        return stats;
    }
    
    // Enhanced connectivity test with better validation
    TestStats runConnectivityTest(std::shared_ptr<Brain> brain) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        TestStats stats;
        stats.name = "Connectivity Test";
        
        try {
            // Inject current into a few neurons and see if spikes propagate
            auto* region = brain->getRegion(RegionId(1));
            if (!region) {
                stats.passed = false;
                stats.details = "No brain region found";
                return stats;
            }
            
            auto neurons = region->getAllNeurons();
            if (neurons.empty()) {
                stats.passed = false;
                stats.details = "No neurons found";
                return stats;
            }
            
            // Get initial spike count
            size_t initialSpikes = brain->getTotalSpikeCount();
            
            // Inject strong current into first 10 neurons
            for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
                neurons[i]->injectCurrent(config_.connectivityTestCurrent);
            }
            
            // Run a few steps
            for (SimulationStep step = 0; step < 50; ++step) {
                brain->step(step, step * 0.001);
            }
            
            size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
            stats.totalSpikes = spikes;
            
            if (spikes >= config_.minSpikesForConnectivity) {
                stats.passed = true;
                stats.details = "Spikes propagate through network successfully";
                stats.logMessages.push_back(f"Injected current into {std::min(size_t(10), neurons.size())} neurons");
                stats.logMessages.push_back(f"Generated {spikes} spikes");
            } else {
                stats.passed = false;
                stats.details = f"Insufficient spikes: got {spikes}, expected at least {config_.minSpikesForConnectivity}";
                stats.logMessages.push_back(f"Only {spikes} spikes generated");
                
                // Log neuron parameters for debugging
                for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
                    stats.logMessages.push_back(f"  Neuron {i}: V={neurons[i]->getMembranePotential()}, thresh={neurons[i]->getThreshold()}");
                }
            }
            
        } catch (const std::exception& e) {
            stats.passed = false;
            stats.details = f"Exception: {e.what()}";
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        stats.executionTimeMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        
        return stats;
    }
    
    // Enhanced plasticity test with more detailed analysis
    TestStats runPlasticityTest(std::shared_ptr<Brain> brain) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        TestStats stats;
        stats.name = "Plasticity Test";
        
        try {
            // Initialize experiment tracking
            auto* region = brain->getRegion(RegionId(1));
            if (!region) {
                stats.passed = false;
                stats.details = "No brain region found";
                return stats;
            }
            
            // Record initial weights
            std::vector<float> initialWeights;
            for (const auto& syn : region->getSynapses()) {
                initialWeights.push_back(syn->getWeight());
            }
            stats.neuronsCreated = initialWeights.size();
            
            if (initialWeights.empty()) {
                stats.passed = false;
                stats.details = "No synapses found for plasticity test";
                return stats;
            }
            
            // Calculate initial mean weight
            float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
            float initialMean = initialSum / initialWeights.size();
            
            // Apply repeated input pattern to stimulate learning
            std::vector<NeuronId> patternNeurons;
            for (auto* neuron : region->getAllNeurons()) {
                if (neuron->getType() == NeuronType::Sensory && patternNeurons.size() < 20) {
                    patternNeurons.push_back(neuron->getId());
                }
            }
            
            stats.logMessages.push_back(f"Stimulating {patternNeurons.size()} sensory neurons");
            
            // Apply repeated input patterns
            for (SimulationStep step = 0; step < config_.plasticityTestSteps; ++step) {
                for (NeuronId id : patternNeurons) {
                    brain->injectCurrent(id, config_.plasticityTestCurrent);
                }
                
                brain->step(step, step * 0.001);
                
                // Log progress every 200 steps
                if (config_.verboseLogging && step % 200 == 0) {
                    stats.logMessages.push_back(f"  Step {step} | Spikes: {brain->getTotalSpikeCount()} | Firing: {brain->getFiringNeuronCount()}");
                }
            }
            
            // Record final weights
            std::vector<float> finalWeights;
            for (const auto& syn : region->getSynapses()) {
                finalWeights.push_back(syn->getWeight());
            }
            
            stats.totalSpikes = brain->getTotalSpikeCount();
            
            // Calculate weight change
            if (!finalWeights.empty()) {
                float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
                float finalMean = finalSum / finalWeights.size();
                stats.weightChange = finalMean - initialMean;
            }
            
            // Check for learning
            bool learningOccurred = (std::abs(stats.weightChange) > config_.minWeightChangeForPlasticity);
            
            if (learningOccurred) {
                stats.passed = true;
                stats.details = f"Learning detected: weight changed from {initialMean:.4f} to {finalMean:.4f} (Δ={stats.weightChange:.4f})";
                stats.logMessages.push_back(f"Initial mean weight: {initialMean:.4f}");
                stats.logMessages.push_back(f"Final mean weight: {finalMean:.4f}");
            } else {
                stats.passed = false;
                stats.details = f"No significant learning: weight change {stats.weightChange:.4f} below threshold {config_.minWeightChangeForPlasticity:.4f}";
                stats.logMessages.push_back(f"Learning threshold not met: Δ={stats.weightChange:.4f} < {config_.minWeightChangeForPlasticity:.4f}");
            }
            
        } catch (const std::exception& e) {
            stats.passed = false;
            stats.details = f"Exception: {e.what()}";
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        stats.executionTimeMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        
        return stats;
    }
    
    // Enhanced STDP test with precise correlation
    TestStats runStdpTest(std::shared_ptr<Brain> brain) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        TestStats stats;
        stats.name = "STDP Test";
        
        try {
            auto* region = brain->getRegion(RegionId(1));
            if (!region) {
                stats.passed = false;
                stats.details = "No brain region found";
                return stats;
            }
            
            auto& synapses = region->getSynapses();
            if (synapses.empty()) {
                stats.passed = false;
                stats.details = "No synapses for STDP test";
                return stats;
            }
            
            // Use one real synapse and its own pre/post neurons
            Synapse* syn = synapses[0].get();
            syn->enablePlasticity(false, true, false); // STDP only
            float beforeWeight = syn->getWeight();
            
            stats.logMessages.push_back(f"Testing STDP on synapse (pre={syn->getSourceNeuron().index()}, post={syn->getDestinationNeuron().index()})");
            stats.logMessages.push_back(f"  Weight before: {beforeWeight:.4f}");
            
            // Create correlated pre->post activity (potentiation)
            for (int trial = 0; trial < config_.stdpTestTrials; ++trial) {
                brain->injectCurrent(syn->getSourceNeuron(), config_.stdpTestCurrent);
                brain->step(trial * 2, trial * 2 * 0.001);
                brain->injectCurrent(syn->getDestinationNeuron(), config_.stdpTestCurrent);
                brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
            }
            
            float delta = syn->getWeight() - beforeWeight;
            stats.weightChange = delta;
            
            stats.logMessages.push_back(f"  Weight after: {syn->getWeight():.4f}");
            stats.logMessages.push_back(f"  Δ: {delta:.4f}");
            
            if (std::abs(delta) > config_.minWeightChangeForStdp) {
                if (delta > 0.0f) {
                    stats.passed = true;
                    stats.details = "STDP working: Pre-before-post produced potentiation";
                } else {
                    stats.passed = false;
                    stats.details = "STDP reversed: Check parameters";
                }
            } else {
                stats.passed = false;
                stats.details = "No significant change: STDP may not be triggering";
            }
            
        } catch (const std::exception& e) {
            stats.passed = false;
            stats.details = f"Exception: {e.what()}";
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        stats.executionTimeMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        
        return stats;
    }
};