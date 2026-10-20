// Test utilities for NLM Phase 2
// This module provides reusable test infrastructure and utilities

#pragma once

#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <vector>

namespace nlm {
namespace test {

// Test configuration constants
struct TestConfig {
    static constexpr int32_t DEFAULT_RANDOM_SEED = 42;
    static constexpr double DEFAULT_TIMESTEP = 0.001;
    static constexpr int64_t DEFAULT_NEURON_COUNT = 500;
    static constexpr int64_t DEFAULT_REGION_COUNT = 1;
    static constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.15f;
    
    // STDP parameters
    static constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.02f;
    static constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.015f;
    static constexpr float DEFAULT_STDP_TAU = 20.0f;
    
    // Structural plasticity parameters
    static constexpr float DEFAULT_SYNAPTOGENESIS_RATE = 0.0001f;
    static constexpr float DEFAULT_PRUNING_RATE = 0.00001f;
    
    // Experiment parameters
    static constexpr int DEFAULT_CONNECTIVITY_TEST_STEPS = 50;
    static constexpr int DEFAULT_PLASTICITY_TEST_STEPS = 1000;
    static constexpr int DEFAULT_STDP_TEST_TRIALS = 50;
};

// Test utilities
class TestUtils {
public:
    // Get all weights from a specific region
    static std::vector<float> getAllWeightsFromRegion(std::shared_ptr<Brain> brain, RegionId regionId) {
        std::vector<float> weights;
        if (auto* region = brain->getRegion(regionId)) {
            for (const auto& syn : region->getSynapses()) {
                weights.push_back(syn->getWeight());
            }
        }
        return weights;
    }
    
    // Run basic connectivity test
    static bool runConnectivityTest(std::shared_ptr<Brain> brain, int steps = TestConfig::DEFAULT_CONNECTIVITY_TEST_STEPS) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test: Basic Neural Connectivity ===");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            NLM_LOG_INFO("  No region found!");
            return false;
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            NLM_LOG_INFO("  No neurons found!");
            return false;
        }
        
        // Get initial spike count
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject strong current into first 10 neurons
        NLM_LOG_INFO("  Injecting current into 10 neurons...");
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
        }
        
        // Run a few steps
        for (SimulationStep step = 0; step < steps; ++step) {
            brain->step(step, step * TestConfig::DEFAULT_TIMESTEP);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
        
        if (spikes > 0) {
            NLM_LOG_INFO("  ✓ Spikes propagate through network");
            return true;
        } else {
            NLM_LOG_INFO("  ! No spikes - checking neuron parameters...");
            for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
                NLM_LOG_INFO("    Neuron " + std::to_string(i) + 
                            " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                            " thresh=" + std::to_string(neurons[i]->getThreshold()));
            }
            return false;
        }
    }
    
    // Run plasticity test
    static bool runPlasticityTest(std::shared_ptr<Brain> brain, int steps = TestConfig::DEFAULT_PLASTICITY_TEST_STEPS) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test: Plasticity Learning Experiment ===");
        
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
        NLM_LOG_INFO("Applying repeated input patterns (" + std::to_string(steps) + " steps)...");
        
        for (SimulationStep step = 0; step < steps; ++step) {
            // Create input pattern - inject current into sensory neurons
            for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            brain->step(step, step * TestConfig::DEFAULT_TIMESTEP);
            
            // Log progress every 100 steps
            if (step % 100 == 0) {
                NLM_LOG_INFO("  Step " + std::to_string(step) + 
                            " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                            " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
            }
        }
        
        // Record final state
        experiment.recordFinalState();
        
        // Compute and display statistics
        experiment.computeStatistics();
        return true;
    }
    
    // Run STDP test
    static bool runStdpTest(std::shared_ptr<Brain> brain, int trials = TestConfig::DEFAULT_STDP_TEST_TRIALS) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test: STDP Verification ===");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return false;
        
        // Get first few synapses
        auto& synapses = region->getSynapses();
        if (synapses.size() < 5) {
            NLM_LOG_INFO("  Not enough synapses for STDP test");
            return false;
        }
        
        NLM_LOG_INFO("  Testing STDP on 5 synapses:");
        
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
        
        for (int trial = 0; trial < trials; ++trial) {
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
                preNeuron->injectCurrent(60.0f);
                brain->step(trial * 2, trial * 2 * TestConfig::DEFAULT_TIMESTEP);
                
                // Then post fires
                postNeuron->injectCurrent(60.0f);
                brain->step(trial * 2 + 1, (trial * 2 + 1) * TestConfig::DEFAULT_TIMESTEP);
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
        if (totalDelta > 0.001f) {
            NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
            return true;
        } else if (totalDelta < -0.001f) {
            NLM_LOG_INFO("  ! STDP reversed: Check parameters");
            return false;
        } else {
            NLM_LOG_INFO("  ! No change: STDP may not be triggering");
            return false;
        }
    }
};

} // namespace test
} // namespace nlm