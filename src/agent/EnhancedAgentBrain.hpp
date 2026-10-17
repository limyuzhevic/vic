#pragma once

#include "AgentBrain.hpp"
#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <memory>
#include <unordered_map>
#include <functional>

namespace nlm {

// Agent configuration parameters
struct AgentConfig {
    // Sensory processing
    size_t visionWidth_ = 16;
    size_t visionHeight_ = 16;
    size_t touchSensors_ = 8;
    size_t internalSensors_ = 4;
    size_t proprioceptionSensors_ = 6;
    
    // Motor control
    size_t motorGroups_ = 6;
    
    // Neuromodulation thresholds
    float curiosityThreshold_ = 0.3f;
    float explorationThreshold_ = 0.5f;
    float noveltyDecay_ = 0.99f;
    
    // Performance optimization
    bool enableSensoryBuffering_ = true;
    bool enableMotorOptimization_ = true;
    size_t maxNeuronGroups_ = 100;
    
    // Development
    double developmentalTimeScale_ = 1.0;
    
    // Debug and logging
    bool enableDebugLogging_ = false;
    bool enablePerformanceMetrics_ = false;
};

// Agent statistics for performance monitoring
struct AgentStats {
    size_t totalSensoryInputs_ = 0;
    size_t totalMotorOutputs_ = 0;
    size_t totalEpisodes_ = 0;
    double totalSimulationTime_ = 0.0;
    float averageCuriosity_ = 0.0f;
    float averageNovelty_ = 0.0f;
    float averageDopamine_ = 0.0f;
    size_t neuralOperations_ = 0;
    size_t memoryAllocations_ = 0;
    double startTime_ = 0.0;
    double lastUpdateTime_ = 0.0;
};

// Enhanced AgentBrain with improved memory management
class EnhancedAgentBrain {
public:
    EnhancedAgentBrain(std::shared_ptr<Brain> brain, std::shared_ptr<AgentConfig> config = nullptr);
    ~EnhancedAgentBrain();
    
    // Delete copying, enable moving
    EnhancedAgentBrain(const EnhancedAgentBrain&) = delete;
    EnhancedAgentBrain& operator=(const EnhancedAgentBrain&) = delete;
    EnhancedAgentBrain(EnhancedAgentBrain&&) noexcept;
    EnhancedAgentBrain& operator=(EnhancedAgentBrain&&) noexcept;
    
    // Configuration
    void setConfig(std::shared_ptr<AgentConfig> config) { config_ = config; }
    std::shared_ptr<AgentConfig> getConfig() const { return config_; }
    
    // Initialization
    void initialize(const SimpleWorld& world);
    
    // Core functionality
    void processSensoryInput(const SensoryPercept& percept);
    MotorCommand decodeMotorCommand();
    void applyRewardModulation(float reward, float predictedReward);
    void updateDevelopment(double timestep);
    
    // Information access
    size_t getSensoryInputSize() const;
    size_t getMotorOutputSize() const;
    float getCuriosityLevel() const { return stats_.averageCuriosity_; }
    float getNoveltyLevel() const { return stats_.averageNovelty_; }
    float getNeuromodulationLevel() const { return stats_.averageDopamine_; }
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Control
    void reset();
    void enableFeature(const std::string& feature, bool enable);
    bool isFeatureEnabled(const std::string& feature) const;
    
    // Performance
    const AgentStats& getStats() const { return stats_; }
    double getUptime() const;
    
    // Memory safety
    bool isValid() const;
    
    // Debug
    std::string getDebugInfo() const;
    
    // Advanced features
    void setGoal(const std::string& goal) { currentGoal_ = goal; }
    void setBehaviorPolicy(const std::string& policy) { behaviorPolicy_ = policy; }
    void addCustomAction(const std::string& name, std::function<MotorCommand()> actionFunc);
    
private:
    struct Impl;
    Impl* pImpl_;
};

} // namespace nlm