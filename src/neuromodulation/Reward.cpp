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
    if (!observation.isValid() || !observation.getSensoryInput()) {
        return 0.0f;
    }
    
    const SensoryInput* sensoryInput = observation.getSensoryInput();
    
    // Phase 5 GridWorld environment rewards:
    // -0.01 for each step (encourages efficiency)
    // +1.0 for reaching the goal (primary objective)
    // This reward computation should analyze the sensory input
    // to determine if the agent has achieved a goal or not
    
    float totalReward = 0.0f;
    
    // Check for InternalSignals (GridWorld environment uses these)
    auto* internalSignals = dynamic_cast<const InternalSignals*>(sensoryInput);
    if (internalSignals && internalSignals->getSignalCount() >= 2) {
        // Signals: agent_x, agent_y
        float agentX = internalSignals->getSignal(0);
        float agentY = internalSignals->getSignal(1);
        
        // Calculate goal position based on grid dimensions
        // In GridWorld, goal is typically at (width-1, height-1)
        float goalX = 1.0f;  // Assuming normalized position
        float goalY = 1.0f;  
        
        // Check if agent is close to goal (within 10% of grid)
        float distance = std::sqrt(std::pow(agentX - goalX, 2) + std::pow(agentY - goalY, 2));
        if (distance < 0.1f) {
            totalReward = 1.0f;  // Goal reached
        } else {
            totalReward = -0.01f;  // Small step penalty for efficiency
        }
    } 
    // Store in history for analysis
    pImpl->history.push_back(totalReward);
    if (pImpl->history.size() > 1000) {  // Keep history manageable
        pImpl->history.erase(pImpl->history.begin());
    }
    
    return totalReward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
