// NLM (熙然) - Neural Learning Machine
// LearningExperiment implementation

#include "LearningExperiment.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

LearningExperiment::LearningExperiment(std::shared_ptr<Brain> brain, uint64_t seed)
    : brain(brain), seed(seed), initialSynapseCount(0) {
}

float LearningExperiment::calculateMean(const std::vector<float>& values) const {
    if (values.empty()) {
        return 0.0f;
    }
    float sum = std::accumulate(values.begin(), values.end(), 0.0f);
    return sum / values.size();
}

std::pair<size_t, size_t> LearningExperiment::countSignificantChanges(
    const std::vector<float>& initial,
    const std::vector<float>& final) const {
    size_t strengthened = 0;
    size_t weakened = 0;
    
    size_t minSize = std::min(initial.size(), final.size());
    for (size_t i = 0; i < minSize; ++i) {
        float delta = final[i] - initial[i];
        if (delta > 0.01f) {
            ++strengthened;
        } else if (delta < -0.01f) {
            ++weakened;
        }
    }
    
    return {strengthened, weakened};
}

bool LearningExperiment::hasLearningOccurred(float initialMean, float finalMean,
                                            size_t strengthened, size_t weakened) const {
    return (std::abs(finalMean - initialMean) > 0.001f) ||
           (strengthened > 0 || weakened > 0);
}

void LearningExperiment::recordInitialState() {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Recording initial learning experiment state...");
    
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
        float mean = calculateMean(initialWeights);
        NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        NLM_LOG_INFO("  Weight count: " + std::to_string(initialWeights.size()));
    }
}

void LearningExperiment::recordFinalState() {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Recording final learning experiment state...");
    
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
        float mean = calculateMean(finalWeights);
        NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        NLM_LOG_INFO("  Weight count: " + std::to_string(finalWeights.size()));
        NLM_LOG_INFO("  Most active neurons recorded: " + std::to_string(mostActiveNeurons.size()));
    }
}

void LearningExperiment::computeStatistics() {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Learning Experiment Results ===");
    NLM_LOG_INFO("");
    
    if (initialWeights.empty() || finalWeights.empty()) {
        NLM_LOG_INFO("ERROR: No weights recorded");
        NLM_LOG_INFO("Please ensure recordInitialState() and recordFinalState() were called.");
        return;
    }
    
    // Compute weight statistics
    float initialMean = calculateMean(initialWeights);
    float finalMean = calculateMean(finalWeights);
    
    NLM_LOG_INFO("Weight Statistics:");
    NLM_LOG_INFO("  Initial mean weight: " + std::to_string(initialMean));
    NLM_LOG_INFO("  Final mean weight: " + std::to_string(finalMean));
    NLM_LOG_INFO("  Change: " + std::to_string(finalMean - initialMean));
    
    // Count synapses that changed significantly
    auto changes = countSignificantChanges(initialWeights, finalWeights);
    size_t strengthened = changes.first;
    size_t weakened = changes.second;
    size_t unchanged = finalWeights.size() - strengthened - weakened;
    
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
    bool learningOccurred = hasLearningOccurred(initialMean, finalMean, strengthened, weakened);
    
    NLM_LOG_INFO("");
    if (learningOccurred) {
        NLM_LOG_INFO("✓ LEARNING DETECTED: Synaptic weights changed through experience");
        NLM_LOG_INFO("  The neural system has demonstrated plastic behavior.");
    } else {
        NLM_LOG_INFO("✗ NO LEARNING: Weights did not change significantly");
        NLM_LOG_INFO("  The network may need more stimulation or different parameters.");
    }
}

} // namespace nlm
