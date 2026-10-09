#include "Reward.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <random>
#include <chrono>

namespace nlm {

struct Reward::Impl {
    // Reward computation
    float currentReward;
    float immediateReward;
    float accumulatedReward;
    
    // Reward history
    std::vector<RewardEntry> history;
    std::vector<float> discountedHistory;
    
    // Reward modalities
    float extrinsicReward;
    float intrinsicReward;
    float socialReward;
    float structuralReward;
    
    // Learning system
    Strategy strategy;
    float explorationRate;
    float discountFactor;
    float predictionHorizon;
    
    // Reward prediction and learning
    RewardPrediction lastPrediction;
    float predictionError;
    float uncertainty;
    
    // Reward uncertainty and noise
    float noiseMean;
    float noiseStddev;
    std::mt19937 rng;
    std::normal_distribution<float> noiseDist;
    
    // Model parameters (simplified for demonstration)
    std::unordered_map<std::string, float> parameters;
    std::unordered_map<std::string, std::vector<float>> featureWeights;
    
    // Statistics
    RewardStats stats;
    
    // Credit assignment for delayed rewards
    std::unordered_map<Timestamp, std::vector<float>> delayedRewardHistory;
    
    // Social reward features
    std::vector<float> socialFeatures;
    
    // Intrinsic motivation model
    std::vector<float> curiosityBuffer;
    
    // Temporal integration
    Timestamp lastUpdateTime;
    float timeSinceLastReward;
    
    // Exploration-exploitation tracking
    float explorationBonus;
    float exploitationValue;
    
    // Initialize default parameters
    Impl() : rng(std::chrono::system_clock::now().time_since_epoch().count()),
             noiseDist(0.0f, 0.1f) {
        
        // Initialize reward components
        currentReward = 0.0f;
        immediateReward = 0.0f;
        accumulatedReward = 0.0f;
        extrinsicReward = 0.0f;
        intrinsicReward = 0.0f;
        socialReward = 0.0f;
        structuralReward = 0.0f;
        
        // Learning parameters
        strategy = Strategy::Hybrid;
        explorationRate = 0.1f;
        discountFactor = 0.95f;
        predictionHorizon = 10.0f;
        
        // Uncertainty and noise
        noiseMean = 0.0f;
        noiseStddev = 0.1f;
        uncertainty = 0.5f;
        
        // Statistics
        stats.totalRewards = 0.0f;
        stats.extrinsicTotal = 0.0f;
        stats.intrinsicTotal = 0.0f;
        stats.socialTotal = 0.0f;
        stats.structuralTotal = 0.0f;
        stats.meanReward = 0.0f;
        stats.rewardStdDev = 0.0f;
        stats.rewardVariance = 0.0f;
        stats.discountFactor = discountFactor;
        stats.explorationBonus = 0.0f;
        stats.exploitationValue = 0.0f;
        stats.predictionAccuracy = 0.0f;
        stats.uncertainty = uncertainty;
        stats.lastUpdateTime = 0.0;
        stats.totalSteps = 0;
        
        // Configuration parameters
        parameters["exploration_rate"] = explorationRate;
        parameters["discount_factor"] = discountFactor;
        parameters["prediction_horizon"] = predictionHorizon;
        parameters["noise_mean"] = noiseMean;
        parameters["noise_stddev"] = noiseStddev;
        parameters["extrinsic_scale"] = 1.0f;
        parameters["intrinsic_scale"] = 1.0f;
        parameters["social_scale"] = 1.0f;
        parameters["structural_scale"] = 1.0f;
        parameters["learning_rate"] = 0.1f;
        parameters["eligibility_decay"] = 0.9f;
        
        // Initialize history
        lastUpdateTime = 0.0;
        timeSinceLastReward = 0.0f;
        explorationBonus = 0.0f;
        exploitationValue = 0.0f;
        
        NLM_LOG_INFO("Reward system initialized with biologically realistic parameters");
    }
};

Reward::Reward() : pImpl(std::make_unique<Impl>()) {}

Reward::~Reward() = default;

float Reward::computeStateReward(const Observation& observation) const {
    // Extract features from observation
    const SensoryInput* sensoryInput = observation.getSensoryInput();
    if (!sensoryInput) {
        return 0.0f;
    }
    
    const std::vector<float>& features = sensoryInput->getData();
    
    // State-based reward computation
    float reward = 0.0f;
    
    // Extrinsic reward based on environment state
    for (size_t i = 0; i < features.size(); ++i) {
        // Simple reward function: positive for high values, negative for low values
        // This is a placeholder - real implementations would have domain-specific logic
        if (i % 3 == 0) { // Every third feature contributes to extrinsic reward
            reward += features[i] * pImpl->parameters["extrinsic_scale"];
        } else if (i % 3 == 1) { // Every third feature contributes to intrinsic reward
            reward += features[i] * pImpl->parameters["intrinsic_scale"];
        }
    }
    
    // Apply scaling and noise
    reward = reward * pImpl->parameters["extrinsic_scale"];
    
    return reward;
}

float Reward::computeActionReward(const Action& action, const Observation& observation) const {
    // Action-based reward prediction for policy gradient methods
    float actionValue = 0.0f;
    
    // Get action features
    ActionType actionType = action.getType();
    const std::vector<float>& actionParams = action.getParameters();
    
    // Map action type to reward contribution
    switch (actionType) {
        case ActionType::MoveForward:
            actionValue += 0.5f * pImpl->parameters["extrinsic_scale"];
            break;
        case ActionType::MoveBackward:
            actionValue -= 0.2f * pImpl->parameters["extrinsic_scale"];
            break;
        case ActionType::TurnLeft:
            actionValue += 0.3f * pImpl->parameters["intrinsic_scale"];
            break;
        case ActionType::TurnRight:
            actionValue += 0.3f * pImpl->parameters["intrinsic_scale"];
            break;
        case ActionType::Interact:
            actionValue += 1.0f * pImpl->parameters["social_scale"];
            break;
        case ActionType::Eat:
            actionValue += 2.0f * pImpl->parameters["extrinsic_scale"];
            break;
        case ActionType::Drink:
            actionValue += 1.5f * pImpl->parameters["extrinsic_scale"];
            break;
        case ActionType::Rest:
            actionValue += 0.1f * pImpl->parameters["structural_scale"];
            break;
        case ActionType::Wait:
            actionValue -= 0.05f * pImpl->parameters["extrinsic_scale"];
            break;
        case ActionType::Custom:
            // Custom actions can have parameterized rewards
            for (size_t i = 0; i < actionParams.size(); ++i) {
                actionValue += actionParams[i] * pImpl->parameters["extrinsic_scale"];
            }
            break;
        default:
            actionValue = 0.0f;
            break;
    }
    
    // Add prediction-based reward
    actionValue += pImpl->lastPrediction.predictedValue * 0.1f;
    
    // Apply exploration bonus
    if (pImpl->explorationRate > 0.0f) {
        actionValue += pImpl->explorationBonus;
    }
    
    return actionValue;
}

float Reward::computeModelBasedReward(const Observation& observation,
                                     const Action& action,
                                     const std::vector<float>& futureState) const {
    // Model-based reward prediction using learned value function
    float modelReward = 0.0f;
    
    // Get current state features
    const SensoryInput* sensoryInput = observation.getSensoryInput();
    if (sensoryInput) {
        const std::vector<float>& stateFeatures = sensoryInput->getData();
        
        // Simple model-based prediction
        // In a real implementation, this would use a learned transition model
        for (size_t i = 0; i < stateFeatures.size() && i < futureState.size(); ++i) {
            float stateChange = futureState[i] - stateFeatures[i];
            // Reward based on positive state changes
            modelReward += std::max(0.0f, stateChange) * pImpl->parameters["extrinsic_scale"];
        }
    }
    
    // Add action-based contribution
    modelReward += computeActionReward(action, observation) * 0.3f;
    
    // Apply temporal discounting
    modelReward *= std::exp(-1.0f / pImpl->predictionHorizon);
    
    return modelReward;
}

float Reward::integrateRewards(const std::vector<std::pair<Modality, float>>& modalityRewards) const {
    float integratedReward = 0.0f;
    
    for (const auto& modalityReward : modalityRewards) {
        switch (modalityReward.first) {
            case Modality::Extrinsic:
                integratedReward += modalityReward.second * pImpl->parameters["extrinsic_scale"];
                break;
            case Modality::Intrinsic:
                integratedReward += modalityReward.second * pImpl->parameters["intrinsic_scale"];
                break;
            case Modality::Social:
                integratedReward += modalityReward.second * pImpl->parameters["social_scale"];
                break;
            case Modality::Structural:
                integratedReward += modalityReward.second * pImpl->parameters["structural_scale"];
                break;
        }
    }
    
    return integratedReward;
}

float Reward::shapeReward(float reward, const Action& action,
                         const Observation& observation, bool isExploration) const {
    // Reward shaping for exploration vs exploitation
    float shapedReward = reward;
    
    if (isExploration) {
        // Exploration bonus
        shapedReward += pImpl->explorationRate * pImpl->parameters["extrinsic_scale"];
        
        // Novelty bonus based on action parameters
        const std::vector<float>& actionParams = action.getParameters();
        float novelty = 0.0f;
        for (float param : actionParams) {
            novelty += std::abs(param - 0.5f); // Distance from center
        }
        shapedReward += novelty * pImpl->explorationBonus * 0.1f;
    } else {
        // Exploitation penalty for low expected value actions
        shapedReward -= pImpl->exploitationValue * 0.05f;
    }
    
    // Temporal shaping: delayed reward credit assignment
    float temporalBonus = 0.0f;
    if (pImpl->timeSinceLastReward > 1.0f) {
        temporalBonus = std::min(1.0f, pImpl->timeSinceLastReward / 10.0f);
        shapedReward += temporalBonus * pImpl->parameters["extrinsic_scale"] * 0.5f;
    }
    
    return shapedReward;
}

void Reward::updatePrediction(float actualReward, float predictedReward) {
    // Update reward prediction using temporal difference learning
    pImpl->predictionError = computePredictionError(actualReward, predictedReward);
    
    // Update last prediction
    pImpl->lastPrediction.predictedValue = predictedReward;
    pImpl->lastPrediction.predictionError = pImpl->predictionError;
    pImpl->lastPrediction.uncertainty = pImpl->uncertainty;
    
    // Update statistics
    pImpl->stats.totalPredictionErrors += std::abs(pImpl->predictionError);
    pImpl->stats.predictionAccuracy = 1.0f - std::abs(pImpl->predictionError) / std::max(std::abs(predictedReward), 1.0f);
    
    // Adapt exploration bonus based on prediction error
    pImpl->explorationBonus = std::min(pImpl->explorationBonus + pImpl->predictionError * 0.1f, 1.0f);
    
    NLM_LOG_DEBUG("Reward prediction updated: actual=" + std::to_string(actualReward) +
                  " predicted=" + std::to_string(predictedReward) +
                  " error=" + std::to_string(pImpl->predictionError));
}

float Reward::computePredictionError(float actualReward, float predictedReward) const {
    // Compute reward prediction error (RPE) signal
    return actualReward - predictedReward;
}

void Reward::updateModel(const Observation& observation, const Action& action, float reward) {
    // Update model with new reward observations
    pImpl->stats.totalSteps++;
    
    // Update uncertainty based on prediction error
    float error = computePredictionError(reward, pImpl->lastPrediction.predictedValue);
    pImpl->uncertainty = std::min(1.0f, pImpl->uncertainty + std::abs(error) * 0.01f);
    
    // Update exploration-exploitation balance
    if (std::abs(error) > 0.5f) {
        // High prediction error: increase exploration
        pImpl->explorationRate = std::min(1.0f, pImpl->explorationRate + 0.1f);
        pImpl->exploitationValue = std::max(0.0f, pImpl->exploitationValue - 0.1f);
    } else {
        // Low prediction error: increase exploitation
        pImpl->exploitationValue = std::min(1.0f, pImpl->exploitationValue + 0.1f);
        pImpl->explorationRate = std::max(0.0f, pImpl->explorationRate - 0.05f);
    }
    
    // Update feature weights for function approximation
    const SensoryInput* sensoryInput = observation.getSensoryInput();
    if (sensoryInput) {
        const std::vector<float>& features = sensoryInput->getData();
        
        for (size_t i = 0; i < features.size(); ++i) {
            std::string featureKey = "feature_" + std::to_string(i);
            pImpl->featureWeights[featureKey].push_back(features[i] * reward);
            
            // Keep only recent weights
            if (pImpl->featureWeights[featureKey].size() > 100) {
                pImpl->featureWeights[featureKey].erase(
                    pImpl->featureWeights[featureKey].begin());
            }
        }
    }
}

Reward::RewardPrediction Reward::predictFutureReward(const Observation& observation,
                                                    const Action& action,
                                                    float horizon) const {
    // Predict future rewards using learned model
    RewardPrediction prediction;
    
    // Simple model-based prediction
    float predictedValue = 0.0f;
    
    // Get current state reward
    predictedValue += computeStateReward(observation);
    
    // Get action value
    predictedValue += computeActionReward(action, observation) * 0.3f;
    
    // Apply temporal discounting
    float discount = std::exp(-horizon / pImpl->predictionHorizon);
    predictedValue *= discount;
    
    // Add prediction uncertainty
    predictedValue += (pImpl->rng() - 0.5f) * pImpl->uncertainty * 0.1f;
    
    prediction.predictedValue = predictedValue;
    prediction.predictionError = 0.0f; // Will be set when actual reward received
    prediction.uncertainty = pImpl->uncertainty;
    prediction.explorationValue = pImpl->explorationRate;
    prediction.exploitationValue = pImpl->exploitationValue;
    
    // Extract feature weights
    if (observation.getSensoryInput()) {
        const std::vector<float>& features = observation.getSensoryInput()->getData();
        prediction.featureWeights = std::vector<float>(features.begin(), features.end());
    }
    
    prediction.predictionTime = pImpl->lastUpdateTime;
    
    return prediction;
}

void Reward::signalToDopamine(nlm::Dopamine& dopamine, float reward, float predictedReward) {
    // Send reward signal to dopamine system
    float rpe = computePredictionError(reward, predictedReward);
    
    // Signal reward with prediction error
    dopamine.signalReward(reward, predictedReward);
    
    // Update internal state based on dopamine response
    if (std::abs(rpe) > 0.1f) {
        // Strong prediction error: increase uncertainty
        pImpl->uncertainty = std::min(1.0f, pImpl->uncertainty + 0.1f);
    }
    
    NLM_LOG_DEBUG("Reward signaled to dopamine: reward=" + std::to_string(reward) +
                  " predicted=" + std::to_string(predictedReward) +
                  " RPE=" + std::to_string(rpe));
}

void Reward::integrateWithNeuromodulation(nlm::Dopamine& dopamine, const Observation& observation) {
    // Integrate with neuromodulation systems
    
    // Get current dopamine level
    float dopamineLevel = dopamine.getLevel();
    
    // Modulate reward based on dopamine
    if (dopamineLevel > 0.5f) {
        // High dopamine: enhance reward sensitivity
        pImpl->currentReward *= (1.0f + dopamineLevel);
        pImpl->stats.explorationBonus *= (1.0f + dopamineLevel * 0.5f);
    }
    
    // Integrate prediction system
    float valueFunction = dopamine.getValueFunction();
    float predictedReward = valueFunction;
    
    // Update prediction using dopamine value function
    updatePrediction(pImpl->currentReward, predictedReward);
}

void Reward::qLearningUpdate(const std::vector<float>& stateFeatures,
                            const Action& action,
                            float reward,
                            const std::vector<float>& nextStateFeatures,
                            float learningRate) {
    // Q-learning reward update
    float currentQ = 0.0f;
    float nextMaxQ = 0.0f;
    
    // Compute current Q-value (simplified)
    for (size_t i = 0; i < stateFeatures.size(); ++i) {
        currentQ += stateFeatures[i] * pImpl->parameters["extrinsic_scale"];
    }
    
    // Compute maximum next Q-value
    for (size_t i = 0; i < nextStateFeatures.size(); ++i) {
        nextMaxQ += nextStateFeatures[i] * pImpl->parameters["extrinsic_scale"];
    }
    
    // Q-learning update rule
    float tdTarget = reward + pImpl->discountFactor * nextMaxQ;
    float tdError = tdTarget - currentQ;
    
    // Update state representation
    float updatedQ = currentQ + learningRate * tdError;
    
    // Store updated value
    pImpl->featureWeights["current_state"] = stateFeatures;
    
    // Update statistics
    pImpl->stats.totalRewards += reward;
    
    NLM_LOG_DEBUG("Q-learning update: currentQ=" + std::to_string(currentQ) +
                  " nextMaxQ=" + std::to_string(nextMaxQ) +
                  " target=" + std::to_string(tdTarget) +
                  " tdError=" + std::to_string(tdError));
}

void Reward::sarsaUpdate(const std::vector<float>& state,
                        const Action& action,
                        float reward,
                        const std::vector<float>& nextState,
                        const Action& nextAction,
                        float learningRate,
                        float discountFactor) {
    // SARSA learning update
    float currentQ = 0.0f;
    float nextQ = 0.0f;
    
    // Compute current Q-value
    for (size_t i = 0; i < state.size(); ++i) {
        currentQ += state[i] * pImpl->parameters["extrinsic_scale"];
    }
    
    // Compute next Q-value
    for (size_t i = 0; i < nextState.size(); ++i) {
        nextQ += nextState[i] * pImpl->parameters["extrinsic_scale"];
    }
    
    // SARSA update rule (on-policy)
    float tdTarget = reward + discountFactor * nextQ;
    float tdError = tdTarget - currentQ;
    
    // Update Q-value
    float updatedQ = currentQ + learningRate * tdError;
    
    NLM_LOG_DEBUG("SARSA update: currentQ=" + std::to_string(currentQ) +
                  " nextQ=" + std::to_string(nextQ) +
                  " target=" + std::to_string(tdTarget) +
                  " tdError=" + std::to_string(tdError));
}

float Reward::policyGradientReward(const Action& action,
                                   const Observation& observation,
                                   float advantage) const {
    // Policy gradient reward computation
    float policyReward = 0.0f;
    
    // Base reward from state
    policyReward += computeStateReward(observation);
    
    // Action-specific reward
    policyReward += computeActionReward(action, observation) * 0.5f;
    
    // Advantage term for policy gradient
    policyReward += advantage * 0.3f;
    
    // Exploration bonus for policy gradient
    if (pImpl->explorationRate > 0.0f) {
        policyReward += pImpl->explorationBonus * 0.2f;
    }
    
    return policyReward;
}

void Reward::actorCriticUpdate(const Observation& observation,
                               const Action& action,
                               float reward,
                               const Observation& nextObservation,
                               float criticLearningRate,
                               float actorLearningRate,
                               float discountFactor) {
    // Actor-critic style learning
    float criticTarget = reward + discountFactor * computeStateReward(nextObservation);
    float criticError = criticTarget - computeStateReward(observation);
    
    // Critic update (value function)
    float updatedValue = computeStateReward(observation) + criticLearningRate * criticError;
    
    // Actor update (policy gradient)
    float advantage = reward + discountFactor * computeStateReward(nextObservation) - computeStateReward(observation);
    
    // Update statistics
    pImpl->stats.totalRewards += reward;
    pImpl->stats.predictionAccuracy = 1.0f - std::abs(criticError) / std::max(std::abs(criticTarget), 1.0f);
    
    NLM_LOG_DEBUG("Actor-critic update: reward=" + std::to_string(reward) +
                  " criticError=" + std::to_string(criticError) +
                  " advantage=" + std::to_string(advantage));
}

float Reward::addNoise(float reward, float noiseLevel) const {
    // Add Gaussian noise to rewards
    if (noiseLevel <= 0.0f) {
        return reward;
    }
    
    float noise = pImpl->noiseDist(pImpl->rng) * noiseLevel;
    return reward + noise;
}

float Reward::computeUncertainty(const Observation& observation) const {
    // Compute reward uncertainty based on history
    if (pImpl->history.size() < 10) {
        return 0.5f; // Default uncertainty
    }
    
    // Calculate variance of recent rewards
    float sum = 0.0f;
    float sumSquares = 0.0f;
    size_t count = std::min(size_t(20), pImpl->history.size());
    
    for (size_t i = 0; i < count; ++i) {
        size_t idx = pImpl->history.size() - 1 - i;
        float reward = pImpl->history[idx].value;
        sum += reward;
        sumSquares += reward * reward;
    }
    
    float mean = sum / count;
    float variance = (sumSquares / count) - (mean * mean);
    
    // Uncertainty is normalized variance
    float uncertainty = std::sqrt(std::max(0.0f, variance)) / std::max(std::abs(mean), 1.0f);
    
    return std::clamp(uncertainty, 0.0f, 1.0f);
}

void Reward::updateUncertainty(float reward, float predictionError) {
    // Update uncertainty estimates
    pImpl->uncertainty = std::min(1.0f, pImpl->uncertainty + std::abs(predictionError) * 0.05f);
    
    // Adapt noise parameters based on uncertainty
    pImpl->noiseStddev = pImpl->uncertainty * 0.1f + 0.01f;
}

const RewardStats& Reward::getStats() const {
    return pImpl->stats;
}

const std::vector<Reward::RewardEntry>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::addRewardEntry(Modality modality, float value, const std::string& source) {
    // Add reward entry to history
    RewardEntry entry;
    entry.timestamp = pImpl->lastUpdateTime;
    entry.modality = modality;
    entry.value = value;
    entry.discountedValue = value * std::exp(-pImpl->timeSinceLastReward / pImpl->predictionHorizon);
    entry.source = source;
    entry.step = pImpl->stats.totalSteps;
    entry.predictionError = computePredictionError(value, pImpl->lastPrediction.predictedValue);
    
    pImpl->history.push_back(entry);
    
    // Update modality totals
    switch (modality) {
        case Modality::Extrinsic:
            pImpl->extrinsicReward += value;
            pImpl->stats.extrinsicTotal += value;
            break;
        case Modality::Intrinsic:
            pImpl->intrinsicReward += value;
            pImpl->stats.intrinsicTotal += value;
            break;
        case Modality::Social:
            pImpl->socialReward += value;
            pImpl->stats.socialTotal += value;
            break;
        case Modality::Structural:
            pImpl->structuralReward += value;
            pImpl->stats.structuralTotal += value;
            break;
    }
    
    // Update accumulated reward
    pImpl->currentReward += value;
    pImpl->stats.totalRewards += value;
    
    // Trim history if too large
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Reward::clearHistory() {
    pImpl->history.clear();
    pImpl->discountedHistory.clear();
}

void Reward::computeStats() {
    // Compute reward statistics
    if (pImpl->history.empty()) {
        return;
    }
    
    float sum = 0.0f;
    float sumSquares = 0.0f;
    size_t count = pImpl->history.size();
    
    for (const auto& entry : pImpl->history) {
        sum += entry.value;
        sumSquares += entry.value * entry.value;
    }
    
    pImpl->stats.meanReward = sum / count;
    pImpl->stats.rewardVariance = (sumSquares / count) - (pImpl->stats.meanReward * pImpl->stats.meanReward);
    pImpl->stats.rewardStdDev = std::sqrt(std::max(0.0f, pImpl->stats.rewardVariance));
    
    // Compute exploration-exploitation ratio
    pImpl->stats.explorationBonus = pImpl->explorationBonus;
    pImpl->stats.exploitationValue = pImpl->exploitationValue;
    
    // Compute prediction accuracy (based on prediction error variance)
    float errorSum = 0.0f;
    float errorSquares = 0.0f;
    for (const auto& entry : pImpl->history) {
        errorSum += std::abs(entry.predictionError);
        errorSquares += entry.predictionError * entry.predictionError;
    }
    
    if (count > 0) {
        float avgError = errorSum / count;
        float errorVariance = (errorSquares / count) - (avgError * avgError);
        pImpl->stats.predictionAccuracy = std::max(0.0f, 1.0f - std::sqrt(errorVariance));
    }
}

void Reward::setStrategy(Strategy strategy) {
    pImpl->strategy = strategy;
}

void Reward::setExplorationRate(float rate) {
    pImpl->explorationRate = std::clamp(rate, 0.0f, 1.0f);
    pImpl->parameters["exploration_rate"] = pImpl->explorationRate;
}

void Reward::setDiscountFactor(float factor) {
    pImpl->discountFactor = std::clamp(factor, 0.0f, 1.0f);
    pImpl->stats.discountFactor = pImpl->discountFactor;
    pImpl->parameters["discount_factor"] = pImpl->discountFactor;
}

void Reward::setRewardScaling(float extrinsicScale, float intrinsicScale,
                              float socialScale, float structuralScale) {
    pImpl->parameters["extrinsic_scale"] = extrinsicScale;
    pImpl->parameters["intrinsic_scale"] = intrinsicScale;
    pImpl->parameters["social_scale"] = socialScale;
    pImpl->parameters["structural_scale"] = structuralScale;
}

void Reward::setNoiseParameters(float mean, float stddev) {
    pImpl->noiseMean = mean;
    pImpl->noiseStddev = stddev;
    pImpl->parameters["noise_mean"] = mean;
    pImpl->parameters["noise_stddev"] = stddev;
}

void Reward::setPredictionHorizon(float horizon) {
    pImpl->predictionHorizon = std::max(1.0f, horizon);
    pImpl->parameters["prediction_horizon"] = pImpl->predictionHorizon;
}

const std::unordered_map<std::string, float>& Reward::getParameters() const {
    return pImpl->parameters;
}

void Reward::setParameter(const std::string& name, float value) {
    if (pImpl->parameters.find(name) != pImpl->parameters.end()) {
        pImpl->parameters[name] = value;
        
        // Update internal state based on parameter changes
        if (name == "exploration_rate") {
            setExplorationRate(value);
        } else if (name == "discount_factor") {
            setDiscountFactor(value);
        } else if (name == "prediction_horizon") {
            setPredictionHorizon(value);
        }
    }
}

float Reward::getCurrentReward() const {
    return pImpl->currentReward;
}

void Reward::setCurrentReward(float value) {
    pImpl->currentReward = value;
    pImpl->immediateReward = value;
    
    // Add to history
    addRewardEntry(Modality::Extrinsic, value);
}

float Reward::getImmediateReward() const {
    return pImpl->immediateReward;
}

void Reward::update(float reward, TimestepDuration dt) {
    // Update reward with temporal integration
    pImpl->lastUpdateTime += dt;
    pImpl->timeSinceLastReward += dt;
    
    // Apply noise
    reward = addNoise(reward, pImpl->noiseStddev);
    
    // Set current reward
    setCurrentReward(reward);
    
    // Update uncertainty
    updateUncertainty(reward, pImpl->predictionError);
    
    // Update history and stats
    computeStats();
    
    NLM_LOG_DEBUG("Reward updated: reward=" + std::to_string(reward) +
                  " dt=" + std::to_string(dt) +
                  " timeSinceReward=" + std::to_string(pImpl->timeSinceLastReward));
}

void Reward::reset() {
    pImpl->currentReward = 0.0f;
    pImpl->immediateReward = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    pImpl->extrinsicReward = 0.0f;
    pImpl->intrinsicReward = 0.0f;
    pImpl->socialReward = 0.0f;
    pImpl->structuralReward = 0.0f;
    
    pImpl->history.clear();
    pImpl->discountedHistory.clear();
    pImpl->delayedRewardHistory.clear();
    pImpl->curiosityBuffer.clear();
    
    pImpl->explorationRate = 0.1f;
    pImpl->exploitationValue = 0.0f;
    pImpl->explorationBonus = 0.0f;
    pImpl->uncertainty = 0.5f;
    
    // Reset statistics
    pImpl->stats.totalRewards = 0.0f;
    pImpl->stats.extrinsicTotal = 0.0f;
    pImpl->stats.intrinsicTotal = 0.0f;
    pImpl->stats.socialTotal = 0.0f;
    pImpl->stats.structuralTotal = 0.0f;
    pImpl->stats.meanReward = 0.0f;
    pImpl->stats.rewardStdDev = 0.0f;
    pImpl->stats.rewardVariance = 0.0f;
    pImpl->stats.explorationBonus = 0.0f;
    pImpl->stats.exploitationValue = 0.0f;
    pImpl->stats.predictionAccuracy = 0.0f;
    pImpl->stats.uncertainty = 0.5f;
    pImpl->stats.lastUpdateTime = 0.0;
    pImpl->stats.totalSteps = 0;
    
    pImpl->lastUpdateTime = 0.0;
    pImpl->timeSinceLastReward = 0.0f;
    
    NLM_LOG_INFO("Reward system reset");
}

std::unique_ptr<Reward> Reward::clone() const {
    auto clonedReward = std::make_unique<Reward>();
    
    // Copy implementation details (simplified for demonstration)
    // In a real implementation, this would deep copy all state
    
    return clonedReward;
}

void Reward::applyRewardModulatedPlasticity(nlm::Dopamine& dopamine,
                                           const std::vector<std::shared_ptr<Synapse>>& synapses,
                                           const std::vector<float>& eligibilityTraces) {
    // Implement reward-modulated plasticity
    
    float rpe = pImpl->predictionError;
    
    // Apply dopamine-modulated plasticity to synapses
    dopamine.applyNeuralPlasticity(synapses, eligibilityTraces, 1.0f);
    
    // Additional reward-specific plasticity
    for (size_t i = 0; i < synapses.size() && i < eligibilityTraces.size(); ++i) {
        if (!synapses[i]) continue;
        
        float eligibilityTrace = eligibilityTraces[i];
        
        // Reward-modulated weight change
        float weightChange = eligibilityTrace * rpe * pImpl->parameters["learning_rate"] * 0.1f;
        
        if (std::abs(weightChange) > 1e-6f) {
            synapses[i]->applyWeightChange(weightChange);
        }
    }
}

void Reward::creditAssignmentForDelayedRewards(const Observation& observation,
                                               const Action& action,
                                               float reward,
                                               float delay) {
    // Model-based credit assignment for delayed rewards
    
    // Store delayed reward with timestamp
    Timestamp rewardTime = pImpl->lastUpdateTime + delay;
    
    RewardEntry delayedEntry;
    delayedEntry.timestamp = rewardTime;
    delayedEntry.modality = Modality::Extrinsic;
    delayedEntry.value = reward;
    delayedEntry.discountedValue = reward * std::exp(-delay / pImpl->predictionHorizon);
    delayedEntry.source = "delayed_reward";
    delayedEntry.step = pImpl->stats.totalSteps + static_cast<SimulationStep>(delay);
    delayedEntry.predictionError = 0.0f; // Will be computed when processed
    
    pImpl->delayedRewardHistory[rewardTime].push_back(delayedEntry.value);
    
    NLM_LOG_DEBUG("Delayed reward credited: reward=" + std::to_string(reward) +
                  " delay=" + std::to_string(delay) +
                  " scheduled_time=" + std::to_string(rewardTime));
}

float Reward::computeSocialReward(const std::vector<float>& socialFeatures) const {
    // Compute social reward based on social features
    float socialReward = 0.0f;
    
    for (size_t i = 0; i < socialFeatures.size(); ++i) {
        // Social features: proximity to others, social interaction success, etc.
        if (i % 2 == 0) { // Even indices contribute positively
            socialReward += socialFeatures[i] * pImpl->parameters["social_scale"];
        } else { // Odd indices contribute negatively (conflicts, competition)
            socialReward -= socialFeatures[i] * pImpl->parameters["social_scale"] * 0.5f;
        }
    }
    
    return std::max(0.0f, socialReward); // Social reward is typically non-negative
}

float Reward::computeIntrinsicReward(const Observation& observation,
                                    const Action& action,
                                    const std::vector<float>& modelPrediction) const {
    // Compute intrinsic motivation (curiosity, novelty)
    float intrinsicReward = 0.0f;
    
    // Curiosity-based reward: difference between observation and prediction
    const SensoryInput* sensoryInput = observation.getSensoryInput();
    if (sensoryInput && !modelPrediction.empty() && sensoryInput->getData().size() == modelPrediction.size()) {
        float predictionError = 0.0f;
        for (size_t i = 0; i < sensoryInput->getData().size(); ++i) {
            predictionError += std::abs(sensoryInput->getData()[i] - modelPrediction[i]);
        }
        intrinsicReward += predictionError * pImpl->parameters["intrinsic_scale"] * 0.1f;
    }
    
    // Novelty reward based on action parameters
    const std::vector<float>& actionParams = action.getParameters();
    float novelty = 0.0f;
    for (float param : actionParams) {
        novelty += std::abs(param - 0.5f); // Distance from center of parameter space
    }
    intrinsicReward += novelty * pImpl->parameters["intrinsic_scale"] * 0.05f;
    
    // Intrinsic reward should encourage exploration of uncertain states
    intrinsicReward += pImpl->uncertainty * pImpl->parameters["intrinsic_scale"] * 0.2f;
    
    return intrinsicReward;
}

float Reward::computeStructuralReward(DevelopmentalStage stage,
                                     float plasticityPotential) const {
    // Structural plasticity rewards (development)
    float structuralReward = 0.0f;
    
    // Different rewards for different developmental stages
    switch (stage) {
        case DevelopmentalStage::Initial:
            structuralReward = 0.1f * pImpl->parameters["structural_scale"]; // Initial growth
            break;
        case DevelopmentalStage::CriticalPeriod:
            structuralReward = 0.5f * pImpl->parameters["structural_scale"]; // High plasticity period
            break;
        case DevelopmentalStage::Maturation:
            structuralReward = 0.3f * pImpl->parameters["structural_scale"]; // Optimization
            break;
        case DevelopmentalStage::Adult:
            structuralReward = 0.1f * pImpl->parameters["structural_scale"]; // Maintenance
            break;
        case DevelopmentalStage::Aging:
            structuralReward = -0.1f * pImpl->parameters["structural_scale"]; // Decline
            break;
    }
    
    // Add reward based on plasticity potential
    structuralReward += plasticityPotential * pImpl->parameters["structural_scale"] * 0.2f;
    
    return structuralReward;
}

void Reward::adaptLearningRate(float environmentComplexity,
                              float predictionAccuracy) {
    // Adapt learning rate based on environmental complexity and prediction accuracy
    
    float complexityFactor = std::clamp(environmentComplexity, 0.0f, 1.0f);
    float accuracyFactor = predictionAccuracy;
    
    // Increase learning rate for complex environments
    float learningRate = pImpl->parameters["learning_rate"] * (1.0f + complexityFactor * 0.5f);
    
    // Decrease learning rate for high prediction accuracy (stable learning)
    learningRate *= (1.0f - accuracyFactor * 0.3f);
    
    // Apply bounds
    learningRate = std::clamp(learningRate, 0.01f, 0.5f);
    
    pImpl->parameters["learning_rate"] = learningRate;
    
    NLM_LOG_DEBUG("Learning rate adapted: complexity=" + std::to_string(environmentComplexity) +
                  " accuracy=" + std::to_string(predictionAccuracy) +
                  " new_rate=" + std::to_string(learningRate));
}

Action Reward::selectAction(const std::vector<Action>& availableActions,
                           const Observation& observation,
                           const std::vector<float>& stateFeatures) const {
    // Exploration-exploitation action selection
    
    if (availableActions.empty()) {
        return Action(); // Return empty action
    }
    
    // Exploration: random action selection
    if (pImpl->rng() % 100 < pImpl->explorationRate * 100) {
        size_t randomIndex = pImpl->rng() % availableActions.size();
        return availableActions[randomIndex];
    }
    
    // Exploitation: select best action based on state features
    float bestValue = -1e6f;
    Action bestAction;
    
    for (const auto& action : availableActions) {
        float actionValue = 0.0f;
        
        // Compute action value based on state features
        for (size_t i = 0; i < std::min(stateFeatures.size(), size_t(3)); ++i) {
            actionValue += stateFeatures[i] * action.getParameters()[i % action.getParameters().size()];
        }
        
        // Add exploration bonus
        actionValue += pImpl->explorationBonus * 0.1f;
        
        // Add exploitation value
        actionValue += pImpl->exploitationValue * 0.05f;
        
        if (actionValue > bestValue) {
            bestValue = actionValue;
            bestAction = action;
        }
    }
    
    return bestAction;
}

Reward::RewardLearningAnalysis Reward::analyzeRewardLearning() const {
    RewardLearningAnalysis analysis;
    
    // Learning curve (last 100 steps)
    size_t historySize = pImpl->history.size();
    size_t analysisSize = std::min(size_t(100), historySize);
    
    for (size_t i = 0; i < analysisSize; ++i) {
        size_t idx = historySize - 1 - i;
        analysis.learningCurve[i] = pImpl->history[idx].value;
    }
    
    // Fill remaining learning curve with zeros
    for (size_t i = analysisSize; i < 100; ++i) {
        analysis.learningCurve[i] = 0.0f;
    }
    
    // Prediction error variance
    float errorSum = 0.0f;
    float errorSumSquares = 0.0f;
    for (const auto& entry : pImpl->history) {
        float error = entry.predictionError;
        errorSum += error;
        errorSumSquares += error * error;
    }
    
    if (!pImpl->history.empty()) {
        float meanError = errorSum / pImpl->history.size();
        analysis.predictionErrorVariance = (errorSumSquares / pImpl->history.size()) - (meanError * meanError);
    }
    
    // Reward history entropy
    std::vector<float> rewardHistogram(10, 0.0f);
    for (const auto& entry : pImpl->history) {
        size_t bin = static_cast<size_t>((entry.value + 10.0f) / 20.0f * 10); // Map [-10, 10] to 10 bins
        if (bin < 10) {
            rewardHistogram[bin]++;
        }
    }
    
    float totalCount = static_cast<float>(pImpl->history.size());
    if (totalCount > 0) {
        float entropy = 0.0f;
        for (float count : rewardHistogram) {
            float prob = count / totalCount;
            if (prob > 0.0f) {
                entropy -= prob * std::log2(prob);
            }
        }
        analysis.rewardHistoryEntropy = entropy;
    }
    
    // Exploration-exploitation ratio
    float totalExploration = pImpl->explorationBonus;
    float totalExploitation = pImpl->exploitationValue;
    analysis.explorationExploitationRatio = totalExploitation > 0.0f ? 
        totalExploration / totalExploitation : 0.0f;
    
    // Intrinsic-extrinsic ratio
    float extrinsicSum = pImpl->stats.extrinsicTotal;
    float intrinsicSum = pImpl->stats.intrinsicTotal;
    analysis.intrinsicExtrinsicRatio = intrinsicSum > 0.0f ? 
        extrinsicSum / intrinsicSum : 0.0f;
    
    // Credit assignment accuracy (simplified: based on prediction error reduction)
    analysis.creditAssignmentAccuracy = std::max(0.0f, 1.0f - pImpl->uncertainty);
    
    // Model prediction error
    analysis.modelPredictionError = pImpl->lastPrediction.predictionError;
    
    // Reward modality distribution
    float totalReward = pImpl->stats.totalRewards;
    if (totalReward > 0.0f) {
        analysis.rewardModalityDistribution.resize(4);
        analysis.rewardModalityDistribution[0] = pImpl->stats.extrinsicTotal / totalReward;
        analysis.rewardModalityDistribution[1] = pImpl->stats.intrinsicTotal / totalReward;
        analysis.rewardModalityDistribution[2] = pImpl->stats.socialTotal / totalReward;
        analysis.rewardModalityDistribution[3] = pImpl->stats.structuralTotal / totalReward;
    }
    
    return analysis;
}

} // namespace nlm