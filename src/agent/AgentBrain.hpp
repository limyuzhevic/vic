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
    
    // Get underlying brain (advanced usage)
    Brain* getBrain() { return brain_.get(); }
    
    // Memory system access (advanced usage)
    NeuralWorkingMemory* getWorkingMemory() { return brain_ ? brain_->getWorkingMemory() : nullptr; }
    NeuralEpisodicMemory* getEpisodicMemory() { return brain_ ? brain_->getEpisodicMemory() : nullptr; }
    NeuralAssociativeMemory* getAssociativeMemory() { return brain_ ? brain_->getAssociativeMemory() : nullptr; }
    
    // Prediction system access (advanced usage)
    PredictionSystem* getPredictionSystem() { return brain_ ? brain_->getPredictionSystem() : nullptr; }
    
    // Neuromodulator access (advanced usage)
    Neuromodulator* getNeuromodulator(NeuromodulatorType type);
    Dopamine* getDopamine() { return brain_ ? brain_->getDopamine() : nullptr; }
    Novelty* getNovelty() { return brain_ ? brain_->getNovelty() : nullptr; }
    Curiosity* getCuriosity() { return brain_ ? brain_->getCuriosity() : nullptr; }
    PredictionError* getPredictionErrorSignal() { return brain_ ? brain_->getPredictionErrorSignal() : nullptr; }
    
    // Prediction-based action selection (enhanced)
    MotorCommand decodeMotorCommandWithPrediction();
    
    // Get prediction error for learning
    float getPredictionErrorSignal() const { return predictionError_; }
    
    // Enable prediction-based action selection
    void enablePredictionBasedSelection(bool enable) { predictionBasedSelectionEnabled_ = enable; }
    
    // Check if prediction-based selection is enabled
    bool isPredictionBasedSelectionEnabled() const { return predictionBasedSelectionEnabled_; }
    
    // Set prediction horizon for planning
    void setPredictionHorizon(int horizon) { predictionHorizon_ = horizon; }
    int getPredictionHorizon() const { return predictionHorizon_; }
    
    // Get action value estimates (Q-values for actions)
    std::vector<float> getActionValueEstimates() const;
    
    // Get expected rewards for actions
    std::vector<float> getExpectedRewards() const;
    
    // Get exploration bonus based on prediction uncertainty
    float getExplorationBonus() const;
    
    // Get action diversity (how different actions are from expected)
    float getActionDiversity() const;

    // Development system access (advanced usage)
    DevelopmentSystem* getDevelopmentSystem() { return brain_ ? brain_->getDevelopmentSystem() : nullptr; }
    
    // Visualization interface (advanced usage)
    VisualizationInterface* getVisualizer() { return brain_ ? brain_->getVisualizationInterface() : nullptr; }
    
    // Statistics and monitoring (advanced usage)
    std::string getBrainStats() const;
    std::string getMemoryStats() const;
    std::string getPredictionStats() const;
    std::string getNeuromodulationStats() const;
    std::string getDevelopmentStats() const;
    std::string getWorkingMemoryStats() const;
    std::string getEpisodicMemoryStats() const;
    std::string getAssociativeMemoryStats() const;
    
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
