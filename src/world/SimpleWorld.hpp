#pragma once

#include "../agent/AgentBody.hpp"
#include "../agent/SensoryPercept.hpp"
#include "SimpleWorldCore.hpp"
#include "SimpleWorldAdvanced.hpp"
#include <memory>

namespace nlm {

// Simple 2D world for NLM development
// Provides sensory experience without semantic labels
// Uses new modern C++20 architecture with enhanced performance
class SimpleWorld {
public:
    // Default constructor
    SimpleWorld();
    
    // Constructor with configuration
    explicit SimpleWorld(const WorldConfig& config);
    
    ~SimpleWorld();
    
    // Configuration with validation
    void configure(const WorldConfig& config);
    void configure(float width, float height, size_t visionWidth, size_t visionHeight);
    const WorldConfig& getConfig() const { return world_.getConfig(); }
    
    // Reset world to initial state
    void reset();
    
    // Set agent starting position
    void setAgentStart(float x, float y);
    
    // Update world state (call each simulation step)
    void update(double timestep);
    
    // Apply motor command to agent
    ActionResult applyMotorCommand(MotorCommand cmd, double currentTime);
    
    // Get current sensory percept for the agent
    const SensoryPercept& getSensoryPercept() const { return world_.getSensoryPercept(); }
    
    // Get agent body state
    const AgentBody& getAgentBody() const { return world_.getAgentBody(); }
    
    // Add object to world
    void addObject(const WorldObject& obj);
    
    // Remove object at position
    void removeObject(float x, float y);
    
    // Check if position is valid (not wall)
    bool isValidPosition(float x, float y) const { return world_.isValidPosition(x, y); }
    
    // Get object at position (if any)
    WorldObject* getObjectAt(float x, float y) { return world_.getObjectAt(x, y); }
    
    // World dimensions
    float getWidth() const { return world_.getWidth(); }
    float getHeight() const { return world_.getHeight(); }
    
    // Energy settings
    float getMaxEnergy() const { return world_.getMaxEnergy(); }
    void setMaxEnergy(float e) { world_.setMaxEnergy(e); }
    float getEnergyDecayRate() const { return world_.getEnergyDecayRate(); }
    void setEnergyDecayRate(float r) { world_.setEnergyDecayRate(r); }
    
    // Time
    double getSimulationTime() const { return world_.getSimulationTime(); }
    
    // Seed for reproducibility
    void setRandomSeed(uint64_t seed) { world_.setRandomSeed(seed); }
    uint64_t getRandomSeed() const { return world_.getRandomSeed(); }
    
    // Thread safety (forward to implementation)
    void lock() const { world_.lock(); }
    void unlock() const { world_.unlock(); }
    bool tryLock() const { return world_.tryLock(); }
    
    // Performance statistics
    const SimpleWorld::PerformanceStats& getPerformanceStats() const { return world_.getPerformanceStats(); }
    
    // Error handling
    ErrorCode getLastError() const { return lastError_; }
    void clearError() { lastError_ = ErrorCode::Success; }
    
    // Configuration validation levels
    enum class ValidationLevel {
        None = 0,
        Lenient = 1,
        Moderate = 2,
        Strict = 3
    };
    
    // Helper methods for advanced features
    void enableMultithreading(bool enabled) { multithreaded_ = enabled; }
    void setThreadCount(size_t count) { threadCount_ = count; }
    void enableAdaptiveSpatialIndex(bool enabled) { useAdaptiveIndex_ = enabled; }
    void enableMemoryPool(bool enabled) { useMemoryPool_ = enabled; }
    
private:
    // Delegate all functionality to implementation class
    std::unique_ptr<SimpleWorldCore> world_;
    
    // Error handling
    ErrorCode lastError_ = ErrorCode::Success;
    
    // Advanced features toggle
    bool multithreaded_ = true;
    size_t threadCount_ = std::thread::hardware_concurrency();
    bool useAdaptiveIndex_ = true;
    bool useMemoryPool_ = true;
};

// Factory function for creating world with advanced features
std::unique_ptr<SimpleWorld> createAdvancedWorld(const WorldConfig& config = WorldConfig());

} // namespace nlm
