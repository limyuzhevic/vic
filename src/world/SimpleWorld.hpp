#pragma once

#include "../agent/AgentBody.hpp"
#include "../agent/SensoryPercept.hpp"
#include "../agent/Action.hpp"
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

// Configuration structure for advanced world settings
struct WorldConfig {
    float width = 20.0f;
    float height = 20.0f;
    size_t visionWidth = 16;
    size_t visionHeight = 16;
    float maxEnergy = 100.0f;
    float energyDecayRate = 0.01f;
    float resourceEnergyGain = 30.0f;
    float hazardDamage = 0.2f;
    float movementSpeed = 3.0f;
    float turnSpeed = 2.0f;
    float visionFOV = M_PI / 2.0f;  // 90 degree FOV
    float visionMaxRange = 8.0f;
    float interactionRange = 1.0f;
    bool enableWalls = false;
    size_t resourceCount = 5;
    size_t hazardCount = 3;
    
    WorldConfig() = default;
};

// Simple 2D world for NLM development
// Provides sensory experience without semantic labels
class SimpleWorld {
public:
    SimpleWorld();
    ~SimpleWorld();
    
    // Configuration
    void configure(float width, float height, size_t visionWidth, size_t visionHeight);
    
    // Advanced configuration with validation
    void configureAdvanced(const WorldConfig& config);
    
    // Reset world to initial state
    void reset();
    
    // Set agent starting position
    void setAgentStart(float x, float y);
    
    // Set agent starting position with validation
    void setAgentStartSafe(float x, float y);
    
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
    
    // Energy system
    float getMaxEnergy() const { return maxEnergy_; }
    void setMaxEnergy(float e) { maxEnergy_ = e; }
    float getEnergyDecayRate() const { return energyDecayRate_; }
    void setEnergyDecayRate(float r) { energyDecayRate_ = r; }
    float getResourceEnergyGain() const { return resourceEnergyGain_; }
    float getHazardDamage() const { return hazardDamage_; }
    
    // Movement parameters
    float getMovementSpeed() const { return movementSpeed_; }
    float getTurnSpeed() const { return turnSpeed_; }
    
    // Vision parameters
    float getVisionFOV() const { return visionFOV_; }
    float getVisionMaxRange() const { return visionMaxRange_; }
    float getInteractionRange() const { return interactionRange_; }
    size_t getVisionWidth() const { return visionWidth_; }
    size_t getVisionHeight() const { return visionHeight_; }
    
    // Time
    double getSimulationTime() const { return simTime_; }
    
    // Seed for reproducibility
    void setRandomSeed(uint64_t seed);
    uint64_t getRandomSeed() const { return rngSeed_; }
    
    // Object management
    size_t getObjectCount() const { return objects_.size(); }
    std::vector<WorldObject>& getObjects() { return objects_; }
    
    // Status checks
    bool isPositionOccupied(float x, float y) const;
    bool isAgentSafe(float x, float y) const;
    float getAgentEnergyPercentage() const { return agent_.energy / maxEnergy_; }
    
    // Configuration helpers
    bool validateConfig(const WorldConfig& config) const;
    void printConfig() const;
    
    // Statistics and diagnostics
    void logWorldStatus() const;
    std::string getWorldInfo() const;
    
    // Reset object generation seed
    void setObjectGenerationSeed(uint64_t seed) { objectGenerationSeed_ = seed; }
    
private:
    // Enhanced generateVision with configurable parameters
    void generateVision();
    
    // Check for collisions with enhanced detection
    void checkCollisions();
    
    // Interact with nearby objects
    ActionResult interact();
    
    // Object generation with configurable parameters
    void generateResources();
    void generateHazards();
    void generateWalls();
    void generateObjects(WorldObjectType type, size_t count, float value, float radius);
    
    // Validation and safety checks
    bool validateObjectPosition(float x, float y) const;
    bool isWithinWorldBounds(float x, float y) const;
    
    // Enhanced random generation with thread safety
    float generateRandomFloat(float min, float max) const;
    float generateRandomFloatWeighted(float min, float max, float weight) const;
    
    // Collision detection
    bool checkCollisionWithObject(float x, float y, const WorldObject& obj) const;
    
    // Movement with enhanced physics
    void applyMovement(double timestep, float maxSpeed);
    void applyRotation(double timestep, float maxAngularSpeed);
    
    // Energy and health management
    void updateEnergy(double timestep);
    void updateHealth(double timestep);
    
    // Vision with enhanced features
    void generateVisionEnhanced();
    void generateTouchEnhanced();
    void generateInternalEnhanced();
    void generateProprioceptionEnhanced();
    
    // Statistics tracking
    void updateStatistics();
    void resetStatistics();
    
    // Configuration
    void applyConfig(const WorldConfig& config);
    
    // World geometry
    float width_;
    float height_;
    
    // Vision configuration
    size_t visionWidth_;
    size_t visionHeight_;
    
    // Energy system
    float maxEnergy_;
    float energyDecayRate_;
    float resourceEnergyGain_;
    float hazardDamage_;
    
    // Movement parameters
    float movementSpeed_;
    float turnSpeed_;
    
    // Vision parameters
    float visionFOV_;
    float visionMaxRange_;
    float interactionRange_;
    
    // Agent
    AgentBody agent_;
    double lastEnergyDepletion_;
    
    // Objects in world
    std::vector<WorldObject> objects_;
    
    // Sensory state
    SensoryPercept sensory_;
    
    // Simulation time
    double simTime_;
    
    // Random seed
    uint64_t rngSeed_;
    
    // Enhanced RNG state
    mutable uint64_t rngState_;
    uint64_t objectGenerationSeed_;
    
    // Statistics
    struct WorldStatistics {
        size_t objectsGenerated = 0;
        size_t objectsConsumed = 0;
        size_t collisions = 0;
        float totalEnergyGained = 0.0f;
        float totalDamageTaken = 0.0f;
        double lastUpdateTime = 0.0;
        
        void reset() {
            objectsGenerated = 0;
            objectsConsumed = 0;
            collisions = 0;
            totalEnergyGained = 0.0f;
            totalDamageTaken = 0.0f;
            lastUpdateTime = 0.0;
        }
    };
    
    WorldStatistics stats_;
    
    uint64_t nextRandom();
};

} // namespace nlm
