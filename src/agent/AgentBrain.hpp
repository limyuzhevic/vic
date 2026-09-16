#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "MotorDecoder.hpp"
#include "SensoryProcessor.hpp"
#include "NeuromodulationManager.hpp"
#include "DevelopmentManager.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>

namespace nlm {

// AgentBrain: Connects NLM brain to the world
// Orchestrates sensory processing, motor decoding, neuromodulation, and development
// Now uses separate components for better modularity and testability
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
    void enableRewardModulation(bool enable) { neuromodulationManager_.enableRewardModulation(enable); }
    void enableStructuralPlasticity(bool enable) { developmentManager_.enableStructuralPlasticity(enable); }
    void enableDevelopment(bool enable) { developmentManager_.enableDevelopment(enable); }
    void enableCuriosity(bool enable) { neuromodulationManager_.enableCuriosityDriven(enable); }
    
    bool isRewardModulationEnabled() const { return neuromodulationManager_.isRewardModulationEnabled(); }
    bool isStructuralPlasticityEnabled() const { return developmentManager_.isStructuralPlasticityEnabled(); }
    bool isDevelopmentEnabled() const { return developmentManager_.isDevelopmentEnabled(); }
    bool isCuriosityEnabled() const { return neuromodulationManager_.isCuriosityDrivenEnabled(); }
    
    // Component access for advanced use
    MotorDecoder& getMotorDecoder() { return motorDecoder_; }
    SensoryProcessor& getSensoryProcessor() { return sensoryProcessor_; }
    NeuromodulationManager& getNeuromodulationManager() { return neuromodulationManager_; }
    DevelopmentManager& getDevelopmentManager() { return developmentManager_; }
    
private:
    // Validate brain pointer
    bool validateBrain() const;
    
    // Initialize motor and sensory neuron groups
    void initializeMotorAndSensoryGroups();
    
    std::shared_ptr<Brain> brain_;
    
    // Components
    MotorDecoder motorDecoder_;
    SensoryProcessor sensoryProcessor_;
    NeuromodulationManager neuromodulationManager_;
    DevelopmentManager developmentManager_;
    
    // Legacy neuron groups (maintained for backward compatibility)
    // These are populated during initialization for compatibility
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    // Legacy state variables (synchronized with component managers)
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    double developmentalAge_;
    float plasticityModifier_;
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
};

} // namespace nlm