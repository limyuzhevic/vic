#include "Reward.hpp"
#include "../brain/Brain.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    
    // Reward statistics
    float predictionErrorRewardSum;
    float noveltyRewardSum;
    float outcomeRewardSum;
    float consolidationRewardSum;
    float explorationRewardSum;
    int rewardCount;
    
    // Moving average window
    std::vector<float> recentPredictionErrors;
    std::vector<float> recentNovelty;
    std::vector<float> recentOutcomes;
    std::vector<float> recentConsolidation;
    std::vector<float> recentExploration;
    
    // Integration parameters
    float predictionErrorWeight;
    float noveltyWeight;
    float outcomeWeight;
    float consolidationWeight;
    float explorationWeight;
    float movingAverageWindowSize;
    float predictionAccuracyThreshold;
    float noveltyThreshold;
    
    // Brain reference for system integration
    class Brain* brain;
    
    Impl() 
        : currentValue(0.0f)
        , accumulatedReward(0.0f)
        , predictionErrorRewardSum(0.0f)
        , noveltyRewardSum(0.0f)
        , outcomeRewardSum(0.0f)
        , consolidationRewardSum(0.0f)
        , explorationRewardSum(0.0f)
        , rewardCount(0)
        , predictionErrorWeight(0.4f)
        , noveltyWeight(0.2f)
        , outcomeWeight(0.2f)
        , consolidationWeight(0.1f)
        , explorationWeight(0.1f)
        , movingAverageWindowSize(10.0f)
        , predictionAccuracyThreshold(0.8f)
        , noveltyThreshold(0.5f)
        , brain(nullptr) {}
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
    pImpl->accumulatedReward += delta;
    pImpl->currentValue += delta;
    pImpl->history.push_back(delta);
    
    // Keep history limited
    if (pImpl->history.size() > pImpl->movingAverageWindowSize) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    pImpl->predictionErrorRewardSum = 0.0f;
    pImpl->noveltyRewardSum = 0.0f;
    pImpl->outcomeRewardSum = 0.0f;
    pImpl->consolidationRewardSum = 0.0f;
    pImpl->explorationRewardSum = 0.0f;
    pImpl->rewardCount = 0;
    pImpl->recentPredictionErrors.clear();
    pImpl->recentNovelty.clear();
    pImpl->recentOutcomes.clear();
    pImpl->recentConsolidation.clear();
    pImpl->recentExploration.clear();
}

float Reward::computeReward(const Observation& observation) const {
    // Integrate with brain systems to compute real reward
    if (!pImpl) {
        return 0.0f;
    }
    
    // Get references to brain systems
    class Brain* brain = const_cast<class Brain*>(pImpl->brain);
    if (!brain) {
        NLM_LOG_WARNING("Reward::computeReward: Brain not available");
        return 0.0f;
    }
    
    // Get various neuromodulation signals
    float predictionError = 0.0f;
    if (brain->getPredictionErrorSignal()) {
        predictionError = brain->getPredictionErrorSignal()->getError();
    }
    
    float novelty = 0.0f;
    if (brain->getNovelty()) {
        novelty = brain->getNovelty()->getLevel();
    }
    
    float curiosity = 0.0f;
    if (brain->getCuriosity()) {
        curiosity = brain->getCuriosity()->getLevel();
    }
    
    float dopamine = 0.0f;
    if (brain->getDopamine()) {
        dopamine = brain->getDopamine()->getLevel();
    }
    
    // Get prediction confidence
    float predictionConfidence = brain->getPredictionConfidence();
    
    // Get current brain state for outcome computation
    const std::vector<float>& sensoryState = observation.getData();
    
    // Compute different reward components
    float predictionErrorReward = computePredictionErrorReward(
        predictionError, predictionConfidence, dopamine);
    
    float noveltyReward = computeNoveltyReward(
        novelty, curiosity, sensoryState);
    
    float outcomeReward = computeOutcomeReward(
        observation, predictionError, dopamine);
    
    float consolidationReward = computeConsolidationReward(
        brain->getEpisodicMemory(), dopamine);
    
    float explorationReward = computeExplorationReward(
        curiosity, predictionError, sensoryState);
    
    // Update reward statistics
    updateRewardStatistics(predictionErrorReward, noveltyReward, 
                          outcomeReward, consolidationReward, explorationReward);
    
    // Compute weighted sum of reward components
    float totalReward = predictionErrorReward * pImpl->predictionErrorWeight +
                       noveltyReward * pImpl->noveltyWeight +
                       outcomeReward * pImpl->outcomeWeight +
                       consolidationReward * pImpl->consolidationWeight +
                       explorationReward * pImpl->explorationWeight;
    
    // Apply adaptive modulation based on brain state
    float adaptiveModulation = computeAdaptiveModulation(
        predictionConfidence, dopamine, curiosity);
    
    totalReward *= adaptiveModulation;
    
    // Clip to reasonable range
    totalReward = std::clamp(totalReward, -1.0f, 1.0f);
    
    // Store for history
    pImpl->history.push_back(totalReward);
    if (pImpl->history.size() > pImpl->movingAverageWindowSize) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    return totalReward;
}

float Reward::computePredictionErrorReward(float predictionError, 
                                           float predictionConfidence,
                                           float dopamine) const {
    // Reward for accurate predictions
    if (predictionConfidence > 0.0f) {
        // Positive reward for low prediction error
        float accuracy = std::max(0.0f, 1.0f - std::abs(predictionError));
        
        // Scale by prediction confidence
        float reward = accuracy * predictionConfidence;
        
        // Modulate by dopamine level (dopamine enhances reward learning)
        float dopamineModulation = 1.0f + dopamine * 0.5f;
        
        return reward * dopamineModulation;
    }
    
    // Negative reward for very high prediction errors
    return -std::min(1.0f, std::abs(predictionError) * 2.0f);
}

float Reward::computeNoveltyReward(float novelty, float curiosity, 
                                  const std::vector<float>& sensoryState) const {
    // Reward for novel experiences
    if (sensoryState.empty()) {
        return 0.0f;
    }
    
    // Base novelty reward
    float reward = novelty;
    
    // Modulate by curiosity level (curiosity drives exploration)
    float curiosityModulation = 1.0f + curiosity * 0.8f;
    
    // Decay novelty over time (higher novelty for newer experiences)
    float decayFactor = std::min(1.0f, static_cast<float>(pImpl->recentNovelty.size()) / 5.0f);
    
    reward *= curiosityModulation * decayFactor;
    
    // Threshold for minimal novelty
    return reward > pImpl->noveltyThreshold ? reward : 0.0f;
}

float Reward::computeOutcomeReward(const Observation& observation,
                                   float predictionError,
                                   float dopamine) const {
    // Reward based on action outcomes
    const std::vector<float>& sensoryData = observation.getData();
    
    if (sensoryData.empty()) {
        return 0.0f;
    }
    
    // Compute outcome quality based on sensory input patterns
    float meanSensory = std::accumulate(sensoryData.begin(), sensoryData.end(), 0.0f) / sensoryData.size();
    float variance = 0.0f;
    
    // Calculate variance in sensory input (indicates interesting outcomes)
    for (float value : sensoryData) {
        float diff = value - meanSensory;
        variance += diff * diff;
    }
    variance /= sensoryData.size();
    
    // Higher variance indicates more interesting/diverse outcomes
    float outcomeQuality = std::min(1.0f, variance * 10.0f);
    
    // Modulate by prediction error (large errors can indicate surprising outcomes)
    float predictionModulation = 1.0f + std::abs(predictionError) * 0.5f;
    
    // Modulate by dopamine (dopamine enhances reward learning)
    float dopamineModulation = 1.0f + dopamine * 0.3f;
    
    float reward = outcomeQuality * predictionModulation * dopamineModulation;
    
    // Positive reward for high-quality outcomes
    return reward > 0.1f ? reward : 0.0f;
}

float Reward::computeConsolidationReward(class NeuralEpisodicMemory* episodicMemory,
                                         float dopamine) const {
    // Reward for memory consolidation
    if (!episodicMemory) {
        return 0.0f;
    }
    
    float episodeCount = static_cast<float>(episodicMemory->getEpisodeCount());
    
    // Reward for maintaining stable memory storage
    float stabilityReward = std::min(1.0f, episodeCount / 100.0f);
    
    // Modulate by dopamine (dopamine supports memory consolidation)
    float dopamineModulation = 1.0f + dopamine * 0.2f;
    
    float reward = stabilityReward * dopamineModulation;
    
    // Additional reward for high consolidation quality
    if (episodeCount > 50.0f) {
        float qualityFactor = std::min(1.0f, (episodeCount - 50.0f) / 200.0f);
        reward += qualityFactor * 0.2f;
    }
    
    return reward;
}

float Reward::computeExplorationReward(float curiosity, float predictionError,
                                        const std::vector<float>& sensoryState) const {
    // Reward for exploring new states
    if (sensoryState.empty()) {
        return 0.0f;
    }
    
    // Base exploration reward from curiosity
    float reward = curiosity * 0.6f;
    
    // Additional reward from high prediction error (indicating new information)
    float predictionErrorModulation = std::min(1.0f, std::abs(predictionError) * 2.0f);
    
    // Reward for encountering rare sensory patterns
    float sensoryNovelty = computeSensoryNovelty(sensoryState);
    
    float totalReward = reward + predictionErrorModulation * 0.3f + sensoryNovelty * 0.1f;
    
    return totalReward;
}

float Reward::computeSensoryNovelty(const std::vector<float>& sensoryState) const {
    // Simple novelty detection for sensory patterns
    if (pImpl->recentNovelty.empty()) {
        return 0.0f;
    }
    
    // Compute similarity with recent patterns (simplified)
    float meanRecent = std::accumulate(pImpl->recentNovelty.begin(), 
                                      pImpl->recentNovelty.end(), 0.0f) / pImpl->recentNovelty.size();
    
    float maxSensory = *std::max_element(sensoryState.begin(), sensoryState.end());
    float minSensory = *std::min_element(sensoryState.begin(), sensoryState.end());
    
    // Novelty as deviation from recent patterns
    float deviation = std::abs(maxSensory - minSensory) / 2.0f;
    float noveltyScore = std::min(1.0f, deviation / meanRecent);
    
    return noveltyScore;
}

void Reward::updateRewardStatistics(float predError, float novelty,
                                    float outcome, float consolidation,
                                    float exploration) {
    if (!pImpl) {
        return;
    }
    
    // Update sum statistics
    pImpl->predictionErrorRewardSum += predError;
    pImpl->noveltyRewardSum += novelty;
    pImpl->outcomeRewardSum += outcome;
    pImpl->consolidationRewardSum += consolidation;
    pImpl->explorationRewardSum += exploration;
    pImpl->rewardCount++;
    
    // Update recent values for moving average
    pImpl->recentPredictionErrors.push_back(predError);
    pImpl->recentNovelty.push_back(novelty);
    pImpl->recentOutcomes.push_back(outcome);
    pImpl->recentConsolidation.push_back(consolidation);
    pImpl->recentExploration.push_back(exploration);
    
    // Maintain window size
    if (pImpl->recentPredictionErrors.size() > pImpl->movingAverageWindowSize) {
        pImpl->recentPredictionErrors.erase(pImpl->recentPredictionErrors.begin());
        pImpl->recentNovelty.erase(pImpl->recentNovelty.begin());
        pImpl->recentOutcomes.erase(pImpl->recentOutcomes.begin());
        pImpl->recentConsolidation.erase(pImpl->recentConsolidation.begin());
        pImpl->recentExploration.erase(pImpl->recentExploration.begin());
    }
}

float Reward::computeAdaptiveModulation(float predictionConfidence,
                                        float dopamine,
                                        float curiosity) const {
    // Adaptive modulation based on current brain state
    
    // High confidence reduces exploration need
    float confidenceModulation = std::max(0.5f, 1.0f - predictionConfidence);
    
    // Dopamine modulates plasticity and learning rate
    float dopamineModulation = 1.0f + dopamine * 0.3f;
    
    // Curiosity modulates exploration drive
    float curiosityModulation = 1.0f + curiosity * 0.5f;
    
    // Combine modulations
    float modulation = confidenceModulation * dopamineModulation * curiosityModulation;
    
    return modulation;
}

float Reward::getPredictionErrorRewardAverage() const {
    if (pImpl->rewardCount == 0) return 0.0f;
    return pImpl->predictionErrorRewardSum / pImpl->rewardCount;
}

float Reward::getNoveltyRewardAverage() const {
    if (pImpl->rewardCount == 0) return 0.0f;
    return pImpl->noveltyRewardSum / pImpl->rewardCount;
}

float Reward::getOutcomeRewardAverage() const {
    if (pImpl->rewardCount == 0) return 0.0f;
    return pImpl->outcomeRewardSum / pImpl->rewardCount;
}

float Reward::getConsolidationRewardAverage() const {
    if (pImpl->rewardCount == 0) return 0.0f;
    return pImpl->consolidationRewardSum / pImpl->rewardCount;
}

float Reward::getExplorationRewardAverage() const {
    if (pImpl->rewardCount == 0) return 0.0f;
    return pImpl->explorationRewardSum / pImpl->rewardCount;
}

float Reward::getTotalRewardAverage() const {
    if (pImpl->rewardCount == 0) return 0.0f;
    return (pImpl->predictionErrorRewardSum + pImpl->noveltyRewardSum + 
            pImpl->outcomeRewardSum + pImpl->consolidationRewardSum + 
            pImpl->explorationRewardSum) / pImpl->rewardCount;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

// Implement the initialize method
void Reward::initialize(Brain* brain) {
    // Store brain reference for system integration
    pImpl->brain = brain;
    NLM_LOG_INFO("Reward system initialized");
}

// Implement the signalMemoryConsolidation method
void Reward::signalMemoryConsolidation(const class EpisodicMemoryItem& episode) {
    // Integrate with episodic memory consolidation process
    if (!pImpl->brain) {
        NLM_LOG_WARNING("Reward::signalMemoryConsolidation: Brain not available");
        return;
    }
    
    // Signal reward-based memory consolidation
    // Higher reward values should strengthen memory consolidation
    float consolidationStrength = episode.reward * 0.5f + 0.5f;
    
    // Modulate episodic memory consolidation based on reward
    auto episodicMemory = pImpl->brain->getEpisodicMemory();
    if (episodicMemory) {
        // Use reward to influence memory relevance weighting
        episodicMemory->updateRelevance(episode.timestamp, consolidationStrength);
        
        // Log consolidation-related reward
        float consolidationReward = computeConsolidationReward(episodicMemory, 0.0f);
        pImpl->history.push_back(consolidationReward);
        
        if (pImpl->history.size() > pImpl->movingAverageWindowSize) {
            pImpl->history.erase(pImpl->history.begin());
        }
        
        NLM_LOG_INFO("Memory consolidation signaled with reward: " + std::to_string(consolidationReward));
    }
}

// Set the brain pointer in the Impl structure
void Reward::Impl::setBrain(Brain* brain) {
    Reward::getImpl(brain); // This is a placeholder implementation
}

} // namespace nlm
