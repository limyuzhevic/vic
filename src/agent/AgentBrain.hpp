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
    
    // Advanced Control Commands
    
    // Set exploration level (0.0 = deterministic, 1.0 = fully random)
    void setExplorationLevel(float level) { explorationLevel_ = std::clamp(level, 0.0f, 1.0f); }
    
    // Set curiosity threshold for exploration (lower = more exploration)
    void setCuriosityThreshold(float threshold) { curiosityThreshold_ = threshold; }
    
    // Enable/disable automatic reward prediction
    void enableRewardPrediction(bool enable) { rewardPredictionEnabled_ = enable; }
    
    // Set desired novelty level for behavior adaptation
    void setDesiredNoveltyLevel(float level) { desiredNoveltyLevel_ = std::clamp(level, 0.0f, 1.0f); }
    
    // Enable/disable memory consolidation
    void enableMemoryConsolidation(bool enable) { memoryConsolidationEnabled_ = enable; }
    
    // Set memory retention target (0.0 = short-term, 1.0 = long-term)
    void setMemoryRetentionTarget(float target) { memoryRetentionTarget_ = std::clamp(target, 0.0f, 1.0f); }
    
    // Set learning rate modifier (affects plasticity)
    void setLearningRateModifier(float modifier) { learningRateModifier_ = std::clamp(modifier, 0.1f, 5.0f); }
    
    // Enable/disable structural plasticity
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    
    // Set developmental acceleration factor
    void setDevelopmentalAcceleration(float factor) { developmentalAcceleration_ = std::clamp(factor, 0.1f, 10.0f); }
    
    // Set reward sensitivity (affects motivation)
    void setRewardSensitivity(float sensitivity) { rewardSensitivity_ = std::clamp(sensitivity, 0.1f, 5.0f); }
    
    // Enable/disable fatigue mechanism (behavioral endurance)
    void enableFatigueMechanism(bool enable) { fatigueEnabled_ = enable; }
    
    // Set energy conservation target
    void setEnergyConservationTarget(float target) { energyConservationTarget_ = std::clamp(target, 0.0f, 1.0f); }
    
    // Enable/disable social learning
    void enableSocialLearning(bool enable) { socialLearningEnabled_ = enable; }
    
    // Set imitation threshold
    void setImitationThreshold(float threshold) { imitationThreshold_ = std::clamp(threshold, 0.0f, 1.0f); }
    
    // Get current exploration level
    float getExplorationLevel() const { return explorationLevel_; }
    
    // Get current curiosity threshold
    float getCuriosityThreshold() const { return curiosityThreshold_; }
    
    // Check if reward prediction is enabled
    bool isRewardPredictionEnabled() const { return rewardPredictionEnabled_; }
    
    // Get current novelty target
    float getDesiredNoveltyLevel() const { return desiredNoveltyLevel_; }
    
    // Check if memory consolidation is enabled
    bool isMemoryConsolidationEnabled() const { return memoryConsolidationEnabled_; }
    
    // Get current memory retention target
    float getMemoryRetentionTarget() const { return memoryRetentionTarget_; }
    
    // Get current learning rate modifier
    float getLearningRateModifier() const { return learningRateModifier_; }
    
    // Check if structural plasticity is enabled
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    
    // Get developmental acceleration factor
    float getDevelopmentalAcceleration() const { return developmentalAcceleration_; }
    
    // Get reward sensitivity
    float getRewardSensitivity() const { return rewardSensitivity_; }
    
    // Check if fatigue mechanism is enabled
    bool isFatigueEnabled() const { return fatigueEnabled_; }
    
    // Get energy conservation target
    float getEnergyConservationTarget() const { return energyConservationTarget_; }
    
    // Check if social learning is enabled
    bool isSocialLearningEnabled() const { return socialLearningEnabled_; }
    
    // Get imitation threshold
    float getImitationThreshold() const { return imitationThreshold_; }
    
    // Get current reward prediction error
    float getRewardPredictionError() const { return rewardPredictionError_; }
    
    // Set target reward prediction error (learning goal)
    void setTargetPredictionError(float error) { targetPredictionError_ = error; }
    
    // Get target prediction error
    float getTargetPredictionError() const { return targetPredictionError_; }
    
    // Get current behavioral state descriptor
    std::string getBehavioralState() const;
    
    // Get current cognitive load indicator
    float getCognitiveLoad() const { return cognitiveLoad_; }
    
    // Check if agent is in exploration mode
    bool isExploring() const;
    
    // Get adaptation progress for current task
    float getAdaptationProgress() const { return adaptationProgress_; }
    
    // Set task context for behavior adaptation
    void setTaskContext(const std::string& task) { currentTask_ = task; }
    
    // Get current task context
    const std::string& getTaskContext() const { return currentTask_; }
    
    // Enable/disable debugging output
    void enableDebugOutput(bool enable) { debugOutputEnabled_ = enable; }
    
    // Check if debug output is enabled
    bool isDebugOutputEnabled() const { return debugOutputEnabled_; }
    
    // Get performance metrics summary
    std::string getPerformanceMetrics() const;
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
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
    
    // Advanced control state
    float explorationLevel_;
    float curiosityThreshold_;
    bool rewardPredictionEnabled_;
    float desiredNoveltyLevel_;
    bool memoryConsolidationEnabled_;
    float memoryRetentionTarget_;
    float learningRateModifier_;
    bool fatigueEnabled_;
    float energyConservationTarget_;
    bool socialLearningEnabled_;
    float imitationThreshold_;
    float rewardPredictionError_;
    float targetPredictionError_;
    float cognitiveLoad_;
    float adaptationProgress_;
    std::string currentTask_;
    bool debugOutputEnabled_;
    uint64_t explorationCount_;
    uint64_t exploitationCount_;
    uint64_t totalRewards_;
    uint64_t totalSteps_;
};

} // namespace nlm
