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
    
    // ========== ADVANCED AGENT CAPABILITIES ==========
    
    // Enhanced sensory processing
    std::vector<float> extractSensoryFeatures(const SensoryPercept& percept) const;
    std::vector<float> processSensoryHistory(const std::vector<SensoryPercept>& history) const;
    float assessSensoryQuality(const SensoryPercept& percept) const;
    
    // Multi-modal sensory fusion
    std::vector<float> fuseSensoryModality(const std::vector<float>& visionFeatures,
                                          const std::vector<float>& touchFeatures,
                                          const std::vector<float>& internalFeatures,
                                          const std::vector<float>& proprioceptionFeatures) const;
    
    // Policy-based action selection
    MotorCommand selectAction(const std::vector<float>& state, bool useExploration = true);
    MotorCommand applyReinforcementPolicy(float reward, float predictedReward, const std::vector<float>& state);
    MotorCommand applyRuleBasedPolicy(const std::vector<float>& state) const;
    
    // Behavior planning and sequencing
    std::vector<MotorCommand> planBehaviorSequence(const std::vector<float>& goalState) const;
    MotorCommand getNextActionInSequence(const std::vector<MotorCommand>& sequence, size_t stepIndex) const;
    bool shouldContinueSequence(const std::vector<MotorCommand>& sequence, size_t currentStep) const;
    
    // Advanced curiosity and exploration
    float computeCuriosityDrive(const SensoryPercept& currentPercept) const;
    MotorCommand applyCuriosityDrivenExploration(const std::vector<float>& state);
    MotorCommand balanceExplorationExploitation(const std::vector<float>& state, float explorationRate);
    
    // Sensorimotor integration
    std::vector<float> predictSensoryOutcome(const MotorCommand& action) const;
    float computeSensoryPredictionError(const SensoryPercept& actual, const SensoryPercept& predicted) const;
    void updateSensorimotorMapping(const MotorCommand& action, const SensoryPercept& outcome);
    
    // Goal-based behavior
    bool hasGoal() const { return !goalState_.empty(); }
    void setGoal(const std::vector<float>& goalState);
    void clearGoal();
    float getGoalProgress(const std::vector<float>& currentState) const;
    
    // Social learning capabilities
    void learnFromObservation(const std::vector<float>& observerState, const MotorCommand& observedAction);
    std::vector<MotorCommand> getSociallyInformedActions(const std::vector<float>& socialContext) const;
    void enableSocialLearning(bool enabled) { socialLearningEnabled_ = enabled; }
    bool isSocialLearningEnabled() const { return socialLearningEnabled_; }
    
    // Advanced agent state and monitoring
    std::vector<float> getCurrentAgentState() const;
    void setAgentState(const std::vector<float>& state);
    float getActionValue(const MotorCommand& action, const std::vector<float>& state) const;
    void updateActionValues(const MotorCommand& action, float value, const std::vector<float>& state);
    
    // Performance monitoring
    size_t getTotalActionsTaken() const { return totalActionsTaken_; }
    float getActionSuccessRate() const { return actionSuccessRate_; }
    float getExplorationRatio() const { return explorationRatio_; }
    double getTotalSimulationTime() const { return totalSimulationTime_; }
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // Advanced configuration
    void setExplorationRate(float rate) { explorationRate_ = rate; }
    void setExploitationWeight(float weight) { exploitationWeight_ = weight; }
    void setLearningRate(float rate) { learningRate_ = rate; }
    void setActionSelectionMethod(const std::string& method) { actionSelectionMethod_ = method; }
    
    float getExplorationRate() const { return explorationRate_; }
    float getExploitationWeight() const { return exploitationWeight_; }
    float getLearningRate() const { return learningRate_; }
    const std::string& getActionSelectionMethod() const { return actionSelectionMethod_; }
    
    // Episodic memory for agent behavior
    void storeEpisode(const std::vector<float>& state, const MotorCommand& action, 
                     float reward, const std::vector<float>& nextState);
    std::vector<std::tuple<std::vector<float>, MotorCommand, float, std::vector<float>>> getRecentEpisodes(size_t count) const;
    
    // Configuration groups
    void addSensorGroup(const std::string& groupName);
    void removeSensorGroup(const std::string& groupName);
    bool hasSensorGroup(const std::string& groupName) const;
    
    // Performance and debugging
    void enablePerformanceMonitoring(bool enabled) { performanceMonitoringEnabled_ = enabled; }
    bool isPerformanceMonitoringEnabled() const { return performanceMonitoringEnabled_; }
    void setActionSelectionTimeout(float timeout) { actionSelectionTimeout_ = timeout; }
    float getActionSelectionTimeout() const { return actionSelectionTimeout_; }
    
    // Health and robustness
    bool isHealthy() const;
    std::string getHealthStatus() const;
    void resetPerformanceCounters();
    
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
    bool socialLearningEnabled_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // Advanced agent state
    std::vector<float> currentState_;
    std::vector<float> goalState_;
    
    // Policy and learning state
    std::unordered_map<MotorCommand, std::vector<float>> actionValues_;
    std::vector<std::tuple<std::vector<float>, MotorCommand, float, std::vector<float>>> episodeMemory_;
    std::string actionSelectionMethod_;
    float explorationRate_;
    float exploitationWeight_;
    float learningRate_;
    
    // Performance monitoring
    size_t totalActionsTaken_;
    size_t successfulActions_;
    float actionSuccessRate_;
    float explorationRatio_;
    double totalSimulationTime_;
    bool performanceMonitoringEnabled_;
    
    // Configuration groups
    std::unordered_set<std::string> sensorGroups_;
    
    // Action selection timing
    float actionSelectionTimeout_;
    
    // Social learning
    std::unordered_map<MotorCommand, std::vector<float>> socialActionWeights_;
    
    // Helper methods
    std::vector<float> encodeSensoryPercept(const SensoryPercept& percept) const;
    MotorCommand decodeMotorFromActivity(const std::vector<float>& motorActivity) const;
    float calculateExplorationBonus(const std::vector<float>& state) const;
    void updatePerformanceCounters(bool success);
    bool isActionInGroup(const MotorCommand& action, const std::string& groupName) const;
};

} // namespace nlm
