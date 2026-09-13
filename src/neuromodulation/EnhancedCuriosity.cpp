#include "EnhancedCuriosity.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>

namespace nlm {

struct InformationGainCalculator::Impl {
    float entropyThreshold_;
    float maxGain_;
    
    Impl() : entropyThreshold_(0.5f), maxGain_(1.0f) {}
};

InformationGainCalculator::InformationGainCalculator() : pImpl(new Impl) {}

float InformationGainCalculator::calculateGain(const std::vector<float>& predictionErrors,
                                               const std::vector<float>& novelties,
                                               TimestepDuration dt) const {
    if (predictionErrors.empty()) return 0.0f;
    
    // Calculate entropy of prediction error distribution
    float entropy = calculateEntropy(predictionErrors);
    
    // Calculate information gain from novelties
    float noveltyInformation = 0.0f;
    for (float novelty : novelties) {
        // Information gain is higher for unexpected novelties
        noveltyInformation += std::log(1.0f + novelty) * novelty;
    }
    
    // Combine entropy and novelty information
    float combinedInformation = (1.0f - entropy) * noveltyInformation;
    
    // Apply entropy threshold - only consider information if uncertainty is above threshold
    if (entropy < pImpl->entropyThreshold_) {
        return 0.0f;
    }
    
    // Apply maximum gain cap
    return std::min(combinedInformation, pImpl->maxGain_);
}

float InformationGainCalculator::calculateEntropy(const std::vector<float>& errors) const {
    if (errors.empty()) return 0.0f;
    
    // Normalize errors to [0, 1] range
    float maxError = *std::max_element(errors.begin(), errors.end());
    if (maxError <= 0.0f) return 0.0f;
    
    std::vector<float> normalized(errors.size());
    for (size_t i = 0; i < errors.size(); ++i) {
        normalized[i] = errors[i] / maxError;
    }
    
    // Calculate entropy using normalized distribution
    float entropy = 0.0f;
    for (float norm : normalized) {
        if (norm > 0.0f) {
            entropy -= norm * std::log(norm + 1e-6f);
        }
    }
    
    // Normalize entropy to [0, 1] range
    float maxPossibleEntropy = std::log(static_cast<float>(normalized.size()) + 1e-6f);
    return entropy / maxPossibleEntropy;
}

bool InformationGainCalculator::isInformationGainWorthExploring(float gain, float currentUncertainty) const {
    // Explore if gain is significant and uncertainty is high
    return gain > 0.1f && currentUncertainty > pImpl->entropyThreshold_;
}

struct IntrinsicMotivation::Impl {
    float motivationLevel_;
    float explorationRate_;
    float uncertaintySensitivity_;
    float decayRate_;
    
    Impl() : motivationLevel_(0.0f), explorationRate_(0.1f), 
             uncertaintySensitivity_(1.0f), decayRate_(0.05f) {}
};

IntrinsicMotivation::IntrinsicMotivation() : pImpl(new Impl) {}

float IntrinsicMotivation::calculateMotivation(const std::vector<float>& stateEntropy,
                                               const std::vector<float>& predictionAccuracy,
                                               TimestepDuration dt) {
    // Calculate average uncertainty from state entropy
    float avgUncertainty = 0.0f;
    if (!stateEntropy.empty()) {
        avgUncertainty = std::accumulate(stateEntropy.begin(), stateEntropy.end(), 0.0f) / stateEntropy.size();
    }
    
    // Calculate average prediction accuracy (inverse of error)
    float avgAccuracy = 0.0f;
    if (!predictionAccuracy.empty()) {
        avgAccuracy = std::accumulate(predictionAccuracy.begin(), predictionAccuracy.end(), 0.0f) / predictionAccuracy.size();
    }
    
    // Intrinsic motivation increases with uncertainty and decreases with accuracy
    float motivation = pImpl->uncertaintySensitivity_ * avgUncertainty * (1.0f - avgAccuracy);
    
    // Apply exploration rate
    motivation *= pImpl->explorationRate_;
    
    // Update historical motivation for smoothing
    motivationHistory_.push_back(motivation);
    if (motivationHistory_.size() > MOTIVATION_HISTORY_SIZE) {
        motivationHistory_.pop_front();
    }
    
    // Smooth with recent history
    float avgHistoryMotivation = 0.0f;
    if (!motivationHistory_.empty()) {
        avgHistoryMotivation = std::accumulate(motivationHistory_.begin(), motivationHistory_.end(), 0.0f) / motivationHistory_.size();
    }
    
    // Blend current and historical values
    motivation = 0.7f * motivation + 0.3f * avgHistoryMotivation;
    
    // Decay towards baseline
    motivation = std::max(0.0f, motivation - pImpl->decayRate_ * static_cast<float>(dt));
    
    pImpl->motivationLevel_ = motivation;
    return motivation;
}

float IntrinsicMotivation::getExplorationBonus(float uncertainty) const {
    // Exploration bonus is higher when uncertainty is high
    return pImpl->explorationRate_ * uncertainty * pImpl->uncertaintySensitivity_;
}

void IntrinsicMotivation::update(float currentMotivation, TimestepDuration dt) {
    // Smooth update
    pImpl->motivationLevel_ = 0.9f * currentMotivation + 0.1f * pImpl->motivationLevel_;
    
    // Apply decay
    pImpl->motivationLevel_ = std::max(0.0f, pImpl->motivationLevel_ - pImpl->decayRate_ * static_cast<float>(dt));
}

void IntrinsicMotivation::reset() {
    pImpl->motivationLevel_ = 0.0f;
    motivationHistory_.clear();
}

struct EnhancedCuriosity::Impl {
    class Brain* brain_;
    float level_;
    float informationGain_;
    float uncertaintyEstimate_;
    
    float noveltyWeight_;
    float predictionErrorWeight_;
    float informationGainWeight_;
    float intrinsicMotivationWeight_;
    
    float explorationRate_;
    float minExplorationRate_;
    float maxExplorationRate_;
    
    Impl() : brain_(nullptr), level_(0.0f), informationGain_(0.0f), uncertaintyEstimate_(0.0f),
             noveltyWeight_(0.3f), predictionErrorWeight_(0.3f), 
             informationGainWeight_(0.2f), intrinsicMotivationWeight_(0.2f),
             explorationRate_(0.1f), minExplorationRate_(0.01f), maxExplorationRate_(0.5f) {}
};

EnhancedCuriosity::EnhancedCuriosity() : pImpl(new Impl) {}

EnhancedCuriosity::~EnhancedCuriosity() = default;

void EnhancedCuriosity::initialize(Brain* brain) {
    pImpl->brain_ = brain;
    NLM_LOG_INFO("Enhanced Curiosity system initialized");
}

void EnhancedCuriosity::update(float novelty, float predictionError,
                               const std::vector<float>& predictionErrors,
                               const std::vector<float>& sensoryEntropy,
                               TimestepDuration dt) {
    // Calculate information gain from prediction errors
    informationGain_ = infoGainCalculator_.calculateGain(
        predictionErrors, std::vector<float>{novelty}, dt);
    
    // Get uncertainty estimate
    uncertaintyEstimate_ = 0.0f;
    if (!sensoryEntropy.empty()) {
        uncertaintyEstimate_ = std::accumulate(
            sensoryEntropy.begin(), sensoryEntropy.end(), 0.0f) / sensoryEntropy.size();
    }
    
    // Calculate intrinsic motivation
    intrinsicMotivation_.calculateMotivation(
        sensoryEntropy, std::vector<float>{predictionError}, dt);
    
    // Combine all curiosity components
    float target = pImpl->noveltyWeight_ * novelty +
                   pImpl->predictionErrorWeight_ * predictionError +
                   pImpl->informationGainWeight_ * informationGain_ +
                   pImpl->intrinsicMotivationWeight_ * intrinsicMotivation_.getMotivationLevel();
    
    // Smooth update with adaptive exploration rate
    float adaptiveExplorationRate = pImpl->explorationRate_;
    if (uncertaintyEstimate_ > 0.7f) {
        // Increase exploration rate when uncertainty is high
        adaptiveExplorationRate = std::min(pImpl->maxExplorationRate_,
                                           pImpl->explorationRate_ * (1.0f + uncertaintyEstimate_));
    }
    
    level_ += (target - level_) * adaptiveExplorationRate;
    
    // Update historical curiosity for smoothing
    curiosityHistory_.push_back(level_);
    if (curiosityHistory_.size() > CURIOSITY_HISTORY_SIZE) {
        curiosityHistory_.pop_front();
    }
    
    // Smooth with recent history
    float avgHistoryCuriosity = 0.0f;
    if (!curiosityHistory_.empty()) {
        avgHistoryCuriosity = std::accumulate(
            curiosityHistory_.begin(), curiosityHistory_.end(), 0.0f) / curiosityHistory_.size();
    }
    
    level_ = 0.8f * level_ + 0.2f * avgHistoryCuriosity;
}

float EnhancedCuriosity::getLevel() const {
    return level_;
}

float EnhancedCuriosity::getExplorationDrive() const {
    // Combined curiosity and exploration drive
    float baseCuriosity = level_;
    float explorationBonus = intrinsicMotivation_.getExplorationBonus(uncertaintyEstimate_);
    
    return baseCuriosity + explorationBonus;
}

void EnhancedCuriosity::reset() {
    level_ = 0.0f;
    informationGain_ = 0.0f;
    uncertaintyEstimate_ = 0.0f;
    curiosityHistory_.clear();
    intrinsicMotivation_.reset();
}

} // namespace nlm
