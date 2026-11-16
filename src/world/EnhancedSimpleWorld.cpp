/*
 * Enhanced Reward System for NLM SimpleWorld
 *
 * This improved reward system provides better motivation for learning
 * and more balanced exploration vs. exploitation trade-offs.
 */

namespace nlm {

// Enhanced reward constants for better learning
const float MIN_REWARD = -1.0f;          // Minimum possible reward per step
const float MAX_REWARD = 1.0f;           // Maximum possible reward per step
const float BASE_REWARD_PER_STEP = 0.0f; // Baseline reward for basic survival
const float ENERGY_REWARD_FACTOR = 0.5f; // Reward per unit of energy maintained
const float EXPLORATION_BONUS = 0.1f;   // Bonus for moving to new areas
const float RISK_REWARD_RATIO = 0.3f;    // Balance between risk and reward

// Improved reward calculation for movement actions
ActionResult SimpleWorld::applyEnhancedMovement(MotorCommand cmd, double currentTime) {
    ActionResult result;
    const float moveSpeed = 3.0f;
    const float turnSpeed = 2.0f;
    
    // Calculate movement based on command
    switch (cmd) {
        case MotorCommand::MoveForward:
            // Calculate movement
            float dx = std::cos(agent_.orientation) * moveSpeed;
            float dy = std::sin(agent_.orientation) * moveSpeed;
            
            float newX = agent_.x + dx * 0.1f;
            float newY = agent_.y + dy * 0.1f;
            
            if (isValidPosition(newX, newY)) {
                // Positive reward for successful movement
                result.reward = 0.05f;  // Small positive reward for movement
                result.success = true;
                
                // Exploration bonus for moving to less explored areas
                if (shouldGiveExplorationBonus(agent_.x, agent_.y, newX, newY)) {
                    result.reward += EXPLORATION_BONUS;
                }
                
                // Energy reward factor
                float energyReward = agent_.energy / maxEnergy_ * ENERGY_REWARD_FACTOR;
                result.reward += energyReward;
                
                // Apply movement
                agent_.x = newX;
                agent_.y = newY;
                agent_.angularVelocity = 0.0f;
                agent_.isTurning = false;
                agent_.isMoving = true;
                
                // Small time cost for movement
                result.reward -= 0.001f;
            } else {
                // Penalty for hitting walls
                result.reward = -0.05f;
                result.success = false;
            }
            break;
            
        case MotorCommand::MoveBackward:
            // Similar to MoveForward but with different weights
            dx = -std::cos(agent_.orientation) * moveSpeed * 0.5f;
            dy = -std::sin(agent_.orientation) * moveSpeed * 0.5f;
            
            newX = agent_.x + dx * 0.1f;
            newY = agent_.y + dy * 0.1f;
            
            if (isValidPosition(newX, newY)) {
                result.reward = 0.02f;  // Smaller reward for backward movement
                result.success = true;
                
                // Energy reward factor
                float energyReward = agent_.energy / maxEnergy_ * ENERGY_REWARD_FACTOR * 0.5f;
                result.reward += energyReward;
                
                agent_.x = newX;
                agent_.y = newY;
                agent_.angularVelocity = 0.0f;
                agent_.isTurning = false;
                agent_.isMoving = true;
                
                result.reward -= 0.002f;
            } else {
                result.reward = -0.08f;
                result.success = false;
            }
            break;
            
        case MotorCommand::TurnLeft:
        case MotorCommand::TurnRight:
            // Rotation rewards - encourage orientation changes for exploration
            result.reward = -0.01f;  // Small cost for turning
            result.success = true;
            
            // Bonus for turning to face new directions
            if (shouldGiveExplorationBonus(agent_.x, agent_.y, 
                                           agent_.x + std::cos(agent_.orientation + (cmd == MotorCommand::TurnLeft ? -1.0f : 1.0f)) * turnSpeed * 0.1f,
                                           agent_.y + std::sin(agent_.orientation + (cmd == MotorCommand::TurnLeft ? -1.0f : 1.0f)) * turnSpeed * 0.1f)) {
                result.reward += EXPLORATION_BONUS * 0.5f;
            }
            
            agent_.angularVelocity = (cmd == MotorCommand::TurnLeft ? -turnSpeed : turnSpeed);
            agent_.isTurning = true;
            agent_.isMoving = false;
            break;
            
        case MotorCommand::LookLeft:
        case MotorCommand::LookRight:
            // Look actions - no movement cost, but encourage exploration
            result.reward = 0.0f;  // Free to look around
            result.success = true;
            
            // Small exploration bonus for looking in new directions
            if (shouldGiveExplorationBonus(agent_.x, agent_.y,
                                           agent_.x + std::cos(agent_.orientation) * 1.0f,
                                           agent_.y + std::sin(agent_.orientation) * 1.0f)) {
                result.reward += EXPLORATION_BONUS * 0.2f;
            }
            
            agent_.angularVelocity = 0.0f;
            agent_.isTurning = false;
            agent_.isMoving = false;
            break;
            
        case MotorCommand::Interact:
            // Interaction rewards - enhanced with better risk/reward balance
            return applyEnhancedInteraction(currentTime);
            
        case MotorCommand::Wait:
            default:
                // Wait action - encourage staying alive
                result.reward = BASE_REWARD_PER_STEP;
                result.success = true;
                
                // Energy decay reward factor (negative if low energy)
                if (agent_.energy < maxEnergy_ * 0.3f) {
                    result.reward -= 0.02f;  // Penalty for low energy
                }
                
                // Decay velocities
                agent_.velocityX *= 0.9f;
                agent_.velocityY *= 0.9f;
                agent_.angularVelocity *= 0.9f;
                agent_.isMoving = false;
                agent_.isTurning = false;
                break;
    }
    
    // Clamp reward to reasonable bounds
    result.reward = std::max(MIN_REWARD, std::min(MAX_REWARD, result.reward));
    
    return result;
}

// Enhanced interaction with better reward shaping
ActionResult SimpleWorld::applyEnhancedInteraction(double currentTime) {
    ActionResult result;
    result.reward = 0.0f;
    result.success = false;
    
    const float interactRange = 1.0f;
    
    // Track if we've found anything valuable
    bool foundResource = false;
    bool foundHazard = false;
    float bestResourceValue = 0.0f;
    float worstHazardValue = 0.0f;
    
    // Scan for objects in range
    for (auto& obj : objects_) {
        if (!obj.active) continue;
        
        float dx = obj.x - agent_.x;
        float dy = obj.y - agent_.y;
        float dist = std::sqrt(dx * dx + dy * dy);
        
        if (dist < interactRange + obj.radius) {
            obj.active = false;  // Consume the object
            
            if (obj.type == WorldObjectType::Resource) {
                foundResource = true;
                bestResourceValue = std::max(bestResourceValue, obj.value);
                
                // Enhanced energy gain
                float energyGain = resourceEnergyGain_ * (1.0f + (agent_.energy / maxEnergy_) * 0.5f); // More energy when low
                agent_.energy = std::min(maxEnergy_, agent_.energy + energyGain);
                
                // Resource reward with risk/reward balance
                result.reward += obj.value * 1.5f;  // 1.5x multiplier for resources
                result.reward += (agent_.energy / maxEnergy_) * 0.1f;  // Bonus for efficient gathering
                
                result.success = true;
                result.message = "consumed_resource";
                
            } else if (obj.type == WorldObjectType::Hazard) {
                foundHazard = true;
                worstHazardValue = std::min(worstHazardValue, obj.value);
                
                // Calculate risk/reward
                float riskLevel = -obj.value; // Negative value
                float energyBefore = agent_.energy;
                agent_.health -= 0.2f;
                
                // Hazard reward with risk/reward balance
                result.reward += obj.value * 0.5f;  // Smaller penalty for hazards
                
                // If hazard was avoided or mitigated, give credit
                if (agent_.health > 0.7f && energyBefore > agent_.energy) {
                    result.reward += 0.2f;  // Small credit for surviving hazard
                }
                
                result.success = true;
                result.message = "hit_hazard";
            }
            
            // Only interact with one object at a time
            break;
        }
    }
    
    // Exploration bonuses for interaction attempts
    if (!result.success) {
        result.message = "no_object";
        
        // Small penalty for failed interaction attempts (encourage persistence)
        result.reward -= 0.01f;
        
        // Exploration bonus for looking in new areas during interaction
        if (shouldGiveExplorationBonus(agent_.x, agent_.y,
                                       agent_.x + std::cos(agent_.orientation) * interactRange,
                                       agent_.y + std::sin(agent_.orientation) * interactRange)) {
            result.reward += EXPLORATION_BONUS * 0.3f;
        }
    }
    
    // Large exploration bonus for successful resource discovery
    if (foundResource && bestResourceValue > 5.0f) {
        result.reward += 0.5f;  // Significant bonus for valuable resources
    }
    
    // Adaptive risk/reward based on current state
    if (agent_.health < 0.5f) {
        // When health is low, avoid risks
        result.reward *= RISK_REWARD_RATIO;
    }
    
    return result;
}

// Helper function to determine if movement/area is worth exploring
bool SimpleWorld::shouldGiveExplorationBonus(float oldX, float oldY, float newX, float newY) {
    if (!isValidPosition(newX, newY)) {
        return false;  // Invalid position
    }
    
    // Check if position is already well-known (has been visited before)
    bool isWellKnown = false;
    for (const auto& knownPos : visitedPositions_) {
        float dist = std::sqrt((knownPos.x - newX) * (knownPos.x - newX) +
                              (knownPos.y - newY) * (knownPos.y - newY));
        if (dist < 1.0f) {
            isWellKnown = true;
            break;
        }
    }
    
    // Give exploration bonus for moving to less known areas
    bool shouldGiveBonus = !isWellKnown;
    
    if (shouldGiveBonus) {
        // Mark this position as visited
        visitedPositions_.push_back({newX, newY});
        // Limit memory of visited positions
        if (visitedPositions_.size() > 100) {
            visitedPositions_.erase(visitedPositions_.begin());
        }
    }
    
    return shouldGiveBonus;
}

// Set agent start position (overloaded for enhanced version)
void SimpleWorld::setAgentStart(float x, float y) {
    agent_.x = x;
    agent_.y = y;
    
    // Initialize visited positions with starting area
    visitedPositions_.clear();
    visitedPositions_.push_back({x, y});
}

// Get energy reward efficiency
float SimpleWorld::getEnergyRewardEfficiency() const {
    return (agent_.energy / maxEnergy_) * ENERGY_REWARD_FACTOR;
}

// Get exploration level
float SimpleWorld::getExplorationLevel() const {
    return visitedPositions_.size() / 100.0f;  // Normalized to 0-1
}

// Reset visited positions
void SimpleWorld::resetVisitedPositions() {
    visitedPositions_.clear();
    visitedPositions_.push_back({agent_.x, agent_.y});
}

} // namespace nlm