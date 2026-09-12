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
    
    // Get expected reward
    float getExpectedReward() const;
    
    // Get plasticity modifier
    float getPlasticityModifier() const;
    
    // Get developmental age
    float getDevelopmentalAge() const;
    
    // Get previous vision state
    const std::vector<float>& getPreviousVision() const;
    
    // Get sensory novelty decay
    float getSensoryNoveltyDecay() const;
    
    // Set neuromodulation state
    void setNeuromodulationState(float dopamine, float novelty, float curiosity, float predictionError, float expectedReward);
    
    // Set development state
    void setDevelopmentState(float age, float plasticity);
    
    // Get motor neuron groups
    const std::vector<Neuron*>& getMotorForwardNeurons() const;
    const std::vector<Neuron*>& getMotorBackwardNeurons() const;
    const std::vector<Neuron*>& getMotorTurnLeftNeurons() const;
    const std::vector<Neuron*>& getMotorTurnRightNeurons() const;
    const std::vector<Neuron*>& getMotorInteractNeurons() const;
    const std::vector<Neuron*>& getMotorWaitNeurons() const;
    
    // Get sensory neuron groups
    const std::vector<Neuron*>& getSensoryVisionNeurons() const;
    const std::vector<Neuron*>& getSensoryTouchNeurons() const;
    const std::vector<Neuron*>& getSensoryInternalNeurons() const;
    const std::vector<Neuron*>& getSensoryProprioceptionNeurons() const;
    
    // Batch process multiple sensory inputs
    void batchProcessSensoryInputs(const std::vector<SensoryPercept>& percepts);
    
    // Get comprehensive statistics
    std::map<std::string, double> getStatistics() const;
    
    // Print neural architecture (for debugging)
    void printNeuralArchitecture() const;
    
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
    float developmentalAge_;
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
