#pragma once

#include "../agent/AgentBody.hpp"
#include "../agent/SensoryPercept.hpp"
#include "../agent/AgentBrain.hpp"
#include "WorldObjects.hpp"
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <cmath>
#include <random>
#include <array>

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
    
    // Enhanced world configuration
    void configureAdvanced(const WorldConfig& config);
    const WorldConfig& getConfig() const { return config_; }
    
    // Environmental systems
    void updateEnvironment(float dt);
    const EnvironmentalState& getEnvironment() const { return environment_; }
    
    // Advanced agent capabilities
    void updateAgentState(float dt);
    const AdvancedAgentState& getAgentState() const { return advancedAgent_; }
    void setGoal(const std::string& goal, float priority);
    void learnBehavior(const std::string& behavior, float successRate);
    
    // Object spawning and management
    void spawnObject(WorldObjectType type, float x, float y, float radius = 0.5f);
    void spawnEnvironmentalEffect(WorldObjectType type, float x, float y);
    void updateObjectBehaviors(float dt);
    
    // Interaction systems
    InteractionResult craft(const std::vector<WorldObjectEnhanced>& materials);
    InteractionResult construct(const WorldObjectEnhanced& blueprint, float x, float y);
    InteractionResult deconstruct(WorldObjectEnhanced& obj);
    InteractionResult transferEnergy(WorldObjectEnhanced& from, WorldObjectEnhanced& to, float amount);
    
    // Advanced sensory processing
    void updateMultimodalSenses();
    void generateAudioSignals();
    void generateChemicalSignals();
    void generateThermalSignals();
    
    // Physics updates
    void updatePhysics(float dt);
    void updateFluidDynamics(float dt);
    void updateRigidBodyDynamics(float dt);
    void updateParticleSystems(float dt);
    
    // Time and development
    void updateTimeProgression(float dt);
    void updateDayNightCycle(float dt);
    void updateSeasons(float dt);
    
    // Statistics and metrics
    float getTotalResourceValue() const;
    int getPopulationCount(WorldObjectType type) const;
    float getAverageHealth() const;
    float get biodiversityIndex() const;
    
private:
    // Generate vision from current view
    void generateVision();
    
    // Check for collisions
    void checkCollisions();
    
    // Interact with nearby objects
    ActionResult interact();
    
    // Enhanced world object with behaviors
struct WorldObjectEnhanced {
    float x;
    float y;
    float radius;
    WorldObjectType type;
    float value;        // Reward value (for resources) or damage (for hazards)
    bool active;
    std::unique_ptr<WorldObjectBehavior> behavior;
    ObjectState state;
    
    WorldObjectEnhanced()
        : x(0.0f), y(0.0f), radius(0.5f), type(WorldObjectType::Empty)
        , value(0.0f), active(true), state(ObjectState::Active) {}
    
    WorldObjectEnhanced(float px, float py, WorldObjectType t, float v = 0.0f, float r = 0.5f)
        : x(px), y(py), radius(r), type(t), value(v), active(true), state(ObjectState::Active) {}
};

// Environmental state
struct EnvironmentalState {
    float temperature = 20.0f;      // Celsius
    float humidity = 0.5f;          // 0-1
    float pressure = 1.0f;          // Atmospheric pressure
    float windSpeed = 0.0f;         // m/s
    float windDirection = 0.0f;     // radians
    float precipitation = 0.0f;     // mm
    float dayNightCycle = 0.0f;     // 0-1, where 0 = night, 1 = day
    bool isStorm = false;
    WeatherType weather = WeatherType::Clear;
};

// Agent state with advanced capabilities
struct AdvancedAgentState {
    float memoryCapacity = 100.0f;
    float emotionalState = 0.5f;    // 0 = low, 1 = high
    float motivation = 0.0f;
    float curiosity = 0.0f;
    float empathy = 0.0f;
    std::unordered_map<std::string, float> goals;
    std::vector<std::string> learnedBehaviors;
    std::vector<std::string> socialConnections;
    std::unordered_map<std::string, float> skills;
};

// Physics state
struct PhysicsState {
    float timeStep = 0.01f;
    float gravity = 9.81f;
    float airResistance = 0.1f;
    float fluidViscosity = 0.001f;
    float elasticModulus = 1.0f;
    float plasticity = 0.1f;
};

// Interaction system for crafting, construction, etc.
struct InteractionResult {
    bool success = false;
    std::string action;
    float cost = 0.0f;
    float reward = 0.0f;
    std::vector<WorldObjectEnhanced> producedObjects;
    std::vector<WorldObjectEnhanced> consumedObjects;
};

// World configuration
struct WorldConfig {
    float width = 20.0f;
    float height = 20.0f;
    size_t visionWidth = 16;
    size_t visionHeight = 16;
    float maxEnergy = 100.0f;
    float energyDecayRate = 0.01f;
    float resourceEnergyGain = 30.0f;
    PhysicsState physics;
    EnvironmentalState environment;
    AdvancedAgentState agent;
    bool enableWeather = false;
    bool enableEcosystem = false;
    bool enableTimeProgression = false;
    float timeScale = 1.0f;
};
};

} // namespace nlm
