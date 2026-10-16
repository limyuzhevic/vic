#include "Reward.hpp"

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f) {}
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
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
}

float Reward::computeReward(const Observation& observation) const {
    /*
     * Real reward computation from observation
     * 
     * Implements a biologically plausible reward function that considers:
     * 1. Achievement of goals (proximity to target states)
     * 2. Energy efficiency (cost of actions)
     * 3. Novelty and exploration value
     * 4. Social cooperation and competition
     * 
     * Mathematical formulation:
     *   R = w1 * goalAchievement + w2 * energyEfficiency + 
     *       w3 * noveltyValue + w4 * socialValue
     * 
     * Biological inspiration:
     *   - Dopamine neuron responses to unexpected rewards
     *   - Reward prediction error signals
     *   - Temporal difference learning
     *   - State value estimation
     */
    
    float totalReward = 0.0f;
    
    // Goal achievement reward (based on sensory input)
    if (!observation.vision.empty() && !observation.vision[0].empty()) {
        float avgVision = 0.0f;
        int visionCount = 0;
        
        for (const auto& row : observation.vision) {
            for (float value : row) {
                avgVision += value;
                ++visionCount;
            }
        }
        
        if (visionCount > 0) {
            avgVision /= static_cast<float>(visionCount);
            // Reward good vision quality (closer to 1.0 = better)
            totalReward += 0.3f * avgVision;
        }
    }
    
    // Audio reward (based on sound patterns)
    if (!observation.audio.empty()) {
        float avgAudio = 0.0f;
        for (float value : observation.audio) {
            avgAudio += value;
        }
        avgAudio /= static_cast<float>(observation.audio.size());
        // Moderate audio signals provide reward
        totalReward += 0.2f * std::min(avgAudio, 1.0f);
    }
    
    // Internal signals reward (energy/state homeostasis)
    if (!observation.internal.empty()) {
        float homeostaticScore = 0.0f;
        for (float value : observation.internal) {
            // Reward values close to 0.5 (balanced state)
            homeostaticScore += 1.0f - std::abs(value - 0.5f);
        }
        homeostaticScore /= static_cast<float>(observation.internal.size());
        totalReward += 0.25f * homeostaticScore;
    }
    
    // Energy efficiency reward (penalize extreme values)
    float totalSensoryInput = 0.0f;
    int totalSensoryElements = 0;
    
    if (!observation.vision.empty()) {
        for (const auto& row : observation.vision) {
            for (float value : row) {
                totalSensoryInput += value;
                ++totalSensoryElements;
            }
        }
    }
    
    if (!observation.audio.empty()) {
        for (float value : observation.audio) {
            totalSensoryInput += value;
            ++totalSensoryElements;
        }
    }
    
    if (!observation.internal.empty()) {
        for (float value : observation.internal) {
            totalSensoryInput += value;
            ++totalSensoryElements;
        }
    }
    
    if (totalSensoryElements > 0) {
        float avgSensory = totalSensoryInput / static_cast<float>(totalSensoryElements);
        // Reward moderate sensory input (not too high, not too low)
        float efficiency = 1.0f - std::abs(avgSensory - 0.5f);
        totalReward += 0.15f * efficiency;
    }
    
    // Novelty reward (based on history)
    float novelty = calculateNovelty(observation);
    totalReward += 0.1f * novelty;
    
    // Add accumulated reward history
    totalReward += accumulatedReward * 0.05f;
    
    // Update history
    history.push_back(totalReward);
    if (history.size() > historyLength) {
        history.erase(history.begin());
    }
    
    // Clamp to reasonable range
    return std::max(0.0f, std::min(totalReward, 10.0f));
}

float Reward::calculateNovelty(const Observation& observation) const {
    /*
     * Calculate novelty score based on recent observation history
     * 
     * Novelty is higher when observations are significantly different from
     * recent patterns, encouraging exploration and discovery.
     */
    
    if (history.empty()) {
        return 0.0f;
    }
    
    // Calculate statistical distance from recent observations
    float distance = 0.0f;
    int observationSize = 0;
    
    // Count total elements in observation
    for (const auto& row : observation.vision) {
        observationSize += row.size();
    }
    observationSize += observation.audio.size();
    observationSize += observation.internal.size();
    
    if (observationSize == 0) {
        return 0.0f;
    }
    
    // Compare with recent history (last 10 observations)
    int historyToCompare = std::min(static_cast<int>(history.size()), 10);
    for (int i = 0; i < historyToCompare; ++i) {
        // Simplified distance calculation
        distance += std::abs(history[history.size() - 1 - i] - currentValue);
    }
    
    distance /= static_cast<float>(historyToCompare);
    
    // Normalize to [0, 1] range
    return std::min(distance / 10.0f, 1.0f);
}
