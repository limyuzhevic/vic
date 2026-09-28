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
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    std::shared_ptr<Brain> brain_;
    
    // Motor neuron groups with enhanced organization
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Sensory neuron groups with modality-based organization
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
    
    // Helper methods for neuron organization
    void organizeMotorNeurons(std::shared_ptr<NeuralPopulation> pop, NeuronType type);
    void organizeSensoryNeurons(std::shared_ptr<NeuralPopulation> pop, NeuronType type);
    
    // Motor neuron activity calculation with improved sensitivity
    float calculateMotorActivity(const std::vector<Neuron*>& neurons) const;
    
    // Enhanced motor command selection with multiple strategies
    MotorCommand decodeWithStrategy(MotorCommand defaultCmd, float curiosityFactor);
    
    // Action preference system for learned behavior patterns
    void updateActionPreferences(const MotorCommand& action, float reward);
    float getActionPreference(const MotorCommand& action) const;
    
    // Goal-directed action selection
    MotorCommand selectWithGoals(const std::vector<MotorCommand>& possibleActions, const std::vector<float>& actionValues);
    
    // Exploration-exploitation balance
    MotorCommand balanceExploration(const MotorCommand& bestAction, float explorationRate);
    
    // Statistical tracking for motor command effectiveness
    void updateMotorCommandStatistics(const MotorCommand& action, float reward, float timeStep);
    
    // Advanced motor command filtering and smoothing
    MotorCommand applyMotorCommandFilter(const MotorCommand& rawCommand, float timeStep);
    
    // Multi-modal sensory integration for motor control
    void integrateSensoryInputs(const std::vector<float>& vision, const std::vector<float>& touch, 
                              const std::vector<float>& internal, const std::vector<float>& proprioception);
    
    // Motor command validation and constraint checking
    bool isValidMotorCommand(const MotorCommand& action) const;
    
    // Motor command symmetry and consistency checks
    bool maintainsPosture(const MotorCommand& action, const AgentBody& bodyState) const;
    
    // Fatigue and stamina effects on motor commands
    MotorCommand applyFatigueEffects(const MotorCommand& action, float fatigueLevel);
    
    // Priority-based motor command selection
    MotorCommand selectWithPriority(const std::vector<MotorCommand>& candidates, 
                                   const std::vector<float>& priorities) const;
    
    // Motor command prediction for action planning
    std::vector<MotorCommand> predictActions(const std::vector<float>& sensoryState, int lookaheadSteps) const;
    
    // Motor command adaptation and learning
    void adaptMotorPattern(const MotorCommand& action, float predictionError, float learningRate);
    
    // Motor command conflict resolution
    MotorCommand resolveCommandConflicts(const std::vector<MotorCommand>& competingActions) const;
    
    // Motor command context awareness
    bool isActionAppropriateForContext(const MotorCommand& action, const std::string& context) const;
    
    // Motor command hysteresis for stability
    MotorCommand applyHysteresis(const MotorCommand& currentAction, const MotorCommand& previousAction,
                                float threshold) const;
};

} // namespace nlm
