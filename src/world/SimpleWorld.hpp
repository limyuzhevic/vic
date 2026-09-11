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
    
    // ========== ENHANCED WORLD API ==========
    
    // Method chaining for world configuration
    class ConfigurationBuilder {
    public:
        ConfigurationBuilder(SimpleWorld* world);
        
        // Chain configuration methods
        ConfigurationBuilder& setWidth(float width);
        ConfigurationBuilder& setHeight(float height);
        ConfigurationBuilder& setVisionSize(size_t visionWidth, size_t visionHeight);
        ConfigurationBuilder& setEnergySettings(float maxEnergy, float energyDecayRate);
        ConfigurationBuilder& setRandomSeed(uint64_t seed);
        
        // Apply all accumulated configuration
        void apply();
        
        // Reset builder state
        void reset();
        
        // Get current configuration state
        float getWidth() const;
        float getHeight() const;
        size_t getVisionWidth() const;
        size_t getVisionHeight() const;
        
    private:
        SimpleWorld* world_;
        float width_;
        float height_;
        size_t visionWidth_;
        size_t visionHeight_;
        float maxEnergy_;
        float energyDecayRate_;
        uint64_t rngSeed_;
    };
    
    // Create configuration builder for method chaining
    ConfigurationBuilder configureWorld();
    
    // High-level navigation and interaction methods
    void moveAgent(float dx, float dy);
    void turnAgent(float angleDelta);
    void moveTo(float x, float y, float maxSteps = 100);
    void exploreArea(float radius, float searchPattern = 0.0f);
    
    // Interaction with objects
    ActionResult interactWithObject(float x, float y);
    void collectResource(float x, float y);
    void avoidHazard(float x, float y);
    void useMarker(float x, float y);
    
    // Vision and perception helpers
    void generateVision() { generateVision(); }
    float getVisionValue(size_t x, size_t y) const;
    bool isVisionInRange(size_t x, size_t y) const;
    
    // Agent state management
    void resetAgent(float x = 0.0f, float y = 0.0f, float energy = 1.0f);
    void setAgentEnergy(float energy);
    void setAgentHealth(float health);
    void setAgentAge(double age);
    
    // Object manipulation
    void addResource(float x, float y, float value = 1.0f);
    void addHazard(float x, float y, float damage = 1.0f);
    void addWall(float x, float y, float radius = 0.5f);
    void addMarker(float x, float y, float value = 0.0f);
    
    // Pathfinding and navigation
    std::vector<std::pair<float, float>> findPath(float startX, float startY, float targetX, float targetY);
    std::vector<std::pair<float, float>> exploreNeighborhood(float centerX, float centerY, float radius);
    
    // Statistical analysis
    struct StatisticalSummary {
        double mean;
        double variance;
        double stddev;
        double min;
        double max;
        size_t count;
        
        StatisticalSummary() : mean(0.0), variance(0.0), stddev(0.0), min(0.0), max(0.0), count(0) {}
    };
    
    StatisticalSummary computeVisionStatistics() const;
    StatisticalSummary computeObjectDistribution() const;
    
    // Batch operations
    void addBatchObjects(const std::vector<WorldObject>& objects);
    void removeBatchObjects(const std::vector<std::pair<float, float>>& positions);
    std::vector<WorldObject> getObjectsInArea(float centerX, float centerY, float radius);
    
    // Visualization and debugging helpers
    std::string generateWorldMap() const;
    std::string generateObjectReport() const;
    std::string generateAgentState() const;
    
    // Experimental and research mode helpers
    bool enableExperimentalMode();
    bool enableResearchMode();
    bool disableAllObjects();
    
    // Debugging and profiling methods
    void startProfiler();
    void stopProfiler();
    bool isProfilerActive() const;
    std::string getProfilerReport() const;
    
    void setDebugLevel(int level);
    int getDebugLevel() const;
    
    void enableLoggingToFile(const std::string& filepath);
    void disableLoggingToFile();
    bool isLoggingToFile() const;
    
    // Performance monitoring
    double getSimulationSpeed() const;
    double getAverageUpdateTime() const;
    size_t getActiveObjectCount() const;
    
    // State management
    void saveState(const std::string& filepath);
    bool loadState(const std::string& filepath);
    
    // Configuration validation
    bool validateConfiguration(std::vector<std::string>& errors) const;
    bool isConfigurationValid() const;
    
    // Export and visualization helpers
    std::string exportToJson() const;
    bool importFromJson(const std::string& jsonStr);
    void exportToImage(const std::string& filepath, float scale = 1.0f) const;
    
    // Simulation state management
    void pauseSimulation();
    void resumeSimulation();
    bool isSimulationPaused() const;
    
    // Checkpoint and recovery
    bool createCheckpoint(const std::string& prefix = "checkpoint");
    bool restoreFromCheckpoint(const std::string& filepath);
    std::vector<std::string> getAvailableCheckpoints() const;
    
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
