#pragma once

#include "../agent/AgentBody.hpp"
#include "../agent/SensoryPercept.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <stdexcept>

namespace nlm {

// World object - represents an object in the world
// Position and type only - no semantic labels
struct WorldObject {
    float x;                       ///< X coordinate position
    float y;                       ///< Y coordinate position
    float radius;                   ///< Collision radius
    WorldObjectType type;           ///< Type of object (Resource, Hazard, etc.)
    float value;                   ///< Reward value (for resources) or damage (for hazards)
    bool active;                   ///< Whether object is active in simulation
    
    WorldObject() 
        : x(0.0f), y(0.0f), radius(0.5f), type(WorldObjectType::Empty)
        , value(0.0f), active(true) {}
    
    WorldObject(float px, float py, WorldObjectType t, float v = 0.0f, float r = 0.5f)
        : x(px), y(py), radius(r), type(t), value(v), active(true) {}
    
    /**
     * @brief Check if object is within collision distance of given position
     * 
     * @param testX X coordinate to test
     * @param testY Y coordinate to test
     * @return true if within collision radius
     */
    bool contains(float testX, float testY) const {
        float dx = x - testX;
        float dy = y - testY;
        return (dx * dx + dy * dy) <= (radius * radius);
    }
};

/**
 * @brief Result of applying a motor command
 */
struct ActionResult {
    float reward;                   ///< Reward received from action
    bool success;                   ///< Whether action was successful
    std::string message;            ///< Additional information about action
    
    ActionResult() : reward(0.0f), success(true), message("") {}
    ActionResult(float r, bool s, const std::string& msg = "") 
        : reward(r), success(s), message(msg) {}
};

/**
 * @brief Motor command - agent's desired action
 */
struct MotorCommand {
    ActionType action;              ///< Type of action to perform
    float force;                    ///< Force/magnitude of action
    float duration;                 ///< Duration to apply action
    
    MotorCommand() : action(ActionType::MoveForward), force(1.0f), duration(0.1f) {}
    MotorCommand(ActionType act, float f, float d) : action(act), force(f), duration(d) {}
};

/**
 * @brief Simple 2D world for NLM development
 * 
 * Provides sensory experience without semantic labels and manages
 * agent interactions with the environment.
 */
class SimpleWorld {
public:
    SimpleWorld();
    ~SimpleWorld();
    
    /**
     * @brief Configure world dimensions and sensory capabilities
     * 
     * @param width World width in meters
     * @param height World height in meters
     * @param visionWidth Number of vision cells horizontally
     * @param visionHeight Number of vision cells vertically
     */
    void configure(float width, float height, size_t visionWidth, size_t visionHeight);
    
    /**
     * @brief Reset world to initial state
     */
    void reset();
    
    /**
     * @brief Set agent starting position
     * 
     * @param x X coordinate
     * @param y Y coordinate
     */
    void setAgentStart(float x, float y);
    
    /**
     * @brief Update world state (call each simulation step)
     * 
     * @param timestep Duration of simulation step in seconds
     */
    void update(double timestep);
    
    /**
     * @brief Apply motor command to agent
     * 
     * @param cmd Motor command from agent
     * @param currentTime Current simulation time
     * @return ActionResult with reward and status
     */
    ActionResult applyMotorCommand(MotorCommand cmd, double currentTime);
    
    /**
     * @brief Get current sensory percept for the agent
     * 
     * @return Constant reference to sensory percept
     */
    const SensoryPercept& getSensoryPercept() const { return sensory_; }
    
    /**
     * @brief Get agent body state
     * 
     * @return Constant reference to agent body
     */
    const AgentBody& getAgentBody() const { return agent_; }
    
    /**
     * @brief Add object to world
     * 
     * @param obj World object to add
     */
    void addObject(const WorldObject& obj);
    
    /**
     * @brief Remove object at position
     * 
     * @param x X coordinate
     * @param y Y coordinate
     */
    void removeObject(float x, float y);
    
    /**
     * @brief Check if position is valid (not wall)
     * 
     * @param x X coordinate
     * @param y Y coordinate
     * @return true if position is valid
     */
    bool isValidPosition(float x, float y) const;
    
    /**
     * @brief Get object at position (if any)
     * 
     * @param x X coordinate
     * @param y Y coordinate
     * @return Pointer to object at position or nullptr
     */
    WorldObject* getObjectAt(float x, float y);
    
    // World dimensions
    float getWidth() const { return width_; }
    float getHeight() const { return height_; }
    
    // Energy settings
    float getMaxEnergy() const { return maxEnergy_; }
    void setMaxEnergy(float e) { maxEnergy_ = e; }
    float getEnergyDecayRate() const { return energyDecayRate_; }
    void setEnergyDecayRate(float r) { energyDecayRate_ = r; }
    
    // Time
    double getSimulationTime() const { return simTime_; }
    
    // Seed for reproducibility
    void setRandomSeed(uint64_t seed);
    uint64_t getRandomSeed() const { return rngSeed_; }
    
    // Configuration management
    /**
     * @brief Load configuration from file
     * 
     * @param filepath Path to configuration file
     * @return true if configuration loaded successfully
     */
    bool loadConfig(const std::string& filepath);
    
    /**
     * @brief Save current configuration to file
     * 
     * @param filepath Path to save configuration
     * @return true if configuration saved successfully
     */
    bool saveConfig(const std::string& filepath) const;
    
    /**
     * @brief Get configuration summary
     * 
     * @return String containing configuration summary
     */
    std::string getConfigSummary() const;
    
private:
    // Generate vision from current view
    void generateVision();
    
    // Check for collisions
    void checkCollisions();
    
    // Interact with nearby objects
    ActionResult interact();
    
    // Apply energy decay
    void applyEnergyDecay(double timestep);
    
    // Update agent velocity based on physics
    void updateAgentPhysics(double timestep);
    
    // Validate world configuration
    void validateConfig() const;
    
    // Generate random number
    float randomFloat(float min, float max) const;
    
    // World geometry
    float width_;                    ///< World width in meters
    float height_;                   ///< World height in meters
    size_t visionWidth_;             ///< Horizontal vision resolution
    size_t visionHeight_;            ///< Vertical vision resolution
    
    // Agent state
    AgentBody agent_;                ///< Agent body state
    
    // World objects
    std::vector<WorldObject> objects_; ///< Collection of world objects
    
    // Sensory data
    SensoryPercept sensory_;         ///< Current sensory percept
    
    // Physics and energy
    float maxEnergy_;                ///< Maximum agent energy
    float energyDecayRate_;          ///< Rate of energy decay per timestep
    float resourceEnergyGain_;       ///< Energy gain from collecting resources
    
    // Time tracking
    double simTime_;                 ///< Current simulation time
    
    // Random number generation
    uint64_t rngSeed_;               ///< Seed for reproducible random number generation
    mutable uint64_t rngState_;      ///< Current state of RNG
    
    // Configuration
    std::shared_ptr<Config> config_; ///< World configuration
    
    // Statistics
    size_t stepsCount_;              ///< Number of simulation steps executed
    float totalReward_;              ///< Cumulative reward received
};

} // namespace nlm
    
    // Agent
    AgentBody agent_;
    
    // Objects in world
    std::vector<WorldObject> objects_;
    
    // Sensory state
    SensoryPercept sensory_;
    
    // Vision configuration
    size_t visionWidth_;
    size_t visionHeight_;
    
    // Energy system
    float maxEnergy_;
    float energyDecayRate_;
    float resourceEnergyGain_;
    
    // Simulation time
    double simTime_;
    
    // Random seed
    uint64_t rngSeed_;
    
    // Simple RNG state
    uint64_t rngState_;
    uint64_t nextRandom();
};

} // namespace nlm
