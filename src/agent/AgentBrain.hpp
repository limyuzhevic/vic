#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../core/Types/Types.hpp"
#include <memory>
#include <vector>
#include <unordered_map>
#include <deque>

namespace nlm {

// Forward declarations
class RandomGenerator;

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
// EXTENDED: Now includes prediction-driven action selection
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
    
    // Update action selection strategy
    void updateActionSelectionStrategy();
    
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
    
    // Record action outcome for learning
    void recordActionOutcome(MotorCommand action, const std::vector<float>& beforeState,
                           const std::vector<float>& afterState, float reward);
    
    // Get predicted action for current state
    MotorCommand getPredictedAction(const std::vector<float>& currentState);
    
    // Enable/disable prediction-driven action selection
    void enablePredictionDrivenActionSelection(bool enable);
    bool isPredictionDrivenActionSelectionEnabled() const;
    
    // Clear prediction history
    void clearPredictionHistory();
    
    // Reset agent for new episode
    void reset();
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }
    
    // Sleep/rest state management
    bool isResting() const { return isResting_; }
    void resetRestTimer();
    void enterRestState();
    void exitRestState();

    // === ADVANCED SLEEP/REST CYCLE ===
    
    // Rest state management
    bool isResting_;                      // Current rest/sleep state
    size_t restTimer_;                    // Steps since last rest
    std::chrono::steady_clock::time_point restStartTime_;  // When rest started
    
    static const size_t REST_ENTRY_THRESHOLD = 5000;  // Steps to enter rest
    static const size_t REST_TIMER_RESET = 100;  // Reset timer after this many steps
    
    // Offline consolidation
    void updateRestState();
    void sendRestSignals();
    void sendWakeSignals();
    void performOfflineConsolidation();
    void applySleepMetaplasticity();
    Neuromodulator createSleepNeuromodulatorSignal();
    
    // Metaplastic adaptation during sleep
    void updateMetaplasticDuringSleep();
    float getSleepMetaplasticBenefit() const;
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Prediction-driven action selection
    MotorCommand selectActionWithPrediction(const std::vector<float>& currentState);
    MotorCommand selectActionBayesian(const std::vector<float>& currentState);
    MotorCommand getPredictedAction(const std::vector<float>& currentState);
    MotorCommand getGoalDirectedAction(const std::vector<float>& currentState);
    MotorCommand combineActionSelections(
        MotorCommand predAction, MotorCommand bayesianAction, MotorCommand goalAction,
        const std::vector<float>& currentState);
    MotorCommand selectLeastCertainAction(
        const std::vector<MotorCommand>& actions, const std::vector<float>& state);
    
    // Helper methods
    float computeActionValue(MotorCommand action, const std::vector<float>& state);
    float computeActionUncertainty(MotorCommand action, const std::vector<float>& state);
    float getDevelopmentalModulationFactor() const;
    float getActionSelectionThreshold() const;
    
    // Action conversion
    ActionType actionTypeToAction(MotorCommand motorCmd) const;
    MotorCommand actionTypeToMotorCommand(ActionType actionType) const;
    
    // State management
    std::vector<float> getCurrentState() const;
    
    // Meta-exploration
    void updateMetaExploration();
    
    // Learning and memory
    void updateSelfModel(MotorCommand action, const std::vector<float>& beforeState,
                        const std::vector<float>& afterState);
    
    // Utility
    RandomGenerator* getRandomGenerator();
    
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
    float developmentModulation_;
    
    // Prediction-driven action selection state
    bool predictionDrivenEnabled_;
    bool metaExplorationActive_;
    float metaExplorationStartTime_;
    float developmentModulation_;
    
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    
    // Parameters for prediction-driven selection
    float metaExplorationWeight_;
    float metaExplorationBaseThreshold_;
    float metaExplorationBaseDuration_;
    float metaExplorationActivationThreshold_;
    float bayesianExplorationBonus_;
    float uncertaintyScaling_;
    float actionSelectionBaseThreshold_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // Random generator (cached)
    RandomGenerator* rng_;
    
    // Development stage cache
    DevelopmentalStage cachedDevelopmentalStage_;
    
    // Performance tracking
    size_t totalPredictionsMade_;
    size_t successfulPredictions_;
    
    // Learning history
    std::deque<float> predictionErrorHistory_;
    std::deque<float> rewardHistory_;
    
    // Self-model cache (simplified)
    std::unordered_map<MotorCommand, float> actionOutcomePredictions_;
};

} // namespace nlm
