// NLM Learning Experiment Implementation
// Phase 2: Real Neural Computation

#include "experiments/LearningExperiment.hpp"
#include <cmath>
#include <iostream>

namespace nlm {

LearningExperiment::LearningExperiment(std::shared_ptr<Brain> brain, uint64_t seed)
    : brain(brain), seed(seed), initialSynapseCount(0),
      initialMeanWeight(0.0), finalMeanWeight(0.0),
      strengthenedCount(0), weakenedCount(0), unchangedCount(0),
      learningOccurred(false) {}

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
        initialMeanWeight = sum / initialWeights.size();
        NLM_LOG_INFO("  Mean weight: " + std::to_string(initialMeanWeight));
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
        finalMeanWeight = sum / finalWeights.size();
        NLM_LOG_INFO("  Mean weight: " + std::to_string(finalMeanWeight));
    }
}

void LearningExperiment::computeStatistics() const {
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
    initialMeanWeight = initialSum / initialWeights.size();
    finalMeanWeight = finalSum / finalWeights.size();
    
    NLM_LOG_INFO("Weight Statistics:");
    NLM_LOG_INFO("  Initial mean weight: " + std::to_string(initialMeanWeight));
    NLM_LOG_INFO("  Final mean weight: " + std::to_string(finalMeanWeight));
    NLM_LOG_INFO("  Change: " + std::to_string(finalMeanWeight - initialMeanWeight));
    
    // Count synapses that changed significantly
    strengthenedCount = 0;
    weakenedCount = 0;
    unchangedCount = 0;
    
    size_t minSize = std::min(initialWeights.size(), finalWeights.size());
    for (size_t i = 0; i < minSize; ++i) {
        float delta = finalWeights[i] - initialWeights[i];
        if (delta > 0.01f) ++strengthenedCount;
        else if (delta < -0.01f) ++weakenedCount;
        else ++unchangedCount;
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Synaptic Changes:");
    NLM_LOG_INFO("  Strengthened: " + std::to_string(strengthenedCount));
    NLM_LOG_INFO("  Weakened: " + std::to_string(weakenedCount));
    NLM_LOG_INFO("  Unchanged: " + std::to_string(unchangedCount));
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Spike Activity:");
    NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
    NLM_LOG_INFO("  Most active neurons recorded: " + std::to_string(mostActiveNeurons.size()));
    
    // Determine if learning occurred
    learningOccurred = (std::abs(finalMeanWeight - initialMeanWeight) > 0.001f) ||
                      (strengthenedCount > 0 || weakenedCount > 0);
    
    NLM_LOG_INFO("");
    if (learningOccurred) {
        NLM_LOG_INFO("✓ LEARNING DETECTED: Synaptic weights changed through experience");
    } else {
        NLM_LOG_INFO("✗ NO LEARNING: Weights did not change significantly");
    }
}

} // namespace nlm