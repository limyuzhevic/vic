#pragma once

#include "Action.hpp"
#include "../world/WorldObject.hpp"
#include "../brain/Neuron.hpp"
#include <string>
#include <memory>
#include <vector>

namespace nlm {

// Action result from world interaction
struct ActionResult {
    float reward;        // Reward received from world
    bool success;        // Whether action succeeded
    std::string message; // Human-readable message
    
    ActionResult() : reward(0.0f), success(false), message("") {}
    ActionResult(float r, bool s, const std::string& msg = "") 
        : reward(r), success(s), message(msg) {}
};

// Agent body state management
class AgentBody {
public:
    AgentBody();
    ~AgentBody();
    
    // Position and movement
    float x;              // X position
    float y;              // Y position
    float orientation;    // Orientation in radians
    float velocityX;      // X velocity
    float velocityY;      // Y velocity
    float angularVelocity; // Angular velocity
    
    // Physical properties
    float energy;         // Current energy level
    float health;         // Health status (0.0 - 1.0)
    float age;            // Age in simulation steps
    
    // State
    bool isMoving;        // Is currently moving
    bool isTurning;       // Is currently turning
    double lastActionTime; // Time of last action
    
    // Reset to initial state
    void reset();
};

} // namespace nlm
