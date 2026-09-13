#include "EnhancedNeuromodulation.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Dopamine::Impl {
    float phasicResponseRate_;
    float tonicDecayRate_;
    
    Impl() : phasicResponseRate_(1.0f), tonicDecayRate_(0.05f) {}
};

Dopamine::Dopamine() : pImpl(new Impl), predictionError_(0.0f) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return state_.tonicLevel + state_.phasicLevel;
}

void Dopamine::setLevel(float level) {
    // Set both tonic and phasic components proportionally
    state_.tonicLevel = level * 0.7f;  // Tonic is 70% of total
    state_.phasicLevel = level * 0.3f; // Phasic is 30% of total
}

float Dopamine::getPhasicLevel() const {
    return state_.phasicLevel;
}

float Dopamine::getTonicLevel() const {
    return state_.tonicLevel;
}

float Dopamine::getPlasticityFactor() const {
    // Plasticity factor combines both tonic and phasic dopamine
    // Phasic dopamine (prediction errors) is particularly important for plasticity
    return 0.3f * state_.tonicLevel + 0.7f * state_.phasicLevel;
}

void Dopamine::update(TimestepDuration dt) {
    // Decay tonic dopamine towards adaptive baseline
    float tonicDecay = state_.tonicDecayRate_ * static_cast<float>(dt);
    state_.tonicLevel = std::max(
        state_.adaptiveBaseline,
        state_.tonicLevel - tonicDecay
    );
    
    // Phasic dopamine decays quickly
    float phasicDecay = state_.phasicResponseRate_ * static_cast<float>(dt);
    state_.phasicLevel = std::max(0.0f, state_.phasicLevel - phasicDecay);
    
    // Adapt baseline based on recent activity
    float activityAdaptation = state_.baselineAdaptation * static_cast<float>(dt);
    state_.adaptiveBaseline = state_.adaptiveBaseline * (1.0f - activityAdaptation) +
                               (state_.tonicLevel + state_.phasicLevel) * activityAdaptation;
    
    // Update recent phasic activity for history
    state_.recentPhasicActivity = state_.phasicLevel;
    
    // Update phasic history for smoothing
    phasicHistory_.push_back(state_.phasicLevel);
    if (phasicHistory_.size() > PHASIC_HISTORY_SIZE) {
        phasicHistory_.pop_front();
    }
}

void Dopamine::phasicResponse(float magnitude) {
    // Apply phasic dopamine response
    state_.phasicLevel += magnitude * pImpl->phasicResponseRate_;
    
    // Clamp to reasonable range
    state_.phasicLevel = std::min(1.0f, state_.phasicLevel);
}

const NeuromodulatorState& Dopamine::getState() const {
    return state_;
}

void Dopamine::signalReward(float reward) {
    // Reward causes phasic dopamine response
    float rewardResponse = std::min(1.0f, reward);
    phasicResponse(rewardResponse);
    
    // Update adaptive baseline
    state_.adaptiveBaseline = std::max(state_.adaptiveBaseline, rewardResponse);
}

void Dopamine::signalRewardPredictionError(float error, float predictedReward) {
    // Reward prediction error is a powerful phasic signal
    float reward = error + predictedReward; // RPE formula
    signalReward(reward);
    
    // Store prediction error for reference
    predictionError_ = error;
}

} // namespace nlm
