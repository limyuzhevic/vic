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
    
    // Advanced exploration methods
    MotorCommand advancedExplorationSelection();
    MotorCommand contextAwareExploration();
    
    // Advanced exploration and learning configuration
    void enableExploration(bool enable) { explorationEnabled_ = enable; }
    void setExplorationRate(float rate) { explorationRate_ = std::clamp(rate, 0.0f, 1.0f); }
    void setRandomSeed(uint64_t seed) { randomSeed_ = seed; }
    void setMaxActionChoices(size_t maxChoices) { maxActionChoices_ = maxChoices; }
    
    // Neuromodulation configuration
    void enableDopamineModulation(bool enable) { dopamineModulationEnabled_ = enable; }
    void enableNoveltyDetection(bool enable) { noveltyDetectionEnabled_ = enable; }
    void setNoveltyThreshold(float threshold) { noveltyThreshold_ = std::clamp(threshold, 0.0f, 1.0f); }
    
    // Developmental configuration
    void enableNeuralDevelopment(bool enable) { neuralDevelopmentEnabled_ = enable; }
    void setDevelopmentalRate(float rate) { developmentalRate_ = std::clamp(rate, 0.0f, 1.0f); }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    // Advanced learning and adaptation configuration
    void enableMetaPlasticity(bool enable) { metaPlasticityEnabled_ = enable; }
    void setMetaPlasticityRate(float rate) { metaPlasticityRate_ = std::clamp(rate, 0.0f, 1.0f); }
    void enableAdaptiveLearning(bool enable) { adaptiveLearningEnabled_ = enable; }
    void setAdaptiveLearningRate(float rate) { adaptiveLearningRate_ = std::clamp(rate, 0.0f, 1.0f); }
    void enableContinuousSelfOptimization(bool enable) { continuousSelfOptimizationEnabled_ = enable; }
    void setSelfOptimizationRate(float rate) { selfOptimizationRate_ = std::clamp(rate, 0.0f, 1.0f); }
    
    // Environmental adaptation configuration
    void enableEnvironmentalAdaptation(bool enable) { environmentalAdaptationEnabled_ = enable; }
    void setEnvironmentalAdaptationRate(float rate) { environmentalAdaptationRate_ = std::clamp(rate, 0.0f, 1.0f); }
    void enableClimateAdaptation(bool enable) { climateAdaptationEnabled_ = enable; }
    void setClimateAdaptationThreshold(float threshold) { climateAdaptationThreshold_ = std::clamp(threshold, 0.0f, 1.0f); }
    void enableResourceAdaptation(bool enable) { resourceAdaptationEnabled_ = enable; }
    void setResourceAdaptationEfficiency(float efficiency) { resourceAdaptationEfficiency_ = std::clamp(efficiency, 0.0f, 1.0f); }
    
    // Social adaptation configuration
    void enableSocialAdaptation(bool enable) { socialAdaptationEnabled_ = enable; }
    void setSocialAdaptationRate(float rate) { socialAdaptationRate_ = std::clamp(rate, 0.0f, 1.0f); }
    void enableTheoryOfMind(bool enable) { theoryOfMindEnabled_ = enable; }
    void setTheoryOfMindCapacity(float capacity) { theoryOfMindCapacity_ = std::clamp(capacity, 0.0f, 1.0f); }
    void enableEmpathySimulation(bool enable) { empathySimulationEnabled_ = enable; }
    
    // Temporal processing configuration
    void enableTemporalAdaptation(bool enable) { temporalAdaptationEnabled_ = enable; }
    void setTemporalProcessingRange(float range) { temporalProcessingRange_ = std::clamp(range, 0.0f, 10.0f); }
    void enableTimePerceptionTraining(bool enable) { timePerceptionTrainingEnabled_ = enable; }
    void setTimePerceptionAccuracy(float accuracy) { timePerceptionAccuracy_ = std::clamp(accuracy, 0.0f, 1.0f); }
    
    // Quantum and consciousness configuration (experimental)
    void enableQuantumProcessing(bool enable) { quantumProcessingEnabled_ = enable; }
    void setQuantumCoherenceLevel(float level) { quantumCoherenceLevel_ = std::clamp(level, 0.0f, 1.0f); }
    void enableSyntheticConsciousness(bool enable) { syntheticConsciousnessEnabled_ = enable; }
    void setConsciousnessLevel(float level) { consciousnessLevel_ = std::clamp(level, 0.0f, 1.0f); }
    void enableQuantumEntanglement(bool enable) { quantumEntanglementEnabled_ = enable; }
    void setEntanglementStrength(float strength) { entanglementStrength_ = std::clamp(strength, 0.0f, 1.0f); }
    
    // Advanced functionality flags
    bool isExplorationEnabled() const { return explorationEnabled_; }
    bool isDopamineModulationEnabled() const { return dopamineModulationEnabled_; }
    bool isNoveltyDetectionEnabled() const { return noveltyDetectionEnabled_; }
    bool isNeuralDevelopmentEnabled() const { return neuralDevelopmentEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    bool isMetaPlasticityEnabled() const { return metaPlasticityEnabled_; }
    bool isAdaptiveLearningEnabled() const { return adaptiveLearningEnabled_; }
    bool isContinuousSelfOptimizationEnabled() const { return continuousSelfOptimizationEnabled_; }
    bool isEnvironmentalAdaptationEnabled() const { return environmentalAdaptationEnabled_; }
    bool isSocialAdaptationEnabled() const { return socialAdaptationEnabled_; }
    bool isTemporalAdaptationEnabled() const { return temporalAdaptationEnabled_; }
    bool isQuantumProcessingEnabled() const { return quantumProcessingEnabled_; }
    bool isSyntheticConsciousnessEnabled() const { return syntheticConsciousnessEnabled_; }
    
    // Get current advanced configuration values
    float getExplorationRate() const { return explorationRate_; }
    uint64_t getRandomSeed() const { return randomSeed_; }
    size_t getMaxActionChoices() const { return maxActionChoices_; }
    float getNoveltyThreshold() const { return noveltyThreshold_; }
    float getDevelopmentalRate() const { return developmentalRate_; }
    float getMetaPlasticityRate() const { return metaPlasticityRate_; }
    float getAdaptiveLearningRate() const { return adaptiveLearningRate_; }
    float getSelfOptimizationRate() const { return selfOptimizationRate_; }
    float getEnvironmentalAdaptationRate() const { return environmentalAdaptationRate_; }
    float getSocialAdaptationRate() const { return socialAdaptationRate_; }
    float getTemporalProcessingRange() const { return temporalProcessingRange_; }
    float getQuantumCoherenceLevel() const { return quantumCoherenceLevel_; }
    float getConsciousnessLevel() const { return consciousnessLevel_; }
    // Get current advanced configuration values
    float getExplorationRate() const { return explorationRate_; }
    uint64_t getRandomSeed() const { return randomSeed_; }
    size_t getMaxActionChoices() const { return maxActionChoices_; }
    float getNoveltyThreshold() const { return noveltyThreshold_; }
    float getDevelopmentalRate() const { return developmentalRate_; }
    float getMetaPlasticityRate() const { return metaPlasticityRate_; }
    float getAdaptiveLearningRate() const { return adaptiveLearningRate_; }
    float getSelfOptimizationRate() const { return selfOptimizationRate_; }
    float getEnvironmentalAdaptationRate() const { return environmentalAdaptationRate_; }
    float getSocialAdaptationRate() const { return socialAdaptationRate_; }
    float getTemporalProcessingRange() const { return temporalProcessingRange_; }
    float getQuantumCoherenceLevel() const { return quantumCoherenceLevel_; }
    float getConsciousnessLevel() const { return consciousnessLevel_; }
    float getEntanglementStrength() const { return entanglementStrength_; }

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
    bool explorationEnabled_;
    bool dopamineModulationEnabled_;
    bool noveltyDetectionEnabled_;
    bool neuralDevelopmentEnabled_;
    bool metaPlasticityEnabled_;
    bool adaptiveLearningEnabled_;
    bool continuousSelfOptimizationEnabled_;
    bool environmentalAdaptationEnabled_;
    bool socialAdaptationEnabled_;
    bool temporalAdaptationEnabled_;
    bool quantumProcessingEnabled_;
    bool syntheticConsciousnessEnabled_;
    bool quantumEntanglementEnabled_;
    
    // Environmental adaptation flags
    bool climateAdaptationEnabled_;
    bool resourceAdaptationEnabled_;
    bool theoryOfMindEnabled_;
    bool empathySimulationEnabled_;
    bool timePerceptionTrainingEnabled_;
    
    // Advanced configuration parameters
    float explorationRate_;
    float noveltyThreshold_;
    float developmentalRate_;
    float metaPlasticityRate_;
    float adaptiveLearningRate_;
    float selfOptimizationRate_;
    float environmentalAdaptationRate_;
    float socialAdaptationRate_;
    float temporalProcessingRange_;
    float quantumCoherenceLevel_;
    float consciousnessLevel_;
    float entanglementStrength_;
    float climateAdaptationThreshold_;
    float resourceAdaptationEfficiency_;
    float theoryOfMindCapacity_;
    float timePerceptionAccuracy_;
    
    // Random seed and action choices
    uint64_t randomSeed_;
    size_t maxActionChoices_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // Advanced neural diversity and specialization configuration
    bool neuralSpecializationEnabled_;
    float neuralDiversityLevel_;
    bool regionSpecializationEnabled_;
    float regionSpecializationLevel_;
    bool connectomeOptimizationEnabled_;
    float optimalConnectivity_;
    bool structuralOptimizationEnabled_;
    float structuralComplexity_;
    bool functionalClusteringEnabled_;
    size_t functionalClusterCount_;
    bool adaptiveConnectivityEnabled_;
    float adaptiveConnectivityRate_;
    bool homeostaticRegulationEnabled_;
    float homeostaticSetPoint_;
    bool synapticPruningEnabled_;
    float pruningIntensity_;
    bool activityDependentGrowthEnabled_;
    float growthRate_;
};

} // namespace nlm
