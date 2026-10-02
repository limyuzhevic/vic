#include "Reward.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>

namespace nlm {

struct Reward::Impl {
    // Reward computation components
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    
    // Reward prediction (temporal difference learning)
    float predictedValue;        // V(s) - expected future reward
    float valueFunction;         // V(s') - next state value
    float predictionError;       // δ = r + γV(s') - V(s)
    float discountFactor;        // γ = 0.95 (standard TD learning)
    
    // Reward components (primary, secondary, social)
    float primaryReward;         // Biological/reward value
    float secondaryReward;       // Instrumental/value learned
    float socialReward;          // Social/affiliative value
    
    // Multimodal processing weights
    float sensoryWeight;         // Sensory input contribution
    float internalWeight;        // Internal state contribution
    float goalWeight;            // Goal proximity contribution
    
    // Temporal integration
    float temporalSum;           // Running sum of rewards
    float rewardHistory[100];    // Recent rewards for temporal integration
    int historyIndex;            // Circular buffer index
    
    // State estimation
    std::vector<float> stateEstimate;  // Estimated current state
    std::vector<float> rewardPredictor; // Predicted reward components
    
    // Novelty integration
    float noveltyBonus;          // Surprise-driven exploration bonus
    std::vector<float> surpriseHistory;  // Recent prediction errors
    
    // Randomization for exploration
    std::mt19937 rng;
    std::normal_distribution<float> rewardNoise;
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f),
             predictedValue(0.0f), valueFunction(0.0f), predictionError(0.0f),
             discountFactor(0.95f), primaryReward(0.0f), secondaryReward(0.0f),
             socialReward(0.0f), sensoryWeight(0.4f), internalWeight(0.3f),
             goalWeight(0.3f), temporalSum(0.0f), historyIndex(0),
             noveltyBonus(0.0f), rng(std::random_device{}()),
             rewardNoise(0.0f, 0.1f) {
        for (int i = 0; i < 100; ++i) rewardHistory[i] = 0.0f;
    }
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
    // Add reward with temporal integration and adaptation
    pImpl->accumulatedReward += delta;
    
    // Apply noise for realistic variability
    float noise = pImpl->rewardNoise(pImpl->rng);
    float adjustedDelta = delta + noise * std::abs(delta) * 0.1f;
    
    // Temporal integration (running average)
    pImpl->temporalSum += adjustedDelta;
    int avgSize = std::min(static_cast<int>(pImpl->history.size()) + 1, 100);
    pImpl->currentValue = pImpl->temporalSum / avgSize;
    
    // Add to history
    pImpl->history.push_back(pImpl->currentValue);
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    pImpl->predictedValue = 0.0f;
    pImpl->valueFunction = 0.0f;
    pImpl->predictionError = 0.0f;
    pImpl->temporalSum = 0.0f;
    pImpl->noveltyBonus = 0.0f;
}

float Reward::computeReward(const Observation& observation) const {
    // Real reward computation using multimodal inputs
    const SensoryInput* sensoryInput = observation.getSensoryInput();
    if (!sensoryInput || !observation.isValid()) {
        return 0.0f;
    }
    
    const auto& sensoryData = sensoryInput->getData();
    if (sensoryData.empty()) {
        return 0.0f;
    }
    
    // Extract multimodal components from observation
    std::vector<float> sensoryComponents(3, 0.0f);
    std::vector<float> internalComponents(3, 0.0f);
    std::vector<float> goalComponents(3, 0.0f);
    
    // Process sensory input (vision, audio, etc.)
    if (!sensoryData.empty()) {
        // Estimate sensory components from data
        float maxVal = *std::max_element(sensoryData.begin(), sensoryData.end());
        float minVal = *std::min_element(sensoryData.begin(), sensoryData.end());
        
        if (maxVal > minVal) {
            sensoryComponents[0] = (maxVal - minVal) / (maxVal + minVal + 0.001f); // Intensity/contrast
            sensoryComponents[1] = (maxVal + minVal) * 0.5f; // Overall magnitude
            sensoryComponents[2] = 1.0f - std::abs(maxVal - minVal) / (maxVal + minVal + 0.001f); // Balance
        }
    }
    
    // Internal state components (hypothetical - would come from brain)
    // In a real system, these would be retrieved from neural activity
    internalComponents[0] = 0.5f + 0.3f * std::sin(pImpl->accumulatedReward * 0.1f); // Homeostatic
    internalComponents[1] = 0.7f * std::exp(-pImpl->accumulatedReward * 0.05f); // Arousal
    internalComponents[2] = 1.0f - 0.5f * std::tanh(pImpl->accumulatedReward * 0.1f); // Stress
    
    // Goal proximity components
    goalComponents[0] = 1.0f / (1.0f + std::exp(-pImpl->accumulatedReward * 0.2f + 2.0f)); // Approach
    goalComponents[1] = 1.0f - goalComponents[0]; // Avoidance
    goalComponents[2] = 0.5f + 0.5f * std::cos(pImpl->accumulatedReward * 0.3f); // Fluctuation
    
    // Weighted combination of modalities (temporal integration)
    float sensoryContribution = std::inner_product(sensoryData.begin(), 
                                                   sensoryData.end(), 
                                                   sensoryComponents.begin(), 
                                                   0.0f) / sensoryData.size() * pImpl->sensoryWeight;
    
    float internalContribution = std::inner_product(internalComponents.begin(),
                                                     internalComponents.end(),
                                                     internalComponents.begin(),
                                                     0.0f) * pImpl->internalWeight;
    
    float goalContribution = std::inner_product(goalComponents.begin(),
                                                goalComponents.end(),
                                                goalComponents.begin(),
                                                0.0f) * pImpl->goalWeight;
    
    // Compute primary, secondary, and social rewards
    pImpl->primaryReward = sensoryContribution * 0.5f + internalComponents[0] * 0.3f;
    pImpl->secondaryReward = goalContribution * 0.4f + sensoryComponents[1] * 0.3f;
    pImpl->socialReward = internalComponents[2] * 0.5f + sensoryComponents[0] * 0.2f;
    
    // Temporal difference learning for reward prediction
    float immediateReward = pImpl->primaryReward + pImpl->secondaryReward + pImpl->socialReward;
    immediateReward += pImpl->noveltyBonus; // Add surprise-driven exploration bonus
    
    // Apply TD update
    float temporalDifference = immediateReward + pImpl->discountFactor * pImpl->valueFunction - pImpl->predictedValue;
    pImpl->predictionError = temporalDifference;
    pImpl->valueFunction += pImpl->discountFactor * temporalDifference;
    
    // Update predicted reward
    pImpl->predictedValue = pImpl->predictedValue * 0.9f + immediateReward * 0.1f;
    
    // Update state estimate
    pImpl->stateEstimate.clear();
    for (size_t i = 0; i < sensoryData.size(); ++i) {
        pImpl->stateEstimate.push_back(sensoryData[i] * (1.0f + i * 0.01f)); // Weighted by position
    }
    
    // Add noise for exploration
    std::normal_distribution<float> noiseDist(0.0f, 0.05f * immediateReward);
    immediateReward += noiseDist(pImpl->rng);
    
    return std::max(0.0f, immediateReward); // Rewards cannot be negative
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
