#include "LearningExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include <numeric>
#include <cmath>

using namespace nlm;

LearningExperiment::LearningExperiment(std::shared_ptr<Brain> brain, uint64_t seed)
    : brain_(brain), seed_(seed) {
    if (!brain_) {
        NLM_LOG_ERROR("Cannot create LearningExperiment: Brain is null");
    }
}

void LearningExperiment::recordInitialState() {
    if (!brain_) {
        NLM_LOG_ERROR("Cannot record initial state: Brain is null");
        return;
    }
    
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
    if (!brain_) {
        NLM_LOG_ERROR("Cannot record final state: Brain is null");
        return;
    }
    
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

void LearningExperiment::computeStatistics() {
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
    NLM_LOG_INFO("  Total spikes: " + std::to_string(brain_->getTotalSpikeCount()));
    NLM_LOG_INFO("  Most active neurons recorded: " + std::to_string(mostActiveNeurons_.size()));
    
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

uint64_t LearningExperiment::getTotalSpikeCount() const {
    return brain_ ? brain_->getTotalSpikeCount() : 0;
}

bool LearningExperiment::didLearningOccur() const {
    if (initialWeights_.empty() || finalWeights_.empty()) return false;
    
    float initialSum = std::accumulate(initialWeights_.begin(), initialWeights_.end(), 0.0f);
    float finalSum = std::accumulate(finalWeights_.begin(), finalWeights_.end(), 0.0f);
    float initialMean = initialSum / initialWeights_.size();
    float finalMean = finalSum / finalWeights_.size();
    
    return (std::abs(finalMean - initialMean) > 0.001f) || 
           (std::any_of(initialWeights_.begin(), initialWeights_.end(),
                       [this](float w) { return std::any_of(finalWeights_.begin(), finalWeights_.end(),
                           [w](float fw) { return std::abs(fw - w) > 0.01f; }); });
}

std::string LearningExperiment::getDescription() const {
    std::ostringstream oss;
    oss << "Learning Experiment (Seed: " << seed_ << ")";
    oss << " - Initial Synapses: " << initialSynapseCount_;
    oss << " - Final Spike Count: " << getTotalSpikeCount();
    oss << " - Learning: " << (didLearningOccur() ? "Yes" : "No");
    return oss.str();
}