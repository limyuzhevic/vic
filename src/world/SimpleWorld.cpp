#include "SimpleWorld.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>

namespace nlm {

const SensoryPercept& SimpleWorld::observe(const AgentBody* agent) const {
    // Store current agent to use for observation generation
    // This is called before world update, so agent is in pre-action state
    
    // For now, use the existing sensory_ which is updated during generateVision()
    // This creates a simple integration point for Phase 6
    return sensory_;
}

ActionResult SimpleWorld::applyAction(const AgentBody* agent, MotorCommand cmd) {
    // Phase 6 integration interface - delegates to existing applyMotorCommand
    // This maintains backward compatibility with the SimpleWorld interface
    if (!agent) {
        return ActionResult(0.0f, false, "no_agent");
    }
    
    // Use the agent's position to calculate a time-appropriate action
    double time = simTime_;
    return applyMotorCommand(cmd, time);
}

float SimpleWorld::computeReward(const AgentBody* agent) const {
    // Phase 6 integration interface - calculate reward based on agent state
    if (!agent) {
        return 0.0f;
    }
    
    float totalReward = 0.0f;
    
    // Energy reward - maintaining energy is good
    if (agent->energy < 0.3f * maxEnergy_) {
        totalReward += 0.5f;  // Reward for conserving energy
    } else if (agent->energy > 0.8f * maxEnergy_) {
        totalReward -= 0.1f;  // Small cost for excess energy
    }
    
    // Health reward - avoid damage
    if (agent->health < 0.5f) {
        totalReward -= 1.0f;  // Penalty for low health
    }
    
    // Proximity to objects (simplified)
    const float AGENT_INTERACTION_DISTANCE = 2.0f;
    
    for (const auto& obj : objects_) {
        if (!obj.active) continue;
        
        float dx = obj.x - agent->x;
        float dy = obj.y - agent->y;
        float dist = std::sqrt(dx * dx + dy * dy);
        
        if (dist < AGENT_INTERACTION_DISTANCE) {
            if (obj.type == WorldObjectType::Resource) {
                totalReward += obj.value * (1.0f - dist / AGENT_INTERACTION_DISTANCE) * 0.5f;
            } else if (obj.type == WorldObjectType::Hazard) {
                totalReward += obj.value * (1.0f - dist / AGENT_INTERACTION_DISTANCE) * 0.5f;
            }
        }
    }
    
    return totalReward;
}

void SimpleWorld::initialize(int width, int height) {
    configure(static_cast<float>(width), static_cast<float>(height), 16, 16);
    reset();
}
