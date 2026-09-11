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
    
// Helper functions for Neuron distribution
    void distributeMotorNeurons();
    void distributeSensoryNeurons();
    
    // Configuration for sensory scaling
    void setVisionScale(float scale) { visionScale_ = scale; }
    void setTouchScale(float scale) { touchScale_ = scale; }
    void setInternalScale(float scale) { internalScale_ = scale; }
    void setProprioceptionScale(float scale) { proprioceptionScale_ = scale; }
    
    // Configuration for curiosity/exploration
    void setCuriosityThreshold(float threshold) { curiosityThreshold_ = threshold; }
    void setExplorationRate(float rate) { explorationRate_ = rate; }
    
    // Configuration for novelty detection
    void setNoveltyThreshold(float threshold) { noveltyThreshold_ = threshold; }
    void setNoveltyDecay(float decay) { noveltyDecay_ = decay; }
    
    // Get current configuration values
    float getVisionScale() const { return visionScale_; }
    float getTouchScale() const { return touchScale_; }
    float getInternalScale() const { return internalScale_; }
    float getProprioceptionScale() const { return proprioceptionScale_; }
    float getCuriosityThreshold() const { return curiosityThreshold_; }
    float getExplorationRate() const { return explorationRate_; }
    float getNoveltyThreshold() const { return noveltyThreshold_; }
    float getNoveltyDecay() const { return noveltyDecay_; }
    
    // Statistics and debugging
    size_t getMotorForwardCount() const { return motorForward_.size(); }
    size_t getMotorBackwardCount() const { return motorBackward_.size(); }
    size_t getMotorTurnLeftCount() const { return motorTurnLeft_.size(); }
    size_t getMotorTurnRightCount() const { return motorTurnRight_.size(); }
    size_t getMotorInteractCount() const { return motorInteract_.size(); }
    size_t getMotorWaitCount() const { return motorWait_.size(); }
    
    size_t getVisionSensoryCount() const { return sensoryVision_.size(); }
    size_t getTouchSensoryCount() const { return sensoryTouch_.size(); }
    size_t getInternalSensoryCount() const { return sensoryInternal_.size(); }
    size_t getProprioceptionSensoryCount() const { return sensoryProprioception_.size(); }
    
    // Validation methods
    bool validateNeuronPointers() const;
    bool validateBrainConnectivity() const;
    
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
