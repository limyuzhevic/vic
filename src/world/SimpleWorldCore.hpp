// SimpleWorldCore.hpp - Core functionality implementation
#pragma once

#include "SimpleWorldDesign.hpp"
#include "../agent/AgentBody.hpp"
#include "../agent/SensoryPercept.hpp"
#include <thread>
#include <future>

namespace nlm {

// Main SimpleWorld implementation using new architecture
// This is the actual implementation class (was SimpleWorld in design)
class SimpleWorldCore {
public:
    SimpleWorldCore(const WorldConfig& config = WorldConfig());
    ~SimpleWorldCore() = default;
    
    // Configuration with validation
    void configure(const WorldConfig& config);
    const WorldConfig& getConfig() const { return config_; }
    
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
    float getWidth() const { return config_.width; }
    float getHeight() const { return config_.height; }
    
    // Energy settings
    float getMaxEnergy() const { return config_.maxEnergy; }
    void setMaxEnergy(float e) { config_.maxEnergy = e; }
    float getEnergyDecayRate() const { return config_.energyDecayRate; }
    void setEnergyDecayRate(float r) { config_.energyDecayRate = r; }
    
    // Time
    double getSimulationTime() const { return simTime_; }
    
    // Seed for reproducibility
    void setRandomSeed(uint64_t seed);
    uint64_t getRandomSeed() const { return rng_.currentSeed_; }
    
    // Thread safety
    void lock() const { worldMutex_.lock(); }
    void unlock() const { worldMutex_.unlock(); }
    bool tryLock() const { return worldMutex_.try_lock(); }
    
    // Performance statistics
    struct PerformanceStats {
        uint64_t visionGenerationTime = 0;
        uint64_t collisionDetectionTime = 0;
        uint64_t interactionTime = 0;
        uint64_t totalUpdates = 0;
        std::atomic<uint64_t> activeThreads{0};
    };
    
    const PerformanceStats& getPerformanceStats() const { return stats_; }
    
    // Error handling
    ErrorCode getLastError() const { return lastError_; }
    void clearError() { lastError_ = ErrorCode::Success; }
    
private:
    // Helper methods
    void initializeComponents();
    void generateVision();
    void checkCollisions();
    
    // Constants
    static constexpr double MIN_ACTION_INTERVAL = 0.1;  // Minimum time between actions
    static constexpr float MOVE_SPEED = 3.0f;
    static constexpr float TURN_SPEED = 2.0f;
    
    // World state
    WorldConfig config_;
    AgentBody agent_;
    SensoryPercept sensory_;
    double simTime_ = 0.0;
    
    // Components
    std::unique_ptr<VisionGenerator> visionGenerator_;
    std::unique_ptr<CollisionDetector> collisionDetector_;
    std::unique_ptr<InteractionEngine> interactionEngine_;
    std::unique_ptr<PhysicsSimulator> physicsSimulator_;
    std::unique_ptr<SpatialIndex> spatialIndex_;
    std::unique_ptr<ObjectPool> objectPool_;
    
    // Random number generator
    RandomGenerator rng_;
    
    // Objects storage
    std::vector<WorldObject> objects_;
    std::mutex objectsMutex_;
    
    // Thread safety
    mutable std::shared_mutex worldMutex_;
    
    // Performance tracking
    PerformanceStats stats_;
    mutable std::mutex statsMutex_;
    
    // Error handling
    ErrorCode lastError_ = ErrorCode::Success;
    
    // Friend classes for component access
    friend class SimpleVisionGenerator;
    friend class SimpleCollisionDetector;
    friend class SimpleInteractionEngine;
    friend class SimplePhysicsSimulator;
};

} // namespace nlm