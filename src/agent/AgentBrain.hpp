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
    
    // Configure neuron distribution for motor and sensory groups
    void configureMotorDistribution(size_t motorForwardCount, size_t motorBackwardCount,
                                   size_t motorTurnLeftCount, size_t motorTurnRightCount,
                                   size_t motorInteractCount, size_t motorWaitCount);
    void configureSensoryDistribution(size_t visionCount, size_t touchCount,
                                     size_t internalCount, size_t proprioceptionCount);
    void configureNeuronsToConnectionsRatio(float ratio);
    
    // Get current configuration
    size_t getMotorForwardCount() const { return motorForward_.size(); }
    size_t getMotorBackwardCount() const { return motorBackward_.size(); }
    size_t getMotorTurnLeftCount() const { return motorTurnLeft_.size(); }
    size_t getMotorTurnRightCount() const { return motorTurnRight_.size(); }
    size_t getMotorInteractCount() const { return motorInteract_.size(); }
    size_t getMotorWaitCount() const { return motorWait_.size(); }
    
    size_t getVisionCount() const { return sensoryVision_.size(); }
    size_t getTouchCount() const { return sensoryTouch_.size(); }
    size_t getInternalCount() const { return sensoryInternal_.size(); }
    size_t getProprioceptionCount() const { return sensoryProprioception_.size(); }
    
    // Enable/disable advanced features
    void enableContextAwareness(bool enable) { contextAwarenessEnabled_ = enable; }
    void enableMetaLearning(bool enable) { metaLearningEnabled_ = enable; }
    void enableNeuralCompression(bool enable) { neuralCompressionEnabled_ = enable; }
    
    bool isContextAware() const { return contextAwarenessEnabled_; }
    bool isMetaLearningEnabled() const { return metaLearningEnabled_; }
    bool isNeuralCompressionEnabled() const { return neuralCompressionEnabled_; }
    
    // Advanced motor control
    void setActionBias(const std::string& bias);  // Set bias towards certain actions
    float getActionEntropy() const;  // Measure of exploration vs exploitation
    
    // Advanced sensory processing
    void enableSensoryFiltering(bool enable) { sensoryFilteringEnabled_ = enable; }
    void setSensoryThreshold(float threshold) { sensoryThreshold_ = threshold; }
    bool isSensoryFilteringEnabled() const { return sensoryFilteringEnabled_; }
    float getSensoryThreshold() const { return sensoryThreshold_; }
    
    // Neuromodulation enhancement
    void setDopamineSensitivity(float sensitivity) { dopamineSensitivity_ = sensitivity; }
    void setCuriosityScaling(float scaling) { curiosityScaling_ = scaling; }
    float getDopamineSensitivity() const { return dopamineSensitivity_; }
    float getCuriosityScaling() const { return curiosityScaling_; }
    
    // Development system enhancement
    void setDevelopmentalSequence(const std::vector<DevelopmentalStage>& stages);
    void setPlasticitySchedule(const std::vector<std::pair<double, float>>& schedule);
    
    // Memory integration
    void enableMemoryIntegration(bool enable) { memoryIntegrationEnabled_ = enable; }
    bool isMemoryIntegrationEnabled() const { return memoryIntegrationEnabled_; }
    
    // Advanced learning
    void enableTransferLearning(bool enable) { transferLearningEnabled_ = enable; }
    void enableContinualLearning(bool enable) { continualLearningEnabled_ = enable; }
    bool isTransferLearningEnabled() const { return transferLearningEnabled_; }
    bool isContinualLearningEnabled() const { return continualLearningEnabled_; }
    
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
};

} // namespace nlm
