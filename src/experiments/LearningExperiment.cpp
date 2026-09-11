// LearningExperiment.cpp
// Refactored class for neural learning experiments
// Maintains backward compatibility while improving code quality

#include "LearningExperiment.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <numeric>
#include <algorithm>
#include <cmath>
#include <iostream>

namespace nlm {

LearningExperiment::LearningExperiment(std::shared_ptr<Brain> brain, uint64_t seed)
    : brain_(brain), seed_(seed), initialSynapseCount_(0) {
    // Initialization through constructor initializer list
}

LearningExperiment::~LearningExperiment() {
    // Cleanup if needed
}

void LearningExperiment::recordWeightsFromRegion(std::vector<float>& weights, RegionId regionId) const {
    if (auto* region = brain_->getRegion(regionId)) {
        for (const auto& syn : region->getSynapses()) {
            weights.push_back(syn->getWeight());
        }
    }
}

float LearningExperiment::computeMean(const std::vector<float>& weights) const {
    if (weights.empty()) return 0.0f;
    float sum = std::accumulate(weights.begin(), weights.end(), 0.0f);
    return sum / weights.size();
}

void LearningExperiment::countWeightChanges(const std::vector<float>& initial,
                                          const std::vector<float>& final,
                                          size_t& strengthened,
                                          size_t& weakened,
                                          size_t& unchanged) const {
    strengthened = 0;
    weakened = 0;
    unchanged = 0;
    
    size_t minSize = std::min(initial.size(), final.size());
    for (size_t i = 0; i < minSize; ++i) {
        float delta = final[i] - initial[i];
        if (delta > WEIGHT_CHANGE_THRESHOLD) ++strengthened;
        else if (delta < -WEIGHT_CHANGE_THRESHOLD) ++weakened;
        else ++unchanged;
    }
}

bool LearningExperiment::validateWeightData() const {
    return !initialWeights_.empty() && !finalWeights_.empty();
}

void LearningExperiment::logExperimentResults(float initialMean, float finalMean,
                                              size_t strengthened, size_t weakened,
                                              size_t unchanged) const {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Learning Experiment Results ===");
    NLM_LOG_INFO("");
    
    NLM_LOG_INFO("Weight Statistics:");
    NLM_LOG_INFO("  Initial mean weight: " + std::to_string(initialMean));
    NLM_LOG_INFO("  Final mean weight: " + std::to_string(finalMean));
    NLM_LOG_INFO("  Change: " + std::to_string(finalMean - initialMean));
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Synaptic Changes:");
    NLM_LOG_INFO("  Strengthened: " + std::to_string(strengthened));
    NLM_LOG_INFO("  Weakened: " + std::to_string(weakened));
    NLM_LOG_INFO("  Unchanged: " + std::to_string(unchanged));
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Spike Activity:");
    NLM_LOG_INFO("  Total spikes: " + std::to_string(brain_->getTotalSpikeCount()));
    NLM_LOG_INFO("  Most active neurons recorded: " + std::to_string(mostActiveNeurons_.size()));
}

// Original public interface - maintains backward compatibility
void LearningExperiment::recordInitialState() {
    initialSynapseCount_ = brain_->getTotalSynapseCount();
    initialWeights_.clear();
    
    // Record initial weights from first region
    recordWeightsFromRegion(initialWeights_, RegionId(1));
    
    NLM_LOG_INFO("Initial state recorded:");
    NLM_LOG_INFO("  Synapses: " + std::to_string(initialSynapseCount_));
    if (!initialWeights_.empty()) {
        float mean = computeMean(initialWeights_);
        NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
    }
}

void LearningExperiment::recordFinalState() {
    finalWeights_.clear();
    
    // Record final weights from first region
    recordWeightsFromRegion(finalWeights_, RegionId(1));
    
    mostActiveNeurons_ = brain_->getSpikeSystem()->getMostActiveNeurons(ACTIVE_NEURONS_COUNT);
    
    NLM_LOG_INFO("Final state recorded:");
    NLM_LOG_INFO("  Total spikes: " + std::to_string(brain_->getTotalSpikeCount()));
    if (!finalWeights_.empty()) {
        float mean = computeMean(finalWeights_);
        NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
    }
}

void LearningExperiment::computeStatistics() {
    if (!validateWeightData()) {
        NLM_LOG_INFO("ERROR: No weights recorded");
        return;
    }
    
    float initialMean = computeMean(initialWeights_);
    float finalMean = computeMean(finalWeights_);
    
    size_t strengthened = 0, weakened = 0, unchanged = 0;
    countWeightChanges(initialWeights_, finalWeights_, strengthened, weakened, unchanged);
    
    logExperimentResults(initialMean, finalMean, strengthened, weakened, unchanged);
    
    // Determine if learning occurred
    bool learningOccurred = (std::abs(finalMean - initialMean) > LEARNING_THRESHOLD) ||
                           (strengthened > 0 || weakened > 0);
    
    NLM_LOG_INFO("");
    if (learningOccurred) {
        NLM_LOG_INFO("✓ LEARNING DETECTED: Synaptic weights changed through experience");
    } else {
        NLM_LOG_INFO("✗ NO LEARNING: Weights did not change significantly");
    }
}

// Enhanced methods with better error handling and validation
bool LearningExperiment::isValid() const {
    return brain_ != nullptr && !initialWeights_.empty() && !finalWeights_.empty();
}

float LearningExperiment::getInitialMeanWeight() const {
    return computeMean(initialWeights_);
}

float LearningExperiment::getFinalMeanWeight() const {
    return computeMean(finalWeights_);
}

float LearningExperiment::getWeightChange() const {
    float initial = computeMean(initialWeights_);
    float final = computeMean(finalWeights_);
    return final - initial;
}

bool LearningExperiment::learningOccurred() const {
    if (!validateWeightData()) return false;
    
    float weightChange = getWeightChange();
    
    if (std::abs(weightChange) > LEARNING_THRESHOLD) return true;
    
    size_t strengthened = 0, weakened = 0, unchanged = 0;
    countWeightChanges(initialWeights_, finalWeights_, strengthened, weakened, unchanged);
    
    return (strengthened > 0 || weakened > 0);
}

} // namespace nlm