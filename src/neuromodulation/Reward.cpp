#include "Reward.hpp"

namespace nlm {

// Helper function to get sign of float
inline float sign(float x) {
    return (x > 0.0f) ? 1.0f : ((x < 0.0f) ? -1.0f : 0.0f);
}

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f) {}
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

float Reward::computeReward(const Observation& observation) const {
    // TODO PHASE 2: Implement real reward computation from observation
    // For now, use the enhanced environmental reward computation
    return computeEnvironmentalReward(observation);
}

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

// Enhanced reward computation with meaningful environmental rewards
// This implementation provides state-based rewards for reinforcement learning
float Reward::computeEnvironmentalReward(const Observation& observation) const {
    float totalReward = 0.0f;
    
    // Get sensory input from observation
    const SensoryInput* sensoryInput = observation.getSensoryInput();
    if (!sensoryInput) {
        return 0.0f;
    }
    
    const std::vector<float>& data = sensoryInput->getData();
    
    // Reward computation based on sensory data
    // This is a simplified example - real implementations would be more sophisticated
    
    // Vision-based rewards (positive for resource proximity, negative for hazards)
    if (strcmp(sensoryInput->getType(), "Vision") == 0) {
        // Check for resources in vision field (positive reward)
        // Resource detection would depend on specific implementation
        // For now, use novelty in input data as proxy for interesting regions
        float maxInput = 0.0f;
        for (float val : data) {
            maxInput = std::max(maxInput, std::abs(val));
        }
        if (maxInput > 0.5f) {
            totalReward += 0.5f * (maxInput - 0.5f);  // Partial reward for interesting regions
        }
        
        // Negative reward for high variation (potentially chaotic environment)
        float variance = 0.0f;
        float mean = 0.0f;
        for (float val : data) {
            mean += val;
        }
        mean /= data.size();
        for (float val : data) {
            variance += (val - mean) * (val - mean);
        }
        variance /= data.size();
        
        if (variance > 0.25f) {
            totalReward -= 0.1f * (variance - 0.25f);  // Penalize overly variable input
        }
    }
    
    // Internal signals (brain state) based rewards
    else if (strcmp(sensoryInput->getType(), "InternalSignals") == 0) {
        // Reward stable internal states
        float signalStability = 0.0f;
        if (data.size() > 1) {
            float maxDev = 0.0f;
            float minDev = 0.0f;
            for (float val : data) {
                maxDev = std::max(maxDev, val);
                minDev = std::min(minDev, val);
            }
            signalStability = 1.0f - (maxDev - minDev) / 2.0f;  // 0 to 1, higher = more stable
        }
        totalReward += 0.3f * signalStability;  // Reward stable internal states
        
        // Negative reward for high energy consumption (efficient behavior)
        if (data.size() > 0 && data[0] > 0.8f) {
            totalReward -= 0.2f;  // Negative reward for high energy use
        }
    }
    
    // Audio-based rewards (placeholder)
    else if (strcmp(sensoryInput->getType(), "Audio") == 0) {
        // For audio, use average amplitude as proxy for activity
        float sum = 0.0f;
        for (float val : data) {
            sum += std::abs(val);
        }
        float avgAmplitude = sum / data.size();
        
        // Moderate amplitude indicates good environmental awareness
        if (avgAmplitude > 0.3f && avgAmplitude < 0.7f) {
            totalReward += 0.2f;  // Optimal alertness range
        } else if (avgAmplitude < 0.1f || avgAmplitude > 1.0f) {
            totalReward -= 0.1f;  // Too silent or too loud
        }
    }
    
    // Age/development-based rewards
    // Younger agents should explore more, older agents should conserve energy
    // This would typically come from world state, not observation alone
    
    // Noise/curiosity reward
    // Reward for processing novel or interesting sensory patterns
    float avgInput = 0.0f;
    for (float val : data) {
        avgInput += val;
    }
    avgInput /= data.size();
    
    // Reward for processing non-zero inputs (active perception)
    float nonZeroRatio = 0.0f;
    for (float val : data) {
        if (std::abs(val) > 0.01f) nonZeroRatio += 1.0f;
    }
    nonZeroRatio /= data.size();
    
    if (nonZeroRatio > 0.5f) {
        totalReward += 0.3f * nonZeroRatio;  // Reward for active perception
    }
    
    // Energy efficiency reward (negative for high energy use)
    totalReward -= 0.05f;  // Small cost for processing sensory data
    
    return std::max(-1.0f, std::min(1.0f, totalReward));  // Clamp to [-1, 1]
}

// Enhanced reward computation that combines environmental and neuromodulatory signals
float Reward::computeIntegratedReward(const Observation& observation, 
                                      float neuromodulationLevel) const {
    float envReward = computeEnvironmentalReward(observation);
    
    // Integrate with neuromodulation
    // Dopamine-like modulation scales reward sensitivity
    float modulatedReward = envReward * (1.0f + neuromodulationLevel * 0.5f);
    
    // Add baseline reward for persistence
    if (getHistory().size() > 0) {
        // Look at recent reward trends
        size_t recentHistory = std::min(size_t(5), getHistory().size());
        float recentAverage = 0.0f;
        for (size_t i = 1; i <= recentHistory && i <= getHistory().size(); ++i) {
            recentAverage += getHistory()[getHistory().size() - i];
        }
        recentAverage /= recentHistory;
        
        // Small reward for consistent reward generation
        if (abs(recentAverage) > 0.1f) {
            modulatedReward += 0.1f * sign(recentAverage);  // Reward consistency
        }
    }
    
    // Store in history for analysis
    pImpl->history.push_back(modulatedReward);
    if (pImpl->history.size() > 100) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    return modulatedReward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
