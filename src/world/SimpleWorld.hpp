#pragma once

#include "../agent/AgentBody.hpp"
#include "../agent/SensoryPercept.hpp"
#include <memory>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <utility>
#include <functional>
#include <chrono>

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

// Spatial grid cell structure for efficient collision detection
struct GridCell {
    std::vector<WorldObject*> objects;
};

// Dynamic behavior types for objects
enum class ObjectBehavior {
    Static,
    Chasing,
    Wandering,
    Rotating,
    Oscillating,
    Periodic
};

// Environmental hazard types
enum class HazardType {
    StaticHazard,
    RotatingHazard,
    ExpandingHazard,
    PulsingHazard,
    StormHazard
};

// Simple linear congruential generator
static uint64_t lcg(uint64_t& state);

static float urand(uint64_t& state, float min, float max);

// Spatial grid manager for O(1) collision detection
class SpatialGrid {
public:
    SpatialGrid(float width, float height, float cellSize = 1.0f); 
    
    void clear();
    
    void insert(WorldObject* obj);
    
    void updateObject(WorldObject* oldObj, WorldObject* newObj);
    
    std::vector<WorldObject*> getObjectsInCell(int x, int y) const;
    
    // Get all objects in the immediate vicinity (3x3 neighborhood)
    std::vector<WorldObject*> getNearbyObjects(float x, float y, float range) const;
    
    float getCellSize() const { return cellSize_; }
    
private:
    float width_;
    float height_;
    float cellSize_;
    std::vector<std::vector<GridCell>> grid_;
};

// Moving object with dynamic behavior
struct DynamicObject {
    WorldObject* obj;
    float velocityX;
    float velocityY;
    float angularVelocity;
    ObjectBehavior behavior;
    float behaviorTimer;
    float behaviorPeriod;
    float chaseDistance;
    float wanderAngle;
    
    DynamicObject(WorldObject* o); 
};

// Environmental hazard with time-varying properties
struct EnvironmentalHazard {
    HazardType type;
    float radius;
    float intensity;  // 0.0-1.0 for pulsing, angle for rotating, etc.
    float growthRate;
    float damagePerSecond;
    float activationTimer;
    bool active;
    
    EnvironmentalHazard(); 
};

// Collision prediction structure
struct CollisionPrediction {
    float timeToCollision;
    WorldObject* obj1;
    WorldObject* obj2;
    float normalX;
    float normalY;
    
    CollisionPrediction(); 
};

// Simple 2D world for NLM development
// Provides sensory experience without semantic labels
class SimpleWorld {
public:
    SimpleWorld();
    ~SimpleWorld();
    
    // Configuration
    void configure(float width, float height, size_t visionWidth, size_t visionHeight);
    
    // Configuration options for dynamic behaviors
    void enableTrajectoryPrediction(bool enabled);
    void enableDynamicObstacles(bool enabled);
    void enableEnvironmentalHazards(bool enabled);
    void setHazardSpawnRate(float rate);
    void setPredatorSpawnRate(float rate);
    void setWanderingResourceRate(float rate);
    void setRotatingHazardRate(float rate);
    
    // Getters for configuration
    bool isTrajectoryPredictionEnabled() const;
    bool isDynamicObstaclesEnabled() const;
    bool isEnvironmentalHazardsEnabled() const;
    float getHazardSpawnRate() const;
    float getPredatorSpawnRate() const;
    float getWanderingResourceRate() const;
    float getRotatingHazardRate() const;
    
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
    
    // Additional utility methods
    void addHazard(const EnvironmentalHazard& hazard);
    void removeHazard(size_t index);
    void clearHazards();
    void setAgentMaxHealth(float health);
    void setAgentDamagePerHazard(float damage);
    void setObjectRepulsionForce(float force);
    void setEnvironmentDecayRate(float rate);
    
    // Getters for object count
    int getObjectCount() const;
    int getHazardCount() const;
    int getDynamicObjectCount() const;
    
    // Utility method to get spatial grid cell size
    float getCellSize() const;
    
private:
    // Dynamic object management
    void updateDynamicObjects(double timestep);
    void setDynamicBehavior(WorldObject* obj, ObjectBehavior behavior);
    void setDynamicBehaviorPeriod(WorldObject* obj, float period);
    void chaseAgent(DynamicObject& dynObj, double timestep);
    void wander(DynamicObject& dynObj, double timestep);
    void rotate(DynamicObject& dynObj, double timestep);
    void oscillate(DynamicObject& dynObj, double timestep);
    void periodic(DynamicObject& dynObj, double timestep);
    void checkDynamicObjectCollision(WorldObject& obj, double timestep);
    
    // Environmental hazard management
    void updateEnvironmentalHazards(double timestep);
    void spawnRandomHazard();
    void checkHazardDamage(const EnvironmentalHazard& hazard, double timestep);
    
    // Collision prediction and resolution
    CollisionPrediction predictCollision(float startX, float startY, float endX, float endY);
    void resolveTrajectoryCollisions(float prevX, float prevY, float newX, float newY, double timestep);
    void checkPathCollisions(float startX, float startY, float endX, float endY);
    
    // Spatial grid management
    void updateSpatialGrid();
    
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
    
    // Dynamic objects
    std::vector<DynamicObject> dynamicObjects_;
    
    // Environmental hazards
    std::vector<EnvironmentalHazard> hazards_;
    
    // Spatial grid for collision detection
    SpatialGrid spatialGrid_;
    
    // Configuration flags
    bool enableTrajectoryPrediction_;
    bool enableDynamicObstacles_;
    bool enableEnvironmentalHazards_;
    
    // Spawn rates for different object types
    float hazardSpawnRate_;
    float predatorSpawnRate_;
    float wanderingResourceRate_;
    float rotatingHazardRate_;
    
    // Physics constants
    float objectRepulsionForce_;
    float environmentDecayRate_;
    
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