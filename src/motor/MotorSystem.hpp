#pragma once

#include "Action.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/Brain.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/SelfModel.hpp"
#include <memory>
#include <vector>
#include <map>
#include <unordered_set>
#include <random>
#include <chrono>

namespace nlm {

// MotorTiming: Controls temporal aspects of motor execution
struct MotorTiming {
    float scheduledStartTime;
    float executionDuration;
    float delayCompensation;
    bool isExecuting;
    float remainingTime;
    
    MotorTiming() : scheduledStartTime(0.0f), executionDuration(0.0f), 
                   delayCompensation(0.0f), isExecuting(false), remainingTime(0.0f) {}
};

// ActionSequence: Represents a planned sequence of motor actions
struct ActionSequence {
    std::vector<ActionType> actions;
    std::vector<MotorTiming> timings;
    float totalSequenceDuration;
    float currentTime;
    bool isActive;
    size_t currentIndex;
    
    ActionSequence() : totalSequenceDuration(0.0f), currentTime(0.0f), 
                      isActive(false), currentIndex(0) {}
    
    void addAction(ActionType action, float duration) {
        actions.push_back(action);
        MotorTiming timing;
        timing.scheduledStartTime = currentTime;
        timing.executionDuration = duration;
        timing.isExecuting = false;
        timing.remainingTime = duration;
        timings.push_back(timing);
        currentTime += duration;
        totalSequenceDuration = currentTime;
    }
    
    bool isComplete() const {
        return currentIndex >= actions.size();
    }
    
    ActionType getCurrentAction() const {
        if (currentIndex < actions.size()) {
            return actions[currentIndex];
        }
        return ActionType::Wait;
    }
    
    void advanceSequence(float dt) {
        if (currentIndex < timings.size()) {
            MotorTiming& timing = timings[currentIndex];
            timing.remainingTime -= dt;
            timing.isExecuting = (timing.remainingTime > 0.0f);
            
            if (timing.remainingTime <= 0.0f) {
                currentIndex++;
                if (currentIndex < timings.size()) {
                    timings[currentIndex].isExecuting = true;
                }
            }
        }
    }
};

// GO/NO-GO signal from basal ganglia
enum class GO_NG_SIGNAL {
    GO,      // Action selected
    NO_GO,   // Action inhibited
    QUEUE    // Action queued but not executed
};

// MotorSystem: Converts neural activity to actions with biological realism
// Integrates motor cortex, basal ganglia, cerebellum, and motor neurons
// Implements motor planning, action selection, sequence learning, and adaptation
class MotorSystem {
public:
    MotorSystem();
    ~MotorSystem();
    
    // Initialize motor system with brain reference
    void initialize(Brain* brain, Neuromodulator* neuromodulator,
                   PredictionSystem* predictionSystem, SelfModel* selfModel);
    
    // Select action based on integrated neural activity and motor planning
    std::unique_ptr<Action> selectAction(const NeuralRegion& motorRegion);
    
    // Get available actions with current preferences
    const std::vector<ActionType>& getAvailableActions() const;
    
    // Set action preference for exploration/exploitation trade-off
    void setActionPreference(ActionType action, float preference);
    
    // Get action history for learning and analysis
    const std::vector<const Action*>& getActionHistory() const;
    void clearHistory();
    
    // Motor planning methods
    ActionSequence planActionSequence(const std::vector<ActionType>& proposedActions,
                                     float timePerAction = 0.1f);
    ActionSequence planGoalDirectedAction(const std::vector<float>& goalState,
                                         const std::vector<float>& currentState);
    
    // Action selection methods
    ActionType selectActionBasedOnActorCritic(const std::vector<ActionType>& candidates,
                                            const std::vector<float>& state,
                                            float explorationBonus = 0.0f);
    ActionType applyBasalGangliaSelection(const std::vector<ActionType>& candidates,
                                         const std::vector<float>& actionValues);
    ActionType applyMotorCortexSpreading(const std::vector<ActionType>& candidates,
                                        const NeuralRegion& motorRegion);
    
    // Motor execution and timing
    void startSequenceExecution(const ActionSequence& sequence);
    void updateSequenceExecution(float dt);
    ActionType getCurrentExecutingAction() const;
    bool isSequenceExecuting() const;
    float getSequenceProgress() const;
    
    // Motor learning and adaptation
    void learnActionOutcome(ActionType action, const std::vector<float>& state,
                           float reward, float predictionError);
    void updateActionValues(ActionType action, float reward, float temporalDifference);
    void adaptMotorParameters(float error, float learningRate);
    
    // Cerebellar forward model integration
    std::vector<float> predictActionConsequence(ActionType action,
                                              const std::vector<float>& currentState);
    float computeMotorPredictionError(const std::vector<float>& predicted,
                                   const std::vector<float>& actual);
    
    // Dynamics compensation and real-time execution
    Action compensateForDynamics(ActionType rawAction, const std::vector<float>& state);
    Action applyGainScheduling(ActionType action, float error);
    Action enforceMotorConstraints(ActionType action, const std::vector<float>& state);
    
    // Reward-based action-outcome associations
    void recordActionOutcome(ActionType action, const std::vector<float>& state,
                           float reward, float temporalDifference);
    float getActionQuality(ActionType action) const;
    ActionType getBestActionBasedOnQuality(const std::vector<ActionType>& candidates) const;
    
    // Motor cortical spreading activation for action representation
    void updateMotorCorticalRepresentation(const NeuralRegion& motorRegion);
    float computeActionActivation(ActionType action, const NeuralRegion& motorRegion) const;
    std::vector<ActionType> getActivelyRepresentedActions() const;
    
    // Motor learning with timing control
    void learnTemporalSequence(const std::vector<ActionType>& actionSequence,
                              const std::vector<float>& timingPattern,
                              float reward);
    void refineActionTiming(ActionType action, float optimalDuration);
    float getOptimalActionDuration(ActionType action) const;
    
    // State-dependent motor behavior
    ActionType selectStateDependentAction(const std::vector<float>& state) const;
    ActionType adaptToContext(const std::vector<float>& context,
                           const std::vector<float>& currentState);
    
    // Configuration and tuning
    void setPlanningHorizon(size_t horizon) { planningHorizon_ = horizon; }
    void setActorCriticLearningRate(float lr) { actorCriticLearningRate_ = lr; }
    void setExplorationRate(float rate) { explorationRate_ = rate; }
    void setActionSequenceTimeout(float timeout) { actionSequenceTimeout_ = timeout; }
    
    // Get statistics and diagnostics
    float getMotorConfidence() const;
    float getPlanningConfidence() const;
    size_t getSequenceLength() const;
    float getCurrentActionValue() const;
    bool isLearningFromError() const;
    
    // Reset and cleanup
    void reset();
    void enableSequenceLearning(bool enable) { sequenceLearningEnabled_ = enable; }
    void enableAdaptiveTiming(bool enable) { adaptiveTimingEnabled_ = enable; }
    
    // Integration with existing systems
    void integrateWithMotorCortex(const NeuralRegion& motorRegion);
    void integrateWithBasalGanglia(GO_NG_SIGNAL signal, ActionType action);
    void integrateWithCerebellum(float predictionError, float timingError);
    void integrateWithMotorNeurons(const Action& action);
    void integrateWithRewardSystem(float reward, float predictionError);
    
private:
    // Internal implementation structure
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Actor-critic learning components
    struct ActorCriticNetworks {
        std::map<ActionType, float> actionValues;      // Critic: Q-values
        std::map<ActionType, float> actionPreferences;  // Actor: policy parameters
        float baselineValue;                            // Value function baseline
        
        ActorCriticNetworks() : baselineValue(0.0f) {}
    };
    
    // Motor learning components
    struct MotorLearningState {
        std::map<ActionType, float> actionRewards;          // Cumulative reward
        std::map<ActionType, size_t> actionCounts;          // Action frequency
        std::map<ActionType, float> actionVariances;        // Action outcome variance
        std::map<ActionType, float> learningRates;          // Adaptive learning rates
        std::map<ActionType, float> temporalPatterns;       // Timing patterns
        
        // Reset learning state
        void reset() {
            actionRewards.clear();
            actionCounts.clear();
            actionVariances.clear();
            learningRates.clear();
            temporalPatterns.clear();
        }
    };
    
    // Neural integration components
    struct NeuralIntegrationState {
        std::vector<ActionType> motorCorticalActivation;    // Current motor cortex representation
        std::vector<float> basalGangliaGoSignals;          // GO pathway strength
        std::vector<float> basalGangliaNoGoSignals;        // NO-GO pathway strength
        std::vector<float> cerebellarForwardModelError;    // Prediction error signals
        std::vector<float> motorNeuronPoolActivity;         // Motor neuron group activity
        
        NeuralIntegrationState() : basalGangliaGoSignals(27, 0.0f),  // 27 actions
                                 basalGangliaNoGoSignals(27, 0.0f),
                                 cerebellarForwardModelError(27, 0.0f),
                                 motorNeuronPoolActivity(27, 0.0f) {}
    };
    
    // Motor timing components
    struct MotorTimingState {
        float globalClock;                                // Global simulation time
        std::map<ActionType, float> optimalDurations;     // Optimal action durations
        std::map<ActionType, float> executionDelays;      // Execution delays
        std::map<ActionType, float> refractoryPeriods;     // Refractory periods
        float adaptationTimeConstant;                     // Time constant for adaptation
        
        MotorTimingState() : globalClock(0.0f), adaptationTimeConstant(1.0f) {}
    };
    
    // Implementation details (pImpl)
    struct Impl {
        // Core motor system state
        Brain* brain_;
        Neuromodulator* neuromodulator_;
        PredictionSystem* predictionSystem_;
        SelfModel* selfModel_;
        
        // Motor regions in the brain
        std::vector<RegionId> motorCortexRegions_;
        std::vector<RegionId> basalGangliaRegions_;
        std::vector<RegionId> cerebellumRegions_;
        
        // Available actions
        std::vector<ActionType> availableActions_;
        
        // Learning and adaptation
        ActorCriticNetworks actorCritic_;
        MotorLearningState learningState_;
        NeuralIntegrationState neuralState_;
        MotorTimingState timingState_;
        
        // Action sequences and execution
        ActionSequence currentSequence_;
        std::vector<const Action*> actionHistory_;
        std::vector<const Action*> sequenceHistory_;
        
        // Configuration
        size_t planningHorizon_;
        float actorCriticLearningRate_;
        float explorationRate_;
        float actionSequenceTimeout_;
        
        // Flags
        bool sequenceLearningEnabled_;
        bool adaptiveTimingEnabled_;
        bool isSequenceExecuting_;
        bool isLearningFromError_;
        
        // Random number generation
        std::mt19937 rng_;
        std::uniform_real_distribution<float> uniformDist_;
        std::normal_distribution<float> normalDist_;
        
        // Forward declarations for neural components
        class MotorCortexModel;
        class BasalGangliaModel;
        class CerebellarForwardModel;
        
        // Implementation constructor
        Impl() : brain_(nullptr), neuromodulator_(nullptr), 
                predictionSystem_(nullptr), selfModel_(nullptr),
                planningHorizon_(3), actorCriticLearningRate_(0.01f),
                explorationRate_(0.1f), actionSequenceTimeout_(10.0f),
                sequenceLearningEnabled_(true), adaptiveTimingEnabled_(true),
                isSequenceExecuting_(false), isLearningFromError_(false),
                rng_(std::chrono::steady_clock::now().time_since_epoch().count()),
                uniformDist_(0.0f, 1.0f), normalDist_(0.0f, 1.0f) {
            
            // Initialize available actions
            availableActions_ = {
                ActionType::MoveForward, ActionType::MoveBackward,
                ActionType::MoveLeft, ActionType::MoveRight,
                ActionType::TurnLeft, ActionType::TurnRight,
                ActionType::Look, ActionType::LookUp, ActionType::LookDown,
                ActionType::Interact, ActionType::Eat, ActionType::Drink,
                ActionType::Rest, ActionType::Wait, ActionType::Custom
            };
            
            // Initialize neural state
            neuralState_.motorCorticalActivation.resize(static_cast<size_t>(ActionType::Custom) + 1, 0.0f);
            neuralState_.motorCorticalActivation[static_cast<size_t>(ActionType::MoveForward)] = 0.3f;
            neuralState_.motorCorticalActivation[static_cast<size_t>(ActionType::MoveBackward)] = 0.3f;
            neuralState_.motorCorticalActivation[static_cast<size_t>(ActionType::TurnLeft)] = 0.3f;
            neuralState_.motorCorticalActivation[static_cast<size_t>(ActionType::TurnRight)] = 0.3f;
        }
        
        // Motor planning with cost-to-go computation
        ActionSequence planActionSequence(const std::vector<ActionType>& proposedActions,
                                         float timePerAction = 0.1f) {
            ActionSequence sequence;
            for (size_t i = 0; i < proposedActions.size(); ++i) {
                float duration = timePerAction * (1.0f + neuralState_.cerebellarForwardModelError[i % 27]);
                sequence.addAction(proposedActions[i], duration);
            }
            return sequence;
        }
        
        // Action selection using actor-critic methods
        ActionType selectActionBasedOnActorCritic(const std::vector<ActionType>& candidates,
                                                const std::vector<float>& state,
                                                float explorationBonus = 0.0f) {
            if (candidates.empty()) return ActionType::Wait;
            
            // Compute exploration bonuses based on action history
            std::vector<float> actionScores;
            for (const auto& action : candidates) {
                size_t actionIndex = static_cast<size_t>(action);
                float score = actorCritic_.actionValues.at(action);
                
                // Exploration based on frequency
                float frequency = static_cast<float>(learningState_.actionCounts.at(action)) / 
                                 std::max(1.0f, static_cast<float>(learningState_.actionCounts.size()));
                score += explorationRate_ * (1.0f - frequency) + explorationBonus;
                
                // Add neural activation
                score += neuralState_.motorCorticalActivation[actionIndex] * 0.5f;
                
                actionScores.push_back(score);
            }
            
            // Softmax selection
            float sumExp = 0.0f;
            for (float score : actionScores) {
                sumExp += std::exp(score);
            }
            
            std::uniform_real_distribution<float> dist(0.0f, sumExp);
            float threshold = dist(rng_);
            float cumulative = 0.0f;
            
            for (size_t i = 0; i < candidates.size(); ++i) {
                cumulative += std::exp(actionScores[i]);
                if (threshold <= cumulative) {
                    return candidates[i];
                }
            }
            
            return candidates.back();
        }
        
        // Learn action-outcome associations
        void learnActionOutcome(ActionType action, const std::vector<float>& state,
                               float reward, float predictionError) {
            auto& actionLearning = learningState_;
            auto it = actionLearning.actionRewards.find(action);
            if (it == actionLearning.actionRewards.end()) {
                actionLearning.actionRewards[action] = 0.0f;
                actionLearning.actionCounts[action] = 0;
                actionLearning.actionVariances[action] = 1.0f;
                actionLearning.learningRates[action] = actorCriticLearningRate_;
            }
            
            // Update average reward
            size_t count = actionLearning.actionCounts[action];
            actionLearning.actionRewards[action] += (reward - actionLearning.actionRewards[action]) / (count + 1);
            actionLearning.actionCounts[action] = count + 1;
            
            // Update action value using temporal difference
            float temporalDifference = reward - predictionError;
            if (temporalDifference != 0.0f) {
                actorCritic_.actionValues[action] += actorCriticLearningRate_ * temporalDifference;
                actorCritic_.actionPreferences[action] += actorCriticLearningRate_ * temporalDifference;
            }
            
            // Adapt learning rate based on prediction error
            actionLearning.learningRates[action] = std::clamp(
                actorCriticLearningRate_ * (1.0f + std::abs(predictionError)),
                0.001f, 0.1f);
        }
        
        // Update action values
        void updateActionValues(ActionType action, float reward, float temporalDifference) {
            actorCritic_.actionValues[action] += actorCriticLearningRate_ * temporalDifference;
            actorCritic_.actionPreferences[action] += actorCriticLearningRate_ * reward;
            
            // Update baseline
            float oldBaseline = actorCritic_.baselineValue;
            actorCritic_.baselineValue += actorCriticLearningRate_ * (reward - oldBaseline);
        }
        
        // Get action quality
        float getActionQuality(ActionType action) const {
            auto it = learningState_.actionRewards.find(action);
            if (it != learningState_.actionRewards.end()) {
                return it->second;
            }
            return 0.0f;
        }
    };
};

} // namespace nlm

#endif // MOTOR_SYSTEM_HPP
