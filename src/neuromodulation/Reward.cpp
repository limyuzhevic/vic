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
    pImpl->history.push_back(pImpl->currentValue);
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    pImpl->history.clear();
}

float Reward::computeReward(const Observation& observation) const {
    // Real reward computation based on observation
    // Extract information from observation for reward calculation
    const auto& data = observation.getData();
    
    float reward = 0.0f;
    
    // Reward based on energy level (assuming data[0] = energy)
    if (!data.empty()) {
        float energy = data[0];
        
        // Positive reward for maintaining healthy energy
        if (energy > 50.0f && energy < 90.0f) {
            reward += (energy - 50.0f) * 0.1f;  // Healthy energy bonus
        } else if (energy < 20.0f) {
            reward -= 2.0f;  // Low energy penalty
        }
        
        // Reward for recovering from low energy
        if (energy > 10.0f && pImpl->history.empty()) {
            reward += 1.0f;
        }
    }
    
    // Reward based on proximity to hazards
    // This would need to be extracted from observation data
    // For now, use a placeholder value
    if (!data.empty() && data.size() > 10) {
        float hazardProximity = data[10];  // Assume hazard proximity at index 10
        reward -= hazardProximity * 10.0f;  // Higher proximity = more negative reward
    }
    
    // Reward based on resources found
    // This would need to be extracted from observation data
    if (!data.empty() && data.size() > 11) {
        float resourceProximity = data[11];  // Assume resource proximity at index 11
        reward += resourceProximity * 15.0f;  // Higher proximity = more positive reward
    }
    
    // Time-based reward encourages exploration and longer survival
    // This would depend on global simulation time (not available in observation)
    // For now, provide small positive reward for being alive
    reward += 0.01f;
    
    // Add exploration bonus based on novelty (simplified)
    float noveltyBonus = 0.0f;
    if (!data.empty() && data.size() > 15) {
        // Based on variation in sensory data
        float variance = 0.0f;
        for (size_t i = 15; i < data.size() && i < 25; ++i) {
            variance += data[i] * data[i];
        }
        variance /= std::max(1.0f, (float)(data.size() - 15));
        noveltyBonus = variance * 5.0f;
    }
    
    reward += noveltyBonus;
    
    // Clamp reward to reasonable range
    reward = std::max(-50.0f, std::min(50.0f, reward));
    
    return reward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
