#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float predictionError_;  // Store for next update
    float predictedReward_;  // Store for prediction error calculation
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
        , predictionError_(0.0f), predictedReward_(0.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Dopamine::getPlasticityFactor() const {
    // TODO PHASE 2: Implement real dopamine-modulated plasticity factor
    // PLACEHOLDER: Higher dopamine increases plasticity
    return 0.5f + 0.5f * pImpl->level;
}

    // Combine prediction error with prediction error signal
    float predictionErrorLevel = pImpl->predictionError ? 
        pImpl->predictionError->getLevel() : 0.0f;
    
    // Dopamine integrates reward prediction error and prediction error
    // Phasic dopamine bursts on unexpected rewards (RPE > 0)
    // Dopamine dips on omitted rewards (RPE < 0)
    float totalError = predictionError_ + predictionErrorLevel;
    
    // Modulate level based on total prediction error
    if (totalError > 0.0f) {
        // Burst of dopamine for prediction errors
        pImpl->level = std::min(pImpl->peak, pImpl->level + totalError * pImpl->releaseRate);
    } else if (totalError < 0.0f) {
        // Dip of dopamine for negative prediction errors
        pImpl->level = std::max(0.0f, pImpl->level + totalError * pImpl->releaseRate * 0.5f);
    }
    
    // Apply memory consolidation effects based on dopamine level
    // Higher dopamine enhances memory consolidation
    float consolidationFactor = 0.5f + 0.5f * pImpl->level;
    
    // Update memory consolidation rates through brain
    if (brain_) {
        if (auto* episodicMem = brain_->getEpisodicMemory()) {
            // Higher dopamine increases memory consolidation
            episodicMem->setConsolidationFactor(consolidationFactor);
        }
        
        if (auto* workingMem = brain_->getWorkingMemory()) {
            // Moderate dopamine enhances working memory stability
            workingMem->setStabilityFactor(1.0f - pImpl->level * 0.2f);
        }
    }
    
    // Apply attention effects through brain
    if (brain_ && pImpl->level > 0.5f) {
        // Higher dopamine increases attention and arousal
        brain_->injectCurrentToNeurons(NeuronType::Internal, pImpl->level * 0.1f);
    }
}

void Dopamine::signalReward(float reward) {
    // Burst of dopamine on reward
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
    
    // Enhance learning for reward prediction errors
    // This creates a teaching signal for future predictions
    float predictedReward = predictedReward_;
    float predictionError = reward - predictedReward;
    
    // Amplify prediction error signaling
    pImpl->level += std::abs(predictionError) * 0.5f * pImpl->releaseRate;
}

void Dopamine::signalRewardPredictionError(float error) {
    // Dopamine responds to reward prediction error
    // Positive error (better than expected): phasic dopamine burst
    // Negative error (worse than expected): phasic dopamine dip
    
    float errorMagnitude = std::abs(error);
    
    if (error > 0.0f) {
        // Better than expected - strengthen predictions
        pImpl->level = std::min(pImpl->peak, pImpl->level + error * pImpl->releaseRate);
    } else if (error < 0.0f) {
        // Worse than expected - weaken predictions
        pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate * 0.7f);
    }
    
    // Store prediction error for next update
    predictionError_ = error;
}

void Dopamine::setBaseline(float baseline) {
    pImpl->baseline = baseline;
}

void Dopamine::setPeak(float peak) {
    pImpl->peak = peak;
}

void Dopamine::setDecayRate(float decayRate) {
    pImpl->decayRate = decayRate;
}

void Dopamine::setReleaseRate(float releaseRate) {
    pImpl->releaseRate = releaseRate;
}

void Dopamine::reset() {
    pImpl->level = 0.0f;
    pImpl->baseline = 0.0f;
    pImpl->peak = 1.0f;
    pImpl->decayRate = 0.1f;
    pImpl->releaseRate = 1.0f;
    predictionError_ = 0.0f;
    predictedReward_ = 0.0f;
}
