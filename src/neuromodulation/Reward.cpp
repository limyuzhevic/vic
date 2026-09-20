#include "Reward.hpp"
#include "../environment/Environment.hpp"
#include "../sensory/InternalSignals.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    
    // For computing spatial rewards in grid world
    float lastAgentX;
    float lastAgentY;
    float lastGoalX;
    float lastGoalY;
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f), lastAgentX(0.0f), 
             lastAgentY(0.0f), lastGoalX(0.0f), lastGoalY(0.0f) {}
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
    pImpl->history.clear();
}

float Reward::computeReward(const Observation& observation) const {
    // Real reward computation from observation
    // Based on standard reinforcement learning principles
    
    // Get sensory input from observation
    const SensoryInput* sensoryInput = observation.getSensoryInput();
    if (!sensoryInput) {
        return 0.0f;
    }
    
    const std::vector<float>& data = sensoryInput->getData();
    
    // Handle different types of observations
    if (sensoryInput->getType() == std::string("InternalSignals")) {
        // Grid world environment observation: [agent_x, agent_y, goal_x, goal_y]
        if (data.size() >= 4) {
            float agentX = data[0];
            float agentY = data[1];
            float goalX = data[2];
            float goalY = data[3];
            
            // Store last position for delta rewards
            pImpl->lastAgentX = agentX;
            pImpl->lastAgentY = agentY;
            pImpl->lastGoalX = goalX;
            pImpl->lastGoalY = goalY;
            
            // Compute various reward components
            
            // 1. Proximity reward - closer to goal gives higher reward
            float dx = goalX - agentX;
            float dy = goalY - agentY;
            float distance = std::sqrt(dx * dx + dy * dy);
            float proximityReward = (1.0f - distance) * 10.0f;  // Max 10.0 at goal
            
            // 2. Directional reward - moving towards goal
            // This is more sophisticated: check if movement was beneficial
            // For now, we assume the agent is trying to reach the goal
            
            // 3. Step penalty - encourage efficiency (small negative reward per step)
            float stepPenalty = -0.01f;
            
            // 4. Goal reached bonus - large positive reward at goal
            float goalBonus = 0.0f;
            if (distance < 0.1f) {  // Very close to goal
                goalBonus = 100.0f;
            }
            
            // 5. Reward for making progress - compare to previous position
            float progressReward = 0.0f;
            if (pImpl->lastAgentX > 0.0f || pImpl->lastAgentY > 0.0f) {
                // Check if agent moved closer to goal than previous step
                // This is a simplified implementation
            }
            
            // Combine rewards with appropriate weights
            float totalReward = proximityReward + stepPenalty + goalBonus + progressReward;
            
            // Add to history
            pImpl->history.push_back(totalReward);
            
            return totalReward;
        }
    } else if (sensoryInput->getType() == std::string("Vision")) {
        // Vision-based reward computation
        // This would involve analyzing visual input for rewards
        // For now, implement a simple placeholder
        float visionScore = 0.0f;
        const float* dataPtr = data.data();
        size_t size = data.size();
        
        // Simple heuristic: average of all pixel values (0-1 range)
        float sum = 0.0f;
        for (size_t i = 0; i < size && i < 1000; ++i) {
            sum += dataPtr[i];
        }
        float avgValue = (size > 0) ? (sum / size) : 0.0f;
        
        // Bright areas might indicate rewards, dark areas penalties
        visionScore = (avgValue - 0.5f) * 20.0f;
        
        pImpl->history.push_back(visionScore);
        return visionScore;
    }
    
    // Default fallback
    return 0.0f;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm