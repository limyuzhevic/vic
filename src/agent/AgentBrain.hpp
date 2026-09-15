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
    
    // Direct access to brain neuromodulation systems
    Dopamine* getBrainDopamine() { return brain_ ? brain_->getDopamine() : nullptr; }
    Curiosity* getBrainCuriosity() { return brain_ ? brain_->getCuriosity() : nullptr; }
    Novelty* getBrainNovelty() { return brain_ ? brain_->getNovelty() : nullptr; }
    PredictionError* getBrainPredictionError() { return brain_ ? brain_->getPredictionErrorSignal() : nullptr; }
    
    WorkingMemory* getBrainWorkingMemory() { return brain_ ? brain_->getWorkingMemory() : nullptr; }
    EpisodicMemory* getBrainEpisodicMemory() { return brain_ ? brain_->getEpisodicMemory() : nullptr; }
    AssociativeMemory* getBrainAssociativeMemory() { return brain_ ? brain_->getAssociativeMemory() : nullptr; }
    
    PredictionSystem* getBrainPredictionSystem() { return brain_ ? brain_->getPredictionSystem() : nullptr; }
    NeuralPlanner* getBrainPlanner() { return brain_ ? brain_->getPlanner() : nullptr; }
    ConceptFormation* getBrainConceptFormation() { return brain_ ? brain_->getConceptFormation() : nullptr; }
    AttentionalSelection* getBrainAttention() { return brain_ ? brain_->getAttention() : nullptr; }
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
};

} // namespace nlm
