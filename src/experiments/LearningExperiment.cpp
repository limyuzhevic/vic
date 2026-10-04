#include "LearningExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

LearningExperiment::LearningExperiment(std::shared_ptr<Brain> brain, uint64_t seed)
    : brain_(brain)
    , seed_(seed)
    , detailedLogging_(true)
    , learningThreshold_(0.001f)
    , initialSynapseCount_(0)
    , initialMeanWeight_(0.0f)
    , finalMeanWeight_(0.0f)
    , weightChange_(0.0f)
    , strengthenedSynapses_(0)
    , weakenedSynapses_(0)
    , unchangedSynapses_(0)
    , learningDetected_(false) {}

void LearningExperiment::recordInitialState() {
    if (detailedLogging_) {
        NLM_LOG_INFO("Recording initial brain state for experiment (seed: " + std::to_string(seed_) + ")");
    }
    
    initialSynapseCount_ = brain_->getTotalSynapseCount();
    initialWeights_.clear();
    
    // Record initial weights from first region
    if (auto* region = brain_->getRegion(RegionId(1))) {
        for (const auto& syn : region->getSynapses()) {
            initialWeights_.push_back(syn->getWeight());
        }
    }
    
    if (detailedLogging_) {
        NLM_LOG_INFO("Initial state recorded:");
        NLM_LOG_INFO("  Synapses: " + std::to_string(initialSynapseCount_));
        if (!initialWeights_.empty()) {
            initialMeanWeight_ = calculateMean(initialWeights_);
            NLM_LOG_INFO("  Mean weight: " + std::to_string(initialMeanWeight_));
            logWeightDistribution("Initial", initialWeights_);
        }
    }
}

void LearningExperiment::recordFinalState() {
    if (detailedLogging_) {
        NLM_LOG_INFO("Recording final brain state");
    }
    
    finalWeights_.clear();
    
    // Record final weights from first region
    if (auto* region = brain_->getRegion(RegionId(1))) {
        for (const auto& syn : region->getSynapses()) {
            finalWeights_.push_back(syn->getWeight());
        }
    }
    
    mostActiveNeurons_ = brain_->getSpikeSystem()->getMostActiveNeurons(10);
    
    if (detailedLogging_) {
        NLM_LOG_INFO("Final state recorded:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain_->getTotalSpikeCount()));
        NLM_LOG_INFO("  Most active neurons: " + std::to_string(mostActiveNeurons_.size()));
        if (!finalWeights_.empty()) {
            finalMeanWeight_ = calculateMean(finalWeights_);
            NLM_LOG_INFO("  Mean weight: " + std::to_string(finalMeanWeight_));
            logWeightDistribution("Final", finalWeights_);
        }
    }
}

void LearningExperiment::computeStatistics() {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Learning Experiment Results ===");
    NLM_LOG_INFO("");
    
    if (initialWeights_.empty() || finalWeights_.empty()) {
        NLM_LOG_ERROR("ERROR: No weights recorded");
        return;
    }
    
    // Compute weight changes
    float initialSum = std::accumulate(initialWeights_.begin(), initialWeights_.end(), 0.0f);
    float finalSum = std::accumulate(finalWeights_.begin(), finalWeights_.end(), 0.0f);
    float initialMean = calculateMean(initialWeights_);
    float finalMean = calculateMean(finalWeights_);
    
    weightChange_ = finalMean - initialMean;
    
    NLM_LOG_INFO("Weight Statistics:");
    NLM_LOG_INFO("  Initial mean weight: " + std::to_string(initialMean));
    NLM_LOG_INFO("  Final mean weight: " + std::to_string(finalMean));
    NLM_LOG_INFO("  Change: " + std::to_string(weightChange_));
    
    // Count synapses that changed significantly
    size_t minSize = std::min(initialWeights_.size(), finalWeights_.size());
    strengthenedSynapses_ = 0;
    weakenedSynapses_ = 0;
    unchangedSynapses_ = 0;
    
    for (size_t i = 0; i < minSize; ++i) {
        float delta = finalWeights_[i] - initialWeights_[i];
        if (isSignificantChange(delta)) {
            if (delta > 0.0f) ++strengthenedSynapses_;
            else ++weakenedSynapses_;
        } else {
            ++unchangedSynapses_;
        }
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Synaptic Changes:");
    NLM_LOG_INFO("  Strengthened: " + std::to_string(strengthenedSynapses_));
    NLM_LOG_INFO("  Weakened: " + std::to_string(weakenedSynapses_));
    NLM_LOG_INFO("  Unchanged: " + std::to_string(unchangedSynapses_));
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Spike Activity:");
    NLM_LOG_INFO("  Total spikes: " + std::to_string(brain_->getTotalSpikeCount()));
    NLM_LOG_INFO("  Most active neurons recorded: " + std::to_string(mostActiveNeurons_.size()));
    
    // Determine if learning occurred
    learningDetected_ = (std::abs(weightChange_) > learningThreshold_) ||
                       (strengthenedSynapses_ > 0 || weakenedSynapses_ > 0);
    
    NLM_LOG_INFO("");
    if (learningDetected_) {
        NLM_LOG_INFO("✓ LEARNING DETECTED: Synaptic weights changed through experience");
    } else {
        NLM_LOG_INFO("✗ NO LEARNING: Weights did not change significantly");
    }
}

std::string LearningExperiment::getSummaryReport() const {
    std::string report = "Learning Experiment Summary (seed: " + std::to_string(seed_) + ")\n";
    report += "Weight change: " + std::to_string(weightChange_) + "\n";
    report += "Learning detected: " + std::to_string(learningDetected_) + "\n";
    report += "Strengthened synapses: " + std::to_string(strengthenedSynapses_) + "\n";
    report += "Weakened synapses: " + std::to_string(weakenedSynapses_) + "\n";
    report += "Total spikes: " + std::to_string(brain_ ? brain_->getTotalSpikeCount() : 0) + "\n";
    return report;
}

float LearningExperiment::calculateMean(const std::vector<float>& weights) const {
    if (weights.empty()) return 0.0f;
    float sum = std::accumulate(weights.begin(), weights.end(), 0.0f);
    return sum / weights.size();
}

void LearningExperiment::logWeightDistribution(const std::string& label, const std::vector<float>& weights) const {
    if (!detailedLogging_ || weights.empty()) return;
    
    float minWeight = *std::min_element(weights.begin(), weights.end());
    float maxWeight = *std::max_element(weights.begin(), weights.end());
    float range = maxWeight - minWeight;
    
    NLM_LOG_INFO("  " + label + " weight distribution:");
    NLM_LOG_INFO("    Min: " + std::to_string(minWeight));
    NLM_LOG_INFO("    Max: " + std::to_string(maxWeight));
    NLM_LOG_INFO("    Range: " + std::to_string(range));
    
    // Show some sample weights if many
    if (weights.size() > 10) {
        NLM_LOG_INFO("    Sample weights: ");
        size_t samples = std::min(size_t(5), weights.size());
        for (size_t i = 0; i < samples; ++i) {
            NLM_LOG_INFO("      w[" + std::to_string(i) + "] = " + std::to_string(weights[i]));
        }
    }
}

bool LearningExperiment::isSignificantChange(float delta) const {
    return std::abs(delta) > 0.01f;
}

} // namespace nlm
