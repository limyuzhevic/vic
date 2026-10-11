#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include <memory>
#include <vector>

namespace nlm {

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
    // Disable copying, enable moving
    AgentBrain(const AgentBrain&) = delete;
    AgentBrain& operator=(const AgentBrain&) = delete;
    AgentBrain(AgentBrain&&) noexcept;
    AgentBrain& operator=(AgentBrain&&) noexcept;
    
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
    
    // Get sensory and motor neuron groups
    const std::vector<Neuron*>& getSensoryVision() const { return sensoryVision_; }
    const std::vector<Neuron*>& getSensoryTouch() const { return sensoryTouch_; }
    const std::vector<Neuron*>& getSensoryInternal() const { return sensoryInternal_; }
    const std::vector<Neuron*>& getSensoryProprioception() const { return sensoryProprioception_; }
    const std::vector<Neuron*>& getMotorForward() const { return motorForward_; }
    const std::vector<Neuron*>& getMotorBackward() const { return motorBackward_; }
    const std::vector<Neuron*>& getMotorTurnLeft() const { return motorTurnLeft_; }
    const std::vector<Neuron*>& getMotorTurnRight() const { return motorTurnRight_; }
    const std::vector<Neuron*>& getMotorInteract() const { return motorInteract_; }
    const std::vector<Neuron*>& getMotorWait() const { return motorWait_; }
    
    // Advanced cognitive integration methods (Phase 6)
    void processWithWorkingMemory(const std::vector<NeuronId>& activeNeurons);
    void updateWithEpisodicMemory();
    void updateWithPredictionSystem();
    void updateWithPlanning();
    void updateWithConceptFormation();
    void updateWithAttention();
    
    // Configuration for cognitive integration
    void enableCuriosity(bool enabled) { curiosityEnabled_ = enabled; }
    void enableDevelopment(bool enabled) { developmentEnabled_ = enabled; }
    void enableRewardModulation(bool enabled) { rewardModulationEnabled_ = enabled; }
    void enableStructuralPlasticity(bool enabled) { structuralPlasticityEnabled_ = enabled; }
    
    // Memory and cognition access (for advanced integration)
    NeuralWorkingMemory* getWorkingMemory() { return brain_ ? brain_->getWorkingMemory() : nullptr; }
    NeuralEpisodicMemory* getEpisodicMemory() { return brain_ ? brain_->getEpisodicMemory() : nullptr; }
    NeuralAssociativeMemory* getAssociativeMemory() { return brain_ ? brain_->getAssociativeMemory() : nullptr; }
    PredictionSystem* getPredictionSystem() { return brain_ ? brain_->getPredictionSystem() : nullptr; }
    NeuralPlanner* getPlanner() { return brain_ ? brain_->getPlanner() : nullptr; }
    ConceptFormation* getConceptFormation() { return brain_ ? brain_->getConceptFormation() : nullptr; }
    AttentionalSelection* getAttention() { return brain_ ? brain_->getAttention() : nullptr; }
    
    // Neuromodulators
    Dopamine* getDopamine() { return brain_ ? brain_->getDopamine() : nullptr; }
    Curiosity* getCuriosity() { return brain_ ? brain_->getCuriosity() : nullptr; }
    Novelty* getNovelty() { return brain_ ? brain_->getNovelty() : nullptr; }
    PredictionError* getPredictionError() { return brain_ ? brain_->getPredictionErrorSignal() : nullptr; }
    
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