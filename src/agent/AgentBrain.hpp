#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../motor/MotorSystem.hpp"
#include <memory>
#include <vector>

namespace nlm {

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding with real dynamics
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
    // Initialize with world and motor system
    void initialize(const SimpleWorld& world, MotorSystem* motorSystem = nullptr);
    
    // Get sensory input size expected by brain
    size_t getSensoryInputSize() const;
    
    // Get motor output size expected
    size_t getMotorOutputSize() const;
    
    // Process sensory percept and inject into brain
    void processSensoryInput(const SensoryPercept& percept);
    
    // Decode brain motor activity into motor command with real dynamics
    MotorCommand decodeMotorCommand();
    
    // Advanced motor decoding with neural integration
    std::unique_ptr<Action> decodeActionCommand();
    MotorCommand decodeMotorCommandWithLearning();
    
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
    const Brain* getBrain() const { return brain_.get(); }
    
    // Get motor system
    MotorSystem* getMotorSystem() { return motorSystem_.get(); }
    const MotorSystem* getMotorSystem() const { return motorSystem_.get(); }
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    void enableMotorLearning(bool enable) { motorLearningEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    bool isMotorLearningEnabled() const { return motorLearningEnabled_; }
    
    // Motor control statistics
    size_t getTotalActionsExecuted() const;
    float getAverageActionSuccessRate() const;
    float getMotorFatigueLevel() const;
    float getMotorPerformanceLevel() const;
    
    // Action sequence and planning
    std::unique_ptr<Action> planActionSequence(const std::vector<ActionType>& sequence);
    std::unique_ptr<Action> adaptActionToContext(const Action& baseAction, 
                                                const std::vector<float>& context);
    
    // Motor learning and adaptation
    void learnFromActionOutcome(const Action& action, float reward, bool success);
    void updateMotorAdaptation(float adaptationDelta);
    void adjustMotorPreferences(const std::vector<ActionType>& actions, 
                               const std::vector<float>& outcomes);
    
    // Biological realism integration
    void updateArousal(float baseline, float sensoryInput);
    void updateNeuromodulation(float dopamine, float novelty, float curiosity);
    void updateMotorFatigue(float fatigueRate, double dt);
    
    // Advanced motor control methods
    std::unique_ptr<Action> selectActionWithCostBenefit(const NeuralRegion& motorRegion);
    std::unique_ptr<Action> selectActionWithCuriosity(MotorCommand defaultCmd);
    std::unique_ptr<Action> selectActionWithExploration(const NeuralRegion& motorRegion);
    
    // Motor command execution and monitoring
    void executeMotorCommand(const MotorCommand& cmd, double currentTime);
    void monitorMotorPerformance();
    void emergencyStopMotorSystem();
    
    // State management
    bool isMotorSystemActive() const { return motorSystemActive_; }
    void activateMotorSystem(bool active) { motorSystemActive_ = active; }
    
    // Development and plasticity
    void applyMotorDevelopmentChanges();
    void updateMotorPlasticity(double timestep);
    
private:
    // Core systems
    std::shared_ptr<Brain> brain_;
    std::unique_ptr<MotorSystem> motorSystem_;
    
    // Motor neuron groups for direct access
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
    float arousalLevel_;
    
    // Development state
    double developmentalAge_;
    float plasticityModifier_;
    float motorLearningRate_;
    
    // Motor state
    float fatigueFactor_;
    float motorPerformanceLevel_;
    
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    bool motorLearningEnabled_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // Execution state
    bool motorSystemActive_;
    size_t actionsExecuted_;
    float averageActionSuccessRate_;
    
    // Action history for learning
    std::vector<const Action*> actionHistory_;
    
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Advanced motor decoding methods
    std::unique_ptr<Action> decodeActionFromNeuralActivity(const NeuralRegion& motorRegion);
    MotorCommand decodeMotorFromNeuralPopulation(const NeuralRegion& motorRegion);
    
    // Motor learning integration
    void integrateMotorLearningIntoActionSelection();
    void updateMotorBasedOnOutcome(const Action& action, float reward, bool success);
    
    // Performance monitoring
    void updateMotorPerformanceStatistics();
    void adjustMotorParametersBasedOnPerformance();
    
    // Biological realism
    void updateBiologicalMotorState(double timestep);
    void applyNeuromodulationToMotorSystem();
};

} // namespace nlm
