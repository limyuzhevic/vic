#pragma once

#include "ExperimentExecutionBase.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <numeric>

namespace nlm {

/**
 * Basic connectivity test experiment
 */
class ConnectivityExperiment : public ExperimentBaseImpl<ConnectivityExperiment> {
public:
    static std::string getName() { return "ConnectivityTest"; }
    static std::string getDescription() { 
        return "Tests basic neural connectivity and spike propagation through the network";
    }
    
    ConnectivityExperiment() {
        config_.name = getName();
        config_.maxSteps = 50;
    }
    
    ExperimentResult run(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> environment) override {
        auto startTime = std::chrono::high_resolution_clock::now();
        lastResult_.experimentName = getName();
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
        
        if (!brain || !environment) {
            lastResult_.success = false;
            lastResult_.message = "Invalid brain or environment";
            return lastResult_;
        }
        
        // Inject current into a few neurons and see if spikes propagate
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            lastResult_.success = false;
            lastResult_.message = "No neurons found in brain region";
            return lastResult_;
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            lastResult_.success = false;
            lastResult_.message = "No neurons found in region";
            return lastResult_;
        }
        
        // Get initial spike count
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject strong current into first 10 neurons
        NLM_LOG_INFO("  Injecting current into 10 neurons...");
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
        }
        
        // Run a few steps
        for (SimulationStep step = 0; step < config_.maxSteps; ++step) {
            brain->step(step, step * 0.001);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        lastResult_.stepsCompleted = config_.maxSteps;
        
        NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
        
        auto endTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = endTime - startTime;
        lastResult_.executionTime = duration.count();
        
        if (spikes > 0) {
            NLM_LOG_INFO("  ✓ Spikes propagate through network");
            lastResult_.success = true;
            lastResult_.message = "Connectivity test passed - spikes propagate through network";
        } else {
            NLM_LOG_INFO("  ! No spikes - checking neuron parameters...");
            for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
                NLM_LOG_INFO("    Neuron " + std::to_string(i) + 
                            " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                            " thresh=" + std::to_string(neurons[i]->getThreshold()));
            }
            lastResult_.success = false;
            lastResult_.message = "No spikes generated - connectivity may be impaired";
        }
        
        return lastResult_;
    }
};

/**
 * Plasticity learning experiment
 */
class PlasticityLearningExperiment : public ExperimentBaseImpl<PlasticityLearningExperiment> {
public:
    static std::string getName() { return "PlasticityLearning"; }
    static std::string getDescription() { 
        return "Demonstrates measurable synaptic changes through Hebbian and STDP learning";
    }
    
    PlasticityLearningExperiment() {
        config_.name = getName();
        config_.maxSteps = 1000;
    }
    
    ExperimentResult run(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> environment) override {
        auto startTime = std::chrono::high_resolution_clock::now();
        lastResult_.experimentName = getName();
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
        
        if (!brain || !environment) {
            lastResult_.success = false;
            lastResult_.message = "Invalid brain or environment";
            return lastResult_;
        }
        
        // Record initial state
        recordInitialState(brain);
        
        // Enable plasticity on synapses
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
            }
        }
        
        // Apply repeated input pattern to stimulate learning
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Applying repeated input patterns (" + std::to_string(config_.maxSteps) + " steps)...");
        
        for (SimulationStep step = 0; step < config_.maxSteps; ++step) {
            // Create input pattern - inject current into sensory neurons
            for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            brain->step(step, step * 0.001);
            
            // Log progress every 200 steps
            if (step % 200 == 0) {
                NLM_LOG_INFO("  Step " + std::to_string(step) + 
                            " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                            " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
            }
        }
        
        // Record final state and compute statistics
        recordFinalState(brain);
        
        computeStatistics(brain);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = endTime - startTime;
        lastResult_.executionTime = duration.count();
        
        lastResult_.success = checkLearningOccurred();
        lastResult_.message = "Plasticity learning experiment completed - check logs for details";
        lastResult_.stepsCompleted = config_.maxSteps;
        
        return lastResult_;
    }
    
private:
    void recordInitialState(std::shared_ptr<Brain> brain) {
        initialSynapseCount_ = brain->getTotalSynapseCount();
        initialWeights_.clear();
        
        // Record initial weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights_.push_back(syn->getWeight());
            }
        }
        
        NLM_LOG_INFO("Initial state recorded:");
        NLM_LOG_INFO("  Synapses: " + std::to_string(initialSynapseCount_));
        if (!initialWeights_.empty()) {
            float sum = std::accumulate(initialWeights_.begin(), initialWeights_.end(), 0.0f);
            float mean = sum / initialWeights_.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        }
    }
    
    void recordFinalState(std::shared_ptr<Brain> brain) {
        finalWeights_.clear();
        
        // Record final weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                finalWeights_.push_back(syn->getWeight());
            }
        }
        
        mostActiveNeurons_ = brain->getSpikeSystem()->getMostActiveNeurons(10);
        
        NLM_LOG_INFO("Final state recorded:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        if (!finalWeights_.empty()) {
            float sum = std::accumulate(finalWeights_.begin(), finalWeights_.end(), 0.0f);
            float mean = sum / finalWeights_.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        }
    }
    
    void computeStatistics(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Learning Experiment Results ===");
        NLM_LOG_INFO("");
        
        if (initialWeights_.empty() || finalWeights_.empty()) {
            NLM_LOG_INFO("ERROR: No weights recorded");
            return;
        }
        
        // Compute weight changes
        float initialSum = std::accumulate(initialWeights_.begin(), initialWeights_.end(), 0.0f);
        float finalSum = std::accumulate(finalWeights_.begin(), finalWeights_.end(), 0.0f);
        float initialMean = initialSum / initialWeights_.size();
        float finalMean = finalSum / finalWeights_.size();
        
        NLM_LOG_INFO("Weight Statistics:");
        NLM_LOG_INFO("  Initial mean weight: " + std::to_string(initialMean));
        NLM_LOG_INFO("  Final mean weight: " + std::to_string(finalMean));
        NLM_LOG_INFO("  Change: " + std::to_string(finalMean - initialMean));
        
        // Count synapses that changed significantly
        size_t strengthened = 0;
        size_t weakened = 0;
        size_t unchanged = 0;
        
        size_t minSize = std::min(initialWeights_.size(), finalWeights_.size());
        for (size_t i = 0; i < minSize; ++i) {
            float delta = finalWeights_[i] - initialWeights_[i];
            if (delta > 0.01f) ++strengthened;
            else if (delta < -0.01f) ++weakened;
            else ++unchanged;
        }
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Synaptic Changes:");
        NLM_LOG_INFO("  Strengthened: " + std::to_string(strengthened));
        NLM_LOG_INFO("  Weakened: " + std::to_string(weakened));
        NLM_LOG_INFO("  Unchanged: " + std::to_string(unchanged));
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Spike Activity:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        NLM_LOG_INFO("  Most active neurons recorded: " + std::to_string(mostActiveNeurons_.size()));
        
        // Determine if learning occurred
        learningOccurred_ = (std::abs(finalMean - initialMean) > 0.001f) ||
                           (strengthened > 0 || weakened > 0);
        
        NLM_LOG_INFO("");
        if (learningOccurred_) {
            NLM_LOG_INFO("✓ LEARNING DETECTED: Synaptic weights changed through experience");
        } else {
            NLM_LOG_INFO("✗ NO LEARNING: Weights did not change significantly");
        }
    }
    
    bool checkLearningOccurred() {
        return learningOccurred_;
    }
    
    // Member variables for recording state
    size_t initialSynapseCount_ = 0;
    std::vector<float> initialWeights_;
    std::vector<float> finalWeights_;
    std::vector<NeuronId> mostActiveNeurons_;
    bool learningOccurred_ = false;
};

/**
 * STDP verification experiment
 */
class StdpExperiment : public ExperimentBaseImpl<StdpExperiment> {
public:
    static std::string getName() { return "STDPVerification"; }
    static std::string getDescription() { 
        return "Verifies that STDP plasticity rule correctly implements pre-before-post spike timing dependent plasticity";
    }
    
    StdpExperiment() {
        config_.name = getName();
        config_.maxSteps = 100;
    }
    
    ExperimentResult run(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> environment) override {
        auto startTime = std::chrono::high_resolution_clock::now();
        lastResult_.experimentName = getName();
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 3: STDP Verification ===");
        
        if (!brain || !environment) {
            lastResult_.success = false;
            lastResult_.message = "Invalid brain or environment";
            return lastResult_;
        }
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            lastResult_.success = false;
            lastResult_.message = "No brain region found";
            return lastResult_;
        }
        
        auto& synapses = region->getSynapses();
        if (synapses.size() < 5) {
            lastResult_.success = false;
            lastResult_.message = "Not enough synapses for STDP test";
            return lastResult_;
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
        float totalDelta = 0.0f;
        for (size_t i = 0; i < 5; ++i) {
            float delta = synapses[i]->getWeight() - beforeWeights[i];
            totalDelta += delta;
            NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                        " after: " + std::to_string(synapses[i]->getWeight()) +
                        " (Δ=" + std::to_string(delta) + ")");
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = endTime - startTime;
        lastResult_.executionTime = duration.count();
        
        lastResult_.stepsCompleted = 100;
        
        if (totalDelta > 0.001f) {
            NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
            lastResult_.success = true;
            lastResult_.message = "STDP verification passed - pre-before-post spike timing produced LTP";
        } else if (totalDelta < -0.001f) {
            NLM_LOG_INFO("  ! STDP reversed: Check parameters");
            lastResult_.success = false;
            lastResult_.message = "STDP verification failed - unexpected depression";
        } else {
            NLM_LOG_INFO("  ! No change: STDP may not be triggering");
            lastResult_.success = false;
            lastResult_.message = "STDP verification failed - no weight change detected";
        }
        
        return lastResult_;
    }
};

// Register experiments with the factory
namespace {
    // Registration happens at program startup
    struct ExperimentRegistrations {
        ExperimentRegistrations() {
            ExperimentFactory::getInstance()
                .registerExperiment<ConnectivityExperiment>("ConnectivityTest")
                .registerExperiment<PlasticityLearningExperiment>("PlasticityLearning")
                .registerExperiment<StdpExperiment>("STDPVerification");
        }
    };
    
    // Static initialization
    static ExperimentRegistrations experimentRegistrations;
}

} // namespace nlm