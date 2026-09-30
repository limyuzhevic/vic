#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

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
    const Brain* getBrain() const { return brain_.get(); }
    
    // Configuration with getter/setter pairs
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // Advanced configuration with validation
    void setCuriosityThreshold(float threshold);
    void setExplorationRate(float rate);
    void setNoveltyDecayRate(float decay);
    void setSensoryScaling(float vision, float touch, float internal, float proprio);
    void setMotorDecodingMode(int mode); // 0=threshold, 1=weighted, 2=fuzzy
    
    // State monitoring and debugging
    bool isHealthy() const;
    std::string getStatus() const;
    double getSimulationTime() const;
    
    // Sensory and motor statistics
    float getSensoryActivations(const std::string& modality) const;
    float getMotorCommandProbabilities(const std::string& command) const;
    
    // Advanced control functions
    void forceAction(MotorCommand command);
    void addSensoryPattern(const std::string& patternName, const std::vector<float>& pattern);
    void clearSensoryMemory();
    
    // Advanced features
    void enableAdvancedLearning(bool enable) { advancedLearningEnabled_ = enable; }
    void enableSelfRepair(bool enable) { selfRepairEnabled_ = enable; }
    void enableMetaLearning(bool enable) { metaLearningEnabled_ = enable; }
    
    bool isAdvancedLearningEnabled() const { return advancedLearningEnabled_; }
    bool isSelfRepairEnabled() const { return selfRepairEnabled_; }
    bool isMetaLearningEnabled() const { return metaLearningEnabled_; }
    
    // Development and learning control
    void setPlasticityRate(float rate);
    void setMemoryConsolidationRate(float rate);
    void setAttentionStrength(float strength);
    
    // Monitoring and statistics
    float getAverageFiringRate() const;
    float getActionDiversity() const;
    float getLearningProgress() const;
    float getSurvivalScore() const;
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Helper to get attention system (placeholder)
    AttentionSystem* getAttentionSystem();
    
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
    
    // Advanced configuration parameters
    float curiosityThreshold_;
    float explorationRate_;
    float noveltyDecayRate_;
    float visionScale_;
    float touchScale_;
    float internalScale_;
    float proprioceptionScale_;
    int motorDecodingMode_;
    double simulationTime_;
    
    // Advanced learning flags
    bool advancedLearningEnabled_;
    bool selfRepairEnabled_;
    bool metaLearningEnabled_;
    
    // Learning control parameters
    float plasticityRate_;
    float memoryConsolidationRate_;
    float attentionStrength_;
    
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // Stored patterns for memory
    std::unordered_map<std::string, std::vector<float>> storedPatterns_;
};

} // namespace nlm
