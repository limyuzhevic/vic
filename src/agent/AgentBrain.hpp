// AgentBrain helper system - separates agent functionality from main brain implementation
// This system provides interface between the neural brain and decision-making agent

#pragma once

#include "../core/Constants.hpp"
#include "../core/ErrorHandling/ErrorHandling.hpp"
#include <vector>
#include <map>
#include <memory>
#include <string>

namespace nlm {

// Agent brain subsystems that handle different aspects of agent behavior
struct AgentSensorySystem {
    // Handles sensory input processing and integration
    // Manages vision, touch, internal signals, and proprioception
    
    AgentSensorySystem() : visionNeurons(), touchNeurons(), internalNeurons(), proprioceptionNeurons() {}
    
    // Vision processing
    std::vector<class Neuron*> visionNeurons;
    std::vector<float> previousVision;
    float noveltyLevel;
    float noveltyDecayRate;
    
    // Touch processing  
    std::vector<class Neuron*> touchNeurons;
    
    // Internal signal processing
    std::vector<class Neuron*> internalNeurons;
    
    // Proprioception processing
    std::vector<class Neuron*> proprioceptionNeurons;
    
    // Process sensory input from perception data
    // @param vision - visual input array
    // @param touch - touch input array  
    // @param internal - internal signal array
    // @param proprioception - proprioception input array
    void processSensoryInput(const std::vector<float>& vision,
                           const std::vector<float>& touch,
                           const std::vector<float>& internal,
                           const std::vector<float>& proprioception);
    
    // Calculate novelty between current and previous vision
    float calculateNovelty(const std::vector<float>& currentVision);
    
    // Update novelty with decay factor
    void updateNovelty();
    
    // Get processed sensory values for output
    std::vector<float> getSensoryValues() const;
};

struct AgentMotorSystem {
    // Handles motor output generation and action selection
    // Manages different action groups (forward, backward, turn, etc.)
    
    AgentMotorSystem() : motorGroups(), activityThreshold(0.0f) {}
    
    // Motor neuron groups for different actions
    std::vector<class Neuron*> motorForward;
    std::vector<class Neuron*> motorBackward;
    std::vector<class Neuron*> motorTurnLeft;
    std::vector<class Neuron*> motorTurnRight;
    std::vector<class Neuron*> motorInteract;
    std::vector<class Neuron*> motorWait;
    
    // Motor group mappings
    std::map<MotorCommand, std::vector<class Neuron*>> motorGroups;
    
    // Activity threshold for action selection
    float activityThreshold;
    
    // Calculate activity level for each motor group
    // @param neurons - vector of neurons in group
    // @return average activity level
    float calculateActivity(const std::vector<class Neuron*>& neurons) const;
    
    // Select action based on motor neuron activity
    // @param motorGroups - groups of neurons for each action
    // @return selected motor command
    MotorCommand selectAction(const std::map<MotorCommand, std::vector<class Neuron*>>& motorGroups);
    
    // Apply curiosity-based exploration
    // @param defaultCommand - base action selection
    // @param curiosityLevel - current curiosity (0-1)
    // @param rng - random number generator
    // @return explored action
    MotorCommand applyExploration(MotorCommand defaultCommand, float curiosityLevel, class RandomGenerator* rng);
    
    // Get available motor commands
    std::vector<MotorCommand> getAvailableCommands() const;
};

struct AgentLearningSystem {
    // Handles reward-based learning and plasticity modulation
    // Manages prediction error, expected reward, and dopamine effects
    
    AgentLearningSystem() : 
        dopamineLevel(0.0f),
        predictionError(0.0f),
        expectedReward(0.0f),
        rewardModulationEnabled(true),
        plasticityModifier(1.0f) {}
    
    // Neuromodulation levels
    float dopamineLevel;
    float predictionError;
    float expectedReward;
    
    // Learning system controls
    bool rewardModulationEnabled;
    float plasticityModifier;
    
    // Apply reward modulation based on prediction error
    // @param reward - received reward
    // @param predictedReward - expected reward
    void applyRewardModulation(float reward, float predictedReward);
    
    // Update expected reward using exponential moving average
    // @param reward - current reward
    void updateExpectedReward(float reward);
    
    // Calculate prediction error (reward - expected)
    // @param reward - received reward
    // @param predicted - expected reward
    // @return prediction error
    float calculatePredictionError(float reward, float predicted);
    
    // Modulate plasticity based on dopamine level
    // @param dopamine - current dopamine level (-1 to 1)
    // @param stdp - STDP system for plasticity modulation
    void modulatePlasticity(float dopamine, class STDP* stdp);
    
    // Get current neuromodulation level
    // @return dopamine level
    float getNeuromodulationLevel() const;
};

struct AgentDevelopmentSystem {
    // Handles developmental stages and changing plasticity
    // Manages learning through different developmental phases
    
    AgentDevelopmentSystem() : 
        developmentalAge(0.0),
        developmentalStage(DevelopmentalStage::Initial),
        developmentEnabled(true) {}
    
    // Developmental timing
    double developmentalAge;  // Simulation time (seconds)
    DevelopmentalStage developmentalStage;
    bool developmentEnabled;
    
    // Update developmental stage based on age
    // @param timestep - time elapsed
    // @param brain - brain reference for stage setting
    void updateDevelopmentalStage(double timestep, class Brain* brain);
    
    // Get current developmental stage
    // @return current stage
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Check if agent is in critical period for learning
    // @return true if in critical period
    bool isCriticalPeriod() const;
    
    // Calculate plasticity modifier based on developmental stage
    // @return plasticity modifier (1.0 = high, 0.2 = stable)
    float calculatePlasticityModifier() const;
};

class AgentBrain {
public:
    // Main agent-brain interface combining all subsystems
    AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
    // Initialize agent with world interface
    // @param world - simulation world
    void initialize(const SimpleWorld& world);
    
    // Process sensory input and update agent state
    // @param percept - sensory perception data
    void processSensoryInput(const SensoryPercept& percept);
    
    // Decode motor command based on current neural activity
    // @return selected motor command
    MotorCommand decodeMotorCommand();
    
    // Apply reward signal for learning
    // @param reward - received reward
    // @param predictedReward - expected reward
    void applyRewardModulation(float reward, float predictedReward);
    
    // Update developmental state
    // @param timestep - time elapsed
    void updateDevelopment(double timestep);
    
    // Get current developmental stage
    // @return developmental stage
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Get current neuromodulation level
    // @return dopamine level
    float getNeuromodulationLevel() const;
    
    // Get curiosity level
    // @return curiosity level (0-1)
    float getCuriosityLevel() const;
    
    // Get novelty level
    // @return novelty level (0-1)
    float getNoveltyLevel() const;
    
    // Get prediction error
    // @return prediction error
    float getPredictionError() const;
    
    // Reset agent state
    void reset();
    
    // Get sensory input size required
    // @return number of expected sensory inputs
    size_t getSensoryInputSize() const;
    
    // Get motor output size required  
    // @return number of possible motor outputs
    size_t getMotorOutputSize() const;
    
private:
    // Core brain reference
    std::shared_ptr<Brain> brain_;
    
    // Agent subsystems
    AgentSensorySystem sensorySystem;
    AgentMotorSystem motorSystem;
    AgentLearningSystem learningSystem;
    AgentDevelopmentSystem developmentSystem;
    
    // Debug and status logging
    void logStatus() const;
};

} // namespace nlm
