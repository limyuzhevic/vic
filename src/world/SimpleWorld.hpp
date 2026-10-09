#pragma once

#include "../core/Config/Config.hpp"
#include "../agent/AgentBody.hpp"
#include "../agent/SensoryPercept.hpp"
#include <memory>
#include <string>
#include <vector>

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

// Simple 2D world for NLM development
// Provides sensory experience without semantic labels
class SimpleWorld {
public:
    SimpleWorld(std::shared_ptr<Config> config = nullptr);
    ~SimpleWorld();
    
    // Disable copying
    SimpleWorld(const SimpleWorld&) = delete;
    SimpleWorld& operator=(const SimpleWorld&) = delete;
    
    // Enable moving
    SimpleWorld(SimpleWorld&&) noexcept = default;
    SimpleWorld& operator=(SimpleWorld&&) noexcept = default;
    
    // Configuration
    void configure(float width, float height, size_t visionWidth, size_t visionHeight);
    void configureFromConfig();
    void setRandomSeed(uint64_t seed);
    
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
    
    // Get vision dimensions
    size_t getVisionWidth() const { return visionWidth_; }
    size_t getVisionHeight() const { return visionHeight_; }
    
    // Get energy parameters
    float getMaxEnergy() const { return maxEnergy_; }
    float getEnergyDecayRate() const { return energyDecayRate_; }
    float getResourceEnergyGain() const { return resourceEnergyGain_; }
    
    // Get object placement parameters
    size_t getNumResourceObjects() const { return numResourceObjects_; }
    size_t getNumHazardObjects() const { return numHazardObjects_; }
    
    // Simulation control
    void setSimTime(double time) { simTime_ = time; }
    double getSimTime() const { return simTime_; }
    
    // Random number generation
    uint64_t nextRandom();
    
private:
    // Core configuration
    std::shared_ptr<Config> config_;
    
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
    
    // Object placement configuration
    size_t numResourceObjects_;
    size_t numHazardObjects_;
    
    // Simulation state
    double simTime_;
    
    // Random seed and state
    uint64_t rngSeed_;
    uint64_t rngState_;
    
    // Generate vision from current view
    void generateVision();
    
    // Check for collisions
    void checkCollisions();
    
    // Interact with nearby objects
    ActionResult interact();
};

} // namespace nlm