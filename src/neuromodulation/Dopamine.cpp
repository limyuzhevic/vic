#include "Dopamine.hpp"

namespace nlm {

struct Dopamine::Impl {
    float level;                    // Current dopamine level (-1.0 to 1.0)
    float plasticityFactor;         // Modulates STDP learning rates
    float predictionError;          // Reward prediction error
    float expectedReward;           // Expected future reward
    float rewardHistory;             // Recent reward history
    float decayRate;                // Rate of decay for dopamine
    
    Impl() : level(0.0f), plasticityFactor(1.0f), predictionError(0.0f),
             expectedReward(0.0f), rewardHistory(0.0f), decayRate(0.1f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, -1.0f, 1.0f);
}

void Dopamine::update(double dt, float reward, float predictedReward) {
    // Compute reward prediction error (delta rule)
    float predictionError = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    pImpl->expectedReward = 0.95f * pImpl->expectedReward + 0.05f * reward;
    
    // Update prediction error
    pImpl->predictionError = predictionError;
    
    // Update dopamine level based on prediction error
    // The closer predictionError is to 0, the closer dopamine is to baseline
    // Positive prediction error = dopamine increase (unexpected reward)
    // Negative prediction error = dopamine decrease (unexpected lack of reward)
    float targetLevel = predictionError * 0.5f;  // Scaling factor
    
    // Smooth update of dopamine level
    float learningRate = 0.2f;
    pImpl->level += (targetLevel - pImpl->level) * learningRate;
    
    // Apply decay to maintain homeostasis
    pImpl->level *= std::pow(1.0f - pImpl->decayRate, static_cast<float>(dt));
    
    // Clamp to reasonable range
    pImpl->level = std::clamp(pImpl->level, -1.0f, 1.0f);
    
    // Update plasticity factor based on dopamine level
    // Higher dopamine = higher plasticity, lower dopamine = lower plasticity
    pImpl->plasticityFactor = 0.5f + 0.5f * pImpl->level;
    pImpl->plasticityFactor = std::clamp(pImpl->plasticityFactor, 0.1f, 2.0f);
    
    // Update reward history
    pImpl->rewardHistory = reward;
}

float Dopamine::getPlasticityFactor() const {
    return pImpl->plasticityFactor;
}

void Dopamine::setPlasticityFactor(float factor) {
    pImpl->plasticityFactor = std::clamp(factor, 0.1f, 2.0f);
}

void Dopamine::reset() {
    pImpl->level = 0.0f;
    pImpl->plasticityFactor = 1.0f;
    pImpl->predictionError = 0.0f;
    pImpl->expectedReward = 0.0f;
    pImpl->rewardHistory = 0.0f;
}

} // namespace nlm