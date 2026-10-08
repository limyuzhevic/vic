#include "Reward.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <chrono>

namespace nlm {

struct Reward::Impl {
    // Neuroscience-based reward system state
    float currentValue;
    float accumulatedReward;
    float totalValue;
    float averageValue;
    float rewardMagnitude;
    float rewardDuration;
    float temporalDiscountFactor;
    float rewardPredictionError;
    float expectedValue;
    float predictionErrorVariance;
    float dopamineModulation;
    float rewardHistoryDecay;
    float rewardPersistence;
    
    // Multiple reward dimensions
    struct RewardDimensions {
        float primary;      // Primary/reward (food, water, safety)
        float secondary;    // Secondary/reward (social, achievement)
        float conditioned;  // Conditioned/reward (learned associations)
    } dimensions;
    
    // Temporal processing
    float rewardHistoryDecayRate;
    float rewardIntegrationRate;
    std::vector<float> temporalHistory;
    std::vector<float> valueHistory;
    
    // Reward prediction learning
    float learningRate;
    float eligibilityTrace;
    float explorationVariance;
    
    // Biological mechanisms
    float phasicComponent;
    float tonicComponent;
    float predictionErrorBaseline;
    float adaptiveBaseline;
    
    // Time tracking
    double lastRewardTime;
    double rewardDurationAccumulator;
    double timeSinceLastUpdate;
    std::chrono::steady_clock::time_point lastUpdateTime;
    
    // Randomness for biological variability
    RandomGenerator rng;
    
    // Statistics
    float rewardSum;
    float rewardCount;
    float positiveRewardCount;
    float negativeRewardCount;
    
    Impl() 
        : currentValue(0.0f), accumulatedReward(0.0f), totalValue(0.0f), averageValue(0.0f)
        , rewardMagnitude(0.0f), rewardDuration(0.0f), temporalDiscountFactor(0.95f)
        , rewardPredictionError(0.0f), expectedValue(0.0f), predictionErrorVariance(0.1f)
        , dopamineModulation(1.0f), rewardHistoryDecay(0.9f), rewardPersistence(0.0f)
        , dimensions({0.0f, 0.0f, 0.0f})
        , rewardHistoryDecayRate(0.95f), rewardIntegrationRate(0.1f)
        , phasicComponent(0.0f), tonicComponent(0.5f), predictionErrorBaseline(0.0f)
        , adaptiveBaseline(0.0f), lastRewardTime(-1000.0), rewardDurationAccumulator(0.0)
        , timeSinceLastUpdate(0.0)
        , learningRate(0.1f), eligibilityTrace(0.0f), explorationVariance(0.05f)
        , rewardSum(0.0f), rewardCount(0.0f), positiveRewardCount(0.0f), negativeRewardCount(0.0f) {
        
        // Initialize with biologically realistic parameters
        lastUpdateTime = std::chrono::steady_clock::now();
        
        // Seed with deterministic but variable seed for biological realism
        uint64_t seed = static_cast<uint64_t>(std::chrono::steady_clock::now().time_since_epoch().count());
        rng.seed(seed ^ 0x9e3779b97f4a7c15);
        
        NLM_LOG_INFO("Reward system initialized with tonic level = " + std::to_string(tonicComponent));
    }
    
    ~Impl() {
        NLM_LOG_INFO("Reward system destroyed");
    }
    
    // Update time since last update
    void updateTime() {
        auto now = std::chrono::steady_clock::now();
        timeSinceLastUpdate = std::chrono::duration<float>(now - lastUpdateTime).count();
        lastUpdateTime = now;
    }
    
    // Temporal discounting: future rewards are valued less than immediate rewards
    float discountReward(float value, float timeDelay) const {
        // Exponential discounting with biological parameters
        float k = 0.5f; // Discount rate parameter (lower = steeper discounting)
        return value * std::exp(-k * timeDelay);
    }
    
    // Reward prediction error calculation (TD learning: δ = r + γV(s') - V(s))
    float calculateRewardPredictionError(float immediateReward, float nextValue) const {
        float rpe = immediateReward + temporalDiscountFactor * nextValue - expectedValue;
        return rpe;
    }
    
    // Add biological noise to reward values for realism
    float addBiologicalNoise(float value, float coefficient) const {
        float noise = rng.normal(0.0f, coefficient);
        return value * (1.0f + noise);
    }
    
    // Encode reward duration and magnitude
    void encodeRewardDuration(float duration) {
        rewardDurationAccumulator += duration;
        rewardDuration = std::min(rewardDurationAccumulator, 10.0f); // Cap at 10 seconds
    }
    
    // Update reward dimensions based on observation context
    void updateRewardDimensions(const Observation& observation) {
        const auto& sensoryInput = observation.getSensoryInput();
        if (sensoryInput) {
            const auto& data = sensoryInput->getData();
            
            // Extract different aspects of the sensory input for different reward dimensions
            if (!data.empty()) {
                // Primary reward (0-40% of input range): basic needs, primary reinforcement
                size_t primaryIndex = std::min(static_cast<size_t>(data.size() * 0.4f), data.size() - 1);
                dimensions.primary = std::max(0.0f, std::min(1.0f, data[primaryIndex]));
                
                // Secondary reward (40-80% of input range): social, achievement, status
                size_t secondaryStart = std::min(static_cast<size_t>(data.size() * 0.4f), data.size() - 1);
                size_t secondaryEnd = std::min(static_cast<size_t>(data.size() * 0.8f), data.size());
                if (secondaryEnd > secondaryStart) {
                    float sum = std::accumulate(data.begin() + secondaryStart, data.begin() + secondaryEnd, 0.0f);
                    float count = static_cast<float>(secondaryEnd - secondaryStart);
                    dimensions.secondary = std::max(0.0f, std::min(1.0f, sum / count));
                }
                
                // Conditioned reward (80-100% of input range): learned associations, context
                size_t conditionedStart = std::min(static_cast<size_t>(data.size() * 0.8f), data.size() - 1);
                size_t conditionedEnd = data.size();
                if (conditionedEnd > conditionedStart) {
                    float sum = std::accumulate(data.begin() + conditionedStart, data.begin() + conditionedEnd, 0.0f);
                    float count = static_cast<float>(conditionedEnd - conditionedStart);
                    dimensions.conditioned = std::max(0.0f, std::min(1.0f, sum / count));
                }
                
                // Blend dimensions for total reward (primary weighted more heavily)
                float weightedReward = dimensions.primary * 0.5f + dimensions.secondary * 0.3f + dimensions.conditioned * 0.2f;
                
                // Add exploration variance for uncertainty
                weightedReward += rng.normal(0.0f, explorationVariance);
                
                // Apply dopamine modulation
                weightedReward *= dopamineModulation;
                
                // Apply temporal discounting
                float timeDelay = static_cast<float>(timeSinceLastUpdate);
                weightedReward = discountReward(weightedReward, timeDelay);
                
                // Add biological noise
                weightedReward = addBiologicalNoise(weightedReward, 0.02f);
                
                // Update state
                rewardMagnitude = weightedReward;
                currentValue = weightedReward;
                
                // Update statistics
                rewardSum += weightedReward;
                rewardCount += 1.0f;
                if (weightedReward > 0.0f) {
                    positiveRewardCount += 1.0f;
                } else if (weightedReward < 0.0f) {
                    negativeRewardCount += 1.0f;
                }
                
                // Update accumulated reward (decay and integrate)
                accumulatedReward = accumulatedReward * rewardHistoryDecay + weightedReward * rewardIntegrationRate;
                totalValue += weightedReward;
                averageValue = totalValue / rewardCount;
                
                // Store in history
                temporalHistory.push_back(weightedReward);
                valueHistory.push_back(expectedValue);
                if (temporalHistory.size() > 1000) {
                    temporalHistory.erase(temporalHistory.begin());
                }
                if (valueHistory.size() > 1000) {
                    valueHistory.erase(valueHistory.begin());
                }
                
                // Calculate reward prediction error
                rewardPredictionError = calculateRewardPredictionError(weightedReward, averageValue);
                
                // Update expected value via TD learning
                expectedValue += learningRate * rewardPredictionError;
                
                // Update eligibility trace for credit assignment
                eligibilityTrace = eligibilityTrace * temporalDiscountFactor * (1.0f - learningRate) + weightedReward;
                
                // Update prediction error variance for exploration/exploitation balance
                predictionErrorVariance = 0.9f * predictionErrorVariance + 0.1f * (rewardPredictionError * rewardPredictionError);
                
                NLM_LOG_DEBUG("Reward computed: magnitude=" + std::to_string(weightedReward) +
                             ", primary=" + std::to_string(dimensions.primary) +
                             ", secondary=" + std::to_string(dimensions.secondary) +
                             ", conditioned=" + std::to_string(dimensions.conditioned) +
                             ", RPE=" + std::to_string(rewardPredictionError) +
                             ", expected=" + std::to_string(expectedValue));
            }
        }
    }
    
    // Update dopamine modulation based on reward processing
    void updateDopamineModulation() {
        // Phasic dopamine bursts for positive RPE (unexpected rewards)
        if (rewardPredictionError > 0.0f) {
            phasicComponent = std::min(phasicComponent + rewardPredictionError * 0.5f, 1.0f);
            dopamineModulation = std::min(dopamineModulation * 1.1f, 2.0f);
        }
        // Tonic reduction for negative RPE (unexpected omissions)
        else if (rewardPredictionError < -0.1f) {
            phasicComponent = std::max(phasicComponent - std::abs(rewardPredictionError) * 0.3f, 0.0f);
            dopamineModulation = std::max(dopamineModulation * 0.9f, 0.2f);
        }
        
        // Adaptive baseline mechanism (homeostatic regulation)
        adaptiveBaseline = 0.5f + 0.1f * std::sin(lastRewardTime * 0.1f); // Natural fluctuations
        tonicComponent = adaptiveBaseline * 0.7f + phasicComponent * 0.3f;
        
        // Apply neuromodulatory effects to reward processing
        rewardMagnitude *= dopamineModulation;
        currentValue *= dopamineModulation;
        
        // Record reward
        history.push_back(currentValue);
        if (history.size() > 1000) {
            history.erase(history.begin());
        }
    }
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

float Reward::getValue() const {
    return pImpl->currentValue;
}

void Reward::setValue(float value) {
    pImpl->currentValue = std::clamp(value, -1.0f, 1.0f);
    pImpl->expectedValue = value;
}

void Reward::add(float delta) {
    pImpl->accumulatedReward += delta;
    pImpl->currentValue += delta;
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    pImpl->totalValue = 0.0f;
    pImpl->averageValue = 0.0f;
    pImpl->dimensions = {0.0f, 0.0f, 0.0f};
    pImpl->temporalHistory.clear();
    pImpl->valueHistory.clear();
    pImpl->history.clear();
    pImpl->eligibilityTrace = 0.0f;
    pImpl->rewardPredictionError = 0.0f;
    pImpl->phasicComponent = 0.0f;
    pImpl->dopamineModulation = 1.0f;
    pImpl->rewardSum = 0.0f;
    pImpl->rewardCount = 0.0f;
    pImpl->positiveRewardCount = 0.0f;
    pImpl->negativeRewardCount = 0.0f;
}

float Reward::computeReward(const Observation& observation) const {
    // Check if observation is valid
    if (!observation.isValid()) {
        NLM_LOG_WARNING("Invalid observation provided to reward computation");
        return 0.0f;
    }
    
    // Store current time for temporal processing
    pImpl->updateTime();
    
    // Create a copy of observation for modification if needed
    auto tempObservation = observation.clone();
    
    // Compute reward with neuroscience-based mechanisms
    pImpl->updateRewardDimensions(*tempObservation);
    
    // Update dopamine modulation
    pImpl->updateDopamineModulation();
    
    // Return current reward value
    return pImpl->currentValue;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
    pImpl->temporalHistory.clear();
    pImpl->valueHistory.clear();
}

} // namespace nlm
