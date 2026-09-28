// NLM Brain Simulation - Example Refactoring: Learning Experiment Statistics
// Demonstrates how to extract repetitive code patterns into reusable abstractions

#include "LearningExperimentUtils.hpp"
#include "core/Logger/Logger.hpp"

// ===== BEFORE (from src/main.cpp) =====

// LearningExperiment struct with repetitive weight recording and statistics computation
struct LearningExperiment {
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    size_t initialSynapseCount;
    std::vector<float> initialWeights;
    std::vector<float> finalWeights;
    std::vector<NeuronId> mostActiveNeurons;
    
    LearningExperiment(std::shared_ptr<Brain> b, uint64_t s) 
        : brain(b), seed(s), initialSynapseCount(0) {}
    
    void recordInitialState() {
        initialSynapseCount = brain->getTotalSynapseCount();
        initialWeights.clear();
        
        // Record initial weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights.push_back(syn->getWeight());
            }
        }
        
        NLM_LOG_INFO("Initial state recorded:");
        NLM_LOG_INFO("  Synapses: " + std::to_string(initialSynapseCount));
        if (!initialWeights.empty()) {
            float sum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
            float mean = sum / initialWeights.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        }
    }
    
    void recordFinalState() {
        finalWeights.clear();
        
        // Record final weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                finalWeights.push_back(syn->getWeight());
            }
        }
        
        mostActiveNeurons = brain->getSpikeSystem()->getMostActiveNeurons(10);
        
        NLM_LOG_INFO("Final state recorded:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        if (!finalWeights.empty()) {
            float sum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
            float mean = sum / finalWeights.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        }
    }
    
    void computeStatistics() {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Learning Experiment Results ===");
        NLM_LOG_INFO("");
        
        if (initialWeights.empty() || finalWeights.empty()) {
            NLM_LOG_INFO("ERROR: No weights recorded");
            return;
        }
        
        // Compute weight changes
        float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
        float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
        float initialMean = initialSum / initialWeights.size();
        float finalMean = finalSum / finalWeights.size();
        
        NLM_LOG_INFO("Weight Statistics:");
        NLM_LOG_INFO("  Initial mean weight: " + std::to_string(initialMean));
        NLM_LOG_INFO("  Final mean weight: " + std::to_string(finalMean));
        NLM_LOG_INFO("  Change: " + std::to_string(finalMean - initialMean));
        
        // Count synapses that changed significantly
        size_t strengthened = 0;
        size_t weakened = 0;
        size_t unchanged = 0;
        
        size_t minSize = std::min(initialWeights.size(), finalWeights.size());
        for (size_t i = 0; i < minSize; ++i) {
            float delta = finalWeights[i] - initialWeights[i];
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
        NLM_LOG_INFO("  Most active neurons recorded: " + std::to_string(mostActiveNeurons.size()));
        
        // Determine if learning occurred
        bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) ||
                                (strengthened > 0 || weakened > 0);
        
        NLM_LOG_INFO("");
        if (learningOccurred) {
            NLM_LOG_INFO("✓ LEARNING DETECTED: Synaptic weights changed through experience");
        } else {
            NLM_LOG_INFO("✗ NO LEARNING: Weights did not change significantly");
        }
    }
};

// ===== AFTER (Refactored) =====

// LearningExperimentUtils.hpp - Reusable abstractions for experiment tracking

#pragma once

#include "brain/Brain.hpp"
#include <string>
#include <vector>
#include <memory>

namespace nlm {

// Result structure for experiment outcomes
struct ExperimentResult {
    float initialMeanWeight;
    float finalMeanWeight;
    float weightChange;
    size_t strengthened;
    size_t weakened;
    size_t unchanged;
    size_t totalSpikes;
    size_t mostActiveNeuronsRecorded;
    bool learningOccurred;
    
    ExperimentResult()
        : initialMeanWeight(0.0f), finalMeanWeight(0.0f), weightChange(0.0f)
        , strengthened(0), weakened(0), unchanged(0)
        , totalSpikes(0), mostActiveNeuronsRecorded(0)
        , learningOccurred(false) {}
};

// Utility class for managing experiment state and statistics
class ExperimentTracker {
public:
    // Record initial brain state
    static void recordInitialState(
        std::shared_ptr<Brain> brain,
        size_t& initialSynapseCount,
        std::vector<float>& initialWeights,
        std::vector<NeuronId>& mostActiveNeurons
    ) {
        initialSynapseCount = brain->getTotalSynapseCount();
        initialWeights.clear();
        mostActiveNeurons.clear();
        
        // Record weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights.push_back(syn->getWeight());
            }
        }
        
        mostActiveNeurons = brain->getSpikeSystem()->getMostActiveNeurons(10);
        
        NLM_LOG_INFO("Initial state recorded:");
        NLM_LOG_INFO("  Synapses: " + std::to_string(initialSynapseCount));
        if (!initialWeights.empty()) {
            float sum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
            float mean = sum / initialWeights.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        }
    }
    
    // Record final brain state
    static void recordFinalState(
        std::shared_ptr<Brain> brain,
        std::vector<float>& finalWeights,
        std::vector<NeuronId>& mostActiveNeurons
    ) {
        finalWeights.clear();
        mostActiveNeurons.clear();
        
        // Record weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                finalWeights.push_back(syn->getWeight());
            }
        }
        
        mostActiveNeurons = brain->getSpikeSystem()->getMostActiveNeurons(10);
        
        NLM_LOG_INFO("Final state recorded:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        if (!finalWeights.empty()) {
            float sum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
            float mean = sum / finalWeights.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        }
    }
    
    // Compute comprehensive statistics from initial/final data
    static void computeStatistics(
        const std::vector<float>& initialWeights,
        const std::vector<float>& finalWeights,
        size_t totalSpikes,
        size_t mostActiveNeuronsRecorded,
        ExperimentResult& result
    ) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Learning Experiment Results ===");
        NLM_LOG_INFO("");
        
        if (initialWeights.empty() || finalWeights.empty()) {
            NLM_LOG_INFO("ERROR: No weights recorded");
            return;
        }
        
        // Compute weight changes
        float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
        float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
        result.initialMeanWeight = initialSum / initialWeights.size();
        result.finalMeanWeight = finalSum / finalWeights.size();
        result.weightChange = result.finalMeanWeight - result.initialMeanWeight;
        
        NLM_LOG_INFO("Weight Statistics:");
        NLM_LOG_INFO("  Initial mean weight: " + std::to_string(result.initialMeanWeight));
        NLM_LOG_INFO("  Final mean weight: " + std::to_string(result.finalMeanWeight));
        NLM_LOG_INFO("  Change: " + std::to_string(result.weightChange));
        
        // Count synapses that changed significantly
        size_t strengthened = 0;
        size_t weakened = 0;
        size_t unchanged = 0;
        
        size_t minSize = std::min(initialWeights.size(), finalWeights.size());
        for (size_t i = 0; i < minSize; ++i) {
            float delta = finalWeights[i] - initialWeights[i];
            if (delta > 0.01f) ++strengthened;
            else if (delta < -0.01f) ++weakened;
            else ++unchanged;
        }
        
        result.strengthened = strengthened;
        result.weakened = weakened;
        result.unchanged = unchanged;
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Synaptic Changes:");
        NLM_LOG_INFO("  Strengthened: " + std::to_string(strengthened));
        NLM_LOG_INFO("  Weakened: " + std::to_string(weakened));
        NLM_LOG_INFO("  Unchanged: " + std::to_string(unchanged));
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Spike Activity:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(totalSpikes));
        NLM_LOG_INFO("  Most active neurons recorded: " + std::to_string(mostActiveNeuronsRecorded));
        
        result.totalSpikes = totalSpikes;
        result.mostActiveNeuronsRecorded = mostActiveNeuronsRecorded;
        
        // Determine if learning occurred
        result.learningOccurred = (std::abs(result.weightChange) > 0.001f) ||
                                   (strengthened > 0 || weakened > 0);
        
        NLM_LOG_INFO("");
        if (result.learningOccurred) {
            NLM_LOG_INFO("✓ LEARNING DETECTED: Synaptic weights changed through experience");
        } else {
            NLM_LOG_INFO("✗ NO LEARNING: Weights did not change significantly");
        }
    }
};

// Convenience function for running learning experiments
class LearningExperiment {
public:
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    
    LearningExperiment(std::shared_ptr<Brain> b, uint64_t s) 
        : brain(b), seed(s) {}
    
    // Run complete learning experiment with one call
    ExperimentResult run() {
        // Record initial state
        size_t initialSynapseCount;
        std::vector<float> initialWeights;
        std::vector<NeuronId> mostActiveNeurons;
        
        ExperimentTracker::recordInitialState(
            brain, initialSynapseCount, initialWeights, mostActiveNeurons
        );
        
        // Enable plasticity on synapses
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
        
        // Run plasticity experiment
        for (SimulationStep step = 0; step < 1000; ++step) {
            for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            brain->step(step, step * 0.001);
        }
        
        // Record final state
        std::vector<float> finalWeights;
        std::vector<NeuronId> finalMostActiveNeurons;
        
        ExperimentTracker::recordFinalState(brain, finalWeights, finalMostActiveNeurons);
        
        // Compute statistics
        ExperimentResult result;
        ExperimentTracker::computeStatistics(
            initialWeights, finalWeights,
            brain->getTotalSpikeCount(),
            finalMostActiveNeurons.size(),
            result
        );
        
        return result;
    }
};

// ===== USAGE EXAMPLES =====

// Example 1: Simple learning experiment
void exampleSimpleExperiment() {
    // Create brain (as in main.cpp)
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(500));
    config->set("region_count", static_cast<int64_t>(1));
    auto brain = std::make_shared<Brain>(config);
    
    // Initialize brain
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return;
    }
    
    // Run learning experiment using new refactored approach
    LearningExperiment experiment(brain, 42);
    ExperimentResult result = experiment.run();
    
    // Check if learning occurred
    if (result.learningOccurred) {
        NLM_LOG_INFO("✓ Learning experiment succeeded!")
            " Weight change: " + std::to_string(result.weightChange));
    } else {
        NLM_LOG_INFO("✗ No learning detected");
    }
}

// Example 2: Reusing experiment utilities
void exampleReusability() {
    // Create brain
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100));
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Record initial state using reusable utilities
    size_t initialSynapseCount;
    std::vector<float> initialWeights;
    std::vector<NeuronId> initialActiveNeurons;
    
    ExperimentTracker::recordInitialState(
        brain, initialSynapseCount, initialWeights, initialActiveNeurons
    );
    
    // Run simulation
    for (int step = 0; step < 100; ++step) {
        brain->step(step, step * 0.001);
    }
    
    // Record final state
    std::vector<float> finalWeights;
    std::vector<NeuronId> finalActiveNeurons;
    
    ExperimentTracker::recordFinalState(brain, finalWeights, finalActiveNeurons);
    
    // Compute statistics
    ExperimentResult result;
    ExperimentTracker::computeStatistics(
        initialWeights, finalWeights,
        brain->getTotalSpikeCount(),
        finalActiveNeurons.size(),
        result
    );
    
    // Results are now available for analysis
    NLM_LOG_INFO("Experiment results:");
    NLM_LOG_INFO("  Weight change: " + std::to_string(result.weightChange));
    NLM_LOG_INFO("  Strengthened synapses: " + std::to_string(result.strengthened));
}