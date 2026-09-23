#pragma once

#include <cstdint>
#include <string>

namespace nlm {

/**
 * @struct AgentBody
 * @brief Physical properties and internal state of an agent
 * 
 * AgentBody represents the physical embodiment of an agent in the environment.
 * It contains properties such as position, orientation, velocity, and internal
 * state variables like energy and health. This is part of the environment layer,
 * not the brain layer, and represents the agent's physical presence and condition.
 * 
 * Key physical properties:
 * - **Position**: (x, y) coordinates in world space
 * - **Orientation**: Rotational angle in radians (0 = facing positive X)
 * - **Velocity**: Linear and angular velocity vectors
 * - **Internal State**: Energy (0-1), health (0-1), developmental age
 * - **Movement State**: Moving/turning flags and timing
 * 
 * The AgentBody tracks the agent's physical state and responds to motor commands
 * from the brain, affecting the agent's position and properties in the world.
 * 
 * @warning AgentBody does not contain cognitive processes - all decision-making
 * is performed by the AgentBrain. This struct represents only the physical
 * embodiment and state of the agent.
 * 
 * @see MotorCommand Low-level motor commands that control this body
 * @see ActionResult Result of applying a motor command to the world
 * @see AgentBrain Brain that controls this agent's motor output
 * 
 * @par Example Usage:
 * @code
 * // Create agent body
 * AgentBody body;
 * 
 * // Initialize at specific position
 * body.x = 1.0f;
 * body.y = 2.0f;
 * body.orientation = 0.5f; // radians
 * 
 * // Execute movement command
 * body.applyMotorCommand(MotorCommand::MoveForward, velocity * dt);
 * 
 * // Check physical state
 * if (body.energy < 0.1f) {
 *     // Low energy, may need to rest or seek resources
 * }
 * @endcode
 */
struct AgentBody {
    // Position in world
    float x;                     ///< X coordinate in world space (meters/units)
    float y;                     ///< Y coordinate in world space (meters/units)
    
    // Orientation (radians)
    float orientation;            ///< Orientation angle in radians (0 = positive X, pi/2 = positive Y)
    
    // Velocity
    float velocityX;              ///< Linear velocity in X direction (units/second)
    float velocityY;              ///< Linear velocity in Y direction (units/second)
    float angularVelocity;        ///< Angular velocity (radians/second)
    
    // Internal state
    float energy;                 ///< Energy level (0.0 = depleted, 1.0 = full)
    float health;                 ///< Health status (0.0 = dead, 1.0 = perfect)
    float age;                    ///< Developmental age in simulation steps
    
    // Movement state
    bool isMoving;                ///< Whether the agent is currently moving
    bool isTurning;               ///< Whether the agent is currently turning
    float lastActionTime;         ///< Time since last action was executed (seconds)
    
    /**
     * @brief Default constructor - initializes agent at origin with full resources
     * 
     * Creates a new agent at the world origin (0,0) with facing positive X direction.
     * The agent starts with maximum energy and health, and zero age (new agent).
     * 
     * @note This represents a newborn agent ready to learn and develop.
     * Use setPosition(), setOrientation(), and other setters to customize initial state.
     * 
     * @see reset() Reset to zero state for new episode
     * @see setPosition() Set specific initial position
     * @see setOrientation() Set specific initial orientation
     */
    AgentBody()
        : x(0.0f), y(0.0f), orientation(0.0f)
        , velocityX(0.0f), velocityY(0.0f), angularVelocity(0.0f)
        , energy(1.0f), health(1.0f), age(0.0f)
        , isMoving(false), isTurning(false), lastActionTime(0.0f) {}
    
    /**
     * @brief Reset agent to initial state
     * 
     * Resets all physical and internal state to values appropriate for a new episode.
     * Restores energy and health to maximum, resets age to zero, and stops all movement.
     * This is typically called at the beginning of a new episode or learning session.
     * 
     * @note This does not reset developmental stage - that's handled by the
     * AgentBrain's development system. This only resets the physical embodiment.
     * 
     * @see AgentBrain.updateDevelopment() Update developmental stage
     * @see getAge() Check current developmental age
     * @see getEnergy() Check current energy level
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
    
    /**
     * @brief Apply a motor command to update physical state
     * 
     * Executes a motor command, updating the agent's position, orientation, velocity,
     * and other physical properties based on the command. Also applies world effects
     * like damage, resource consumption, or energy restoration.
     * 
     * @param command Motor command to execute (e.g., MoveForward, TurnLeft)
     * @param dt Time step duration (seconds)
     * @param worldResponse World effects of this action (reward, damage, etc.)
     * @note This method integrates the motor command with the world environment,
     * producing both physical movement and any consequential effects on the agent's
     * internal state (energy, health, etc.).
     * 
     * @warning This method should be called by the world after the agent executes
     * a motor command. It models the physical consequences of movement.
     * 
     * @see MotorCommand Motor command type definition
     * @see ActionResult World effects from actions
     * @see getPosition() Get current position
     */
    void applyMotorCommand(MotorCommand command, float dt, const ActionResult& worldResponse);
    
    /**
     * @brief Set agent position to specified coordinates
     * 
     * @param x New X coordinate
     * @param y New Y coordinate
     * @note Useful for placing agents at specific starting positions or
     * for teleportation for testing/debugging purposes.
     */
    void setPosition(float x, float y);
    
    /**
     * @brief Set agent orientation
     * 
     * @param angle Orientation angle in radians
     * @note 0.0 = facing positive X direction, pi/2 = facing positive Y direction,
     * pi = facing negative X direction, -pi/2 = facing negative Y direction.
     */
    void setOrientation(float angle);
    
    /**
     * @brief Get current position as a vector
     * 
     * @return std::pair<float, float> Current position (x, y)
     */
    std::pair<float, float> getPosition() const;
    
    /**
     * @brief Get distance to target position
     * 
     * @param targetX Target X coordinate
     * @param targetY Target Y coordinate
     * @return Distance to target position
     * @note Euclidean distance calculation, useful for navigation and decision-making.
     */
    float getDistanceTo(float targetX, float targetY) const;
    
    /**
     * @brief Check if agent is within specified radius of position
     * 
     * @param x X coordinate to check
     * @param y Y coordinate to check
     * @param radius Check radius
     * @return true if agent is within radius of specified position
     * @note Useful for proximity detection and interaction.
     */
    bool isNear(float x, float y, float radius) const;
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
