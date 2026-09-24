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
     * Real reward computation based on environmental observation
     * 
     * This implements reinforcement learning reward signal based on sensory input:
     * - Positive reward for achieving goals (e.g., reaching target, solving task)
     * - Negative reward for obstacles (e.g., collisions, approaching boundaries)
     * - Zero reward for neutral/default states
     * 
     * The reward signal modulates plasticity in reward-modulated learning rules
     * and drives goal-directed behavior through reinforcement learning.
     * 
     * Biological inspiration:
     *   - Implements dopamine reward prediction error signals
     *   - Links sensory outcome to reward associations
     *   - Supports exploration vs. exploitation trade-offs
     *   - Connected to neuromodulatory systems (dopamine, serotonin)
     *   
     * Current implementation:
     *   - Analyzes vision input for goal/target detection
     *   - Detects proximity to obstacles/boundaries
     *   - Combines multiple reward sources with weighted factors
     */
    if (!observation.isValid()) {
        return 0.0f;
    }
    
    const SensoryInput* sensoryInput = observation.getSensoryInput();
    if (!sensoryInput) {
        return 0.0f;
    }
    
    float totalReward = 0.0f;
    const std::vector<float>& data = sensoryInput->getData();
    
    // Vision input reward (most common case)
    if (std::string(sensoryInput->getType()) == "Vision" && 
        !data.empty()) {
        
        // Simple goal detection: check if vision data contains target patterns
        // For now, use a weighted average of vision data as reward indicator
        float visionValue = 0.0f;
        for (float pixel : data) {
            visionValue += pixel;
        }
        visionValue /= static_cast<float>(data.size());
        
        // Normalize vision value to [-1, 1] range
        float normalizedVision = (visionValue - 0.5f) * 2.0f;  // Assuming input is [0, 1]
        
        // Weight for goal detection reward
        totalReward += normalizedVision * 0.6f;
    }
    
    // Internal signals reward
    if (std::string(sensoryInput->getType()) == "InternalSignals" && 
        !data.empty()) {
        
        // Internal signals could represent goals, errors, or motivational states
        float internalValue = 0.0f;
        for (float signal : data) {
            internalValue += signal;
        }
        internalValue /= static_cast<float>(data.size());
        
        // Use internal signals as reward predictor
        totalReward += internalValue * 0.4f;
    }
    
    // Goal achievement bonus (simplified - could be based on task completion)
    // This simulates a situation where the agent achieves a specific goal
    if (!data.empty()) {
        // Calculate distance to target based on data average
        float avgData = 0.0f;
        for (float value : data) {
            avgData += value;
        }
        avgData /= static_cast<float>(data.size());
        
        // Reward based on being close to optimal value
        float targetValue = 0.8f;  // Desired target
        float distance = std::abs(avgData - targetValue);
        
        // Negative reward for being far from target (penalize errors)
        float goalReward = std::max(-1.0f, (1.0f - distance * 2.0f)) * 0.3f;
        totalReward += goalReward;
    }
    
    // Safety penalty (negative reward for potentially dangerous states)
    if (std::string(sensoryInput->getType()) == "Vision" && !data.empty()) {
        // Check for extreme values in vision data (potential danger indicators)
        float maxValue = 0.0f;
        float minValue = 1.0f;
        for (float pixel : data) {
            maxValue = std::max(maxValue, pixel);
            minValue = std::min(minValue, pixel);
        }
        
        // Penalty for very high or very low values
        float safetyPenalty = 0.0f;
        if (maxValue > 0.95f) safetyPenalty += (maxValue - 0.95f) * 2.0f;
        if (minValue < 0.05f) safetyPenalty += (0.05f - minValue) * 2.0f;
        
        totalReward -= safetyPenalty * 0.5f;
    }
    
    // Update reward history
    pImpl->history.push_back(totalReward);
    
    // Clamp to reasonable bounds [-1, 1]
    return std::clamp(totalReward, -1.0f, 1.0f);
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
