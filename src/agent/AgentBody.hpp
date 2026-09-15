#ifndef AGENT_BODY_HPP
#define AGENT_BODY_HPP

#include <cstdint>
#include <string>

namespace nlm {

/**
 * @brief Agent body state - contains physical properties and internal state
 * 
 * This is part of the environment, not the brain. The agent body represents
 * the physical embodiment of the agent in the simulated world, including
 * position, orientation, velocity, and internal physiological state.
 */
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
    
    // Movement state
    bool isMoving;
    bool isTurning;
    float lastActionTime;
    
    /**
     * @brief Default constructor initializes agent at origin with full energy
     */
    AgentBody()
        : x(0.0f), y(0.0f), orientation(0.0f)
        , velocityX(0.0f), velocityY(0.0f), angularVelocity(0.0f)
        , energy(1.0f), health(1.0f), age(0.0f)
        , isMoving(false), isTurning(false), lastActionTime(0.0f) {}
    
    /**
     * @brief Reset agent to initial state
     */
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
        isMoving = false;
        isTurning = false;
        lastActionTime = 0.0f;
    }
};

/**
 * @brief Low-level motor commands that the brain can produce
 * 
 * These are innate muscle-like signals, not semantic actions. They represent
 * the basic output commands the agent's motor system can generate.
 */
enum class MotorCommand {
public:
    MoveForward = 0,   // Propel forward
    MoveBackward = 1,  // Propel backward  
    TurnLeft = 2,      // Rotate left
    TurnRight = 3,     // Rotate right
    LookLeft = 4,      // Pan sensor left
    LookRight = 5,     // Pan sensor right
    Interact = 6,      // Interact with nearby object
    Wait = 7           // No movement
};

/**
 * @brief Action result from the world's perspective
 * 
 * This struct captures the outcome of an action taken by the agent,
 * including the reward received and whether the action was successful.
 */
struct ActionResult {
    float reward;
    bool success;
    std::string message;
    
    ActionResult() : reward(0.0f), success(false), message("") {}
    ActionResult(float r, bool s, const std::string& m = "") 
        : reward(r), success(s), message(m) {}
};

/**
 * @brief Object types in the world (for rendering/debug only, NOT given to brain)
 * 
 * These enum values define the types of objects that exist in the simulation
 * world. Note: These types are NOT provided to the brain's sensory input
 * - they are used only for visualization, debugging, and environment management.
 */
enum class WorldObjectType {
public:
    Empty = 0,
    Resource = 1,      // Positive reward
    Hazard = 2,        // Negative reward
    Wall = 3,         // Boundary
    Marker = 4        // Navigation aid
};

} // namespace nlm

#endif // AGENT_BODY_HPP
