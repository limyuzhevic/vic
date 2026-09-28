#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <functional>

namespace nlm {

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
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
    
    // Reset agent for new episode
    void reset();
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }

    // ========================================================================
    // ADVANCED AGENT FEATURES FOR POWER USERS
    // ========================================================================

    // Learning strategy control
    enum class LearningStrategy {
        Conservative,     // Low learning rate, stable behavior
        Balanced,         // Normal learning rate
        Aggressive,       // High learning rate, rapid adaptation
        ConservativeBalanced  // Low learning with balanced exploration
    };

    // Advanced action selection strategies
    enum class ActionSelectionStrategy {
        Deterministic,    // Pure exploitation, choose best action
        Random,          // Pure exploration, random actions
        Boltzmann,       // Soft-max over activity values
        Ucb,             // Upper confidence bound for exploration
        EpsGreedy         // Epsilon-greedy with adaptive epsilon
    };

    // Enhanced configuration methods
    void setLearningStrategy(LearningStrategy strategy) { learningStrategy_ = strategy; }
    void setActionSelectionStrategy(ActionSelectionStrategy strategy) { actionSelectionStrategy_ = strategy; }
    void setExplorationRate(float rate) { explorationRate_ = rate; }
    void setExploitationRate(float rate) { exploitationRate_ = rate; }
    
    LearningStrategy getLearningStrategy() const { return learningStrategy_; }
    ActionSelectionStrategy getActionSelectionStrategy() const { return actionSelectionStrategy_; }
    float getExplorationRate() const { return explorationRate_; }
    float getExploitationRate() const { return exploitationRate_; }

    // Advanced neuromodulation control
    void setDopamineEffect(float effect) { dopamineEffect_ = effect; }
    void setCuriosityEffect(float effect) { curiosityEffect_ = effect; }
    void setNoveltyEffect(float effect) { noveltyEffect_ = effect; }
    void setPredictionErrorEffect(float effect) { predictionErrorEffect_ = effect; }
    
    // Get neuromodulation effects
    float getDopamineEffect() const { return dopamineEffect_; }
    float getCuriosityEffect() const { return curiosityEffect_; }
    float getNoveltyEffect() const { return noveltyEffect_; }
    float getPredictionErrorEffect() const { return predictionErrorEffect_; }

    // Development system enhancement
    void setDevelopmentSpeed(double speed) { developmentSpeed_ = speed; }
    void setCriticalPeriod(double age) { criticalPeriodAge_ = age; }
    void setMaturationPeriod(double age) { maturationPeriodAge_ = age; }
    
    double getDevelopmentSpeed() const { return developmentSpeed_; }
    double getCriticalPeriodAge() const { return criticalPeriodAge_; }
    double getMaturationPeriodAge() const { return maturationPeriodAge_; }
    
    // Custom developmental stages
    void addCustomDevelopmentalStage(double age, DevelopmentalStage stage);
    DevelopmentalStage getCustomDevelopmentalStage(double age) const;
    std::map<double, DevelopmentalStage> getCustomStages() const { return customStages_; }

    // Memory system integration
    void enableWorkingMemoryAccess(bool enable) { workingMemoryEnabled_ = enable; }
    void enableEpisodicMemoryAccess(bool enable) { episodicMemoryEnabled_ = enable; }
    void enableSemanticMemoryAccess(bool enable) { semanticMemoryEnabled_ = enable; }
    
    bool isWorkingMemoryEnabled() const { return workingMemoryEnabled_; }
    bool isEpisodicMemoryEnabled() const { return episodicMemoryEnabled_; }
    bool isSemanticMemoryEnabled() const { return semanticMemoryEnabled_; }

    // Performance monitoring and statistics
    struct PerformanceStats {
        double totalTime;
        size_t stepsExecuted;
        float avgFiringRate;
        float avgSpikesPerStep;
        float totalReward;
        float maxFiringRate;
        float avgNeuromodulationLevel;
        size_t memoryUsage;
        double cpuUsage;
    };

    PerformanceStats getPerformanceStats() const { return currentStats_; }
    void updatePerformanceStats(size_t stepsExecuted, float avgFiringRate, 
                               float totalReward, double timeElapsed);

    // Advanced debugging and monitoring
    enum class DebugLevel {
        Basic,
        Intermediate,
        Advanced,
        Detailed,
        Diagnostic
    };

    void setDebugLevel(DebugLevel level) { debugLevel_ = level; }
    DebugLevel getDebugLevel() const { return debugLevel_; }
    
    // Performance profiling
    void enableProfiling(bool enable) { profilingEnabled_ = enable; }
    bool isProfilingEnabled() const { return profilingEnabled_; }
    
    std::map<std::string, double> getProfileData() const { return profileData_; }
    void recordProfileData(const std::string& name, double value);

    // Advanced sensory processing
    void enableSensoryFiltering(bool enable) { sensoryFilteringEnabled_ = enable; }
    void setVisionFilteringStrength(float strength) { visionFilteringStrength_ = strength; }
    void setTouchFilteringStrength(float strength) { touchFilteringStrength_ = strength; }
    
    bool isSensoryFilteringEnabled() const { return sensoryFilteringEnabled_; }
    float getVisionFilteringStrength() const { return visionFilteringStrength_; }
    float getTouchFilteringStrength() const { return touchFilteringStrength_; }

    // Custom action mapping
    void addCustomActionMapping(ActionType action, const std::string& name);
    void removeCustomActionMapping(ActionType action);
    std::string getActionMappingName(ActionType action) const;
    std::map<ActionType, std::string> getActionMappings() const { return actionMappings_; }

    // Reward shaping
    void setRewardFunction(std::function<float(const SensoryPercept&, const AgentBody&)> func) {
        customRewardFunction_ = func;
        rewardFunctionEnabled_ = true;
    }
    bool isRewardFunctionEnabled() const { return rewardFunctionEnabled_; }
    float computeCustomReward(const SensoryPercept& percept, const AgentBody& body) const {
        if (customRewardFunction_) {
            return customRewardFunction_(percept, body);
        }
        return 0.0f;
    }

    // Episode management
    void setEpisodeLimit(size_t limit) { episodeLimit_ = limit; }
    void setStepLimit(size_t limit) { stepLimit_ = limit; }
    void setTimeLimit(double time) { timeLimit_ = time; }
    
    size_t getEpisodeLimit() const { return episodeLimit_; }
    size_t getStepLimit() const { return stepLimit_; }
    double getTimeLimit() const { return timeLimit_; }
    
    // Episode tracking
    size_t getCurrentEpisode() const { return currentEpisode_; }
    size_t getStepsInCurrentEpisode() const { return stepsInCurrentEpisode_; }
    double getCurrentEpisodeTime() const { return currentEpisodeTime_; }
    void incrementEpisode() { currentEpisode_++; }
    void resetEpisode() { currentEpisode_ = 0; stepsInCurrentEpisode_ = 0; currentEpisodeTime_ = 0.0; }

    // External event handling
    void setExternalEventCallback(std::function<void(const std::string&)> callback) {
        externalEventCallback_ = callback;
    }
    void triggerExternalEvent(const std::string& event, const std::map<std::string, std::string>& data = {});

    // ========================================================================
    // LEGACY COMPATIBILITY METHODS (keep original interface)
    // ========================================================================

    // All original methods preserved for backward compatibility

private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    std::shared_ptr<Brain> brain_;
    
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
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;

    // Advanced configuration
    LearningStrategy learningStrategy_;
    ActionSelectionStrategy actionSelectionStrategy_;
    float explorationRate_;
    float exploitationRate_;
    
    // Neuromodulation effects
    float dopamineEffect_;
    float curiosityEffect_;
    float noveltyEffect_;
    float predictionErrorEffect_;
    
    // Development system enhancement
    double developmentSpeed_;
    double criticalPeriodAge_;
    double maturationPeriodAge_;
    std::map<double, DevelopmentalStage> customStages_;
    
    // Memory system access
    bool workingMemoryEnabled_;
    bool episodicMemoryEnabled_;
    bool semanticMemoryEnabled_;
    
    // Performance monitoring
    PerformanceStats currentStats_;
    
    // Debugging and profiling
    DebugLevel debugLevel_;
    bool profilingEnabled_;
    std::map<std::string, double> profileData_;
    
    // Sensory filtering
    bool sensoryFilteringEnabled_;
    float visionFilteringStrength_;
    float touchFilteringStrength_;
    
    // Custom action mappings
    std::map<ActionType, std::string> actionMappings_;
    
    // Reward shaping
    std::function<float(const SensoryPercept&, const AgentBody&)> customRewardFunction_;
    bool rewardFunctionEnabled_;
    
    // Episode management
    size_t episodeLimit_;
    size_t stepLimit_;
    double timeLimit_;
    size_t currentEpisode_;
    size_t stepsInCurrentEpisode_;
    double currentEpisodeTime_;
    
    // External event handling
    std::function<void(const std::string&)> externalEventCallback_;
};

} // namespace nlm