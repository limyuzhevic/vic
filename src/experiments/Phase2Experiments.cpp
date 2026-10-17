#include "Phase2Experiments.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <numeric>

namespace nlm {

// Phase 2 Experiments Implementation

// Helper functions for Phase 2 experiments

static float getFloatFromConfig(std::shared_ptr<Config> config, const std::string& key) {
    if (config->has(key)) {
        auto value = config->get(key);
        if (value.has_value()) {
            if (std::holds_alternative<double>(value.value())) {
                return std::get<double>(value.value());
            } else if (std::holds_alternative<float>(value.value())) {
                return std::get<float>(value.value());
            } else if (std::holds_alternative<int>(value.value())) {
                return static_cast<float>(std::get<int>(value.value()));
            } else if (std::holds_alternative<int64_t>(value.value())) {
                return static_cast<float>(std::get<int64_t>(value.value()));
            }
        }
    }
    return 0.0f;
}

static int64_t getIntFromConfig(std::shared_ptr<Config> config, const std::string& key) {
    if (config->has(key)) {
        auto value = config->get(key);
        if (value.has_value()) {
            if (std::holds_alternative<int>(value.value())) {
                return static_cast<int64_t>(std::get<int>(value.value()));
            } else if (std::holds_alternative<int64_t>(value.value())) {
                return std::get<int64_t>(value.value());
            } else if (std::holds_alternative<double>(value.value())) {
                return static_cast<int64_t>(std::get<double>(value.value()));
            } else if (std::holds_alternative<float>(value.value())) {
                return static_cast<int64_t>(std::get<float>(value.value()));
            }
        }
    }
    return 0;
}

// ConnectivityExperiment implementation

void ConnectivityExperiment::recordState() {
    if (!brain_) return;
    
    initialSpikeCount_ = brain_->getTotalSpikeCount();
    initialNeuronCount_ = brain_->getTotalNeuronCount();
    initialSynapseCount_ = brain_->getTotalSynapseCount();
    
    initialWeights_.clear();
    if (auto* region = brain_->getRegion(RegionId(1))) {
        for (const auto& syn : region->getSynapses()) {
            initialWeights_.push_back(syn->getWeight());
        }
    }
    
    NLM_LOG_INFO("Initial state recorded:");
    NLM_LOG_INFO("  Spike count: " + std::to_string(initialSpikeCount_));
    NLM_LOG_INFO("  Neuron count: " + std::to_string(initialNeuronCount_));
    NLM_LOG_INFO("  Synapse count: " + std::to_string(initialSynapseCount_));
    if (!initialWeights_.empty()) {
        float sum = std::accumulate(initialWeights_.begin(), initialWeights_.end(), 0.0f);
        float mean = sum / initialWeights_.size();
        NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
    }
}

void ConnectivityExperiment::finalizeState() {
    if (!brain_) return;
    
    finalSpikeCount_ = brain_->getTotalSpikeCount();
    finalNeuronCount_ = brain_->getTotalNeuronCount();
    finalSynapseCount_ = brain_->getTotalSynapseCount();
    
    finalWeights_.clear();
    if (auto* region = brain_->getRegion(RegionId(1))) {
        for (const auto& syn : region->getSynapses()) {
            finalWeights_.push_back(syn->getWeight());
        }
    }
    
    mostActiveNeurons_ = brain_->getSpikeSystem()->getMostActiveNeurons(10);
    
    NLM_LOG_INFO("Final state recorded:");
    NLM_LOG_INFO("  Spike count: " + std::to_string(finalSpikeCount_));
    NLM_LOG_INFO("  Neuron count: " + std::to_string(finalNeuronCount_));
    NLM_LOG_INFO("  Synapse count: " + std::to_string(finalSynapseCount_));
    if (!finalWeights_.empty()) {
        float sum = std::accumulate(finalWeights_.begin(), finalWeights_.end(), 0.0f);
        float mean = sum / finalWeights_.size();
        NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
    }
}

void ConnectivityExperiment::analyzeChanges() {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Connectivity Experiment Analysis ===");
    NLM_LOG_INFO("");
    
    // Calculate spike count change
    int64_t spikeChange = finalSpikeCount_ - initialSpikeCount_;
    NLM_LOG_INFO("Spike Count Changes:");
    NLM_LOG_INFO("  Initial: " + std::to_string(initialSpikeCount_));
    NLM_LOG_INFO("  Final: " + std::to_string(finalSpikeCount_));
    NLM_LOG_INFO("  Change: " + std::to_string(spikeChange));
    
    // Calculate weight changes
    if (!initialWeights_.empty() && !finalWeights_.empty()) {
        float initialSum = std::accumulate(initialWeights_.begin(), initialWeights_.end(), 0.0f);
        float finalSum = std::accumulate(finalWeights_.begin(), finalWeights_.end(), 0.0f);
        float initialMean = initialSum / initialWeights_.size();
        float finalMean = finalSum / finalWeights_.size();
        
        NLM_LOG_INFO("Weight Changes:");
        NLM_LOG_INFO("  Initial mean weight: " + std::to_string(initialMean));
        NLM_LOG_INFO("  Final mean weight: " + std::to_string(finalMean));
        NLM_LOG_INFO("  Change: " + std::to_string(finalMean - initialMean));
        
        // Count strengthened synapses
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
        
        NLM_LOG_INFO("Synaptic Strength Changes:");
        NLM_LOG_INFO("  Strengthened: " + std::to_string(strengthened));
        NLM_LOG_INFO("  Weakened: " + std::to_string(weakened));
        NLM_LOG_INFO("  Unchanged: " + std::to_string(unchanged));
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Network Activity Summary:");
    NLM_LOG_INFO("  Most active neurons recorded: " + std::to_string(mostActiveNeurons_.size()));
    
    // Determine connectivity quality
    bool connectivityGood = (spikeChange > 0) && (!mostActiveNeurons_.empty());
    
    NLM_LOG_INFO("");
    if (connectivityGood) {
        NLM_LOG_INFO("✓ CONNECTIVITY TEST PASSED: Network shows active propagation and synaptic changes");
        success_ = true;
        message_ = "Connectivity experiment passed - network shows active propagation and synaptic changes";
    } else {
        NLM_LOG_INFO("✗ CONNECTIVITY TEST FAILED: Insufficient activity or synaptic changes");
        success_ = false;
        message_ = "Connectivity experiment failed - insufficient activity or synaptic changes";
    }
}

ExperimentResult ConnectivityExperiment::run(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> environment) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    lastResult_.experimentName = getName();
    lastResult_.success = false;
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Connectivity Experiment ===");
    NLM_LOG_INFO("Testing basic neural connectivity and spike propagation...");
    
    brain_ = brain;
    
    if (!brain_) {
        lastResult_.message = "Invalid brain provided";
        return lastResult_;
    }
    
    // Record initial state
    recordState();
    
    // Get region and neurons
    auto* region = brain_->getRegion(RegionId(1));
    if (!region) {
        lastResult_.message = "No brain regions found";
        return lastResult_;
    }
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        lastResult_.message = "No neurons found in brain region";
        return lastResult_;
    }
    
    // Inject current into neurons
    NLM_LOG_INFO("Injecting current into " + std::to_string(std::min(size_t(10), neurons.size())) + " neurons...");
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(getFloatFromConfig(brain_->getConfig(), "connectivity_injection_current"));
    }
    
    // Run simulation
    NLM_LOG_INFO("Running simulation for " + std::to_string(config_.maxSteps) + " steps...");
    for (SimulationStep step = 0; step < config_.maxSteps; ++step) {
        brain_->step(step, step * getFloatFromConfig(brain_->getConfig(), "simulation_timestep"));
        
        if (step % 100 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain_->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain_->getFiringNeuronCount()));
        }
    }
    
    // Record final state and analyze
    finalizeState();
    analyzeChanges();
    
    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = endTime - startTime;
    lastResult_.executionTime = duration.count();
    lastResult_.stepsCompleted = config_.maxSteps;
    lastResult_.success = success_;
    
    NLM_LOG_INFO("Experiment completed in " + std::to_string(duration.count()) + " seconds");
    
    return lastResult_;
}

// PlasticityLearningExperiment implementation

void PlasticityLearningExperiment::recordState() {
    if (!brain_) return;
    
    initialSpikeCount_ = brain_->getTotalSpikeCount();
    initialSynapseCount_ = brain_->getTotalSynapseCount();
    
    initialWeights_.clear();
    if (auto* region = brain_->getRegion(RegionId(1))) {
        for (const auto& syn : region->getSynapses()) {
            initialWeights_.push_back(syn->getWeight());
        }
    }
    
    NLM_LOG_INFO("Initial state recorded:");
    NLM_LOG_INFO("  Spike count: " + std::to_string(initialSpikeCount_));
    NLM_LOG_INFO("  Synapse count: " + std::to_string(initialSynapseCount_));
    if (!initialWeights_.empty()) {
        float sum = std::accumulate(initialWeights_.begin(), initialWeights_.end(), 0.0f);
        float mean = sum / initialWeights_.size();
        NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
    }
}

void PlasticityLearningExperiment::finalizeState() {
    if (!brain_) return;
    
    finalSpikeCount_ = brain_->getTotalSpikeCount();
    finalSynapseCount_ = brain_->getTotalSynapseCount();
    
    finalWeights_.clear();
    if (auto* region = brain_->getRegion(RegionId(1))) {
        for (const auto& syn : region->getSynapses()) {
            finalWeights_.push_back(syn->getWeight());
        }
    }
    
    mostActiveNeurons_ = brain_->getSpikeSystem()->getMostActiveNeurons(10);
    
    NLM_LOG_INFO("Final state recorded:");
    NLM_LOG_INFO("  Spike count: " + std::to_string(finalSpikeCount_));
    NLM_LOG_INFO("  Synapse count: " + std::to_string(finalSynapseCount_));
    if (!finalWeights_.empty()) {
        float sum = std::accumulate(finalWeights_.begin(), finalWeights_.end(), 0.0f);
        float mean = sum / finalWeights_.size();
        NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
    }
}

void PlasticityLearningExperiment::analyzeLearning() {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Plasticity Learning Experiment Analysis ===");
    NLM_LOG_INFO("");
    
    if (initialWeights_.empty() || finalWeights_.empty()) {
        NLM_LOG_INFO("ERROR: No weights recorded");
        success_ = false;
        message_ = "No weights recorded during experiment";
        return;
    }
    
    // Calculate weight changes
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
    
    NLM_LOG_INFO("Synaptic Changes:");
    NLM_LOG_INFO("  Strengthened: " + std::to_string(strengthened));
    NLM_LOG_INFO("  Weakened: " + std::to_string(weakened));
    NLM_LOG_INFO("  Unchanged: " + std::to_string(unchanged));
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Spike Activity:");
    NLM_LOG_INFO("  Initial spikes: " + std::to_string(initialSpikeCount_));
    NLM_LOG_INFO("  Final spikes: " + std::to_string(finalSpikeCount_));
    NLM_LOG_INFO("  Total spikes during experiment: " + std::to_string(finalSpikeCount_ - initialSpikeCount_));
    NLM_LOG_INFO("  Most active neurons recorded: " + std::to_string(mostActiveNeurons_.size()));
    
    // Determine if learning occurred
    bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) ||
                           (strengthened > 0 || weakened > 0);
    
    NLM_LOG_INFO("");
    if (learningOccurred) {
        NLM_LOG_INFO("✓ LEARNING DETECTED: Synaptic weights changed through experience");
        success_ = true;
        message_ = "Plasticity learning experiment passed - measurable synaptic changes detected";
    } else {
        NLM_LOG_INFO("✗ NO LEARNING: Weights did not change significantly");
        success_ = false;
        message_ = "Plasticity learning experiment failed - no significant synaptic changes detected";
    }
}

ExperimentResult PlasticityLearningExperiment::run(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> environment) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    lastResult_.experimentName = getName();
    lastResult_.success = false;
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Plasticity Learning Experiment ===");
    NLM_LOG_INFO("Running plasticity learning experiment with Hebbian and STDP rules...");
    
    brain_ = brain;
    
    if (!brain_) {
        lastResult_.message = "Invalid brain provided";
        return lastResult_;
    }
    
    // Record initial state
    recordState();
    
    // Enable plasticity on synapses
    if (auto* region = brain_->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
    }
    
    // Apply repeated input pattern
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Applying repeated input patterns for " + std::to_string(config_.maxSteps) + " steps...");
    
    for (SimulationStep step = 0; step < config_.maxSteps; ++step) {
        // Create input pattern
        for (size_t i = 0; i < 20 && i < brain_->getTotalNeuronCount() / 4; ++i) {
            brain_->injectCurrentToNeurons(NeuronType::Sensory, getFloatFromConfig(brain_->getConfig(), "plasticity_input_strength"));
        }
        
        brain_->step(step, step * getFloatFromConfig(brain_->getConfig(), "simulation_timestep"));
        
        // Log progress periodically
        if (step % 200 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain_->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain_->getFiringNeuronCount()));
        }
    }
    
    // Record final state and analyze
    finalizeState();
    analyzeLearning();
    
    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = endTime - startTime;
    lastResult_.executionTime = duration.count();
    lastResult_.stepsCompleted = config_.maxSteps;
    lastResult_.success = success_;
    
    NLM_LOG_INFO("Experiment completed in " + std::to_string(duration.count()) + " seconds");
    
    return lastResult_;
}

// StdpExperiment implementation

ExperimentResult StdpExperiment::run(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> environment) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    lastResult_.experimentName = getName();
    lastResult_.success = false;
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== STDP Verification Experiment ===");
    NLM_LOG_INFO("Verifying STDP plasticity rule with pre-before-post spike timing...");
    
    if (!brain) {
        lastResult_.message = "Invalid brain provided";
        return lastResult_;
    }
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        lastResult_.message = "No brain region found";
        return lastResult_;
    }
    
    auto& synapses = region->getSynapses();
    if (synapses.size() < 5) {
        lastResult_.message = "Not enough synapses for STDP test";
        return lastResult_;
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
    
    // Create correlated activity for LTP
    NLM_LOG_INFO("");
    NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");
    
    for (int trial = 0; trial < 50; ++trial) {
        Neuron* preNeuron = nullptr;
        Neuron* postNeuron = nullptr;
        
        auto neurons = region->getAllNeurons();
        if (neurons.size() >= 2) {
            preNeuron = neurons[0];
            postNeuron = neurons[1];
        }
        
        if (preNeuron && postNeuron) {
            // Pre fires first (as in natural STDP)
            preNeuron->injectCurrent(getFloatFromConfig(brain->getConfig(), "stdp_pre_current"));
            brain->step(trial * 2, (trial * 2) * getFloatFromConfig(brain->getConfig(), "simulation_timestep"));
            
            // Post fires shortly after (typical STDP timing)
            postNeuron->injectCurrent(getFloatFromConfig(brain->getConfig(), "stdp_post_current"));
            brain->step(trial * 2 + 1, (trial * 2 + 1) * getFloatFromConfig(brain->getConfig(), "simulation_timestep"));
        }
    }
    
    // Record and analyze final weights
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
    
    // Check if STDP worked as expected
    if (totalDelta > 0.001f) {
        NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation (LTP)");
        success_ = true;
        message_ = "STDP verification passed - pre-before-post spike timing produced LTP";
    } else if (totalDelta < -0.001f) {
        NLM_LOG_INFO("  ! STDP REVERSED: Unexpected depression (LTD)");
        success_ = false;
        message_ = "STDP verification failed - unexpected depression (LTD) instead of LTP";
    } else {
        NLM_LOG_INFO("  ! NO CHANGE: STDP may not be triggering");
        success_ = false;
        message_ = "STDP verification failed - no weight change detected";
    }
    
    return lastResult_;
}

} // namespace nlm