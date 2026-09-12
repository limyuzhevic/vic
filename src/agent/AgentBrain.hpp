#pragma once

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <array>
#include "../brain/Neuron.hpp"
#include "../world/SimpleWorld.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"

namespace nlm {

class Brain;
class SimpleWorld;

class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
    // Delete copying, enable moving
    AgentBrain(const AgentBrain&) = delete;
    AgentBrain& operator=(const AgentBrain&) = delete;
    AgentBrain(AgentBrain&& other) noexcept;
    AgentBrain& operator=(AgentBrain&& other) noexcept;
    
    // Initialization
    void initialize(const SimpleWorld& world);
    
    // Processing
    void processSensoryInput(const SensoryPercept& percept);
    MotorCommand decodeMotorCommand();
    
    // Motor neuron groups
    std::vector<Neuron*>& getMotorForward() { return motorForward_; }
    std::vector<Neuron*>& getMotorBackward() { return motorBackward_; }
    std::vector<Neuron*>& getMotorTurnLeft() { return motorTurnLeft_; }
    std::vector<Neuron*>& getMotorTurnRight() { return motorTurnRight_; }
    std::vector<Neuron*>& getMotorInteract() { return motorInteract_; }
    std::vector<Neuron*>& getMotorWait() { return motorWait_; }
    
    std::vector<Neuron*>& getSensoryVision() { return sensoryVision_; }
    std::vector<Neuron*>& getSensoryTouch() { return sensoryTouch_; }
    std::vector<Neuron*>& getSensoryInternal() { return sensoryInternal_; }
    std::vector<Neuron*>& getSensoryProprioception() { return sensoryProprioception_; }
    
    // Neuromodulation
    void applyRewardModulation(float reward, float predictedReward);
    void updateDevelopment(double timestep);
    
    // State access
    float getNeuromodulationLevel() const;
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getPredictionError() const;
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Configuration
    void enableRewardModulation(bool enable);
    void enableStructuralPlasticity(bool enable);
    void enableDevelopment(bool enable);
    void enableCuriosity(bool enable);
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // Reset
    void reset();
    
    // Accessors
    std::shared_ptr<Brain> getBrain() { return brain_; }
    
private:
    // Helper methods
    MotorCommand decodeFromMotorNeurons();
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Internal data
    std::shared_ptr<Brain> brain_;
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
    
    // Neuromodulation state
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    double developmentalAge_;
    float plasticityModifier_;
    
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    
    // Sensory processing
    float sensoryNoveltyDecay_;
    std::vector<float> previousVision_;
};

} // namespace nlm