#pragma once

#include <cstdint>
#include <string>
#include <cmath>
#include <stdexcept>
#include <limits>

namespace nlm {

// Agent body state - contains physical properties and internal state
// This is part of the environment, not the brain
class AgentBody {
public:
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
    
    // Default constructor
    AgentBody()
        : x(0.0f), y(0.0f), orientation(0.0f)
        , velocityX(0.0f), velocityY(0.0f), angularVelocity(0.0f)
        , energy(1.0f), health(1.0f), age(0.0f)
        , isMoving(false), isTurning(false), lastActionTime(0.0f) {}
    
    // Initialize with specific values
    AgentBody(float posX, float posY, float orient)
        : x(posX), y(posY), orientation(orient)
        , velocityX(0.0f), velocityY(0.0f), angularVelocity(0.0f)
        , energy(1.0f), health(1.0f), age(0.0f)
        , isMoving(false), isTurning(false), lastActionTime(0.0f) {}
    
    // Movement methods
    void moveForward(float distance);
    void moveBackward(float distance);
    void turnLeft(float angle);
    void turnRight(float angle);
    void moveTo(float targetX, float targetY);
    
    // Physics integration
    void updatePhysics(float timestep);
    void applyForces(float forceX, float forceY, float torque);
    float getDistanceTo(float targetX, float targetY) const;
    
    // State management
    void reset();
    void updateAge(float timestep);
    void updateEnergy(float deltaEnergy);
    void updateHealth(float deltaHealth);
    
    // Validation methods
    bool isValid() const;
    void validateState() const;
    bool isAtPosition(float x, float y, float tolerance = 0.1f) const;
    bool isWithinRange(float minEnergy = 0.0f, float maxEnergy = 1.0f,
                      float minHealth = 0.0f, float maxHealth = 1.0f) const;
    
    // Serialization support
    std::string toString() const;
    bool fromString(const std::string& data);
    
    // Comparison operators
    bool operator==(const AgentBody& other) const;
    bool operator!=(const AgentBody& other) const;
    
    // Getters with validation
    float getX() const { return x; }
    float getY() const { return y; }
    float getOrientation() const { return orientation; }
    float getEnergy() const { return energy; }
    float getHealth() const { return health; }
    float getAge() const { return age; }
    bool getIsMoving() const { return isMoving; }
    bool getIsTurning() const { return isTurning; }
    float getLastActionTime() const { return lastActionTime; }
    
    // Setters with validation
    void setX(float x) { validatePosition(x, y); this->x = x; }
    void setY(float y) { validatePosition(x, y); this->y = y; }
    void setOrientation(float orientation) { this->orientation = normalizeAngle(orientation); }
    void setEnergy(float energy) { this->energy = clamp(energy, 0.0f, 1.0f); }
    void setHealth(float health) { this->health = clamp(health, 0.0f, 1.0f); }
    void setAge(float age) { this->age = age; }
    void setIsMoving(bool moving) { isMoving = moving; lastActionTime = age; }
    void setIsTurning(bool turning) { isTurning = turning; lastActionTime = age; }
    
    // Utility functions
    static float normalizeAngle(float angle);
    static float clamp(float value, float min, float max);
    static bool isEqual(float a, float b, float epsilon = 0.001f);
    
private:
    void validatePosition(float x, float y) const;
    static constexpr float MAX_POSITION = 10000.0f;  // Reasonable bounds
    static constexpr float MIN_ENERGY = 0.0f;
    static constexpr float MAX_ENERGY = 1.0f;
    static constexpr float MIN_HEALTH = 0.0f;
    static constexpr float MAX_HEALTH = 1.0f;
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
    
    // Validation
    bool isValid() const { return !std::isnan(reward); }
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
