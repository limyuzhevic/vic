#pragma once

#include "SensoryPercept.hpp"
#include "../prediction/PredictionSystem.hpp"
#include <vector>
#include <memory>

namespace nlm {

class Brain;
class PredictionSystem;

class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain() = default;
    
    // Disable copying, enable moving
    AgentBrain(const AgentBrain&) = delete;
    AgentBrain& operator=(const AgentBrain&) = delete;
    AgentBrain(AgentBrain&&) noexcept;
    AgentBrain& operator=(AgentBrain&&) noexcept;
    
    // Initialize agent with world
    void initialize(const SimpleWorld& world);
    
    // Process sensory input
    void processSensoryInput(const SensoryPercept& percept);
    
    // Select action based on current state
    ActionType selectAction(const std::vector<float>& state,
                          float explorationRate = 0.1f);
    
    // Evaluate state using value function
    float evaluateState(const std::vector<float>& state);
    
    // Decode motor command from neural activity
    MotorCommand decodeMotorCommand();
    
    // Motor neuron groups
    MotorCommand decodeFromMotorNeurons();
    
    // Curiosity-based exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Reward-based learning and neuromodulation
    void applyRewardModulation(float reward, float predictedReward);
    
    // Development and plasticity
    void updateDevelopment(double timestep);
    
    // Getters
    float getNeuromodulationLevel() const;
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getPredictionError() const;
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Reset agent state
    void reset();
    
    // Prediction system access
    PredictionSystem* getPredictionSystem() { return predictionSystem_.get(); }
    void setPredictionSystemEnabled(bool enabled) { predictionSystemEnabled_ = enabled; }
    void setPolicyNetworkEnabled(bool enabled) { policyNetworkEnabled_ = enabled; }
    void setValueNetworkEnabled(bool enabled) { valueNetworkEnabled_ = enabled; }
    void setForwardModelEnabled(bool enabled) { forwardModelEnabled_ = enabled; }
    void setInverseModelEnabled(bool enabled) { inverseModelEnabled_ = enabled; }
    void setCuriosityExplorationEnabled(bool enabled) { curiosityExplorationEnabled_ = enabled; }
    void setRewardPredictionEnabled(bool enabled) { rewardPredictionEnabled_ = enabled; }
    
    // Configuration
    void setLearningRate(float rate) { learningRate_ = rate; }
    void setExplorationRate(float rate) { explorationRate_ = rate; }
    
    // Input/output sizes
    size_t getSensoryInputSize() const;
    size_t getMotorOutputSize() const;
    
private:
    // Core brain
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
    
    // Neuromodulation
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    
    // Development
    double developmentalAge_;
    float plasticityModifier_;
    
    // Configuration
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    bool sensoryNoveltyDecay_;
    
    // Prediction system components
    bool predictionSystemEnabled_;
    bool policyNetworkEnabled_;
    bool valueNetworkEnabled_;
    bool forwardModelEnabled_;
    bool inverseModelEnabled_;
    bool curiosityExplorationEnabled_;
    bool rewardPredictionEnabled_;
    float learningRate_;
    float explorationRate_;
    
    std::unique_ptr<PredictionSystem> predictionSystem_;
    
    // State tracking
    std::vector<float> previousVision_;
    
    // Helper methods
    std::vector<float> perceptToState(const SensoryPercept& percept) const;
    
    // Initialize prediction systems
    void initializePredictionSystems();
    
    // Get greedy action (for fallback)
    ActionType getGreedyAction(const std::vector<float>& state) const;
    
    // Get random number
    float getRandomFloat(float min, float max) const;
    int getRandomInt(int min, int max) const;
};

} // namespace nlm