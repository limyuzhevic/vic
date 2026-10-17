#pragma once

#include "SimpleWorld.hpp"
#include "../core/Config/ConfigManager.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <random>
#include <stdexcept>
#include <unordered_map>
#include <vector>
#include <memory>
#include <chrono>

namespace nlm {

// Forward declarations
class SimpleWorld;
class AgentBody;
class SensoryPercept;

// World configuration parameters
struct WorldConfig {
    // World geometry
    float width_ = 20.0f;
    float height_ = 20.0f;
    
    // Agent properties
    float maxEnergy_ = 100.0f;
    float energyDecayRate_ = 0.01f;
    float resourceEnergyGain_ = 30.0f;
    
    // Object properties
    size_t initialResources_ = 5;
    size_t initialHazards_ = 3;
    float resourceValue_ = 10.0f;
    float hazardDamage_ = -15.0f;
    
    // Vision configuration
    size_t visionRayCount_ = 16;
    float visionFieldOfView_ = 1.5708f; // PI/2 = 90 degrees
    float visionMaxRange_ = 8.0f;
    
    // Physics
    float moveSpeed_ = 3.0f;
    float turnSpeed_ = 2.0f;
    float actionCooldown_ = 0.1f;
    float velocityDamping_ = 0.9f;
    float angularVelocityDamping_ = 0.9f;
    
    // Collision detection
    float collisionTolerance_ = 0.01f;
    float interactRange_ = 1.0f;
    
    // Performance
    size_t maxObjects_ = 1000;
    double timestep_ = 0.01;
    
    // Debug and logging
    bool enableVisionDebug_ = false;
    bool enableCollisionDebug_ = false;
    bool enableObjectDebug_ = false;
    
    // Random seed
    uint64_t randomSeed_ = 42;
};

// World statistics for performance monitoring
struct WorldStats {
    size_t totalSteps_ = 0;
    size_t totalActions_ = 0;
    size_t successfulActions_ = 0;
    float totalEnergyConsumed_ = 0.0f;
    float totalEnergyGained_ = 0.0f;
    size_t collisionEvents_ = 0;
    size_t interactionEvents_ = 0;
    double startTime_ = 0.0;
    double lastStepTime_ = 0.0;
    size_t objectsCreated_ = 0;
    size_t objectsDestroyed_ = 0;
};

// Enhanced SimpleWorld with configurable parameters
class EnhancedSimpleWorld {
public:
    EnhancedSimpleWorld(std::shared_ptr<WorldConfig> config = nullptr);
    ~EnhancedSimpleWorld();
    
    // Delete copying, enable moving
    EnhancedSimpleWorld(const EnhancedSimpleWorld&) = delete;
    EnhancedSimpleWorld& operator=(const EnhancedSimpleWorld&) = delete;
    EnhancedSimpleWorld(EnhancedSimpleWorld&&) noexcept;
    EnhancedSimpleWorld& operator=(EnhancedSimpleWorld&&) noexcept;
    
    // Configuration
    void setConfig(std::shared_ptr<WorldConfig> config) { config_ = config; }
    std::shared_ptr<WorldConfig> getConfig() const { return config_; }
    
    // Configuration methods
    void configure(float width, float height, size_t visionWidth, size_t visionHeight);
    void setRandomSeed(uint64_t seed);
    uint64_t getRandomSeed() const { return config_->randomSeed_; }
    
    // Reset and initialization
    void reset();
    void setAgentStart(float x, float y);
    
    // World update
    void update(double timestep);
    
    // Action handling
    ActionResult applyMotorCommand(MotorCommand cmd, double currentTime);
    
    // Object management
    void addObject(const WorldObject& obj);
    void removeObject(float x, float y);
    WorldObject* getObjectAt(float x, float y);
    bool isValidPosition(float x, float y) const;
    
    // Sensory input
    const SensoryPercept& getSensoryPercept() const { return sensory_; }
    const AgentBody& getAgentBody() const { return agent_; }
    
    // Configuration access
    float getWidth() const { return config_->width_; }
    float getHeight() const { return config_->height_; }
    
    float getMaxEnergy() const { return config_->maxEnergy_; }
    void setMaxEnergy(float e) { config_->maxEnergy_ = e; }
    
    float getEnergyDecayRate() const { return config_->energyDecayRate_; }
    void setEnergyDecayRate(float r) { config_->energyDecayRate_ = r; }
    
    double getSimulationTime() const { return simTime_; }
    
    // Statistics
    const WorldStats& getStats() const { return stats_; }
    double getUptime() const;
    std::string getStatsString() const;
    
    // Performance
    void enableDebugLogging(bool enable) { debugLoggingEnabled_ = enable; }
    bool isDebugLoggingEnabled() const { return debugLoggingEnabled_; }
    
    // Advanced features
    void setEnvironmentType(const std::string& type) { environmentType_ = type; }
    void addHazardPattern(const std::string& pattern);
    void addResourcePattern(const std::string& pattern);
    
    // Memory safety
    bool isValid() const;
    
    // Debug
    std::string getDebugInfo() const;
    
private:
    struct Impl;
    Impl* pImpl_;
};

} // namespace nlm