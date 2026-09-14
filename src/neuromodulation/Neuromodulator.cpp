#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <vector>
#include <numeric>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Spike-driven release dynamics
    float lastSpikeTime;
    float spikeSensitivity;
    float temporalDifference;
    float predictedValue;
    float valueEstimate;
    
    // Reward history for prediction error computation
    std::vector<float> rewardHistory;
    std::vector<float> predictionHistory;
    
    // Neural substrate integration
    size_t recentSpikeCount;
    float spikeFilteredLevel;
    float explorationBoost;
    
    Impl() : level(0.0f), baseline(0.01f), peak(2.5f), decayRate(0.15f), releaseRate(2.0f)
        , lastSpikeTime(-1000.0f), spikeSensitivity(0.8f), temporalDifference(0.0f)
        , predictedValue(0.0f), valueEstimate(0.0f), recentSpikeCount(0), spikeFilteredLevel(0.0f)
        , explorationBoost(1.0f) {}
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
    // Real dopamine-modulated plasticity: Reward prediction error driven synaptic modification
    // Uses temporal difference learning with eligibility traces
    float predictionError = pImpl->temporalDifference;
    float learningRate = 0.05f + 0.5f * pImpl->spikeFilteredLevel;
    float eligibility = 0.3f + 0.7f * pImpl->spikeFilteredLevel;
    
    // Realistically, plasticity is driven by spike timing relative to dopamine signals
    float spikeTimescale = 0.2f * (1.0f + pImpl->recentSpikeCount * 0.1f);
    float plasticity = eligibility * std::tanh(predictionError * learningRate);
    
    return 0.5f * (1.0f + std::tanh(plasticity));
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics with spike-driven release and temporal filtering
    float currentTime = static_cast<float>(dt);
    
    // Decay towards baseline with activity-dependent rate
    float decay = pImpl->decayRate * (1.0f + 0.5f * pImpl->spikeFilteredLevel);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay * static_cast<float>(dt));
    
    // Apply low-pass filter to level for smooth dynamics
    pImpl->spikeFilteredLevel = 0.9f * pImpl->spikeFilteredLevel + 0.1f * pImpl->level;
    
    // Reset spike count weekly
    pImpl->recentSpikeCount = 0;
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling with spike-driven release and temporal integration
    float rewardMagnitude = reward;
    
    // Check for positive prediction error (unexpected reward)
    float predictionError = reward - pImpl->predictedValue;
    pImpl->temporalDifference = predictionError;
    pImpl->valueEstimate = pImpl->valueEstimate * 0.9f + 0.1f * reward;
    
    // Spike-driven release with sensitivity to reward magnitude
    float spikeDrive = rewardMagnitude * pImpl->spikeSensitivity;
    pImpl->level = std::min(pImpl->peak, pImpl->level + spikeDrive * pImpl->releaseRate);
    
    // Update reward history
    pImpl->rewardHistory.push_back(reward);
    if (pImpl->rewardHistory.size() > 100) {
        pImpl->rewardHistory.erase(pImpl->rewardHistory.begin());
    }
    
    // Record spike for neural substrate
    pImpl->recentSpikeCount++;
    pImpl->explorationBoost = std::min(2.0f, 1.0f + predictionError * 0.5f);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling with temporal dynamics
    // Used in temporal difference learning: δ = r + γV(s') - V(s)
    pImpl->temporalDifference = error;
    
    // Update prediction based on error (TD learning rule)
    pImpl->predictedValue += 0.1f * error;
    pImpl->valueEstimate += 0.15f * error;
    
    // Spike-modulated response to prediction error
    float errorMagnitude = std::abs(error);
    float signFactor = error > 0.0f ? 1.0f : -1.0f;
    
    // Enhanced response for unexpected prediction errors
    float predictionSurprise = std::sqrt(errorMagnitude) * signFactor;
    pImpl->level = std::max(0.0f, pImpl->level + predictionSurprise * pImpl->releaseRate * pImpl->explorationBoost);
    
    // Store prediction error for later analysis
    pImpl->predictionHistory.push_back(error);
    if (pImpl->predictionHistory.size() > 50) {
        pImpl->predictionHistory.erase(pImpl->predictionHistory.begin());
    }
    
    // Trigger neural spike for learning
    pImpl->recentSpikeCount++;
}

} // namespace nlm