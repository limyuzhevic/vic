 #pragma once

#include <cstdint>
#include <string>
#include <array>
#include <algorithm>
#include <cmath>
#include <memory>
#include <limits>
#include <mutex>

namespace nlm {

// Forward declarations
class AgentBrain;
class World;

// Enhanced AgentBody with better encapsulation and memory management
// Contains physical properties and internal state of the agent
class AgentBody {
public:
    // Configuration for agent body properties
    struct Config {
        // Position and movement configuration
        float defaultPositionX = 0.0f;
        float defaultPositionY = 0.0f;
        float defaultOrientation = 0.0f;
        float maxPosition = 100.0f;        // World bounds
        float minPosition = -100.0f;
        
        // Velocity configuration
        float maxSpeed = 5.0f;
        float maxAngularSpeed = 3.14f;     // π radians per second
        float acceleration = 2.0f;
        float deceleration = 1.5f;
        
        // Physical properties
        float defaultEnergy = 1.0f;
        float defaultHealth = 1.0f;
        float energyDrainRate = 0.001f;
        float healthDecayRate = 0.0005f;
        float ageDecayRate = 0.0001f;
        
        // Movement state configuration
        float moveThreshold = 0.01f;       // Minimum velocity to consider moving
        float turnThreshold = 0.01f;       // Minimum angular velocity to consider turning
        float actionTimeThreshold = 1.0f;  // Time since last action
        
        // Energy and resource management
        float energyRegenerationRate = 0.0005f;
        float healthRegenerationRate = 0.0002f;
        float energyCostPerAction = 0.1f;
        float healthCostPerCollision = 0.05f;
        
        // Boundary and collision configuration
        float boundaryElasticity = 0.7f;   // 0 = absorbed, 1 = reflected
        float collisionDamping = 0.8f;
        bool wrapAroundWorld = false;
    };

    // Physical state vector for better organization
    struct PhysicalState {
        // Position in world space
        float x = 0.0f;
        float y = 0.0f;
        
        // Orientation (radians)
        float orientation = 0.0f;
        
        // Velocity vector
        float velocityX = 0.0f;
        float velocityY = 0.0f;
        float angularVelocity = 0.0f;
        
        // Physical properties
        float energy = 1.0f;          // Energy level (0-1)
        float health = 1.0f;          // Health (0-1)
        float age = 0.0f;             // Developmental age in simulation steps
        
        // Movement state
        bool isMoving = false;
        bool isTurning = false;
        float lastActionTime = 0.0f;
        
        // Historical state for interpolation
        float previousX = 0.0f;
        float previousY = 0.0f;
        float previousOrientation = 0.0f;
        
        PhysicalState() = default;
        
        PhysicalState(float xPos, float yPos, float orient)
            : x(xPos), y(yPos), orientation(orient) {}
            
        // Interpolation between states
        static PhysicalState interpolate(const PhysicalState& from, const PhysicalState& to, float t) {
            PhysicalState result;
            result.x = from.x + (to.x - from.x) * t;
            result.y = from.y + (to.y - from.y) * t;
            result.orientation = from.orientation + std::fmod(to.orientation - from.orientation + M_PI, 2 * M_PI) - M_PI;
            result.velocityX = from.velocityX + (to.velocityX - from.velocityX) * t;
            result.velocityY = from.velocityY + (to.velocityY - from.velocityY) * t;
            result.angularVelocity = from.angularVelocity + (to.angularVelocity - from.angularVelocity) * t;
            result.energy = from.energy + (to.energy - from.energy) * t;
            result.health = from.health + (to.health - from.health) * t;
            result.age = from.age + (to.age - from.age) * t;
            result.isMoving = to.isMoving; // Use target state for boolean flags
            result.isTurning = to.isTurning;
            result.lastActionTime = to.lastActionTime;
            result.previousX = result.x;
            result.previousY = result.y;
            result.previousOrientation = result.orientation;
            return result;
        }
        
        // State comparison
        bool equals(const PhysicalState& other, float tolerance = 1e-6f) const {
            return std::abs(x - other.x) < tolerance &&
                   std::abs(y - other.y) < tolerance &&
                   std::abs(orientation - other.orientation) < tolerance &&
                   std::abs(velocityX - other.velocityX) < tolerance &&
                   std::abs(velocityY - other.velocityY) < tolerance &&
                   std::abs(angularVelocity - other.angularVelocity) < tolerance &&
                   std::abs(energy - other.energy) < tolerance &&
                   std::abs(health - other.health) < tolerance &&
                   std::abs(age - other.age) < tolerance;
        }
        
        // State bounds checking
        bool isValid(const Config& config) const {
            return x >= config.minPosition && x <= config.maxPosition &&
                   y >= config.minPosition && y <= config.maxPosition &&
                   energy >= 0.0f && energy <= 1.0f &&
                   health >= 0.0f && health <= 1.0f &&
                   std::abs(orientation) < 2.0f * M_PI &&
                   std::abs(velocityX) <= config.maxSpeed &&
                   std::abs(velocityY) <= config.maxSpeed &&
                   std::abs(angularVelocity) <= config.maxAngularSpeed;
        }
        
        // Reset to default values
        void reset() {
            x = 0.0f; y = 0.0f; orientation = 0.0f;
            velocityX = 0.0f; velocityY = 0.0f; angularVelocity = 0.0f;
            energy = 1.0f; health = 1.0f; age = 0.0f;
            isMoving = false; isTurning = false; lastActionTime = 0.0f;
            previousX = x; previousY = y; previousOrientation = orientation;
        }
    };

    // Enhanced motor commands with more sophisticated actions
    enum class MotorCommand {
        // Basic movement
        MoveForward = 0,        // Propel forward
        MoveBackward = 1,       // Propel backward  
        TurnLeft = 2,           // Rotate left
        TurnRight = 3,          // Rotate right
        
        // Sensor control
        LookLeft = 4,           // Pan sensor left
        LookRight = 5,          // Pan sensor right
        LookUp = 6,             // Tilt sensor up
        LookDown = 7,           // Tilt sensor down
        
        // Interaction and manipulation
        Interact = 8,           // Interact with nearby object
        PickUp = 9,             // Pick up object
        Drop = 10,              // Drop carried object
        Use = 11,               // Use equipped item
        
        // Action control
        Wait = 12,              // No movement (default)
        Stop = 13,              // Emergency stop
        Hover = 14,             // Maintain current position
        
        // Advanced control
        Dance = 15,             // Perform dance behavior
        Explore = 16,           // Random exploration
        ReturnHome = 17,        // Return to starting position
        Patrol = 18,            // Follow patrol route
        
        // Sensory modes
        SenseVision = 19,       // Focus on vision
        SenseTouch = 20,        // Focus on touch
        SenseInternal = 21,     // Focus on internal sensors
        SenseProprioception = 22,// Focus on proprioception
        SenseAudio = 23,        // Focus on audio
        
        // Meta-actions
        Reset = 24,             // Reset agent state
        Sleep = 25,             // Enter sleep/rest mode
        Wake = 26,              // Wake from sleep
        
        // Utility
        Count = 27              // Keep as last value for array sizing
    };

    // Enhanced action results with more detailed feedback
    struct ActionResult {
        // Core results
        bool success = false;
        float reward = 0.0f;
        std::string message;
        
        // Detailed outcome information
        float damage = 0.0f;
        float energyChange = 0.0f;
        float healthChange = 0.0f;
        float positionChange = 0.0f;
        
        // Context information
        std::string targetId;
        std::string actionType;
        double timestamp = 0.0;
        
        // State changes
        bool agentDied = false;
        bool episodeTerminated = false;
        std::string terminationReason;
        
        ActionResult() = default;
        
        ActionResult(bool success, float reward, const std::string& message = "")
            : success(success), reward(reward), message(message) {}
            
        ActionResult(bool success, float reward, float damage, float energyChange, float healthChange,
                    const std::string& message = "")
            : success(success), reward(reward), damage(damage), energyChange(energyChange), 
              healthChange(healthChange), message(message) {}
        
        // Serialization for checkpoint/restore
        std::string serialize() const;
        static ActionResult deserialize(const std::string& data);
        
        // Result validation
        bool isValid() const {
            return reward >= -100.0f && reward <= 100.0f && // Reasonable reward range
                   damage >= 0.0f && damage <= 100.0f &&
                   energyChange >= -1.0f && energyChange <= 1.0f &&
                   healthChange >= -1.0f && healthChange <= 1.0f;
        }
        
        // Result comparison
        bool operator==(const ActionResult& other) const {
            return success == other.success && 
                   std::abs(reward - other.reward) < 1e-6f &&
                   damage == other.damage &&
                   energyChange == other.energyChange &&
                   healthChange == other.healthChange;
        }
    };

    // Object types in the world (for rendering/debug only, NOT given to brain)
    enum class WorldObjectType {
        Empty = 0,
        Resource = 1,      // Positive reward
        Hazard = 2,        // Negative reward
        Wall = 3,         // Boundary
        Marker = 4,        // Navigation aid
        Agent = 5,         // Other agents
        Food = 6,          // Energy/health restoration
        Tool = 7,          // Usable item
        Door = 8,          // Interactive object
        Light = 9,         // Illuminated area
        Decay = 10         // Hazardous area
    };

    // World object information for rendering/debug
    struct WorldObjectInfo {
        WorldObjectType type = WorldObjectType::Empty;
        float x = 0.0f;
        float y = 0.0f;
        float size = 1.0f;
        float rotation = 0.0f;
        std::string name;
        std::string description;
        float value = 0.0f;           // For resources, tools, etc.
        float hazardLevel = 0.0f;     // For hazards
        bool interactive = false;
        std::string interactionText;
        
        WorldObjectInfo() = default;
        
        WorldObjectInfo(WorldObjectType t, float xPos, float yPos, float s = 1.0f)
            : type(t), x(xPos), y(yPos), size(s) {}
        
        bool isValid() const {
            return type != WorldObjectType::Empty || 
                   (x >= -1000.0f && x <= 1000.0f &&
                    y >= -1000.0f && y <= 1000.0f);
        }
    };

    // Performance optimization structures
    struct PerformanceMetrics {
        double lastUpdateTime = 0.0;
        double computeTime = 0.0;      // Time spent in computations
        double renderTime = 0.0;       // Time spent in rendering
        double networkTime = 0.0;      // Time spent in communication
        size_t memoryUsage = 0;        // Current memory usage
        size_t signalCount = 0;        // Number of active signals
        
        void reset() {
            lastUpdateTime = 0.0;
            computeTime = 0.0;
            renderTime = 0.0;
            networkTime = 0.0;
            memoryUsage = 0;
            signalCount = 0;
        }
        
        double getTotalTime() const {
            return computeTime + renderTime + networkTime;
        }
        
        float getEfficiency() const {
            if (getTotalTime() <= 0.0) return 1.0f;
            return 1.0f / (1.0f + getTotalTime() / 1000.0f); // Normalized efficiency
        }
    };

    // Constructor and destructor
    AgentBody(const Config& config = Config{});
    ~AgentBody();
    
    // Copy constructor and assignment operator
    AgentBody(const AgentBody& other);
    AgentBody& operator=(const AgentBody& other);
    
    // Move constructor and assignment operator
    AgentBody(AgentBody&& other) noexcept;
    AgentBody& operator=(AgentBody&& other) noexcept;
    
    // Configuration and initialization
    void setConfig(const Config& config) { config_ = config; }
    const Config& getConfig() const { return config_; }
    
    void initialize(float startX, float startY, float startOrientation = 0.0f);
    void reset();
    
    // State management
    void update(double timestep);
    void interpolate(const PhysicalState& targetState, float alpha);
    
    // Position and movement
    void setPosition(float x, float y);
    void setOrientation(float orientation);
    void setVelocity(float vx, float vy, float angularVel = 0.0f);
    
    void move(MotorCommand command, double timestep);
    void applyForce(float fx, float fy, float torque = 0.0f);
    void applyImpulse(float fx, float fy, float torque = 0.0f);
    
    // State queries
    const PhysicalState& getState() const { return state_; }
    PhysicalState getSmoothedState(float alpha = 0.1f) const;
    
    // Physical property queries
    float getX() const { return state_.x; }
    float getY() const { return state_.y; }
    float getOrientation() const { return state_.orientation; }
    float getVelocityX() const { return state_.velocityX; }
    float getVelocityY() const { return state_.velocityY; }
    float getAngularVelocity() const { return state_.angularVelocity; }
    float getEnergy() const { return state_.energy; }
    float getHealth() const { return state_.health; }
    float getAge() const { return state_.age; }
    
    // State flags
    bool isMoving() const { return state_.isMoving; }
    bool isTurning() const { return state_.isTurning; }
    bool hasMoved(double since) const { return (state_.lastActionTime + since) > 0.0; }
    
    // Energy and health management
    void setEnergy(float energy);
    void setHealth(float health);
    void setAge(double age);
    void damage(float amount);
    void heal(float amount);
    void consumeEnergy(float amount);
    void regenerateEnergy(double timestep);
    void regenerateHealth(double timestep);
    
    // Movement and action management
    void executeAction(MotorCommand command, double timestep);
    void stop();
    void setActionTime(double time) { state_.lastActionTime = time; }
    double getLastActionTime() const { return state_.lastActionTime; }
    
    // Interaction and environment
    ActionResult interactWithObject(const WorldObjectInfo& object);
    void checkBoundaries(const Config& config);
    void applyGravity(float strength = 9.8f);
    
    // Validation and safety
    bool isValid() const;
    bool isSafe() const;
    std::string validateState() const;
    
    // Performance monitoring
    const PerformanceMetrics& getPerformanceMetrics() const { return perfMetrics_; }
    void resetPerformanceMetrics() { perfMetrics_.reset(); }
    
    // Serialization for checkpoint/restore
    std::string serialize() const;
    static AgentBody deserialize(const std::string& data);
    
    // Memory efficiency
    size_t memoryUsage() const;
    void optimizeMemoryUsage();
    
    // Debugging and visualization
    std::string debugString() const;
    void logState() const;
    
    // Environment integration
    void connectToBrain(std::shared_ptr<AgentBrain> brain) { brain_ = brain; }
    void connectToWorld(std::shared_ptr<World> world) { world_ = world; }
    
    // Statistical analysis
    float getSpeed() const {
        return std::sqrt(state_.velocityX * state_.velocityX + 
                        state_.velocityY * state_.velocityY);
    }
    
    float getAngularSpeed() const { return std::abs(state_.angularVelocity); }
    
    double getDistanceTo(float x, float y) const {
        float dx = state_.x - x;
        float dy = state_.y - y;
        return std::sqrt(dx * dx + dy * dy);
    }
    
    double getAngleTo(float x, float y) const {
        float dx = x - state_.x;
        float dy = y - state_.y;
        return std::atan2(dy, dx);
    }
    
    // Boundary checking helper
    bool isOutOfBounds(float x, float y) const {
        return x < config_.minPosition || x > config_.maxPosition ||
               y < config_.minPosition || y > config_.maxPosition;
    }
    
    // Movement clamping helper
    void clampToBounds(float& x, float& y) const {
        x = std::max(config_.minPosition, std::min(config_.maxPosition, x));
        y = std::max(config_.minPosition, std::min(config_.maxPosition, y));
    }

private:
    // Core state management
    PhysicalState state_;
    Config config_;
    
    // External systems
    std::shared_ptr<AgentBrain> brain_;
    std::shared_ptr<World> world_;
    
    // Performance monitoring
    mutable PerformanceMetrics perfMetrics_;
    
    // Threading mutex for thread-safe operations
    mutable std::mutex mutex_;
    
    // Private helper methods
    void updateMovementState();
    void updateEnergyHealth(double timestep);
    void updateAge(double timestep);
    void handleBoundaryCollision();
    void applyMovementPhysics(double timestep);
    
    // Movement calculation helpers
    void moveForward(double timestep, float speedFactor = 1.0f);
    void moveBackward(double timestep, float speedFactor = 1.0f);
    void turnLeft(double timestep, float speedFactor = 1.0f);
    void turnRight(double timestep, float speedFactor = 1.0f);
    void lookLeft(double timestep, float speedFactor = 1.0f);
    void lookRight(double timestep, float speedFactor = 1.0f);
    void lookUp(double timestep, float speedFactor = 1.0f);
    void lookDown(double timestep, float speedFactor = 1.0f);
    void interact(double timestep, float speedFactor = 1.0f);
    
    // Action execution helpers
    void executeMovementAction(MotorCommand command, double timestep);
    void executeSensorAction(MotorCommand command, double timestep);
    void executeInteractionAction(MotorCommand command, double timestep);
    void executeMetaAction(MotorCommand command, double timestep);
    
    // State validation helpers
    bool validatePosition(float x, float y) const;
    bool validateEnergy(float energy) const;
    bool validateHealth(float health) const;
    bool validateOrientation(float orientation) const;
    
    // Serialization helpers
    std::string serializeState() const;
    PhysicalState deserializeState(const std::string& data) const;
    std::string serializeConfig() const;
    Config deserializeConfig(const std::string& data) const;
    
    // Memory optimization helpers
    void compactState();
    void clearHistory();
    
    // Debug and logging helpers
    void logMovement() const;
    void logEnergyHealth() const;
    void logPerformance() const;
    
}; // namespace nlm