#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>

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
    
    // Advanced features
    // Get agent state snapshot for checkpointing
    std::string getStateSnapshot() const;
    
    // Set agent state from snapshot
    bool restoreStateFromSnapshot(const std::string& snapshot);
    
    // Get current agent performance metrics
    struct PerformanceMetrics {
        float curiosityLevel = 0.0f;
        float noveltyLevel = 0.0f;
        float predictionError = 0.0f;
        float dopamineLevel = 0.0f;
        float developmentalStage = 0.0f;
        uint64_t totalSteps = 0;
        uint64_t totalSpikes = 0;
    };
    
    PerformanceMetrics getPerformanceMetrics() const;
    
    // Enable/disable specific neuromodulators
    void setNeuromodulatorLevel(float dopamine, float serotonin, float acetylcholine);
    
    // Get current neuromodulator levels
    void getNeuromodulatorLevels(float& dopamine, float& serotonin, float& acetylcholine) const;
    
    // Configuration management
    void setSensoryConfig(const std::string& configJson);
    void setMotorConfig(const std::string& configJson);
    std::string getConfigSummary() const;
    
    // Debug and monitoring
    void enableDebugLogging(bool enable) { debugLoggingEnabled_ = enable; }
    bool isDebugLoggingEnabled() const { return debugLoggingEnabled_; }
    
    // Set curiosity threshold for exploration
    void setCuriosityThreshold(float threshold) { curiosityThreshold_ = threshold; }
    float getCuriosityThreshold() const { return curiosityThreshold_; }
    
    // Set reward prediction learning rate
    void setRewardPredictionLearningRate(float rate) { rewardPredictionLearningRate_ = rate; }
    float getRewardPredictionLearningRate() const { return rewardPredictionLearningRate_; }
    
    // Set neuromodulation strength
    void setNeuromodulationStrength(float strength) { neuromodulationStrength_ = strength; }
    float getNeuromodulationStrength() const { return neuromodulationStrength_; }
    
    // Get number of active neurons by type
    size_t getActiveMotorNeuronCount() const;
    size_t getActiveSensoryNeuronCount() const;
    
    // Set exploration strategy
    enum class ExplorationStrategy {
        None,
        Random,
        CuriosityDriven,
        RewardBased
    };
    
    void setExplorationStrategy(ExplorationStrategy strategy) { explorationStrategy_ = strategy; }
    ExplorationStrategy getExplorationStrategy() const { return explorationStrategy_; }
    
    // Enable/disable curiosity-based exploration
    void setCuriosityExploration(bool enable) { curiosityExplorationEnabled_ = enable; }
    bool isCuriosityExplorationEnabled() const { return curiosityExplorationEnabled_; }
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Internal helper methods
    void clearNeuronGroups();
    void validateNeuronGroups() const;
    
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
    float serotoninLevel_;
    float acetylcholineLevel_;
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
    bool debugLoggingEnabled_;
    bool curiosityExplorationEnabled_;
    
    // Additional configuration
    float curiosityThreshold_;
    float rewardPredictionLearningRate_;
    float neuromodulationStrength_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // Performance tracking
    uint64_t totalSteps_;
    uint64_t totalSpikes_;
    mutable uint64_t debugCounter_;
    
    // Exploration strategy
    ExplorationStrategy explorationStrategy_;
    
    // Performance metrics cache
    mutable PerformanceMetrics cachedMetrics_;
    mutable bool metricsDirty_;
    
    // Internal helpers
    static std::string serializeState(const AgentBrain& agent);
    static bool deserializeState(AgentBrain& agent, const std::string& data);
    
    void updateCachedMetrics() const;
    
    // Legacy compatibility
    void migrateLegacyState();
};
