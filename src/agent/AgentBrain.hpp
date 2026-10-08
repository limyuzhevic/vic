#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "NeuromodulatorManager.h"
#include <memory>
#include <vector>

namespace nlm {

// Forward declarations
class NeuromodulatorManager;

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
    void enableRewardModulation(bool enable) { neuromodulatorManager_->enableRewardModulation(enable); }
    void enableStructuralPlasticity(bool enable) { neuromodulatorManager_->enableStructuralPlasticity(enable); }
    void enableDevelopment(bool enable) { neuromodulatorManager_->enableDevelopment(enable); }
    void enableCuriosity(bool enable) { neuromodulatorManager_->enableCuriosity(enable); }
    
    bool isRewardModulationEnabled() const { return neuromodulatorManager_->isRewardModulationEnabled(); }
    bool isStructuralPlasticityEnabled() const { return neuromodulatorManager_->isStructuralPlasticityEnabled(); }
    bool isDevelopmentEnabled() const { return neuromodulatorManager_->isDevelopmentEnabled(); }
    bool isCuriosityEnabled() const { return neuromodulatorManager_->isCuriosityEnabled(); }
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Set developmental stage based on age (delegates to manager)
    void setDevelopmentalStageFromAge(double age) {
        neuromodulatorManager_->setDevelopmentalStageFromAge(age);
    }
    
    // Helper method for developmental stage calculation
    void updateDevelopmentalStage(double age) {
        developmentalAge_ = age;
    }
    
    std::shared_ptr<Brain> brain_;
    std::unique_ptr<NeuromodulatorManager> neuromodulatorManager_;
    
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
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // Development state (kept for backward compatibility)
    double developmentalAge_;
};

} // namespace nlm

