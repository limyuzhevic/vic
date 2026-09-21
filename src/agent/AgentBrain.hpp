#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include <memory>
#include <vector>
#include <unordered_map>

namespace nlm {

// Forward declarations
class Neuromodulator;
class PredictionError;

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
// PHASE 8: ADVANCED AGENT COGNITION WITH ENHANCED NEUROMODULATION
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
    void enableMetaCognition(bool enable) { metaCognitionEnabled_ = enable; }
    void enableActiveInference(bool enable) { activeInferenceEnabled_ = enable; }
    void enableSleepConsolidation(bool enable) { sleepConsolidationEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    bool isMetaCognitionEnabled() const { return metaCognitionEnabled_; }
    bool isActiveInferenceEnabled() const { return activeInferenceEnabled_; }
    bool isSleepConsolidationEnabled() const { return sleepConsolidationEnabled_; }
    
    // Advanced agent capabilities
    void enableMetaLearning(bool enable) { metaLearningEnabled_ = enable; }
    void enableHierarchicalControl(bool enable) { hierarchicalControlEnabled_ = enable; }
    void enableSocialLearning(bool enable) { socialLearningEnabled_ = enable; }
    void enableTransferLearning(bool enable) { transferLearningEnabled_ = enable; }
    
    // Reward shaping and exploration
    void setRewardShaping(const std::string& strategy) { rewardShapingStrategy_ = strategy; }
    void setExplorationStrategy(const std::string& strategy) { explorationStrategy_ = strategy; }
    void setIntrinsicMotivation(float weight) { intrinsicMotivationWeight_ = weight; }
    
    // State tracking for learning
    std::vector<float> getExperienceBuffer() const { return experienceBuffer_; }
    void addToExperienceBuffer(const std::vector<float>& state) { experienceBuffer_.insert(experienceBuffer_.end(), state.begin(), state.end()); }
    
    // Goal management
    void setPrimaryGoal(const std::string& goal) { primaryGoal_ = goal; }
    void addSecondaryGoal(const std::string& goal) { secondaryGoals_.push_back(goal); }
    
    // Performance metrics
    float getPerformanceScore() const { return performanceScore_; }
    float getLearningRate() const { return learningRate_; }
    
    // Prediction and planning
    void setPredictionHorizon(double horizon) { predictionHorizon_ = horizon; }
    void setPlanningDepth(int depth) { planningDepth_ = depth; }
    
    // Sleep and consolidation
    bool isAsleep() const { return isAsleep_; }
    void enterSleepMode() { isAsleep_ = true; }
    void wakeUp() { isAsleep_ = false; }
    
    // Configuration
    void setSensoryGain(float gain) { sensoryGain_ = gain; }
    void setMotorScaling(float scaling) { motorScaling_ = scaling; }
    void setLearningRate(float rate) { learningRate_ = rate; }
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    MotorCommand selectWithMetaCognition(MotorCommand defaultCmd);
    MotorCommand selectWithActiveInference(MotorCommand defaultCmd);
    
    // Advanced selection methods
    MotorCommand selectWithHierarchicalControl();
    MotorCommand selectWithSocialLearning();
    MotorCommand selectWithTransferLearning();
    
    // Reward shaping and exploration strategies
    MotorCommand selectWithRewardShaping(MotorCommand defaultCmd);
    MotorCommand selectWithExploration(MotorCommand defaultCmd);
    MotorCommand selectWithIntrinsicMotivation(MotorCommand defaultCmd);
    
    // Learning and adaptation
    void updateMetaCognition(double timestep);
    void updateActiveInference(double timestep);
    void updateHierarchicalControl(double timestep);
    void updateMetaLearning(double timestep);
    void updateTransferLearning(double timestep);
    void updateSocialLearning(double timestep);
    
    // Memory consolidation
    void performSleepConsolidation();
    void consolidateMetaMemory();
    void consolidateHierarchicalMemory();
    
    std::shared_ptr<Brain> brain_;
    
    // Motor neuron groups
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    std::vector<Neuron*> motorLookLeft_;
    std::vector<Neuron*> motorLookRight_;
    
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
    
    // Advanced agent state
    double predictionHorizon_;
    int planningDepth_;
    float performanceScore_;
    float learningRate_;
    
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    bool metaCognitionEnabled_;
    bool activeInferenceEnabled_;
    bool sleepConsolidationEnabled_;
    bool metaLearningEnabled_;
    bool hierarchicalControlEnabled_;
    bool socialLearningEnabled_;
    bool transferLearningEnabled_;
    
    // Strategy and configuration
    std::string rewardShapingStrategy_;
    std::string explorationStrategy_;
    float intrinsicMotivationWeight_;
    
    // State tracking
    std::vector<float> experienceBuffer_;
    std::unordered_map<NeuronId, float> neuronSalience_;
    std::unordered_map<NeuronId, float> neuronImportance_;
    
    // Goals
    std::string primaryGoal_;
    std::vector<std::string> secondaryGoals_;
    
    // Sleep state
    bool isAsleep_;
    double sleepStartTime_;
    size_t replayCount_;
    
    // Sensory processing
    float sensoryGain_;
    float motorScaling_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // Active inference components
    std::vector<float> beliefs_;
    std::vector<float> expectations_;
    float predictionErrorThreshold_;
    
    // Hierarchical control
    std::vector<std::string> skillHierarchy_;
    std::unordered_map<std::string, float> skillProficiency_;
    
    // Meta-cognition
    float metacognitionLevel_;
    float selfMonitoring_;
    float confidenceCalibration_;
    
    // Social learning
    std::vector<float> socialRewards_;
    std::unordered_map<NeuronId, float> socialValue_;
    
    // Transfer learning
    std::unordered_map<std::string, std::vector<float>> taskMemories_;
    std::unordered_map<std::string, float> transferStrength_;
    
    // Meta-learning
    std::vector<std::string> learningStrategies_;
    std::unordered_map<std::string, float> strategyEffectiveness_;
    
}; // namespace nlm