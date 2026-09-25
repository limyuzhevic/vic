#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <chrono>

namespace nlm {

// Forward declarations
class Config;

// AgentBrainDebugInfo: Comprehensive debugging information for agents
struct AgentBrainDebugInfo {
    // Neuron statistics
    size_t activeNeuronCount;
    size_t totalSpikeCount;
    float averageFiringRate;
    float excitationInhibitionRatio;
    
    // Neuromodulation levels
    float dopamineLevel;
    float noveltyLevel;
    float curiosityLevel;
    float predictionError;
    float expectedReward;
    
    // Development state
    double developmentalAge;
    float plasticityModifier;
    DevelopmentalStage developmentalStage;
    
    // Memory system status
    size_t workingMemorySize;
    size_t episodicMemorySize;
    size_t associativeMemorySize;
    
    // Performance metrics
    double lastStepTime;
    size_t neuronsPerSecond;
    size_t spikesPerStep;
    float plasticityRate;
    float learningRate;
    
    // Sensory and motor status
    std::vector<float> recentRewards;
    std::map<std::string, float> neuromodulationLevels;
    std::vector<float> sensoryInputs;
    std::vector<float> motorOutputs;
    
    std::chrono::steady_clock::time_point lastUpdate;
    
    AgentBrainDebugInfo()
        : activeNeuronCount(0), totalSpikeCount(0), averageFiringRate(0.0f)
        , excitationInhibitionRatio(0.0f), dopamineLevel(0.0f), noveltyLevel(0.0f)
        , curiosityLevel(0.0f), predictionError(0.0f), expectedReward(0.0f)
        , developmentalAge(0.0), plasticityModifier(1.0f), developmentalStage(DevelopmentalStage::Initial)
        , workingMemorySize(0), episodicMemorySize(0), associativeMemorySize(0)
        , lastStepTime(0.0), neuronsPerSecond(0), spikesPerStep(0)
        , plasticityRate(0.0f), learningRate(0.0f), sensoryNoveltyDecay_(0.99f)
        , lastUpdate(std::chrono::steady_clock::now()) {}
};

// AgentBrain: Enhanced AgentBrain connecting NLM brain to the world
// Handles sensory transduction and motor decoding with advanced features
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
    // Initialize with world
    void initialize(const SimpleWorld& world);
    
    // Get sensory input size expected by brain
    size_t getSensoryInputSize() const;
    
    // Get motor output size expected
    size_t getMotorOutputSize() const;
    
    // Process sensory percept and inject into brain
    void processSensoryInput(const SensoryPercept& percept);
    
    // Decode brain motor activity into motor command
    MotorCommand decodeMotorCommand();
    
    // Apply neuromodulation based on reward
    void applyRewardModulation(float reward, float predictedReward);
    
    // Update development system
    void updateDevelopment(double timestep);
    
    // Get current developmental stage
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Get neuromodulation level
    float getNeuromodulationLevel() const;
    
    // Get curiosity level
    float getCuriosityLevel() const;
    
    // Get novelty level
    float getNoveltyLevel() const;
    
    // Get prediction error
    float getPredictionError() const;
    
    // Get expected reward
    float getExpectedReward() const;
    
    // Reset agent for new episode
    void reset();
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }
    
    // ===== ENHANCED CONFIGURATION =====
    
    // Runtime configuration modification
    void setConfig(const std::string& key, const ConfigValue& value);
    std::optional<ConfigValue> getConfig(const std::string& key) const;
    
    // Advanced neuromodulation controls
    void setNeuromodulationTarget(const std::string& neuromodulator, float target);
    void enableNeuromodulation(const std::string& neuromodulator, bool enable);
    void setNeuromodulationParameters(const std::string& neuromodulator, 
                                      const std::map<std::string, float>& params);
    
    // Development system controls
    void setDevelopmentalStage(DevelopmentalStage stage);
    void accelerateDevelopment(float multiplier = 2.0f);
    void decelerateDevelopment(float multiplier = 0.5f);
    void pauseDevelopment(bool pause = true);
    void setDevelopmentSpeed(double speedFactor);
    
    // Curiosity and exploration controls
    void setCuriosityThreshold(float threshold);
    void setExplorationRate(float rate);
    void forceExploration(bool force = true);
    void setCuriosityParameters(const std::map<std::string, float>& params);
    
    // Memory system controls
    void enableMemoryConsolidation(bool enable);
    void setReplayFrequency(size_t steps);
    void setMemoryCapacity(size_t working, size_t episodic, size_t associative);
    void optimizeMemoryUsage();
    
    // Sensory and motor controls
    void adjustSensoryGain(float gain);
    void adjustMotorOutput(float scaling);
    void calibrateSensors();
    void setNoiseLevel(float level);
    
    // ===== DEBUGGING AND MONITORING =====
    
    // Advanced debugging and monitoring
    AgentBrainDebugInfo getDebugInfo() const;
    
    // Event callbacks for advanced monitoring
    using StateChangeCallback = std::function<void(const std::string&, const ConfigValue&)>;
    using PerformanceCallback = std::function<void(const AgentBrainDebugInfo&)>
    using DebugEventCallback = std::function<void(const std::string&, const std::string&)>
    
    void addStateChangeListener(const std::string& key, StateChangeCallback callback);
    void addPerformanceListener(PerformanceCallback callback);
    void addDebugEventListener(DebugEventCallback callback);
    void removeStateChangeListener(const std::string& key);
    void removeAllListeners();
    
    // Debug target management
    void setDebugTarget(const std::string& target, bool enable);
    bool isDebugTargetEnabled(const std::string& target) const;
    std::vector<std::string> getEnabledDebugTargets() const;
    
    // Performance monitoring
    struct PerformanceMetrics {
        double lastStepTime;
        size_t neuronsPerSecond;
        size_t spikesPerStep;
        float plasticityRate;
        float learningRate;
        std::chrono::steady_clock::time_point lastUpdate;
        
        PerformanceMetrics()
            : lastStepTime(0.0), neuronsPerSecond(0), spikesPerStep(0)
            , plasticityRate(0.0f), learningRate(0.0f)
            , lastUpdate(std::chrono::steady_clock::now()) {}
    };
    
    PerformanceMetrics getPerformanceMetrics() const;
    std::vector<PerformanceMetrics> getPerformanceHistory(size_t maxHistory = 100) const;
    
    // Anomaly detection
    bool detectAnomalies() const;
    std::vector<std::string> getAnomalies() const;
    void clearAnomalies();
    
    // ===== PLUGINS AND EXTENSIBILITY =====
    
    // Plugin system for advanced extensions
    class Plugin {
    public:
        virtual ~Plugin() = default;
        virtual void initialize(AgentBrain* agent) = 0;
        virtual void update(double timestep) = 0;
        virtual void shutdown() = 0;
        virtual std::string getName() const = 0;
    };
    
    using PluginPtr = std::shared_ptr<Plugin>;
    
    void registerPlugin(PluginPtr plugin);
    void unregisterPlugin(const std::string& name);
    std::vector<std::string> getRegisteredPlugins() const;
    PluginPtr getPlugin(const std::string& name) const;
    
    // Custom modules
    void addCustomModule(const std::string& name, std::function<void(AgentBrain*, double)> module);
    void removeCustomModule(const std::string& name);
    
    // ===== CONFIGURATION =====
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // ===== LEGACY COMPATIBILITY =====
    
    // Legacy methods for backward compatibility
    void setLearningRate(float rate) { setConfig("learning_rate", rate); }
    void setExplorationRate(float rate) { setConfig("exploration_rate", rate); }
    void setCuriosity(float level) { setConfig("curiosity_level", level); }
    void setDevelopmentSpeed(float speed) { setDevelopmentSpeed(speed); }
    
private:
    // ===== CORE FUNCTIONALITY =====
    
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // ===== STATE MANAGEMENT =====
    
    std::shared_ptr<Brain> brain_;
    std::map<std::string, ConfigValue> runtimeConfig_;
    std::map<std::string, std::vector<StateChangeCallback>> stateChangeListeners_;
    std::vector<PerformanceCallback> performanceListeners_;
    std::vector<DebugEventCallback> debugEventListeners_;
    
    // Motor neuron groups
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Sensory neuron groups
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    // Neuromodulation state
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    
    // Development state
    double developmentalAge_;
    float plasticityModifier_;
    
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    
    // Debug and plugin system
    std::map<std::string, bool> debugTargets_;
    std::vector<PluginPtr> plugins_;
    std::map<std::string, std::function<void(AgentBrain*, double)>> customModules_;
    
    // Performance monitoring
    mutable PerformanceMetrics performanceMetrics_;
    std::vector<PerformanceMetrics> performanceHistory_;
    mutable std::vector<std::string> anomalies_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // ===== PRIVATE HELPERS =====
    
    void emitStateChange(const std::string& key, const ConfigValue& value);
    void emitPerformanceUpdate(const AgentBrainDebugInfo& info);
    void emitDebugEvent(const std::string& event, const std::string& message);
    void updatePluginSystem(double timestep);
    void updateCustomModules(double timestep);
    void updatePerformanceMetrics();
    AgentBrainDebugInfo collectDebugInfo() const;
    void detectAnomaliesInternal();
};

} // namespace nlm