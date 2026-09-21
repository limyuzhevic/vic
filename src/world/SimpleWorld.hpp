#pragma once

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
    
    // ========== ADVANCED SIMULATION FEATURES ==========
    
    // Run episode with exploration
    bool runEpisode(size_t numSteps, bool explore = true);
    
    // Generate visualization data
    std::string visualize() const;
    
    // Monitor specific agent
    std::string monitorAgent(size_t agentId) const;
    
    // Monitor all agents
    std::map<size_t, std::string> monitorAllAgents() const;
    
    // Get visualization data
    std::map<std::string, float> getVisualizationData() const;
    
    // Get world statistics for debugging
    std::map<std::string, double> getDebugStats() const;
    
    // ========== MULTI-AGENT SUPPORT ==========
    
    // Add agent to world
    void addAgent(const AgentBody& agent);
    
    // Remove agent by ID
    bool removeAgent(size_t agentId);
    
    // Get list of agent IDs
    std::vector<size_t> getAgentIds() const;
    
    // Get agent by ID
    const AgentBody* getAgent(size_t agentId) const;
    
    // Broadcast message to all agents
    void broadcastMessage(const std::string& message);
    
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
