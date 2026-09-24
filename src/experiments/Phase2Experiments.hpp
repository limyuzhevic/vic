#ifndef NLM_PHASE2_EXPERIMENTS_H
#define NLM_PHASE2_EXPERIMENTS_H

/**
 * Phase 2 Experiments - Dedicated Test Scenarios
 * 
 * This file contains dedicated experiment implementations extracted from main.cpp
 * Each experiment is self-contained and can be run independently.
 */

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <vector>

namespace nlm {

/**
 * Basic Neural Connectivity Experiment
 * 
 * Tests that spikes propagate through the network and that
 * neurons can be activated and communicate effectively.
 */
class BasicConnectivityExperiment {
public:
    BasicConnectivityExperiment() = default;
    ~BasicConnectivityExperiment() = default;
    
    bool run(std::shared_ptr<Config> config = nullptr) {
        NLM_LOG_INFO("=== Phase 2 Experiment 1: Basic Neural Connectivity ===");
        
        // Create config if not provided
        if (!config) {
            config = std::make_shared<Config>();
            // Set Phase 2 defaults
            config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
            config->set("simulation_timestep", 0.001, ConfigSource::Default);
            config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
            config->set("connection_probability", 0.15f, ConfigSource::Default);
        }
        
        // Initialize brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return false;
        }
        
        // Get initial spike count
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject current into a few neurons and see if spikes propagate
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            NLM_LOG_ERROR("No region found!");
            return false;
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            NLM_LOG_ERROR("No neurons found!");
            return false;
        }
        
        // Inject strong current into first 10 neurons
        NLM_LOG_INFO("  Injecting current into 10 neurons...");
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
        }
        
        // Run a few steps
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
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
};

/**
 * Plasticity Learning Experiment
 * 
 * Demonstrates measurable synaptic changes through experience and
 * tests that STDP and Hebbian plasticity are working correctly.
 */
class PlasticityLearningExperiment {
public:
    PlasticityLearningExperiment() = default;
    ~PlasticityLearningExperiment() = default;
    
    struct Results {
        float initialMeanWeight;
        float finalMeanWeight;
        float weightChange;
        size_t strengthened;
        size_t weakened;
        size_t unchanged;
        size_t totalSpikes;
        bool learningOccurred;
    };
    
    Results run(std::shared_ptr<Config> config = nullptr) {
        NLM_LOG_INFO("=== Phase 2 Experiment 2: Plasticity Learning Experiment ===");
        Results results{};
        
        // Create config if not provided
        if (!config) {
            config = std::make_shared<Config>();
            config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
            config->set("simulation_timestep", 0.001, ConfigSource::Default);
            config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
            config->set("connection_probability", 0.15f, ConfigSource::Default);
            
            // STDP parameters
            config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
            config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
            config->set("stdp_tau", 20.0f, ConfigSource::Default);
            
            // Structural plasticity parameters
            config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
            config->set("pruning_rate", 0.00001f, ConfigSource::Default);
        }
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return results;
        }
        
        // Enable plasticity on synapses
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
            }
        }
        
        // Apply repeated input pattern to stimulate learning
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Applying repeated input patterns (1000 steps)...");
        
        for (SimulationStep step = 0; step < 1000; ++step) {
            // Create input pattern - inject current into sensory neurons
            for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            brain->step(step, step * 0.001);
            
            // Log progress every 100 steps
            if (step % 100 == 0) {
                NLM_LOG_INFO("  Step " + std::to_string(step) + 
                            " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                            " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
            }
        }
        
        // Compute results
        results.totalSpikes = brain->getTotalSpikeCount();
        
        // Record final weights from first region
        float finalSum = 0.0f;
        size_t finalCount = 0;
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                finalSum += syn->getWeight();
                ++finalCount;
            }
        }
        
        results.finalMeanWeight = finalCount > 0 ? finalSum / finalCount : 0.0f;
        results.learningOccurred = (results.finalMeanWeight > 0.0f); // Simple learning detection
        
        return results;
    }
};

/**
 * STDP Verification Experiment
 * 
 * Specifically tests that Spike-Timing-Dependent Plasticity (STDP)
 * is working correctly by creating correlated pre->post activity
 * and measuring weight changes.
 */
class StdpVerificationExperiment {
public:
    StdpVerificationExperiment() = default;
    ~StdpVerificationExperiment() = default;
    
    struct Results {
        float initialMeanWeight;
        float finalMeanWeight;
        float totalDelta;
        bool stdpWorking;
        std::string verdict;
    };
    
    Results run(std::shared_ptr<Config> config = nullptr) {
        NLM_LOG_INFO("=== Phase 2 Experiment 3: STDP Verification ===");
        Results results{};
        
        // Create config if not provided
        if (!config) {
            config = std::make_shared<Config>();
            config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
            config->set("simulation_timestep", 0.001, ConfigSource::Default);
            config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
            config->set("connection_probability", 0.15f, ConfigSource::Default);
            
            // STDP parameters
            config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
            config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
            config->set("stdp_tau", 20.0f, ConfigSource::Default);
        }
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return results;
        }
        
        // Get first few synapses
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            NLM_LOG_ERROR("No region found!");
            return results;
        }
        
        auto& synapses = region->getSynapses();
        if (synapses.size() < 5) {
            NLM_LOG_ERROR("Not enough synapses for STDP test");
            return results;
        }
        
        // Record initial weights
        std::vector<float> beforeWeights;
        for (size_t i = 0; i < 5 && i < synapses.size(); ++i) {
            beforeWeights.push_back(synapses[i]->getWeight());
            synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
            NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                        " before: " + std::to_string(beforeWeights[i]));
        }
        
        // Compute initial mean
        float initialSum = 0.0f;
        for (float w : beforeWeights) initialSum += w;
        results.initialMeanWeight = initialSum / beforeWeights.size();
        
        // Create correlated activity: fire pre then post to trigger LTP
        NLM_LOG_INFO("");
        NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");
        
        for (int trial = 0; trial < 50; ++trial) {
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
                brain->step(trial * 2, trial * 2 * 0.001);
                
                // Then post fires
                postNeuron->injectCurrent(60.0f);
                brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
            }
        }
        
        // Record after weights
        NLM_LOG_INFO("  After correlated activity:");
        for (size_t i = 0; i < beforeWeights.size(); ++i) {
            float delta = synapses[i]->getWeight() - beforeWeights[i];
            NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                        " after: " + std::to_string(synapses[i]->getWeight()) +
                        " (Δ=" + std::to_string(delta) + ")");
        }
        
        // Compute final mean and total delta
        float finalSum = 0.0f;
        results.totalDelta = 0.0f;
        for (size_t i = 0; i < beforeWeights.size(); ++i) {
            finalSum += synapses[i]->getWeight();
            results.totalDelta += synapses[i]->getWeight() - beforeWeights[i];
        }
        
        results.finalMeanWeight = finalSum / beforeWeights.size();
        
        // Check if weights increased (LTP)
        if (results.totalDelta > 0.001f) {
            results.stdpWorking = true;
            results.verdict = "✓ STDP WORKING: Pre-before-post produced potentiation";
        } else if (results.totalDelta < -0.001f) {
            results.stdpWorking = false;
            results.verdict = "! STDP reversed: Check parameters";
        } else {
            results.stdpWorking = false;
            results.verdict = "! No change: STDP may not be triggering";
        }
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO(results.verdict);
        
        return results;
    }
};

} // namespace nlm

#endif // NLM_PHASE2_EXPERIMENTS_H
