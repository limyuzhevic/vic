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

float Reward::computeReward(const Observation& observation) {
    // Implement real reward computation from observation
    // Uses multiple modalities: goal proximity, novelty, survival metrics
    
    try {
        float reward = 0.0f;
        
        // Extract features from observation
        const auto& state = observation.getState();
        const auto& sensorData = observation.getSensorData();
        
        // 1. Goal proximity reward (higher when approaching goals)
        float goalProximity = 0.0f;
        if (!state.empty()) {
            // Check if any goal-like features are present
            for (float val : state) {
                if (val > 0.8f) {  // Strong activation of goal-related units
                    goalProximity += val;
                }
            }
            goalProximity = std::min(goalProximity / state.size(), 1.0f);
        }
        reward += goalProximity * 2.0f;
        
        // 2. Novelty reward (encourage exploration)
        float novelty = 0.0f;
        if (!pImpl->history.empty()) {
            float currentNovelty = 0.0f;
            for (float val : sensorData) {
                // Calculate deviation from recent history
                float deviation = 0.0f;
                for (float histVal : pImpl->history.back()) {
                    deviation += std::abs(val - histVal);
                }
                currentNovelty += deviation / std::max(1.0f, pImpl->history.back().size());
            }
            novelty = std::min(currentNovelty / sensorData.size(), 1.0f);
        }
        reward += novelty * 1.5f;
        
        // 3. Survival reward (maintain homeostasis)
        float survival = 0.0f;
        if (!sensorData.empty()) {
            float homeostaticScore = 0.0f;
            for (float val : sensorData) {
                // Reward optimal range around 0.5 (moderate activation)
                homeostaticScore += 1.0f - std::abs(val - 0.5f) * 2.0f;
            }
            survival = homeostaticScore / sensorData.size();
        }
        reward += survival * 1.0f;
        
        // 4. Predictive accuracy bonus (if prediction system is available)
        // This would be integrated with the prediction system in a full implementation
        
        // Apply biological scaling and limits
        reward = std::max(-1.0f, std::min(reward, 10.0f));  // Typical reward range
        
        // Update history
        pImpl->history.push_back(sensorData);
        if (pImpl->history.size() > 100) {
            pImpl->history.erase(pImpl->history.begin());
        }
        
        // Apply temporal smoothing for stable learning
        pImpl->currentValue = 0.9f * pImpl->currentValue + 0.1f * reward;
        
        return reward;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Reward::computeReward error: ") + e.what());
        return 0.0f;
    }
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
