#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <string>
#include <functional>

namespace nlm {

// Forward declarations for internal classes
class NeuromodulationSystem;
class DevelopmentalController;
class PerformanceMonitor;

// AgentBrain: Connects NLM brain to the world with improved architecture
// Handles sensory transduction and motor decoding with better separation of concerns
class AgentBrain {
public:
    // Constructor with dependency injection for better testability
    AgentBrain(std::shared_ptr<Brain> brain,
               std::shared_ptr<NeuromodulationSystem> neuromodulation = nullptr,
               std::shared_ptr<DevelopmentalController> development = nullptr,
               std::shared_ptr<PerformanceMonitor> monitor = nullptr);
    
    ~AgentBrain();
    
    // Delete copy operations, enable move operations
    AgentBrain(const AgentBrain&) = delete;
    AgentBrain& operator=(const AgentBrain&) = delete;
    AgentBrain(AgentBrain&&) noexcept = default;
    AgentBrain& operator=(AgentBrain&&) noexcept = default;
    
    // Initialize with world
    void initialize(const SimpleWorld& world);
    
    // Configuration and setup methods for advanced users
    void enableSubsystem(const std::string& subsystem, bool enable);
    bool isSubsystemEnabled(const std::string& subsystem) const;
    void setCuriosityThreshold(float threshold);
    void setExplorationRate(float rate);
    void setNoveltyDecay(float decay);
    
    // Get sensory input size expected by brain
    size_t getSensoryInputSize() const;
    
    // Get motor output size expected
    size_t getMotorOutputSize() const;
    
    // Advanced processing methods
    void processSensoryInput(const SensoryPercept& percept);
    void processSensoryInput(const SensoryPercept& percept, 
                           const std::vector<float>& customWeights);
    
    // Decode brain motor activity into motor command with options
    MotorCommand decodeMotorCommand(const MotorCommandStrategy& strategy = MotorCommandStrategy::Standard);
    MotorCommand decodeMotorCommandWithCuriosity(const MotorCommandStrategy& strategy = MotorCommandStrategy::Standard);
    
    // Neuromodulation with enhanced options
    void applyRewardModulation(float reward, float predictedReward = 0.0f,
                              const RewardContext& context = RewardContext());
    void applyRewardModulation(const RewardSignal& rewardSignal);
    
    // Update development system with enhanced options
    void updateDevelopment(double timestep, const DevelopmentContext& context = DevelopmentContext());
    
    // Performance monitoring
    void startTiming(const std::string& operation);
    double stopTiming(const std::string& operation);
    double getAverageTime(const std::string& operation) const;
    
    // Get current state
    MotorCommand getLastMotorCommand() const { return lastMotorCommand_; }
    float getCurrentCuriosityLevel() const { return curiosityLevel_; }
    float getCurrentNoveltyLevel() const { return noveltyLevel_; }
    float getCurrentPredictionError() const { return predictionError_; }
    double getDevelopmentalAge() const { return developmentalAge_; }
    float getPlasticityModifier() const { return plasticityModifier_; }
    
    // Get current developmental stage
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Get neuromodulation level
    float getNeuromodulationLevel() const { return dopamineLevel_; }
    
    // Get curiosity level (alias for backward compatibility)
    float getCuriosityLevel() const { return curiosityLevel_; }
    
    // Get novelty level (alias for backward compatibility)
    float getNoveltyLevel() const { return noveltyLevel_; }
    
    // Get prediction error (alias for backward compatibility)
    float getPredictionError() const { return predictionError_; }
    
    // Reset agent for new episode with options
    void reset() { reset(true); }
    void reset(bool clearHistory);
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }
    const Brain* getBrain() const { return brain_.get(); }
    
    // Configuration
    void enableRewardModulation(bool enable) { enableSubsystem("reward", enable); }
    void enableStructuralPlasticity(bool enable) { enableSubsystem("plasticity", enable); }
    void enableDevelopment(bool enable) { enableSubsystem("development", enable); }
    void enableCuriosity(bool enable) { enableSubsystem("curiosity", enable); }
    
    bool isRewardModulationEnabled() const { return isSubsystemEnabled("reward"); }
    bool isStructuralPlasticityEnabled() const { return isSubsystemEnabled("plasticity"); }
    bool isDevelopmentEnabled() const { return isSubsystemEnabled("development"); }
    bool isCuriosityEnabled() const { return isSubsystemEnabled("curiosity"); }
    
    // Batch operations for efficiency
    void processMultipleSteps(const std::vector<SensoryPercept>& percepts);
    std::vector<MotorCommand> decodeMultipleSteps(size_t count);
    
    // Statistics and monitoring
    size_t getVisionNeuronCount() const { return sensoryVision_.size(); }
    size_t getTouchNeuronCount() const { return sensoryTouch_.size(); }
    size_t getInternalNeuronCount() const { return sensoryInternal_.size(); }
    size_t getProprioceptionNeuronCount() const { return sensoryProprioception_.size(); }
    
    // Get all subsystem states
    std::unordered_map<std::string, float> getSubsystemStates() const;
    std::string getPerformanceStats() const;
    
    // Configuration loading/saving
    bool saveState(const std::string& filepath) const;
    bool loadState(const std::string& filepath);
    
    // Serialization support
    std::string serializeState() const;
    bool deserializeState(const std::string& data);
    
    // Event system for advanced agents
    void addEventCallback(const std::string& event, std::function<void()> callback);
    void removeEventCallback(const std::string& event);
    
    // Debugging and development helpers
    std::string getDebugInfo() const;
    void enableDebugOutput(bool enable) { debugEnabled_ = enable; }
    bool isDebugEnabled() const { return debugEnabled_; }
    
    // Advanced configuration methods
    void configureMotorNeuronMapping(const MotorMapping& mapping);
    void configureSensoryMapping(const SensoryMapping& mapping);
    
    // Factory methods for convenience
    static std::shared_ptr<AgentBrain> createAdvancedBrain(std::shared_ptr<Config> config);
    static std::shared_ptr<AgentBrain> createDefaultAgent(std::shared_ptr<Brain> brain,
                                                         std::shared_ptr<SimpleWorld> world);

private:
    // Private implementation details
    class Impl;
    std::unique_ptr<Impl> pImpl_;
    
    // Core components
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<NeuromodulationSystem> neuromodulation_;
    std::shared_ptr<DevelopmentalController> developmentController_;
    std::shared_ptr<PerformanceMonitor> performanceMonitor_;
    
    // Motor neuron groups (organized by action type)
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
    
    // Performance and timing
    bool debugEnabled_;
    std::unordered_map<std::string, double> operationTimes_;
    size_t operationCount_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // State tracking
    MotorCommand lastMotorCommand_;
    std::string lastActionDescription_;
    
    // Event system
    std::unordered_map<std::string, std::vector<std::function<void()>>> eventCallbacks_;
    
    // Configuration mappings
    MotorMapping motorMapping_;
    SensoryMapping sensoryMapping_;
    
    // Private helper methods
    void initializeMotorGroups();
    void initializeSensoryGroups();
    void distributeMotorNeurons();
    void distributeSensoryNeurons();
    void updateSubsystemStates() const;
    void triggerEvents(const std::string& event) const;
    void logDebugInfo() const;
    
    // Legacy compatibility
    MotorCommand decodeFromMotorNeurons();
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    friend class AgentBrainFactory;
};

// Supporting classes for advanced functionality
class NeuromodulationSystem {
    // Enhanced neuromodulation system with more sophisticated control
};

class DevelopmentalController {
    // Enhanced developmental control system
};

class PerformanceMonitor {
    // Performance monitoring and statistics
};

class RewardContext {
    // Context for reward signals
};

class DevelopmentContext {
    // Context for development updates
};

class MotorMapping {
    // Custom motor neuron mapping
};

class SensoryMapping {
    // Custom sensory neuron mapping
};

class MotorCommandStrategy {
public:
    enum Type {
        Standard,
        CuriouslyGuided,
        Conservative,
        Exploratory,
        Random
    };
    static MotorCommandStrategy create(Type type, float param = 0.0f);
    
private:
    Type type_;
    float param_;
};

class AgentBrainFactory {
    // Factory class for creating AgentBrain instances with various configurations
};

} // namespace nlm