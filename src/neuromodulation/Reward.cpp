#include "Reward.hpp"
#include "../environment/Observation.hpp"
#include "../agent/AgentBody.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    float explorationBonus;  // Track exploration for reward shaping
    float noveltyThreshold;  // Threshold for considering observation novel
    
    Impl() 
        : currentValue(0.0f), accumulatedReward(0.0f), explorationBonus(0.0f), 
          noveltyThreshold(0.5f) {
        history.reserve(1000);
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
    pImpl->accumulatedReward += delta;
    pImpl->currentValue += delta;
    
    // Record reward in history
    pImpl->history.push_back(delta);
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    pImpl->explorationBonus = 0.0f;
    clearHistory();
}

float Reward::computeReward(const Observation& observation) const {
    float reward = 0.0f;
    
    // Extract agent state from observation if available
    const SensoryInput* sensoryInput = observation.getSensoryInput();
    if (sensoryInput) {
        // Calculate energy-based reward (preserve energy, maximize efficiency)
        float energyLevel = sensoryInput->getEnergyLevel();
        if (energyLevel < 0.3f) {
            // Critical low energy - high negative reward
            reward -= 10.0f * (0.3f - energyLevel) / 0.3f;
        } else if (energyLevel < 0.5f) {
            // Low energy - moderate negative reward
            reward -= 3.0f * (0.5f - energyLevel) / 0.5f;
        } else if (energyLevel > 0.8f) {
            // Good energy - small positive reward (maintain energy)
            reward += 0.5f * (energyLevel - 0.8f) / 0.2f;
        }
        
        // Health-based reward
        float health = sensoryInput->getHealth();
        if (health < 0.3f) {
            // Critical health - high negative reward
            reward -= 15.0f * (0.3f - health) / 0.3f;
        } else if (health < 0.6f) {
            // Moderate health - negative reward
            reward -= 2.0f * (0.6f - health) / 0.6f;
        } else if (health > 0.9f) {
            // Excellent health - small positive reward
            reward += 0.3f * (health - 0.9f) / 0.1f;
        }
        
        // Activity-based reward (encourage exploration and learning)
        bool isMoving = sensoryInput->isMoving();
        bool isInteracting = sensoryInput->isInteracting();
        bool isTurning = sensoryInput->isTurning();
        
        if (isMoving) {
            reward -= 0.01f;  // Movement cost (encourage efficiency)
        }
        
        if (isInteracting) {
            reward += 0.5f;  // Interaction success bonus
        }
        
        if (isTurning) {
            reward -= 0.005f;  // Turning cost (small)
        }
        
        // Novelty-based exploration reward
        static const float explorationDecayRate = 0.99f;
        pImpl->explorationBonus *= explorationDecayRate;
        
        // Get sensory percept to check for novel stimuli
        // This would require access to world state, for now use basic heuristic
        // Based on observation quality/diversity
        reward += pImpl->explorationBonus * 0.1f;
        
        // Record reward in history
        pImpl->history.push_back(reward);
        if (pImpl->history.size() > 1000) {
            pImpl->history.erase(pImpl->history.begin());
        }
    }
    
    // Apply dopamine baseline influence (neuromodulation)
    // This represents the current neuromodulatory state
    reward *= (1.0f + pImpl->currentValue * 0.1f);
    
    // Clamp reward to reasonable bounds
    reward = std::max(-100.0f, std::min(100.0f, reward));
    
    return reward;
}

float Reward::getAverageReward() const {
    if (pImpl->history.empty()) {
        return 0.0f;
    }
    float sum = std::accumulate(pImpl->history.begin(), pImpl->history.end(), 0.0f);
    return sum / pImpl->history.size();
}

float Reward::getMaxReward() const {
    if (pImpl->history.empty()) {
        return 0.0f;
    }
    return *std::max_element(pImpl->history.begin(), pImpl->history.end());
}

float Reward::getMinReward() const {
    if (pImpl->history.empty()) {
        return 0.0f;
    }
    return *std::min_element(pImpl->history.begin(), pImpl->history.end());
}

float Reward::getStdDeviation() const {
    if (pImpl->history.size() < 2) {
        return 0.0f;
    }
    
    float mean = getAverageReward();
    float sumSquares = 0.0f;
    for (float reward : pImpl->history) {
        float diff = reward - mean;
        sumSquares += diff * diff;
    }
    
    return std::sqrt(sumSquares / pImpl->history.size());
}

float Reward::getRewardAt(size_t index) const {
    if (index >= pImpl->history.size()) {
        return 0.0f;
    }
    return pImpl->history[index];
}

size_t Reward::getHistorySize() const {
    return pImpl->history.size();
}

void Reward::normalizeHistory(float minTarget, float maxTarget) {
    if (pImpl->history.empty()) {
        return;
    }
    
    float minReward = getMinReward();
    float maxReward = getMaxReward();
    
    if (maxReward - minReward > 0.0f) {
        for (float& reward : pImpl->history) {
            reward = minTarget + (reward - minReward) * (maxTarget - minTarget) / (maxReward - minReward);
        }
    } else {
        // All rewards are the same, set to midpoint
        float midpoint = (minTarget + maxTarget) / 2.0f;
        for (float& reward : pImpl->history) {
            reward = midpoint;
        }
    }
}

float Reward::getEpisodeReturn() const {
    // Calculate return from start of current episode
    if (pImpl->history.empty()) {
        return 0.0f;
    }
    
    float episodeReturn = 0.0f;
    for (size_t i = 0; i < pImpl->history.size(); ++i) {
        episodeReturn += std::abs(pImpl->history[i]);
    }
    
    return episodeReturn;
}

float Reward::getDiscountedReturn(float gamma) const {
    if (pImpl->history.empty()) {
        return 0.0f;
    }
    
    float discountedReturn = 0.0f;
    float discount = 1.0f;
    
    for (float reward : pImpl->history) {
        discountedReturn += reward * discount;
        discount *= gamma;
    }
    
    return discountedReturn;
}

float Reward::getCurrentValue() const {
    return pImpl->currentValue;
}

float Reward::getAccumulatedReward() const {
    return pImpl->accumulatedReward;
}

float Reward::getExplorationBonus() const {
    return pImpl->explorationBonus;
}

void Reward::setExplorationBonus(float bonus) {
    pImpl->explorationBonus = bonus;
}

void Reward::updateExploration(float novelty) {
    // Update exploration bonus based on novelty detection
    const float explorationLearningRate = 0.1f;
    pImpl->explorationBonus += novelty * explorationLearningRate;
    
    // Cap exploration bonus to prevent runaway
    pImpl->explorationBonus = std::min(pImpl->explorationBonus, 10.0f);
}

float Reward::computeMultiObjectiveReward(const Observation& observation) {
    // Compute reward using multiple objectives for better learning
    float totalReward = 0.0f;
    float survivalReward = 0.0f;
    float efficiencyReward = 0.0f;
    float explorationReward = 0.0f;
    float noveltyReward = 0.0f;
    
    // Extract agent state from observation
    const SensoryInput* sensoryInput = observation.getSensoryInput();
    if (sensoryInput) {
        // Survival objective (health, energy preservation)
        float health = sensoryInput->getHealth();
        float energy = sensoryInput->getEnergyLevel();
        
        if (health < 0.3f) {
            survivalReward -= 15.0f * (0.3f - health) / 0.3f;
        } else if (energy < 0.3f) {
            survivalReward -= 10.0f * (0.3f - energy) / 0.3f;
        } else {
            survivalReward += (health * 0.3f + energy * 0.2f);
        }
        
        // Efficiency objective (reward per action cost)
        bool isMoving = sensoryInput->isMoving();
        bool isInteracting = sensoryInput->isInteracting();
        
        if (isMoving) {
            efficiencyReward -= 0.005f;  // Small movement cost
        }
        
        if (isInteracting) {
            efficiencyReward += 0.8f;  // High reward for successful interactions
        }
        
        // Exploration objective (encourage discovery)
        explorationReward = pImpl->explorationBonus * 0.5f;
        
        // Novelty objective (reward for encountering new situations)
        float novelty = std::min(1.0f, sensoryInput->getNoveltyScore() * 0.5f);
        noveltyReward = novelty * 2.0f;
        
        // Update exploration bonus based on novelty
        updateExploration(novelty);
    }
    
    // Adaptive reward scaling based on performance
    float performanceFactor = 1.0f;
    if (pImpl->history.size() > 10) {
        float recentAvg = getAverageReward();
        performanceFactor = 1.0f / (1.0f + std::abs(recentAvg));
    }
    
    // Combine rewards with adaptive weights
    totalReward = survivalReward * 0.4f + efficiencyReward * 0.3f + 
                  explorationReward * 0.2f + noveltyReward * 0.1f;
    
    // Apply performance scaling
    totalReward *= performanceFactor;
    
    // Apply neuromodulation (dopamine influence)
    totalReward *= (1.0f + pImpl->currentValue * 0.15f);
    
    // Record reward in history
    pImpl->history.push_back(totalReward);
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    return totalReward;
}

float Reward::computeRiskAwareReward(const Observation& observation) {
    // Risk-aware reward that considers uncertainty and potential outcomes
    float baseReward = computeReward(observation);
    
    // Estimate risk based on current state
    float riskScore = estimateRisk(observation);
    
    // Reduce reward for high-risk actions (conservative learning)
    float riskPenalty = riskScore * 2.0f;
    float riskAdjustedReward = baseReward - riskPenalty;
    
    // Maintain exploration despite risk (encourage calculated risk-taking)
    float explorationBonus = getExplorationBonus() * 0.3f * (1.0f - riskScore);
    
    // Combine risk-adjusted and exploration rewards
    float totalReward = riskAdjustedReward + explorationBonus;
    
    // Record risk-adjusted reward
    pImpl->history.push_back(totalReward);
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    return totalReward;
}

float Reward::estimateRisk(const Observation& observation) const {
    // Simple risk estimation based on agent state
    const SensoryInput* sensoryInput = observation.getSensoryInput();
    if (!sensoryInput) {
        return 0.5f; // Default medium risk
    }
    
    float health = sensoryInput->getHealth();
    float energy = sensoryInput->getEnergyLevel();
    bool isMoving = sensoryInput->isMoving();
    
    // High risk when health and energy are low
    float stateRisk = (1.0f - health) * 0.4f + (1.0f - energy) * 0.4f;
    
    // Additional risk for movement in unknown territory
    float movementRisk = isMoving ? 0.3f : 0.0f;
    
    return std::min(1.0f, stateRisk + movementRisk);
}

void Reward::setEpisodeStart(float startValue) {
    pImpl->episodeStartValue = startValue;
}

float Reward::getEpisodeStart() const {
    return pImpl->episodeStartValue;
}

float Reward::computeLongTermReward(float discountFactor, size_t lookbackSteps) const {
    // Compute long-term reward with discount factor
    if (pImpl->history.empty()) {
        return 0.0f;
    }
    
    float longTermReward = 0.0f;
    float discount = 1.0f;
    
    size_t startIndex = std::max(0, static_cast<int>(pImpl->history.size()) - lookbackSteps);
    
    for (size_t i = startIndex; i < pImpl->history.size(); ++i) {
        longTermReward += pImpl->history[i] * discount;
        discount *= discountFactor;
    }
    
    return longTermReward;
}

bool Reward::shouldExplore(float threshold) const {
    // Determine if agent should explore based on current state
    float explorationValue = getExplorationBonus() * 0.5f;
    float avgReward = getAverageReward();
    
    // Explore if: exploration bonus is high OR recent rewards are low
    bool shouldExplore = (explorationValue > threshold) || 
                        (avgReward < 0.0f && getHistorySize() > 10);
    
    return shouldExplore;
}

void Reward::updateFromExternalSource(float externalReward) {
    // Update reward from external sources (e.g., expert demonstrations, curriculum learning)
    add(externalReward);
    
    // Gradually adapt to external rewards
    float adaptationRate = 0.1f;
    pImpl->currentValue = pImpl->currentValue * (1.0f - adaptationRate) + externalReward * adaptationRate;
}

float Reward::getRewardVariance() const {
    // Calculate variance of rewards in history
    return getStdDeviation();
}

float Reward::getRewardEntropy() const {
    // Calculate entropy of reward distribution (measure of unpredictability)
    if (pImpl->history.empty()) {
        return 0.0f;
    }
    
    // Simple entropy calculation (discretize rewards into bins)
    const size_t numBins = 10;
    float minReward = getMinReward();
    float maxReward = getMaxReward();
    float binWidth = (maxReward - minReward) / numBins;
    
    std::vector<float> binProbabilities(numBins, 0.0f);
    
    for (float reward : pImpl->history) {
        size_t binIndex = static_cast<size_t>((reward - minReward) / binWidth);
        if (binIndex >= numBins) binIndex = numBins - 1;
        binProbabilities[binIndex]++;
    }
    
    float entropy = 0.0f;
    for (float prob : binProbabilities) {
        if (prob > 0.0f) {
            float normalizedProb = prob / pImpl->history.size();
            entropy -= normalizedProb * std::log(normalizedProb + 1e-6f);
        }
    }
    
    return entropy;
}

void Reward::saveState(std::vector<float>& state) const {
    // Save current state for checkpointing/restoration
    state.clear();
    state.push_back(pImpl->currentValue);
    state.push_back(pImpl->accumulatedReward);
    state.push_back(pImpl->explorationBonus);
    
    // Save a sample of history
    size_t sampleSize = std::min(100UL, pImpl->history.size());
    size_t startIndex = pImpl->history.size() - sampleSize;
    
    for (size_t i = startIndex; i < pImpl->history.size(); ++i) {
        state.push_back(pImpl->history[i]);
    }
}

void Reward::loadState(const std::vector<float>& state) {
    // Load state from checkpoint
    if (state.size() < 3) {
        return;
    }
    
    pImpl->currentValue = state[0];
    pImpl->accumulatedReward = state[1];
    pImpl->explorationBonus = state[2];
    
    // Clear and restore history
    clearHistory();
    
    size_t historyStart = 3;
    size_t historyEnd = std::min(historyStart + 100, state.size());
    
    for (size_t i = historyStart; i < historyEnd; ++i) {
        pImpl->history.push_back(state[i]);
    }
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}
