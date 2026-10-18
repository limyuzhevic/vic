#include "LearningExperiment.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <numeric>
#include <algorithm>
#include <cmath>

namespace nlm {

LearningExperiment::LearningExperiment(std::shared_ptr<Brain> brain, uint64_t seed)
    : brain_(brain), seed_(seed), initialSynapseCount_(0), 
      initialStep_(0), finalStep_(0), strengthened_(0), weakened_(0), unchanged_(0) {
}

void LearningExperiment::recordInitialState() {
    initialStep_ = brain_->getTotalSpikeCount();
    initialSynapseCount_ = brain_->getTotalSynapseCount();
    initialWeights_.clear();
    
    // Record initial weights from first region
    if (auto* region = brain_->getRegion(RegionId(1))) {
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

void LearningExperiment::recordFinalState() {
    finalStep_ = brain_->getTotalSpikeCount();
    finalWeights_.clear();
    
    // Record final weights from first region
    if (auto* region = brain_->getRegion(RegionId(1))) {
        for (const auto& syn : region->getSynapses()) {
            finalWeights_.push_back(syn->getWeight());
        }
    }
    
    mostActiveNeurons_ = brain_->getSpikeSystem()->getMostActiveNeurons(10);
    
    NLM_LOG_INFO("Final state recorded:");
    NLM_LOG_INFO("  Total spikes: " + std::to_string(brain_->getTotalSpikeCount()));
    if (!finalWeights_.empty()) {
        float sum = std::accumulate(finalWeights_.begin(), finalWeights_.end(), 0.0f);
        float mean = sum / finalWeights_.size();
        NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
    }
}

void LearningExperiment::computeStatistics() const {
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
    strengthened_ = 0;
    weakened_ = 0;
    unchanged_ = 0;
    
    size_t minSize = std::min(initialWeights_.size(), finalWeights_.size());
    for (size_t i = 0; i < minSize; ++i) {
        float delta = finalWeights_[i] - initialWeights_[i];
        if (delta > 0.01f) ++strengthened_;
        else if (delta < -0.01f) ++weakened_;
        else ++unchanged_;
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Synaptic Changes:");
    NLM_LOG_INFO("  Strengthened: " + std::to_string(strengthened_));
    NLM_LOG_INFO("  Weakened: " + std::to_string(weakened_));
    NLM_LOG_INFO("  Unchanged: " + std::to_string(unchanged_));
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Spike Activity:");
    NLM_LOG_INFO("  Total spikes: " + std::to_string(brain_->getTotalSpikeCount()));
    NLM_LOG_INFO("  Most active neurons recorded: " + std::to_string(mostActiveNeurons_.size()));
    
    // Determine if learning occurred
    bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) ||
                            (strengthened_ > 0 || weakened_ > 0);
    
    NLM_LOG_INFO("");
    if (learningOccurred) {
        NLM_LOG_INFO("✓ LEARNING DETECTED: Synaptic weights changed through experience");
    } else {
        NLM_LOG_INFO("✗ NO LEARNING: Weights did not change significantly");
    }
}

bool LearningExperiment::didLearningOccur() const {
    if (initialWeights_.empty() || finalWeights_.empty()) return false;
    
    float initialMean = std::accumulate(initialWeights_.begin(), initialWeights_.end(), 0.0f) / initialWeights_.size();
    float finalMean = std::accumulate(finalWeights_.begin(), finalWeights_.end(), 0.0f) / finalWeights_.size();
    
    return (std::abs(finalMean - initialMean) > 0.001f) ||
           (strengthened_ > 0 || weakened_ > 0);
}

float LearningExperiment::getWeightChange() const {
    if (initialWeights_.empty() || finalWeights_.empty()) return 0.0f;
    
    float initialSum = std::accumulate(initialWeights_.begin(), initialWeights_.end(), 0.0f);
    float finalSum = std::accumulate(finalWeights_.begin(), finalWeights_.end(), 0.0f);
    float initialMean = initialSum / initialWeights_.size();
    float finalMean = finalSum / finalWeights_.size();
    
    return finalMean - initialMean;
}

} // namespace nlm