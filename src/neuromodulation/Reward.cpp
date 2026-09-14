#include "Reward.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <numeric>

namespace nlm {

struct Reward::Impl {
    // Enhanced reward computation based on computational neuroscience
    float currentValue;              // Immediate reward value
    float accumulatedReward;         // Long-term reward history
    std::vector<float> history;      // Reward history for prediction
    float predictionError;           // Reward prediction error (RPE)
    float predictedValue;            // Current value prediction
    float expectedValue;             // Expected future value
    
    // Temporal credit assignment
    float temporalDifference;        // TD error for credit assignment
    float learningRate;              // Reward learning rate
    float discountFactor;            // Future reward discount
    
    // State and action values
    float stateValue;                // Current state value
    float actionValue;               // Last action's value
    
    // Neuromodulatory coupling
    float dopamineCoupling;          // Coupling to dopamine system
    float curiosityModulation;       // Curiosity-driven reward modulation
    
    // Biological parameters
    float baselineReward;            // Baseline reward level
    float rewardSaturation;          // Reward saturation point
    float rewardNoise;               // Biological noise in reward
    
    // Random number generator
    mutable std::mt19937 rng;
    mutable std::uniform_real_distribution<float> uniformDist;
    mutable std::normal_distribution<float> normalDist;
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f), predictionError(0.0f),
             predictedValue(0.0f), expectedValue(0.0f), temporalDifference(0.0f),
             learningRate(0.01f), discountFactor(0.95f), stateValue(0.0f),
             actionValue(0.0f), dopamineCoupling(1.0f), curiosityModulation(0.0f),
             baselineReward(0.0f), rewardSaturation(1.0f), rewardNoise(0.05f),
             rng(std::random_device()()), uniformDist(0.0f, 1.0f),
             normalDist(0.0f, 0.1f) {}
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

float Reward::getValue() const {
    return pImpl->currentValue;
}

void Reward::setValue(float value) {
    pImpl->currentValue = value;
}

void Reward::add(float delta) {
    // Enhanced reward addition with biological realism
    pImpl->accumulatedReward += delta;
    
    // Apply reward saturation (logarithmic or sigmoidal)
    float saturation = 1.0f - std::exp(-std::abs(delta) * 2.0f);
    float scaledDelta = delta * saturation;
    
    // Add biological noise for variability
    float noise = normalDist(pImpl->rng) * pImpl->rewardNoise;
    scaledDelta += noise;
    
    pImpl->currentValue += scaledDelta;
    
    // Store in history
    pImpl->history.push_back(pImpl->currentValue);
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    pImpl->predictionError = 0.0f;
    pImpl->temporalDifference = 0.0f;
    pImpl->history.clear();
}

float Reward::computeReward(const Observation& observation) const {
    // Enhanced reward computation from observations
    // Integrates multiple aspects: goal proximity, novelty, prediction error
    
    float reward = 0.0f;
    
    // 1. State-based reward (goal proximity)
    auto vision = observation.getVision();
    auto touch = observation.getTouch();
    
    // Check for goal states (bright pixels in vision)
    float goalProximity = 0.0f;
    for (float pixel : vision) {
        goalProximity += pixel;  // Bright pixels indicate proximity to goal
    }
    goalProximity /= std::max(1.0f, static_cast<float>(vision.size()));
    reward += goalProximity * 0.5f;
    
    // Check for beneficial touch
    for (float touchVal : touch) {
        if (touchVal > 0.7f) {
            reward += 0.3f;
        }
    }
    
    // 2. Novelty-based reward (encourage exploration)
    // This would be computed by coupling with novelty system
    
    // 3. Prediction error-based reward
    // Positive prediction error = better than expected = reward
    // Negative prediction error = worse than expected = cost
    reward += pImpl->predictionError * 0.3f;
    
    // 4. State value prediction
    reward += (pImpl->predictedValue - pImpl->stateValue) * 0.2f;
    
    // Apply curiosity modulation
    reward *= (1.0f + pImpl->curiosityModulation * 0.3f);
    
    // Add baseline reward
    reward += pImpl->baselineReward;
    
    return std::clamp(reward, -1.0f, 1.0f);
}

void Reward::setPredictionError(float error) {
    pImpl->predictionError = error;
}

void Reward::updatePrediction(float newPrediction) {
    // Update value prediction based on reward received
    float tdError = newPrediction - pImpl->predictedValue;
    pImpl->temporalDifference = tdError;
    
    // Update prediction with learning rate
    pImpl->predictedValue += tdError * pImpl->learningRate;
    
    // Update expected value (discounted future reward)
    pImpl->expectedValue = pImpl->predictedValue * pImpl->discountFactor;
}

void Reward::setStateValue(float value) {
    pImpl->stateValue = value;
}

void Reward::setActionValue(float value) {
    pImpl->actionValue = value;
}

void Reward::setCuriosityModulation(float modulation) {
    pImpl->curiosityModulation = std::clamp(modulation, 0.0f, 1.0f);
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

float Reward::getPredictionError() const {
    return pImpl->predictionError;
}

float Reward::getPredictedValue() const {
    return pImpl->predictedValue;
}

float Reward::getExpectedValue() const {
    return pImpl->expectedValue;
}

float Reward::getTemporalDifference() const {
    return pImpl->temporalDifference;
}

float Reward::getAccumulatedReward() const {
    return pImpl->accumulatedReward;
}

} // namespace nlm
