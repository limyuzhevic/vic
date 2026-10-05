#pragma once

#include "../agent/AgentBody.hpp"
#include "../agent/SensoryPercept.hpp"
#include <memory>
#include <string>
#include <vector>
#include <cmath>

namespace nlm {

// SimpleWorld configuration constants
struct WorldConfig {
    // World geometry
    static constexpr float defaultWidth = 20.0f;
    static constexpr float defaultHeight = 20.0f;
    static constexpr float wallThickness = 0.1f;
    static constexpr float boundaryBuffer = 1.0f;
    
    // Vision system
    static constexpr size_t defaultVisionWidth = 16;
    static constexpr size_t defaultVisionHeight = 16;
    static constexpr float fovAngle = M_PI / 2.0f;  // 90 degrees
    static constexpr float maxVisionRange = 8.0f;
    static constexpr size_t rayCount = 16;
    static constexpr float raySpacing = 0.1f;
    
    // Sensor configurations
    static constexpr size_t touchSensorCount = 8;
    static constexpr size_t internalSensorCount = 4;
    static constexpr size_t proprioceptionSensorCount = 6;
    static constexpr size_t audioSensorCount = 0;  // Future extension
    
    // Agent properties
    static constexpr float defaultMaxEnergy = 100.0f;
    static constexpr float energyDecayRate = 0.01f;
    static constexpr float defaultEnergyGain = 30.0f;
    static constexpr float defaultHealth = 1.0f;
    static constexpr float healthDecayRate = 0.0f;  // No passive decay by default
    
    // Movement and physics
    static constexpr float moveSpeed = 3.0f;
    static constexpr float backwardMoveSpeed = 1.5f;
    static constexpr float turnSpeed = 2.0f;
    static constexpr float maxVelocity = 5.0f;
    static constexpr float maxAngularVelocity = 3.0f;
    static constexpr float velocityDecay = 0.9f;
    
    // Interaction
    static constexpr float interactionRange = 1.0f;
    static constexpr float resourceEnergyGain = 30.0f;
    static constexpr float hazardHealthLoss = 0.2f;
    static constexpr float passiveHazardDamage = 0.01f;
    
    // Object placement
    static constexpr int defaultResourceCount = 5;
    static constexpr int defaultHazardCount = 3;
    static constexpr float minObjectDistance = 2.0f;
    static constexpr float objectRadius = 0.5f;
    
    // Time and simulation
    static constexpr double defaultTimeStep = 0.1f;
    static constexpr double actionCooldown = 0.1f;
    static constexpr double maxSimulationTime = 86400.0;  // 24 hours
    
    // Randomization
    static constexpr uint64_t defaultRandomSeed = 42;
    
    // Performance
    static constexpr size_t maxObjects = 100;
    static constexpr size_t visionCacheSize = 256;
    static constexpr size_t touchCacheSize = 8;
    
    // Debug and development
    static constexpr bool enableDebugLogging = true;
    static constexpr bool enablePerformanceMetrics = true;
    static constexpr bool enableCollisionDetection = true;
    static constexpr bool enableObjectCaching = true;
};

// SimpleWorld event system
class WorldEventListener {
public:
    virtual ~WorldEventListener() = default;
    virtual void onObjectCreated(float x, float y, WorldObjectType type, float value) = 0;
    virtual void onObjectDestroyed(float x, float y, WorldObjectType type) = 0;
    virtual void onCollision(float agentX, float agentY, WorldObjectType objectType) = 0;
    virtual void onInteraction(float reward, bool success, const std::string& message) = 0;
    virtual void onUpdate(double timestep) = 0;
};

// Vision ray caster for efficient line-of-sight calculation
class VisionRayCaster {
public:
    VisionRayCaster(float worldWidth, float worldHeight);
    ~VisionRayCaster() = default;
    
    std::vector<float> castRays(float agentX, float agentY, float orientation) const;
    
    void setFOV(float fov) { fov_ = fov; }
    void setMaxRange(float maxRange) { maxRange_ = maxRange; }
    void setRayCount(size_t rayCount) { rayCount_ = rayCount; }
    
private:
    float worldWidth_;
    float worldHeight_;
    float fov_;
    float maxRange_;
    size_t rayCount_;
    
    float castSingleRay(float startX, float startY, float angle, 
                       const std::vector<WorldObject>& objects) const;
    
    // Precompute ray angles for efficiency
    std::vector<float> precomputedAngles_;
    bool anglesDirty_;
    
    void updatePrecomputedAngles();
};

// Touch sensor system for proximity detection
class TouchSensorSystem {
public:
    TouchSensorSystem(float worldWidth, float worldHeight);
    ~TouchSensorSystem() = default;
    
    std::vector<float> computeTouchSensors(float agentX, float agentY, float orientation,
                                          const std::vector<WorldObject>& objects) const;
    
    void setSensorCount(size_t count) { sensorCount_ = count; }
    void setMaxRange(float maxRange) { maxRange_ = maxRange; }
    
private:
    float worldWidth_;
    float worldHeight_;
    size_t sensorCount_;
    float maxRange_;
    
    float computeSingleSensor(float sensorAngle, float agentX, float agentY,
                            const std::vector<WorldObject>& objects) const;
};

// Object manager for efficient world object handling
class ObjectManager {
public:
    ObjectManager() = default;
    ~ObjectManager() = default;
    
    void addObject(const WorldObject& obj);
    void removeObject(size_t index);
    void removeObjectAt(float x, float y);
    
    WorldObject* getObjectAt(float x, float y);
    const WorldObject* getObjectAt(float x, float y) const;
    
    // Spatial queries
    std::vector<size_t> getObjectsInRange(float x, float y, float range) const;
    
    // Object manipulation
    void updateObject(size_t index, float x, float y);
    void activateObject(size_t index);
    void deactivateObject(size_t index);
    
    // Iteration
    size_t getObjectCount() const { return objects_.size(); }
    const std::vector<WorldObject>& getAllObjects() const { return objects_; }
    
    // Object type statistics
    int getResourceCount() const;
    int getHazardCount() const;
    int getWallCount() const;
    
    // Clearing and resetting
    void clear();
    void resetActiveFlags();
    
private:
    std::vector<WorldObject> objects_;
    
    // Spatial indexing for faster queries (simple implementation)
    std::vector<std::vector<size_t>> spatialGrid_;
    size_t gridSize_;
    
    void rebuildSpatialGrid();
};

// Performance metrics collector
class PerformanceMetrics {
public:
    PerformanceMetrics() = default;
    ~PerformanceMetrics() = default;
    
    void recordVisionTime(double time) { visionTime_ = time; }
    void recordPhysicsTime(double time) { physicsTime_ = time; }
    void recordCollisionTime(double time) { collisionTime_ = time; }
    void recordUpdateTime(double time) { updateTime_ = time; }
    
    void recordSensoryProcessingTime(double time) { sensoryTime_ = time; }
    void recordInteractionTime(double time) { interactionTime_ = time; }
    
    double getVisionTime() const { return visionTime_; }
    double getPhysicsTime() const { return physicsTime_; }
    double getCollisionTime() const { return collisionTime_; }
    double getUpdateTime() const { return updateTime_; }
    double getSensoryTime() const { return sensoryTime_; }
    double getInteractionTime() const { return interactionTime_; }
    
    void reset() {
        visionTime_ = physicsTime_ = collisionTime_ = updateTime_ = 0.0;
        sensoryTime_ = interactionTime_ = 0.0;
        frameCount_ = 0;
    }
    
    double getFPS() const {
        if (frameCount_ > 0 && updateTime_ > 0) {
            return 1000.0 / (updateTime_ / frameCount_);
        }
        return 0.0;
    }
    
    void incrementFrameCount() { frameCount_++; }
    
private:
    double visionTime_ = 0.0;
    double physicsTime_ = 0.0;
    double collisionTime_ = 0.0;
    double updateTime_ = 0.0;
    double sensoryTime_ = 0.0;
    double interactionTime_ = 0.0;
    size_t frameCount_ = 0;
};

// Main SimpleWorld class with improved architecture
class SimpleWorld {
friend class VisionRayCaster;
friend class TouchSensorSystem;

public:
    SimpleWorld();
    ~SimpleWorld();
    
    // Configuration
    void configure(float width, float height, size_t visionWidth, size_t visionHeight);
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
    const WorldObject* getObjectAt(float x, float y) const;
    
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
    
    // Event system
    void addEventListener(WorldEventListener* listener) { eventListeners_.push_back(listener); }
    void removeEventListener(WorldEventListener* listener);
    
    // Performance monitoring
    void enablePerformanceMonitoring(bool enable) { enablePerformanceMetrics_ = enable; }
    const PerformanceMetrics& getPerformanceMetrics() const { return metrics_; }
    void resetPerformanceMetrics() { metrics_.reset(); }
    
    // Debug and development
    void enableDebugLogging(bool enable) { enableDebugLogging_ = enable; }
    void printDebugInfo() const;
    std::string getDebugInfo() const;
    
    // Statistics
    size_t getObjectCount() const { return objectManager_.getObjectCount(); }
    int getResourceCount() const { return objectManager_.getResourceCount(); }
    int getHazardCount() const { return objectManager_.getHazardCount(); }
    int getWallCount() const { return objectManager_.getWallCount(); }
    
private:
    // World geometry
    float width_;
    float height_;
    
    // Agent
    AgentBody agent_;
    
    // Object management
    ObjectManager objectManager_;
    
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
    
    // Systems
    VisionRayCaster visionCaster_;
    TouchSensorSystem touchSystem_;
    
    // Event listeners
    std::vector<WorldEventListener*> eventListeners_;
    
    // Performance metrics
    PerformanceMetrics metrics_;
    bool enablePerformanceMetrics_;
    
    // Debug logging
    bool enableDebugLogging_;
    
    // World generation
    void generateVision();
    void generateTouchSensors();
    void generateInternalSignals();
    void generateProprioception();
    
    // Physics and collision
    void updatePhysics(double timestep);
    void checkCollisions();
    void handleBoundaryCollisions();
    
    // Interaction and objects
    ActionResult interact();
    void updateObjects(double timestep);
    
    // Helper functions
    void logDebug(const std::string& message) const;
    void notifyEventListeners(const std::string& eventType, const std::string& data) const;
    
    // Random number generation
    static uint64_t lcg(uint64_t& state);
    static float urand(uint64_t& state, float min, float max);
    
    // Validation
    void validateWorldState() const;
};

} // namespace nlm