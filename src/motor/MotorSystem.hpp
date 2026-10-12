#pragma once

#include "Action.hpp"
#include "../brain/NeuralRegion.hpp"
#include <memory>
#include <vector>
#include <unordered_map>
#include <queue>

namespace nlm {

// Motor system: converts neural activity to actions with real dynamics
// Implements biological realism, cost-benefit analysis, and motor learning

class MotorSystem {
public:
    MotorSystem();
    ~MotorSystem();
    
    // Select action based on neural activity with real dynamics
    // Implements biological action selection with cost-benefit analysis
    std::unique_ptr<Action> selectAction(const NeuralRegion& motorRegion);
    
    // Select action from neural population activity with advanced selection
    std::unique_ptr<Action> selectActionWithNeuralDynamics(
        const NeuralRegion& motorRegion,
        const std::vector<Neuron*>& neurons,
        float arousalLevel = 0.5f,
        float neuromodulation = 0.0f
    );
    
    // Get available actions
    const std::vector<ActionType>& getAvailableActions() const;
    
    // Set action preferences (for exploration)
    void setActionPreference(ActionType action, float preference);
    
    // Set motor learning parameters
    void setLearningRate(float rate) { learningRate_ = rate; }
    void setExplorationStrength(float strength) { explorationStrength_ = strength; }
    void setCostSensitivity(float sensitivity) { costSensitivity_ = sensitivity; }
    
    // Get action history
    const std::vector<const Action*>& getActionHistory() const;
    void clearHistory();
    
    // Motor adaptation and learning
    void updateMotorAdaptation(const NeuralRegion& motorRegion, float adaptationDelta);
    void adjustMotorPatterns(const Action& successfulAction, const Action& alternativeAction);
    
    // Cost-benefit analysis
    float computeActionCost(const Action& action) const;
    float computeActionBenefit(const Action& action, const NeuralRegion& motorRegion) const;
    float computeActionNetValue(const Action& action, const NeuralRegion& motorRegion) const;
    
    // Motor execution planning
    std::unique_ptr<Action> planActionSequence(const std::vector<ActionType>& sequenceTypes);
    std::unique_ptr<Action> adaptActionForContext(const Action& baseAction, 
                                                  const NeuralRegion& contextRegion);
    
    // Biological realism methods
    void updateNeuromodulation(float dopamine, float novelty);
    void updateArousal(float baseline, float sensoryInput);
    void updateMotorFatigue(float fatigueRate, double dt);
    
    // Motor learning and adaptation
    void learnFromActionOutcome(const Action& action, float reward, bool success);
    void updateActionPreferences(const std::vector<const Action*>& actions, const std::vector<float>& outcomes);
    
    // Statistics and monitoring
    float getCurrentArousal() const { return arousalLevel_; }
    float getCurrentNeuromodulation() const { return neuromodulation_; }
    float getMotorFatigue() const { return fatigueFactor_; }
    size_t getTotalActionsExecuted() const { return totalActionsExecuted_; }
    float getAverageActionSuccessRate() const { return totalActionsExecuted_ > 0 ? 
        static_cast<float>(successfulActions_) / totalActionsExecuted_ : 0.5f; }
    
    // Reset and cleanup
    void reset();
    void emergencyStop();
    
private:
    // Advanced action selection algorithms
    std::unique_ptr<Action> softmaxActionSelection(const std::vector<const Action*>& candidates,
                                                 const std::vector<float>& neuralActivations,
                                                 const std::vector<float>& preferences);
    std::unique_ptr<Action> epsilonGreedySelection(const std::vector<const Action*>& candidates,
                                                  const std::vector<float>& neuralActivations);
    std::unique_ptr<Action> maxEntropySelection(const std::vector<const Action*>& candidates,
                                              const std::vector<float>& neuralActivations);
    
    // Motor dynamics integration
    MotorDynamics computeMotorDynamics(const ActionType& actionType, 
                                      const NeuralRegion& motorRegion,
                                      const std::vector<Neuron*>& neurons);
    void updateMotorCommand(Action& action, const NeuralRegion& motorRegion);
    
    // Biological realism implementation
    void updateNeuralIntegration(const NeuralRegion& motorRegion, const std::vector<Neuron*>& neurons);
    void updateFeedbackLoop(const Action& action, const NeuralRegion& motorRegion);
    
    // Cost-benefit computation helpers
    float computeExecutionCost(const Action& action) const;
    float computeOpportunityCost(const Action& action, const std::vector<ActionType>& availableActions) const;
    float computeRiskAdjustment(float baseValue, float uncertainty) const;
    
    // Learning and adaptation helpers
    void updateActionExpectedValues(const std::vector<const Action*>& actions, 
                                   const std::vector<float>& outcomes);
    void adjustExplorationBasedOnUncertainty(const std::vector<const Action*>& actions);
    
    // Sequence planning and execution
    std::unique_ptr<Action> selectFromSequence(const std::vector<ActionType>& sequence);
    void updateSequenceProgress(size_t sequenceLength, size_t actionPosition);
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Motor state
    float arousalLevel_;
    float neuromodulation_;
    float fatigueFactor_;
    float learningRate_;
    float explorationStrength_;
    float costSensitivity_;
    
    // Statistics
    size_t totalActionsExecuted_;
    size_t successfulActions_;
    size_t sequenceCount_;
};

} // namespace nlm
