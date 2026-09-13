#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Dopamine neuromodulator for reward signaling and reinforcement learning
// Computes reward prediction error and modulates plasticity

class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Prediction error tracking
    float getPredictionError() const;
    float getPredictionErrorBaseline() const;
    float getEligibilityTrace() const;
    void updatePrediction(float predictedReward);
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

inline Dopamine::Dopamine() : pImpl(new Impl) {
    pImpl->predictionErrorBaseline = 0.0f;
    pImpl->currentStep = 0;
}

inline Dopamine::~Dopamine() = default;

inline const char* Dopamine::getName() const {
    return "DA";
}

inline float Dopamine::getLevel() const {
    return pImpl->level;
}

inline void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

inline float Dopamine::getPlasticityFactor() const {
    float modFactor = 0.5f + 0.5f * pImpl->predictionError;
    return std::clamp(modFactor, 0.1f, 2.0f);
}

inline void Dopamine::update(TimestepDuration dt) {
    TimestepDuration currentTime = static_cast<Timestamp>(pImpl->currentStep) * dt;
    
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    pImpl->eligibility *= pImpl->traceDecay;
    pImpl->predictionErrorBaseline *= pImpl->traceDecay;
    pImpl->predictionErrorBaseline = std::max(0.0f, pImpl->predictionErrorBaseline);
}

inline void Dopamine::signalReward(float reward) {
    float error = reward - pImpl->expectedReward;
    
    pImpl->expectedReward = 0.9f * pImpl->expectedReward + 0.1f * reward;
    
    pImpl->predictionErrorBaseline = 0.9f * pImpl->predictionErrorBaseline + 0.1f * std::abs(error);
    
    float normalizedError = error / (pImpl->predictionErrorBaseline + 0.01f);
    
    pImpl->predictionError = normalizedError;
    
    pImpl->eligibility += reward * 0.1f;
    
    Timestamp currentTime = static_cast<Timestamp>(pImpl->currentStep) * dt;
    if (std::abs(error) > pImpl->burstThreshold && 
        (currentTime - pImpl->lastBurstTime) > 1.0f) {
        
        float burst = std::min(pImpl->peakLevel, 0.5f + std::abs(error) * 2.0f);
        pImpl->level = std::min(pImpl->peakLevel, pImpl->level + burst * pImpl->releaseRate);
        pImpl->lastBurstTime = currentTime;
        
        NLM_LOG_DEBUG(std::string("Dopamine burst: ") + std::to_string(burst) + 
                     std::string(" (reward prediction error: ") + std::to_string(error) + std::string(")"));
    } else {
        float gradual = std::min(0.2f, std::abs(error) * 0.5f);
        pImpl->level = std::min(pImpl->peakLevel, pImpl->level + gradual * pImpl->releaseRate);
    }
}

inline void Dopamine::signalRewardPredictionError(float error) {
    pImpl->predictionError = error / (pImpl->predictionErrorBaseline + 0.01f);
    
    pImpl->eligibility += error * 0.1f;
    
    float errorMagnitude = std::abs(error);
    
    if (error > 0.0f) {
        float increase = std::min(0.5f, errorMagnitude * 0.3f);
        pImpl->level = std::min(pImpl->peakLevel, pImpl->level + increase * pImpl->releaseRate);
    } else {
        float decrease = std::min(0.3f, errorMagnitude * 0.2f);
        pImpl->level = std::max(pImpl->baseline, pImpl->level - decrease * pImpl->releaseRate);
    }
    
    pImpl->rewardHistory = error;
}

inline float Dopamine::getPredictionError() const {
    return pImpl->predictionError;
}

inline float Dopamine::getPredictionErrorBaseline() const {
    return pImpl->predictionErrorBaseline;
}

inline float Dopamine::getEligibilityTrace() const {
    return pImpl->eligibility;
}

inline void Dopamine::updatePrediction(float predictedReward) {
    pImpl->expectedReward = predictedReward;
}

inline void Dopamine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->predictionError = 0.0f;
    pImpl->expectedReward = 0.0f;
    pImpl->predictionErrorBaseline = 0.0f;
    pImpl->eligibility = 0.0f;
    pImpl->rewardHistory = 0.0f;
}

} // namespace nlm