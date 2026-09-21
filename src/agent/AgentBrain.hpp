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
    
    // Cache for motor activity calculations (performance optimization)
    mutable float cachedForwardActivity_;
    mutable float cachedBackwardActivity_;
    mutable float cachedLeftActivity_;
    mutable float cachedRightActivity_;
    mutable float cachedInteractActivity_;
    mutable float cachedWaitActivity_;
    mutable bool cachedActivityValid_;
    
    // Statistics tracking
    uint64_t totalStepsProcessed_;
    uint64_t sensoryInputsProcessed_;
    uint64_t motorCommandsGenerated_;
    float cumulativeReward_;
    size_t episodeCount_;
    
    // Performance optimization flags
    bool enableActivityCaching_;
    bool enableStatisticsTracking_;
    
    // Configuration for advanced users
    float activityThreshold_;
    float curiosityThreshold_;
    float noveltyThreshold_;
    
    // Debug and development support
    std::vector<std::string> debugLog_;
    size_t maxDebugLogSize_;
    
    // Development-specific state
    float developmentalPlasticityRate_;
    float maturationProgress_;
    float synapticGrowthRate_;
    
    // Environmental adaptation
    float adaptationRate_;
    float environmentalStability_;
    float habitatComplexity_;
    
    // Learning parameters
    float learningRate_;
    float explorationRate_;
    float exploitationRate_;
    
    // Social learning
    bool enableSocialLearning_;
    float socialLearningRate_;
    std::vector<float> socialInfluence_;
    
    // Stress and recovery
    float stressLevel_;
    float recoveryRate_;
    float stressThreshold_;
    
    // Sleep and consolidation
    bool isSleeping_;
    float sleepDebt_;
    float consolidationEfficiency_;
    
    // Meta-learning
    bool enableMetaLearning_;
    float metaLearningRate_;
    std::vector<float> metaFeatureBuffer_;
    
    // Energy management
    float energyBudget_;
    float energyConsumptionRate_;
    float energyRegenerationRate_;
    
    // Evolution
    float mutationRate_;
    float selectionPressure_;
    float populationDiversity_;
};

} // namespace nlm
