#pragma once

#include "../agent/AgentBody.hpp"
#include "../agent/SensoryPercept.hpp"
#include "../core/Exceptions.hpp"
#include "../core/Logger/Logger.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {

// Forward declarations
class NLMWorldException;
class NLMValidationException;

// Error checking utility functions for SimpleWorld operations
namespace SimpleWorldValidation {
    // Input validation for world operations
    inline void validatePosition(float x, float y, const std::string& operation = "") {
        if (std::isnan(x) || std::isinf(x) || std::isnan(y) || std::isinf(y)) {
            NLM_THROW_AND_LOG(NLMValidationException,
                             "Invalid position coordinates: (" + std::to_string(x) + ", " + 
                             std::to_string(y) + ")",
                             NLMException::Severity::ERROR,
                             static_cast<int>(NLMValidationException::ValidationError::VALUE_OUT_OF_RANGE),
                             "world_position_validation");
        }
    }
    
    inline void validateDimensions(float width, float height, const std::string& operation = "") {
        if (width <= 0 || height <= 0) {
            NLM_THROW_AND_LOG(NLMWorldException,
                             "World dimensions must be positive: width=" + std::to_string(width) + 
                             ", height=" + std::to_string(height),
                             NLMException::Severity::ERROR,
                             static_cast<int>(NLMWorldException::WorldError::CONFIGURATION_FAILED),
                             "world_dimensions_validation");
        }
    }
    
    inline void validateVisionDimensions(size_t width, size_t height, const std::string& operation = "") {
        if (width == 0 || height == 0) {
            NLM_THROW_AND_LOG(NLMWorldException,
                             "Vision dimensions must be positive: width=" + std::to_string(width) + 
                             ", height=" + std::to_string(height),
                             NLMException::Severity::ERROR,
                             static_cast<int>(NLMWorldException::WorldError::CONFIGURATION_FAILED),
                             "world_vision_dimensions_validation");
        }
    }
    
    inline void validateTimestep(double timestep, const std::string& operation = "") {
        if (timestep <= 0.0) {
            NLM_THROW_AND_LOG(NLMWorldException,
                             "Timestep must be positive, got: " + std::to_string(timestep),
                             NLMException::Severity::ERROR,
                             static_cast<int>(NLMWorldException::WorldError::INVALID_TIMESTEP),
                             "world_timestep_validation");
        }
    }
    
    inline void validateCommand(MotorCommand cmd, const std::string& operation = "") {
        // Basic validation - motor command should be valid
        // Additional validation can be added based on specific requirements
    }
    
    inline bool isValidWorldState(const SimpleWorld& world) {
        // Check if world is in a valid state
        return world.getWidth() > 0 && world.getHeight() > 0;
    }
}

// World object - represents an object in the world
// Position and type only - no semantic labels
struct WorldObject {
    float x;
    float y;
    float radius;
    WorldObjectType type;
    float value;        // Reward value (for resources) or damage (for hazards)
    bool active;
    
    WorldObject() 
        : x(0.0f), y(0.0f), radius(0.5f), type(WorldObjectType::Empty)
        , value(0.0f), active(true) {}
    
    WorldObject(float px, float py, WorldObjectType t, float v = 0.0f, float r = 0.5f)
        : x(px), y(py), radius(r), type(t), value(v), active(true) {}
};

// Simple 2D world for NLM development
// Provides sensory experience without semantic labels
class SimpleWorld {
public:
    SimpleWorld();
    ~SimpleWorld();
    
    // Configuration
    void configure(float width, float height, size_t visionWidth, size_t visionHeight);
    
    // Reset world to initial state
    void reset();
    
    // Set agent starting position
    void setAgentStart(float x, float y);
    
    // Update world state (call each simulation step)
    void update(double timestep);
    
    // Apply motor command to agent
    ActionResult applyMotorCommand(MotorCommand cmd, double currentTime);
    
    // Get current sensory percept for the agent
    const SensoryPercept& getSensoryPercept() const { return sensory_; }
    
    // Get agent body state
    const AgentBody& getAgentBody() const { return agent_; }
    
    // Add object to world
    void addObject(const WorldObject& obj);
    
    // Remove object at position
    void removeObject(float x, float y);
    
    // Check if position is valid (not wall)
    bool isValidPosition(float x, float y) const;
    
    // Get object at position (if any)
    WorldObject* getObjectAt(float x, float y);
    
    // World dimensions
    float getWidth() const { return width_; }
    float getHeight() const { return height_; }
    
    // Energy settings
    float getMaxEnergy() const { return maxEnergy_; }
    void setMaxEnergy(float e) { maxEnergy_ = e; }
    float getEnergyDecayRate() const { return energyDecayRate_; }
    void setEnergyDecayRate(float r) { energyDecayRate_ = r; }
    float getResourceEnergyGain() const { return resourceEnergyGain_; }
    void setResourceEnergyGain(float e) { resourceEnergyGain_ = e; }
    
    // Time
    double getSimulationTime() const { return simTime_; }
    
    // Seed for reproducibility
    void setRandomSeed(uint64_t seed);
    uint64_t getRandomSeed() const { return rngSeed_; }
    
private:
    // Generate vision from current view
    void generateVision();
    
    // Check for collisions
    void checkCollisions();
    
    // Interact with nearby objects
    ActionResult interact();
    
    // World geometry
    float width_;
    float height_;
    
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
