#pragma once

#include "../agent/AgentBody.hpp"
#include "../agent/SensoryPercept.hpp"
#include "../agent/AgentBrain.hpp"
#include "WorldConstants.hpp"
#include <memory>
#include <string>
#include <vector>
#include <cmath>

namespace nlm {

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

// Forward declarations
class VisionSystem;
class PhysicsSystem;
class CollisionSystem;
class Environment;

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
    
    // Time
    double getSimulationTime() const { return simTime_; }
    
    // Seed for reproducibility
    void setRandomSeed(uint64_t seed);
    uint64_t getRandomSeed() const { return rngSeed_; }
    
    // Validation and utility methods
    bool validateConfiguration() const;
    void validateBounds(float x, float y) const;
    float clampToBounds(float x, float y, float value) const;
    
private:
    // Initialize systems
    void initializeSystems();
    
    // Update methods
    void updatePhysics(double timestep);
    void updateCollisions();
    void updateEnergy(double timestep);
    void updateAge(double timestep);
    
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
    uint64_t rngState_;
    uint64_t nextRandom();
    
    // Helper systems
    std::unique_ptr<VisionSystem> visionSystem_;
    std::unique_ptr<PhysicsSystem> physicsSystem_;
    std::unique_ptr<CollisionSystem> collisionSystem_;
    std::unique_ptr<Environment> environment_;
};

} // namespace nlm
