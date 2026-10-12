// NLM (熙然) - Neural Learning Machine
// Learning Experiment Method Implementations
//
// This file contains the implementation of the LearningExperiment methods
// declared in LearningExperiment.hpp.

#include "LearningExperiment.hpp"

namespace nlm {

LearningExperiment::LearningExperiment(std::shared_ptr<Brain> brain, uint64_t seed) 
    : brain(brain), seed(seed), initialSynapseCount(0) {
    // Initialize with default values
}

LearningExperiment::~LearningExperiment() = default;

void LearningExperiment::recordInitialState() {
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

void LearningExperiment::recordFinalState() {
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

void LearningExperiment::runLearningExperiment(SimulationStep steps, double timestep) {
    NLM_LOG_INFO("Running learning experiment for " + std::to_string(steps) + " steps...");
    
    for (SimulationStep step = 0; step < steps; ++step) {
        // Apply learning stimulus - inject current into sensory neurons
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * timestep);
        
        // Log progress every 100 steps
        if (step % 100 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
}

void LearningExperiment::computeStatistics() {
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

std::vector<NeuronId> LearningExperiment::getMostActiveNeurons(size_t count) const {
    if (!brain) return {};
    return brain->getSpikeSystem()->getMostActiveNeurons(count);
}

bool LearningExperiment::learningOccurred() const {
    if (initialWeights.empty() || finalWeights.empty()) return false;
    
    float initialMean = computeMean(initialWeights);
    float finalMean = computeMean(finalWeights);
    
    // Check for significant mean weight change
    if (std::abs(finalMean - initialMean) > 0.001f) return true;
    
    // Check for individual significant changes
    return countSignificantChanges(initialWeights, finalWeights) > 0;
}

void LearningExperiment::clearWeights() {
    initialWeights.clear();
    finalWeights.clear();
    mostActiveNeurons.clear();
}

float LearningExperiment::computeMean(const std::vector<float>& weights) const {
    if (weights.empty()) return 0.0f;
    float sum = std::accumulate(weights.begin(), weights.end(), 0.0f);
    return sum / weights.size();
}

size_t LearningExperiment::countSignificantChanges(const std::vector<float>& before, 
                                                   const std::vector<float>& after, 
                                                   float threshold) const {
    size_t count = 0;
    size_t minSize = std::min(before.size(), after.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        float delta = std::abs(after[i] - before[i]);
        if (delta > threshold) ++count;
    }
    
    return count;
}

} // namespace nlm
