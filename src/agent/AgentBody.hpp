#pragma once

#include <cstdint>
#include <string>
#include <map>
#include <vector>
#include <chrono>

namespace nlm {

// Enhanced Agent body state with advanced capabilities
struct AgentBody {
    // Position in world
    float x;
    float y;
    
    // Orientation (radians)
    float orientation;
    
    // Velocity
    float velocityX;
    float velocityY;
    float angularVelocity;
    
    // Internal state
    float energy;          // Energy level (0-1)
    float health;          // Health (0-1)
    float age;             // Developmental age in simulation steps
    float hunger;          // Hunger level (0-1)
    float thirst;          // Thirst level (0-1)
    float fatigue;         // Fatigue level (0-1)
    
    // Movement state
    bool isMoving;
    bool isTurning;
    bool isInteracting;
    bool isFeeding;
    bool isHydrating;
    bool isSleeping;
    float lastActionTime;
    float lastFoodTime;
    float lastWaterTime;
    
    // Advanced capabilities
    float metabolicRate;    // Energy consumption rate
    float learningRate;    // Learning rate (0-1)
    float explorationRate; // Exploration vs exploitation (0-1)
    float curiosityThreshold; // Threshold for curiosity-driven actions
    
    // Social behavior
    bool isSocial;         // Social capability
    float socialBonding;   // Social bonding level (0-1)
    bool followsLeader;    // Follows group leader
    bool shareResources;  // Shares food/water with others
    
    // Advanced agent properties
    float size;            // Physical size (0-1)
    float speed;           // Maximum speed (0-1)
    float agility;         // Agility factor (0-1)
    float perceptionRange; // Sensory perception range
    float hearingRange;    // Auditory perception range
    float smellRange;      // Olfactory perception range
    
    AgentBody()
        : x(0.0f), y(0.0f), orientation(0.0f)
        , velocityX(0.0f), velocityY(0.0f), angularVelocity(0.0f)
        , energy(1.0f), health(1.0f), age(0.0f), hunger(0.0f), thirst(0.0f), fatigue(0.0f)
        , isMoving(false), isTurning(false), isInteracting(false), isFeeding(false), 
          isHydrating(false), isSleeping(false), lastActionTime(0.0f), 
          lastFoodTime(0.0f), lastWaterTime(0.0f)
        , metabolicRate(0.01f), learningRate(0.5f), explorationRate(0.3f), 
          curiosityThreshold(0.5f)
        , isSocial(false), socialBonding(0.0f), followsLeader(false), shareResources(false)
        , size(0.5f), speed(0.5f), agility(0.5f), perceptionRange(10.0f), 
          hearingRange(8.0f), smellRange(6.0f) {}
    
    void reset() {
        x = 0.0f;
        y = 0.0f;
        orientation = 0.0f;
        velocityX = 0.0f;
        velocityY = 0.0f;
        angularVelocity = 0.0f;
        energy = 1.0f;
        health = 1.0f;
        age = 0.0f;
        hunger = 0.0f;
        thirst = 0.0f;
        fatigue = 0.0f;
        isMoving = false;
        isTurning = false;
        isInteracting = false;
        isFeeding = false;
        isHydrating = false;
        isSleeping = false;
        lastActionTime = 0.0f;
        lastFoodTime = 0.0f;
        lastWaterTime = 0.0f;
    }
};

// Low-level motor commands that the brain can produce
// These are innate muscle-like signals, not semantic actions
enum class MotorCommand {
    MoveForward = 0,   // Propel forward
    MoveBackward = 1,  // Propel backward  
    TurnLeft = 2,      // Rotate left
    TurnRight = 3,     // Rotate right
    LookLeft = 4,      // Pan sensor left
    LookRight = 5,     // Pan sensor right
    Interact = 6,      // Interact with nearby object
    Wait = 7           // No movement
};

// Action result from the world's perspective
struct ActionResult {
    float reward;
    bool success;
    std::string message;
    
    ActionResult() : reward(0.0f), success(false), message("") {}
    ActionResult(float r, bool s, const std::string& m = "") 
        : reward(r), success(s), message(m) {}
};

// Object types in the world (for rendering/debug only, NOT given to brain)
enum class WorldObjectType {
    Empty = 0,
    Resource = 1,      // Positive reward
    Hazard = 2,        // Negative reward
    Wall = 3,         // Boundary
    Marker = 4        // Navigation aid
};

} // namespace nlm
