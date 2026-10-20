#pragma once

#include <cstdint>
#include <string>

namespace nlm {

/**
 * @struct AgentBody
 * @brief Physical agent state containing position, orientation, and internal resources
 * @details AgentBody represents the physical embodiment of an agent in the simulated
 * environment. It contains the kinematic properties (position, velocity, orientation)
 * and physiological state (energy, health, age). This structure tracks what the brain
 * does NOT know about directly but must coordinate with for effective action.
 * 
 * Physical Properties:
 * - Position and orientation in 2D space
 * - Linear and angular velocity
 * - Energy reserves (depleted by actions)
 * - Health status (degrades over time)
 * - Developmental age
 * 
 * State Flags:
 * - Movement and turning states
 * - Last action timing
 * - Internal movement indicators
 * 
 * Thread Safety: This struct is not thread-safe for concurrent modification.
 * Should be accessed with external synchronization if shared between threads.
 * 
 * Performance: O(1) for all field access and modification. Constant memory usage
 * (~48 bytes including padding).
 * 
 * Biological Motivation:
 * - Energy depletion mimics metabolic cost of action
 * - Health degradation simulates aging and damage
 * - Age tracks developmental stage and experience
 * - Movement states indicate behavioral modes
 * 
 * Reset Behavior: Reset() restores initial state, useful for:
 * - Beginning new episodes
 * - Recovery from damage
 * - State initialization for testing
 * 
 * @code
 * // Create and use agent body
 * AgentBody body;
 * 
 * // Update position (with physics integration)
 * void updatePosition(AgentBody& body, const MotorCommand& command, float dt) {
 *     // Apply movement
 *     float speed = getSpeed(command);
 *     body.velocityX += speed * cos(body.orientation) * dt;
 *     body.velocityY += speed * sin(body.orientation) * dt;
 *     
 *     // Update position
 *     body.x += body.velocityX * dt;
 *     body.y += body.velocityY * dt;
 *     
 *     // Update orientation
 *     if (command == MotorCommand::TurnLeft) {
 *         body.orientation += TURN_RATE * dt;
 *     } else if (command == MotorCommand::TurnRight) {
 *         body.orientation -= TURN_RATE * dt;
 *     }
 *     
 *     // Deplete energy based on action
 *     body.energy -= ACTION_COST * dt;
 *     body.energy = std::max(0.0f, body.energy);
 * }
 * 
 * // Check for collisions
 * bool checkCollision(const AgentBody& body, const WorldObject& obj) {
 *     // Distance-based collision detection
 *     float dx = body.x - obj.x;
 *     float dy = body.y - obj.y;
 *     float distance = sqrt(dx*dx + dy*dy);
 *     return distance < body.size + obj.size;
 * }
 * @endcode
 * 
 * @see MotorCommand
 * @see ActionResult
 * @see WorldObjectType
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
    
    AgentBody()
        : x(0.0f), y(0.0f), orientation(0.0f)
        , velocityX(0.0f), velocityY(0.0f), angularVelocity(0.0f)
        , energy(1.0f), health(1.0f), age(0.0f)
        , isMoving(false), isTurning(false), lastActionTime(0.0f) {}
    
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
