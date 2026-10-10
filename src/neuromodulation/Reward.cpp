#include "Reward.hpp"
#include "Dopamine.hpp"
#include "Curiosity.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../brain/AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <chrono>
#include <random>

namespace nlm {

struct Reward::Impl {
    // Core reward state
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    
    // Reward components
    float extrinsicReward;      // Environment-based rewards (food, water, etc.)
    float intrinsicReward;      // Internal state-based rewards (curiosity, exploration)
    float totalReward;          // Combined reward signal
    
    // Reward prediction and learning
    float predictedReward;      // Expected reward from prediction
    float predictionError;      // Difference between predicted and actual reward
    float temporalDifference;   // TD error for learning
    float discountFactor;       // Discount factor for future rewards
    
    // Eligibility traces for temporal difference learning
    float eligibilityTrace;     // Marks synapses eligible for plasticity
    float traceDecayRate;       // Decay rate for eligibility traces
    
    // Reward sensitivity and adaptation
    float rewardSensitivity;    // Modulation of reward response strength
    float baselineReward;       // Adaptive baseline for reward levels
    float sensitivityModulation; // Modulates reward sensitivity based on context
    
    // Reward statistics and analysis
    float historicalMean;       // Mean reward over recent history
    float historicalVariance;   // Variance in rewards
    float peakReward;           // Maximum observed reward
    float nadirReward;          // Minimum observed reward
    
    // Reward environment adaptation
    bool environmentChanging;   // Detection of changing reward environment
    float adaptationRate;       // Rate of adaptation to environmental changes
    Timestamp lastEnvironmentChange;
    
    // Integration with brain systems
    Dopamine* dopamine;         // Reference to dopamine system
    Curiosity* curiosity;       // Reference to curiosity system
    AgentBrain* brain;          // Reference to agent brain
    bool integrated;            // Integration state
    
    // Working memory for reward storage
    std::vector<RewardEntry> timeWeightedHistory; // Time-weighted reward entries
    std::vector<std::vector<float>> rewardContexts; // Contextual reward associations
    
    // Reward type configuration
    float extrinsicWeight;      // Weight for extrinsic rewards
    float intrinsicWeight;      // Weight for intrinsic rewards
    float explorationBonus;     // Bonus for exploratory behavior
    
    // Time tracking
    Timestamp currentTime;      // Current simulation time
    Timestamp lastUpdateTime;   // Last update timestamp
    
    // Random number generator for exploration bonuses
    std::mt19937 rng;
    std::uniform_real_distribution<float> rewardNoiseDist;
    
    Impl() : 
        currentValue(0.0f)
        , accumulatedReward(0.0f)
        , extrinsicReward(0.0f)
        , intrinsicReward(0.0f)
        , totalReward(0.0f)
        , predictedReward(0.0f)
        , predictionError(0.0f)
        , temporalDifference(0.0f)
        , discountFactor(0.95f)
        , eligibilityTrace(0.0f)
        , traceDecayRate(0.01f)
        , rewardSensitivity(1.0f)
        , baselineReward(0.0f)
        , sensitivityModulation(1.0f)
        , historicalMean(0.0f)
        , historicalVariance(0.0f)
        , peakReward(0.0f)
        , nadirReward(0.0f)
        , environmentChanging(false)
        , adaptationRate(0.01f)
        , lastEnvironmentChange(0.0)
        , dopamine(nullptr)
        , curiosity(nullptr)
        , brain(nullptr)
        , integrated(false)
        , extrinsicWeight(0.7f)
        , intrinsicWeight(0.3f)
        , explorationBonus(0.2f)
        , currentTime(0.0)
        , lastUpdateTime(0.0)
        , rng(std::chrono::steady_clock::now().time_since_epoch().count())
        , rewardNoiseDist(0.0f, 0.05f) {
    }
};

Reward::Reward() : pImpl(new Impl) {
    NLM_LOG_INFO("Reward system initialized with comprehensive computation");
}

Reward::~Reward() = default;

float Reward::getValue() const {
    return pImpl->currentValue;
}

void Reward::setValue(float value) {
    pImpl->currentValue = value;
}

void Reward::add(float delta) {
    pImpl->accumulatedReward += delta;
    pImpl->currentValue += delta;
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    pImpl->historicalMean = 0.0f;
    pImpl->historicalVariance = 0.0f;
    pImpl->environmentChanging = false;
}

float Reward::computeReward(const Observation& observation) {
    // Comprehensive reward computation integrating multiple sources
    pImpl->currentTime = std::chrono::duration<double>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    
    // Component rewards
    pImpl->extrinsicReward = computeStateReward(observation);
    pImpl->intrinsicReward = computeActionReward(Action(ActionType::Rest)); // Placeholder
    
    // Combine with weights
    pImpl->totalReward = pImpl->extrinsicWeight * pImpl->extrinsicReward +
                        pImpl->intrinsicWeight * pImpl->intrinsicReward;
    
    // Apply biological constraints and noise
    float noise = pImpl->rewardNoiseDist(pImpl->rng);
    float biologicallyConstrainedReward = std::max(0.0f, 
        pImpl->totalReward * pImpl->rewardSensitivity + noise);
    
    // Update historical tracking
    updateRewardHistory(biologicallyConstrainedReward);
    
    // Update predictions and learning
    float predictionError = computeRewardPredictionError(pImpl->predictedReward);
    updateTDLearning(biologicallyConstrainedReward, pImpl->predictedReward);
    
    // Signal to neuromodulators
    signalToNeuromodulators(biologicallyConstrainedReward, predictionError);
    
    return biologicallyConstrainedReward;
}

float Reward::computeOutcomeReward(const SensoryInput& sensoryInput, 
                                  const std::vector<float>& position,
                                  const Action& action,
                                  float outcome) const {
    // Compute reward based on environmental outcome and action
    float reward = 0.0f;
    
    // Distance-based reward (proximity to positive/negative entities)
    float distanceReward = 0.0f;
    if (!position.empty() && position.size() >= 2) {
        float distance = sqrt(position[0] * position[0] + position[1] * position[1]);
        if (distance < 1.0f) {
            distanceReward = 1.0f / (distance + 0.1f); // Inverse distance function
        }
    }
    
    // Action-specific reward
    float actionReward = 0.0f;
    switch (action.getType()) {
        case ActionType::Eat:
            actionReward = 1.0f; // Eating gives positive reward
            break;
        case ActionType::Drink:
            actionReward = 0.8f; // Drinking gives good reward
            break;
        case ActionType::Rest:
            actionReward = 0.5f; // Resting gives moderate reward
            break;
        case ActionType::Interact:
            actionReward = 0.3f; // Interaction gives lower reward
            break;
        default:
            actionReward = 0.1f; // Other actions give minimal reward
            break;
    }
    
    // Outcome-based reward
    float outcomeReward = outcome > 0.0f ? outcome : outcome * 0.5f;
    
    // Sensorial input contribution
    const std::vector<float>& sensorData = sensoryInput.getData();
    float sensoryReward = 0.0f;
    for (float value : sensorData) {
        sensoryReward += std::abs(value) * 0.01f; // Small contribution from sensory input
    }
    
    // Combine components
    reward = 0.4f * distanceReward + 
             0.3f * actionReward + 
             0.2f * outcomeReward + 
             0.1f * sensoryReward;
    
    // Apply sensitivity modulation
    reward *= pImpl->rewardSensitivity;
    
    return std::max(0.0f, std::min(1.0f, reward)); // Clamp to [0, 1]
}

float Reward::computeActionReward(const Action& action) const {
    // Compute action-based reward for intrinsic motivation
    float baseIntrinsicReward = 0.0f;
    
    switch (action.getType()) {
        case ActionType::MoveForward:
        case ActionType::MoveBackward:
        case ActionType::MoveLeft:
        case ActionType::MoveRight:
        case ActionType::TurnLeft:
        case ActionType::TurnRight:
            baseIntrinsicReward = 0.2f; // Movement exploration reward
            break;
        case ActionType::Look:
        case ActionType::LookUp:
        case ActionType::LookDown:
            baseIntrinsicReward = 0.3f; // Information gathering reward
            break;
        case ActionType::Interact:
            baseIntrinsicReward = 0.4f; // Interaction exploration reward
            break;
        case ActionType::Eat:
            baseIntrinsicReward = 0.6f; // Resource acquisition reward
            break;
        case ActionType::Drink:
            baseIntrinsicReward = 0.5f; // Survival reward
            break;
        case ActionType::Rest:
            baseIntrinsicReward = 0.1f; // Low intrinsic value for resting
            break;
        case ActionType::Wait:
            baseIntrinsicReward = 0.0f; // No intrinsic value for waiting
            break;
        default:
            baseIntrinsicReward = 0.15f; // Default intrinsic reward
            break;
    }
    
    // Add novelty bonus based on action history (simplified)
    float noveltyBonus = 0.1f; // Placeholder for actual novelty calculation
    
    return baseIntrinsicReward + noveltyBonus;
}

float Reward::computeStateReward(const Observation& observation) const {
    // Compute state-based reward from observation
    float stateReward = 0.0f;
    
    // Extract sensory input
    const SensoryInput* sensoryInput = observation.getSensoryInput();
    if (sensoryInput) {
        const std::vector<float>& sensorData = sensoryInput->getData();
        
        // Positive sensory input contributes to reward
        for (float value : sensorData) {
            if (value > 0.5f) {
                stateReward += (value - 0.5f) * 0.2f;
            }
        }
    }
    
    // Add some randomness for biological realism
    float noise = pImpl->rewardNoiseDist(pImpl->rng);
    stateReward += noise * 0.1f;
    
    return std::max(0.0f, std::min(1.0f, stateReward)); // Clamp to [0, 1]
}

float Reward::getMeanReward() const {
    if (pImpl->history.empty()) return 0.0f;
    return std::accumulate(pImpl->history.begin(), pImpl->history.end(), 0.0f) /
           static_cast<float>(pImpl->history.size());
}

float Reward::getVarianceReward() const {
    if (pImpl->history.empty()) return 0.0f;
    
    float mean = getMeanReward();
    float variance = 0.0f;
    for (float reward : pImpl->history) {
        variance += (reward - mean) * (reward - mean);
    }
    return variance / static_cast<float>(pImpl->history.size());
}

float Reward::getMaxReward() const {
    if (pImpl->history.empty()) return 0.0f;
    return *std::max_element(pImpl->history.begin(), pImpl->history.end());
}

float Reward::getMinReward() const {
    if (pImpl->history.empty()) return 0.0f;
    return *std::min_element(pImpl->history.begin(), pImpl->history.end());
}

float Reward::getRewardRange() const {
    return getMaxReward() - getMinReward();
}

float Reward::getDiscountedFutureReward(float discountFactor, int steps) const {
    float discountedReward = 0.0f;
    float gamma = discountFactor;
    
    for (int i = 0; i < steps && i < static_cast<int>(pImpl->history.size()); ++i) {
        int index = static_cast<int>(pImpl->history.size()) - 1 - i;
        discountedReward += pImpl->history[index] * std::pow(gamma, static_cast<float>(i));
    }
    
    return discountedReward;
}

float Reward::computeRewardPredictionError(float predictedReward) const {
    // Compute prediction error: difference between predicted and actual reward
    pImpl->predictedReward = predictedReward;
    pImpl->predictionError = pImpl->currentValue - predictedReward;
    return pImpl->predictionError;
}

void Reward::updateRewardHistory(float reward) {
    // Add reward to history
    pImpl->history.push_back(reward);
    
    // Maintain history size
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    // Update statistics
    updateStatistics();
    
    // Update baseline and detect environment changes
    updateAdaptationState(reward, pImpl->predictionError);
    detectRewardEnvironmentChange(0.1f);
}

void Reward::updateRewardPrediction(float newPrediction) {
    pImpl->predictedReward = newPrediction;
}

void Reward::adaptRewardSensitivity(float sensitivity) {
    pImpl->rewardSensitivity = std::clamp(sensitivity, 0.1f, 10.0f);
    pImpl->sensitivityModulation = pImpl->rewardSensitivity;
}

void Reward::integrateWithDopamine(Dopamine* dopamine) {
    pImpl->dopamine = dopamine;
    if (pImpl->dopamine) {
        NLM_LOG_INFO("Reward system integrated with dopamine system");
    }
}

void Reward::integrateWithCuriosity(Curiosity* curiosity) {
    pImpl->curiosity = curiosity;
    if (pImpl->curiosity) {
        NLM_LOG_INFO("Reward system integrated with curiosity system");
    }
}

void Reward::integrateWithBrain(AgentBrain* brain) {
    pImpl->brain = brain;
    pImpl->integrated = true;
    
    if (pImpl->brain) {
        NLM_LOG_INFO("Reward system integrated with agent brain");
        initializeRewardSystem(brain);
    }
}

void Reward::signalToNeuromodulators(float reward, float predictionError) {
    // Signal reward and prediction error to neuromodulators
    if (pImpl->dopamine) {
        // Scale reward for dopamine signaling
        float dopamineLevel = std::min(1.0f, reward * pImpl->rewardSensitivity * 2.0f);
        pImpl->dopamine->signalReward(dopamineLevel);
        pImpl->dopamine->signalRewardPredictionError(predictionError);
    }
    
    if (pImpl->curiosity) {
        // Update curiosity system based on reward and prediction error
        pImpl->curiosity->update(0.0f, predictionError, 0.001f); // Simplified
    }
}

void Reward::updateTDLearning(float reward, float predictedReward, float discountFactor) {
    // Temporal difference learning for reward prediction
    pImpl->discountFactor = discountFactor;
    
    // Compute temporal difference error
    pImpl->temporalDifference = reward + discountFactor * 0.0f - predictedReward;
    pImpl->predictionError = pImpl->temporalDifference;
    
    // Update eligibility trace
    pImpl->eligibilityTrace = pImpl->eligibilityTrace * (1.0f - pImpl->traceDecayRate) +
                              reward * predictionError * 0.1f;
    
    // Constrain eligibility trace to biological limits
    pImpl->eligibilityTrace = std::clamp(pImpl->eligibilityTrace, -1.0f, 1.0f);
}

float Reward::getTemporalDifference() const {
    return pImpl->temporalDifference;
}

float Reward::getEligibilityTrace() const {
    return pImpl->eligibilityTrace;
}

void Reward::modulateRewardSensitivity(float sensitivity) {
    pImpl->rewardSensitivity = std::clamp(sensitivity, 0.1f, 5.0f);
}

float Reward::getRewardSensitivity() const {
    return pImpl->rewardSensitivity;
}

float Reward::getExperienceBasedValue(const Observation& observation) const {
    // Simplified experience-based valuation
    float valuation = 0.0f;
    
    const SensoryInput* sensoryInput = observation.getSensoryInput();
    if (sensoryInput) {
        const std::vector<float>& sensorData = sensoryInput->getData();
        for (float value : sensorData) {
            valuation += value * 0.5f; // Weight sensory input
        }
    }
    
    return valuation;
}

void Reward::updateExperienceValue(const Observation& observation, float reward) {
    // Store experience-reward associations
    if (pImpl->brain) {
        // Integrate with working memory
        std::vector<float> observationFeatures;
        const SensoryInput* sensoryInput = observation.getSensoryInput();
        if (sensoryInput) {
            observationFeatures = sensoryInput->getData();
        }
        
        storeRewardInWorkingMemory(reward, observationFeatures);
    }
}

void Reward::detectRewardEnvironmentChange(float threshold) {
    // Simple environment change detection
    if (pImpl->history.size() >= 20) {
        float recentMean = 0.0f;
        int recentCount = std::min(20, static_cast<int>(pImpl->history.size()));
        
        for (int i = 0; i < recentCount; ++i) {
            int index = static_cast<int>(pImpl->history.size()) - 1 - i;
            recentMean += pImpl->history[index];
        }
        recentMean /= static_cast<float>(recentCount);
        
        float oldMean = pImpl->historicalMean;
        pImpl->historicalMean = recentMean;
        
        float change = std::abs(recentMean - oldMean);
        
        if (change > threshold) {
            pImpl->environmentChanging = true;
            pImpl->lastEnvironmentChange = pImpl->currentTime;
            NLM_LOG_INFO("Reward system detected environment change: " + 
                        std::to_string(change) + " > " + std::to_string(threshold));
        }
    }
}

bool Reward::isEnvironmentChanging() const {
    return pImpl->environmentChanging;
}

void Reward::resetAdaptationState() {
    pImpl->environmentChanging = false;
    pImpl->baselineReward = getMeanReward();
    pImpl->adaptationRate = 0.01f;
}

void Reward::integrateWithCuriositySystem(float curiosityLevel, float novelty) {
    // Integrate curiosity system with reward computation
    float explorationBonus = curiosityLevel * novelty * pImpl->explorationBonus;
    
    // Add exploration bonus to intrinsic reward
    pImpl->intrinsicReward += explorationBonus;
    
    // Update total reward
    pImpl->totalReward = pImpl->extrinsicWeight * pImpl->extrinsicReward +
                        pImpl->intrinsicWeight * pImpl->intrinsicReward;
}

float Reward::getExplorationBonus() const {
    return pImpl->explorationBonus;
}

float Reward::getExtrinsicReward() const {
    return pImpl->extrinsicReward;
}

float Reward::getIntrinsicReward() const {
    return pImpl->intrinsicReward;
}

void Reward::setExtrinsicWeight(float weight) {
    pImpl->extrinsicWeight = std::clamp(weight, 0.0f, 1.0f);
    pImpl->intrinsicWeight = 1.0f - pImpl->extrinsicWeight; // Keep total = 1
}

void Reward::setIntrinsicWeight(float weight) {
    pImpl->intrinsicWeight = std::clamp(weight, 0.0f, 1.0f);
    pImpl->extrinsicWeight = 1.0f - pImpl->intrinsicWeight; // Keep total = 1
}

float Reward::getHistoricalMean(int windowSize) const {
    if (pImpl->history.empty()) return 0.0f;
    
    int startIndex = static_cast<int>(pImpl->history.size()) - std::min(windowSize, static_cast<int>(pImpl->history.size()));
    if (startIndex < 0) startIndex = 0;
    
    float sum = 0.0f;
    int count = 0;
    
    for (int i = startIndex; i < static_cast<int>(pImpl->history.size()); ++i) {
        sum += pImpl->history[i];
        count++;
    }
    
    return count > 0 ? sum / static_cast<float>(count) : 0.0f;
}

float Reward::getTrend(float windowSize) const {
    if (pImpl->history.size() < 2) return 0.0f;
    
    int startIndex = static_cast<int>(pImpl->history.size()) - std::min(static_cast<int>(windowSize), static_cast<int>(pImpl->history.size()));
    if (startIndex < 0) startIndex = 0;
    
    if (startIndex >= static_cast<int>(pImpl->history.size()) - 1) return 0.0f;
    
    float oldValue = pImpl->history[startIndex];
    float newValue = pImpl->history.back();
    
    return (newValue - oldValue) / std::max(0.1f, static_cast<float>(windowSize));
}

bool Reward::detectRewardPattern(const std::vector<float>& pattern) const {
    // Simple pattern detection
    if (pImpl->history.size() < pattern.size()) return false;
    
    int historyIndex = static_cast<int>(pImpl->history.size()) - static_cast<int>(pattern.size());
    
    for (size_t i = 0; i < pattern.size(); ++i) {
        if (std::abs(pImpl->history[historyIndex + i] - pattern[i]) > 0.1f) {
            return false;
        }
    }
    
    return true;
}

float Reward::getTimeWeightedReward(Timestamp currentTime, float decayRate) const {
    float weightedReward = 0.0f;
    
    for (const auto& entry : pImpl->timeWeightedHistory) {
        float timeDiff = static_cast<float>(currentTime - entry.timestamp);
        float weight = std::exp(-decayRate * timeDiff);
        weightedReward += entry.value * weight;
    }
    
    return weightedReward;
}

void Reward::storeRewardInWorkingMemory(float reward, const std::vector<float>& context) {
    // Store reward-context associations in working memory
    pImpl->rewardContexts.push_back(context);
    
    // Maintain context size
    if (pImpl->rewardContexts.size() > 100) {
        pImpl->rewardContexts.erase(pImpl->rewardContexts.begin());
    }
}

std::vector<float> Reward::retrieveRelevantRewards(const std::vector<float>& context) const {
    // Find rewards associated with similar contexts
    std::vector<float> relevantRewards;
    
    for (const auto& ctx : pImpl->rewardContexts) {
        // Simple similarity check (cosine similarity would be better)
        float similarity = 0.0f;
        if (ctx.size() == context.size() && !ctx.empty()) {
            for (size_t i = 0; i < ctx.size(); ++i) {
                similarity += std::abs(ctx[i] - context[i]);
            }
            similarity = 1.0f - (similarity / static_cast<float>(ctx.size()));
            
            if (similarity > 0.7f) { // Threshold for relevance
                relevantRewards.push_back(similarity);
            }
        }
    }
    
    return relevantRewards;
}

void Reward::predictNextReward(float predictedValue) {
    pImpl->predictedReward = predictedValue;
}

float Reward::getPredictedReward() const {
    return pImpl->predictedReward;
}

float Reward::getCurrentPredictionError() const {
    return pImpl->predictionError;
}

Reward::AdaptationState Reward::getAdaptationState() const {
    AdaptationState state;
    state.baselineReward = pImpl->baselineReward;
    state.sensitivity = pImpl->rewardSensitivity;
    state.adaptationRate = pImpl->adaptationRate;
    state.lastAdaptation = pImpl->lastEnvironmentChange;
    return state;
}

void Reward::updateAdaptationState(float reward, float predictionError) {
    // Update adaptation state based on reward and prediction error
    float adaptationAmount = pImpl->adaptationRate * std::abs(predictionError);
    
    pImpl->baselineReward += adaptationAmount * (reward - pImpl->baselineReward);
    pImpl->lastAdaptation = pImpl->currentTime;
    
    // Detect if environment is changing
    if (std::abs(reward - pImpl->baselineReward) > 0.1f) {
        pImpl->environmentChanging = true;
    }
}

void Reward::configure(float extrinsicWeight, float intrinsicWeight,
                       float discountFactor, float sensitivity) {
    setExtrinsicWeight(extrinsicWeight);
    setIntrinsicWeight(intrinsicWeight);
    pImpl->discountFactor = discountFactor;
    adaptRewardSensitivity(sensitivity);
}

void Reward::initializeRewardSystem(AgentBrain* brain) {
    if (brain) {
        integrateWithBrain(brain);
        
        // Initialize reward prediction based on brain state
        predictNextReward(0.5f); // Initial prediction
        
        NLM_LOG_INFO("Reward system initialization complete");
    }
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
    updateStatistics();
}

void Reward::updateStatistics() {
    // Update historical statistics
    if (pImpl->history.empty()) return;
    
    pImpl->historicalMean = getMeanReward();
    pImpl->historicalVariance = getVarianceReward();
    pImpl->peakReward = getMaxReward();
    pImpl->nadirReward = getMinReward();
}

} // namespace nlm
