#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "MotorDistribution.hpp"
#include "MotorDecoder.hpp"
#include "SensoryProcessor.hpp"
#include "CuriosityManager.hpp"
#include "RewardModulator.hpp"
#include "DevelopmentalController.hpp"
#include "AgentStateManager.hpp"
#include <memory>
#include <vector>

namespace nlm {

// Forward declaration
enum class MotorCommand;

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
    void enableRewardModulation(bool enable) { rewardModulator_.setRewardModulationEnabled(enable); }
    void enableStructuralPlasticity(bool enable) { stateManager_.setStructuralPlasticityEnabled(enable); }
    void enableDevelopment(bool enable) { developmentalController_.setDevelopmentEnabled(enable); }
    void enableCuriosity(bool enable) { curiosityManager_.setCuriosityEnabled(enable); }
    
    bool isRewardModulationEnabled() const { return rewardModulator_.isRewardModulationEnabled(); }
    bool isStructuralPlasticityEnabled() const { return stateManager_.isStructuralPlasticityEnabled(); }
    bool isDevelopmentEnabled() const { return developmentalController_.isDevelopmentEnabled(); }
    bool isCuriosityEnabled() const { return curiosityManager_.isCuriosityEnabled(); }
    
private:
    // Component subsystems
    std::shared_ptr<Brain> brain_;
    MotorDistribution motorDistribution_;
    MotorDecoder motorDecoder_;
    SensoryProcessor sensoryProcessor_;
    CuriosityManager curiosityManager_;
    RewardModulator rewardModulator_;
    DevelopmentalController developmentalController_;
    AgentStateManager stateManager_;
    
    // Motor neuron groups (for direct access from components)
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Sensory neuron groups (for direct access from components)
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    // Helper methods for component coordination
    void updateComponentNeuronalGroups();
    void synchronizeStateWithComponents();
};

} // namespace nlm
