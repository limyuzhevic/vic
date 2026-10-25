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
    
    // Advanced sensory processing
    // Extract features from sensory input
    std::vector<float> extractFeatures(const SensoryPercept& percept) const;
    
    // Apply feature filtering
    void applyFeatureFiltering(std::vector<float>& features) const;
    
    // Compute sensory surprise
    float computeSensorySurprise(const SensoryPercept& percept) const;
    
    // Advanced neuromodulation
    // Apply dopamine-based reinforcement learning
    void applyDopamineModulation(float reward, float predictionError);
    
    // Apply serotonin-based behavioral inhibition
    void applySerotoninModulation(float stressLevel);
    
    // Apply acetylcholine-based attention
    void applyAcetylcholineModulation(float noveltyLevel);
    
    // Memory consolidation
    // Consolidate recent experiences into long-term memory
    void consolidateMemory();
    
    // Update memory traces
    void updateMemoryTraces();
    
    // Social learning
    // Observe other agents and learn from them
    void observeOtherAgent(const AgentBrain& otherAgent);
    
    // Imitate observed behaviors
    void imitateBehavior(const MotorCommand& command);
    
    // Curiosity-driven exploration
    // Compute exploration bonus based on uncertainty
    float computeExplorationBonus() const;
    
    // Apply curiosity-based exploration
    MotorCommand applyCuriosityExploration(const MotorCommand& defaultCommand);
    
    // Stress and fatigue systems
    // Update stress level based on environmental demands
    void updateStressLevel(float environmentDifficulty);
    
    // Get current stress level
    float getStressLevel() const;
    
    // Apply fatigue effects
    void applyFatigueEffects();
    
    // Cognitive load management
    // Estimate current cognitive load
    float estimateCognitiveLoad() const;
    
    // Apply cognitive load limits
    void applyCognitiveLoadLimits();
    
    // Sleep and consolidation cycles
    // Enter sleep state for memory consolidation
    void enterSleepCycle();
    
    // Wake from sleep
    void wakeUp();
    
    // Check if sleeping
    bool isSleeping() const;
    
    // Meta-learning
    // Learn how to learn more efficiently
    void metaLearn();
    
    // Adapt learning rate based on performance
    void adaptLearningRate(float performance);
    
    // Get learning efficiency
    float getLearningEfficiency() const;
    
    // Advanced reward prediction
    // Predict future rewards based on current state
    float predictFutureReward() const;
    
    // Update reward prediction model
    void updateRewardPredictionModel();
    
    // Get reward prediction error
    float getRewardPredictionError() const;
    
    // Homeostatic regulation
    // Maintain homeostasis through regulatory mechanisms
    void maintainHomeostasis();
    
    // Apply homeostatic pressure
    void applyHomeostaticPressure();
    
    // Get homeostatic state
    float getHomeostaticState() const;
    
    // Intrinsic motivation
    // Generate intrinsic motivation signals
    float generateIntrinsicMotivation() const;
    
    // Apply intrinsic motivation
    void applyIntrinsicMotivation();
    
    // Get current motivation level
    float getMotivationLevel() const;
    
    // Emergency response
    // Trigger emergency response to dangerous situations
    void triggerEmergencyResponse();
    
    // Apply emergency response
    void applyEmergencyResponse();
    
    // Check emergency status
    bool isInEmergency() const;
    
    // Recovery
    // Recover from emergency state
    void recoverFromEmergency();nlm::MotorCommand decodeMotorCommand();
    
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
    
    // Additional utility methods
    // Check if sensory percept is valid
    bool isValidPercept(const SensoryPercept& percept) const;
    
    // Decay sensory novelty signal
    void decaySensoryNovelty();
    
    // Get sensory neuron group sizes
    struct SensoryNeuronCounts {
        size_t vision;
        size_t touch;
        size_t internal;
        size_t proprioception;
    };
    
    SensoryNeuronCounts getSensoryNeuronCounts() const;
    
    // Get simulation statistics
    struct AgentStats {
        size_t totalMotorNeurons;
        size_t totalSensoryNeurons;
        float currentCuriosity;
        float currentNovelty;
        double simulationAge;
    };
    
    AgentStats getStats() const;
    
    // Advanced features
    // Get stress level
    float getStressLevel() const;
    
    // Get cognitive load
    float getCognitiveLoad() const;
    
    // Check if sleeping
    bool isSleeping() const;
    
    // Get learning efficiency
    float getLearningEfficiency() const;
    
    // Get reward prediction error
    float getRewardPredictionError() const;
    
    // Get homeostatic state
    float getHomeostaticState() const;
    
    // Get motivation level
    float getMotivationLevel() const;
    
    // Check emergency status
    bool isInEmergency() const;
    
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
    
    // Advanced neuromodulation
    float serotoninLevel_;
    float acetylcholineLevel_;
    float norepinephrineLevel_;
    
    // Stress and fatigue
    float stressLevel_;
    float fatigueLevel_;
    
    // Cognitive and meta-learning
    float cognitiveLoad_;
    float learningEfficiency_;
    float homeostaticState_;
    float motivationLevel_;
    
    // Sleep and recovery
    bool isSleeping_;
    double sleepStartTime_;
    
    // Emergency state
    bool inEmergency_;
    
    // Development state
    double developmentalAge_;
    float plasticityModifier_;
    
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    bool enableSocialLearning_;
    bool enableEmergencyResponses_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // Memory consolidation
    std::vector<std::vector<float>> memoryTraces_;
    double lastConsolidationTime_;
    
    // Reward prediction model
    std::vector<float> rewardPredictions_;
    std::vector<float> rewardPredictionErrors_;
    
    // Social learning
    std::vector<MotorCommand> observedBehaviors_;
    double lastObservationTime_;
    
    // Intrinsic motivation
    float intrinsicMotivation_;
    double lastIntrinsicUpdateTime_;
};

} // namespace nlm
