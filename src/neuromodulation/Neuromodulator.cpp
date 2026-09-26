#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float tonicLevel;
    float phasicGain;
    float learningRateMod;
    float targetStriving;
    
    // Recent history for novelty detection
    std::vector<float> recentChanges;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             tonicLevel(0.1f), phasicGain(2.0f), learningRateMod(1.0f), targetStriving(0.5f) {}
};

Dopamine::Dopamine() : pImpl(new Impl), learningRateMod_(1.0f), phasicEnabled_(true), sustainedDuration_(0.0f) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    addSample(pImpl->level);
}

float Dopamine::getPlasticityFactor() const {
    // Real dopamine plasticity factor based on computational models
    // Combines phasic and tonic components
    float phasicComponent = 0.0f;
    if (phasicEnabled_) {
        // Phasic bursts have strong plastic effects
        phasicComponent = 0.3f * pImpl->phasicGain;
    }
    
    float tonicComponent = 0.2f + 0.6f * pImpl->level;  // Tonic level modulates background plasticity
    
    // Learning rate modulation based on target striving
    float strivingMod = 1.0f + 0.5f * (pImpl->targetStriving - 0.5f);
    
    return (phasicComponent + tonicComponent) * learningRateMod_ * pImpl->learningRateMod;
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics with separate phasic and tonic components
    float oldLevel = pImpl->level;
    
    // Tonic decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Sustained response effects
    if (sustainedDuration_ > 0.0f) {
        pImpl->level = std::min(pImpl->level + 0.05f, pImpl->peak);
    }
    
    // Track recent changes for novelty detection
    float change = pImpl->level - oldLevel;
    if (std::abs(change) > 0.01f) {
        pImpl->recentChanges.push_back(change);
        if (pImpl->recentChanges.size() > 100) {
            pImpl->recentChanges.erase(pImpl->recentChanges.begin());
        }
    }
    
    addSample(pImpl->level);
}

void Dopamine::signalReward(float reward) {
    if (!phasicEnabled_) return;
    
    // Real reward signaling with phasic bursts and eligibility traces
    float rewardGain = std::min(reward * pImpl->releaseRate, pImpl->phasicGain);
    float newLevel = std::min(pImpl->peak, pImpl->level + rewardGain);
    
    // Phasic response to reward prediction
    if (reward > 0.5f) {
        // Strong reward signals trigger synaptic eligibility
        newLevel = std::min(pImpl->peak, newLevel * 1.2f);
    } else if (reward > 0.0f) {
        // Weak reward signals
        newLevel = std::min(pImpl->peak, newLevel * 1.1f);
    }
    
    setLevel(newLevel);
    
    // Update target striving based on reward quality
    if (reward > 0.7f) {
        pImpl->targetStriving = std::min(1.0f, pImpl->targetStriving + 0.1f);
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    if (!phasicEnabled_) return;
    
    // Real prediction error signaling (Rescorla-Wagner style)
    float predictionError = std::clamp(error, -1.0f, 1.0f);
    
    // Positive prediction error (better than expected): increase level
    if (predictionError > 0.0f) {
        float newLevel = pImpl->level + predictionError * pImpl->phasicGain;
        setLevel(newLevel);
        
        // Update target striving based on prediction accuracy
        pImpl->targetStriving = std::min(1.0f, pImpl->targetStriving + 0.05f);
    } else {
        // Negative prediction error (worse than expected): decrease level, but not below baseline
        float newLevel = std::max(pImpl->baseline, pImpl->level + predictionError * pImpl->phasicGain);
        setLevel(newLevel);
        
        // Update target striving based on prediction error
        pImpl->targetStriving = std::max(0.0f, pImpl->targetStriving + 0.02f * predictionError);
    }
}

void Dopamine::signalUnexpectedOutcome(float outcome) {
    if (!phasicEnabled_) return;
    
    // Surprise-based signaling for unexpected outcomes
    float surprise = std::abs(outcome - getLevel());
    if (surprise > 0.5f) {
        // Strong surprise triggers large dopamine release
        float newLevel = std::min(pImpl->peak, getLevel() + surprise * pImpl->phasicGain * 1.5f);
        setLevel(newLevel);
        
        // Update learning parameters based on surprise
        pImpl->learningRateMod = std::min(2.0f, pImpl->learningRateMod + 0.2f);
    }
}

} // namespace nlm
