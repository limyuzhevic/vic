#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../motor/Action.hpp"
#include <memory>
#include <vector>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <numeric>
#include <random>

namespace nlm {

// ==================== ADVANCED MOTOR DECODING ====================

// Softmax action selection with temperature control
class SoftmaxActionSelector {
public:
    SoftmaxActionSelector(float temperature = 1.0f, float explorationBoost = 0.0f);
    
    // Select action based on neural activities and preferences
    MotorCommand selectAction(const std::vector<float>& activities, 
                             const std::vector<float>& preferences = {},
                             float temperature = -1.0f) const;
    
    // Set temperature (higher = more random, lower = more deterministic)
    void setTemperature(float temp) { temperature_ = temp; }
    
    // Set exploration boost for certain action types
    void setExplorationBoost(ActionType action, float boost) {
        explorationBoosts_[action] = boost;
    }
    
    // Get current temperature
    float getTemperature() const { return temperature_; }
    
    // Update exploration based on neuromodulators
    void updateFromNeuromodulators(float dopamine, float curiosity);
    
private:
    float temperature_;
    float explorationBoost_;
    std::unordered_map<ActionType, float> explorationBoosts_;
    std::mt19937 rng_;
};

// Temporal difference learning for action values
class TemporalDifferenceLearner {
public:
    TemporalDifferenceLearner(float alpha = 0.01f, float gamma = 0.99f);
    
    // Update action value based on reward and next state
    void updateActionValue(ActionType action, float reward, float nextValue);
    
    // Get estimated value for an action
    float getActionValue(ActionType action) const;
    
    // Get best action based on learned values
    ActionType getBestAction() const;
    
    // Reset all values
    void reset();
    
    // Set learning rate and discount factor
    void setAlpha(float alpha) { alpha_ = alpha; }
    void setGamma(float gamma) { gamma_ = gamma; }
    
private:
    std::unordered_map<ActionType, float> actionValues_;
    float alpha_;
    float gamma_;
};

// ==================== MOTOR PLANNING & SEQUENCES ====================

// Action sequence for goal-directed behavior
struct ActionSequence {
    std::vector<MotorCommand> commands;
    std::vector<float> expectedRewards;
    std::vector<float> expectedDurations;
    float totalExpectedReward;
    float totalDuration;
    std::string goalDescription;
    
    ActionSequence() : totalExpectedReward(0.0f), totalDuration(0.0f) {}
    
    // Add a command to the sequence
    void addCommand(MotorCommand cmd, float reward, float duration);
    
    // Execute next command in sequence
    MotorCommand executeNext();
    
    // Check if sequence is complete
    bool isComplete() const { return executionIndex_ >= commands.size(); }
    
    // Reset sequence for replay
    void reset() { executionIndex_ = 0; }
    
private:
    size_t executionIndex_ = 0;
};

// Hierarchical controller for goal-directed behavior
class HierarchicalController {
public:
    HierarchicalController();
    
    // Set high-level goal
    void setGoal(const std::string& goal, const std::vector<float>& goalParameters);
    
    // Plan sequence of actions to achieve goal
    std::unique_ptr<ActionSequence> planSequence(const std::vector<float>& context);
    
    // Execute current planned action
    MotorCommand executeCurrentAction();
    
    // Get current goal
    std::string getCurrentGoal() const { return currentGoal_; }
    
    // Check if goal is achieved
    bool isGoalAchieved() const;
    
private:
    std::string currentGoal_;
    std::vector<float> goalParameters_;
    std::unique_ptr<ActionSequence> currentSequence_;
    size_t currentPlanIndex_ = 0;
};

// ==================== REINFORCEMENT LEARNING ====================

// Q-learning algorithm for discrete action spaces
class QLearner {
public:
    QLearner(float alpha = 0.1f, float gamma = 0.99f, float epsilon = 0.1f);
    
    // Select action using epsilon-greedy policy
    ActionType selectAction(const std::vector<ActionType>& availableActions,
                           const std::unordered_map<ActionType, float>& qValues);
    
    // Update Q-values based on transition
    void updateQValue(ActionType action, float reward, ActionType nextAction, float maxNextQ);
    
    // Set exploration rate
    void setEpsilon(float epsilon) { epsilon_ = epsilon; }
    
private:
    float alpha_;
    float gamma_;
    float epsilon_;
};

// Policy gradient for continuous action spaces
class PolicyGradientLearner {
public:
    PolicyGradientLearner(float learningRate = 0.001f, float entropyCoeff = 0.01f);
    
    // Update policy based on trajectory
    void updatePolicy(const std::vector<ActionType>& actions,
                     const std::vector<float>& advantages,
                     const std::vector<float>& logProbabilities);
    
    // Get action probability using softmax policy
    float getActionProbability(const std::vector<ActionType>& actions, ActionType action) const;
    
private:
    float learningRate_;
    float entropyCoeff_;
};

// Actor-critic algorithm combining policy and value learning
class ActorCriticLearner {
public:
    ActorCriticLearner(float actorAlpha = 0.001f, float criticAlpha = 0.01f,
                      float gamma = 0.99f, float tau = 0.95f);
    
    // Update both actor and critic networks
    void update(const std::vector<ActionType>& states,
                const std::vector<ActionType>& actions,
                const std::vector<float>& rewards,
                const std::vector<ActionType>& nextStates,
                const std::vector<float>& dones);
    
    // Select action using current policy
    ActionType selectAction(const std::vector<ActionType>& availableActions,
                           const std::vector<float>& stateFeatures);
    
    // Get estimated value of state
    float getValue(const std::vector<float>& stateFeatures) const;
    
private:
    float actorAlpha_;
    float criticAlpha_;
    float gamma_;
    float tau_;
};

// ==================== ADVANCED NEUROMODULATION ====================

// Advanced neuromodulation with dopamine prediction errors
class DopamineModulator {
public:
    DopamineModulator(float learningRate = 0.01f, float predictionErrorDecay = 0.99f);
    
    // Update prediction error and dopamine level
    void update(float reward, float predictedReward, float learningRate = -1.0f);
    
    // Get current dopamine level
    float getDopamineLevel() const { return dopamineLevel_; }
    
    // Get prediction error
    float getPredictionError() const { return predictionError_; }
    
    // Update learning rate based on environmental stability
    void adaptLearningRate(float environmentalStability);
    
    // Reset modulator
    void reset() {
        dopamineLevel_ = 0.0f;
        predictionError_ = 0.0f;
        predictedReward_ = 0.0f;
    }
    
private:
    float dopamineLevel_;
    float predictionError_;
    float predictedReward_;
    float learningRate_;
    float predictionErrorDecay_;
};

// ==================== MOTOR SKILL LEARNING ====================

// Hebbian learning for forming motor associations
class HebbianSkillLearner {
public:
    HebbianSkillLearner(float learningRate = 0.001f, float consolidationRate = 0.0001f);
    
    // Learn association between sensory input and motor output
    void learnAssociation(const std::vector<float>& sensoryInput, 
                         MotorCommand motorOutput,
                         float reward);
    
    // Retrieve best motor response for given sensory input
    MotorCommand retrieveMotorResponse(const std::vector<float>& sensoryInput) const;
    
    // Consolidate learned skills
    void consolidateSkills();
    
    // Get skill strength for association
    float getSkillStrength(const std::vector<float>& sensoryInput, 
                          MotorCommand motorOutput) const;
    
private:
    // Maps sensory patterns to motor responses
    std::unordered_map<size_t, std::unordered_map<MotorCommand, float>> associations_;
    float learningRate_;
    float consolidationRate_;
    std::mt19937 rng_;
};

// Motor memory for storing and retrieving motor patterns
class MotorMemory {
public:
    MotorMemory(size_t capacity = 1000);
    
    // Store motor pattern
    void storeMotorPattern(const std::vector<MotorCommand>& pattern,
                          const std::vector<float>& sensoryContext,
                          float reward);
    
    // Retrieve similar motor pattern
    std::unique_ptr<ActionSequence> retrievePattern(const std::vector<float>& sensoryContext,
                                                   float similarityThreshold = 0.8f);
    
    // Update pattern usage statistics
    void updatePatternUsage(const std::vector<MotorCommand>& pattern);
    
    // Get pattern statistics
    float getPatternUsageCount(const std::vector<MotorCommand>& pattern) const;
    
private:
    struct MotorPattern {
        std::vector<MotorCommand> commands;
        std::vector<float> context;
        float reward;
        float usageCount;
        float lastUsed;
    };
    
    std::vector<MotorPattern> patterns_;
    size_t capacity_;
    std::mt19937 rng_;
};

// ==================== SENSORY-MOTOR INTEGRATION ====================

// Forward model for predicting sensory consequences of actions
class ForwardModel {
public:
    ForwardModel();
    
    // Learn action-outcome mapping
    void learnActionOutcome(MotorCommand action, const std::vector<float>& expectedSensory);
    
    // Predict sensory consequences of action
    std::vector<float> predictSensory(MotorCommand action) const;
    
    // Get prediction error
    float getPredictionError(MotorCommand action, const std::vector<float>& actualSensory) const;
    
private:
    std::unordered_map<MotorCommand, std::vector<float>> actionOutcomes_;
    std::mt19937 rng_;
};

// Internal model for world representation
class InternalModel {
public:
    InternalModel();
    
    // Update world model based on sensory input
    void updateWorldModel(const std::vector<float>& sensoryInput, MotorCommand action);
    
    // Get internal state
    std::vector<float> getInternalState() const { return internalState_; }
    
    // Predict future state
    std::vector<float> predictFutureState(const std::vector<float>& action, 
                                          size_t steps) const;
    
private:
    std::vector<float> internalState_;
    std::vector<std::vector<float>> stateTransitions_;
};

// ==================== MAIN AGENT BRAIN CLASS ====================

// Enhanced AgentBrain with advanced motor control capabilities
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
    
    // Decode brain motor activity into motor command (enhanced)
    MotorCommand decodeMotorCommand();
    
    // Apply neuromodulation based on reward (enhanced)
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
    
    // ========== ENHANCED CONFIGURATION ==========
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    // Advanced control strategies
    void enableAdvancedMotorDecoding(bool enable) { advancedMotorDecodingEnabled_ = enable; }
    void enableMotorPlanning(bool enable) { motorPlanningEnabled_ = enable; }
    void enableReinforcementLearning(bool enable) { reinforcementLearningEnabled_ = enable; }
    void enableSkillLearning(bool enable) { skillLearningEnabled_ = enable; }
    void enableSensoryMotorIntegration(bool enable) { sensoryMotorIntegrationEnabled_ = enable; }
    
    // Advanced motor decoding configuration
    void setActionTemperature(float temp) { softmaxSelector_.setTemperature(temp); }
    void setQLearningParameters(float alpha, float gamma, float epsilon);
    void setPolicyGradientParameters(float learningRate, float entropyCoeff);
    
    // Motor planning configuration
    void setMaxSequenceLength(size_t length) { maxSequenceLength_ = length; }
    void setHierarchicalControlEnabled(bool enabled) { hierarchicalControlEnabled_ = enabled; }
    
    // RL configuration
    void setDiscountFactor(float gamma) { discountFactor_ = gamma; }
    void setExplorationRate(float rate) { explorationRate_ = rate; }
    
    // Skill learning configuration
    void setHebbianLearningRate(float rate) { hebbianLearner_.setLearningRate(rate); }
    void setMemoryCapacity(size_t capacity) { motorMemory_.setCapacity(capacity); }
    
    // Sensory-motor integration configuration
    void setPredictionModelEnabled(bool enabled) { predictionModelEnabled_ = enabled; }
    void setInternalModelEnabled(bool enabled) { internalModelEnabled_ = enabled; }
    
    // Get current motor command using advanced decoding
    MotorCommand getAdvancedMotorCommand(const std::vector<float>& neuralActivity);
    
    // Get action sequence for current goal
    std::unique_ptr<ActionSequence> getCurrentActionSequence() const;
    
    // Update action sequence
    void updateActionSequence();
    
    // Get learned skill strength
    float getSkillStrength(const std::vector<float>& sensoryInput, 
                          MotorCommand motorOutput) const;
    
    // Get predicted sensory consequences
    std::vector<float> getPredictedSensory(MotorCommand action) const;
    
    // Get internal world model
    std::vector<float> getInternalWorldModel() const { return internalModel_.getInternalState(); }
    
    // Set high-level goal
    void setGoal(const std::string& goal, const std::vector<float>& parameters = {});
    
    // Check if current sequence is complete
    bool isActionSequenceComplete() const;
    
private:
    // Enhanced motor decoding components
    SoftmaxActionSelector softmaxSelector_;
    TemporalDifferenceLearner tdLearner_;
    
    // Motor planning components
    HierarchicalController hierarchicalController_;
    std::unique_ptr<ActionSequence> currentSequence_;
    
    // Reinforcement learning components
    QLearner qLearner_;
    PolicyGradientLearner policyLearner_;
    ActorCriticLearner actorCriticLearner_;
    
    // Neuromodulation components
    DopamineModulator dopamineModulator_;
    
    // Skill learning components
    HebbianSkillLearner hebbianLearner_;
    MotorMemory motorMemory_;
    
    // Sensory-motor integration components
    ForwardModel forwardModel_;
    InternalModel internalModel_;
    
    // Main brain components
    std::shared_ptr<Brain> brain_;
    
    // Motor neuron groups (legacy)
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Sensory neuron groups (legacy)
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    // Neuromodulation state (legacy)
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    
    // Development state (legacy)
    double developmentalAge_;
    float plasticityModifier_;
    
    // Configuration flags (legacy + new)
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    bool advancedMotorDecodingEnabled_;
    bool motorPlanningEnabled_;
    bool reinforcementLearningEnabled_;
    bool skillLearningEnabled_;
    bool sensoryMotorIntegrationEnabled_;
    
    // Advanced configuration
    size_t maxSequenceLength_;
    bool hierarchicalControlEnabled_;
    float discountFactor_;
    float explorationRate_;
    bool predictionModelEnabled_;
    bool internalModelEnabled_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // Private helper methods
    MotorCommand decodeFromMotorNeurons();
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Legacy helper methods
    float calcActivity(const std::vector<Neuron*>& neurons) const;
    ActionType getLegacyMotorCommandAsAction(MotorCommand cmd) const;
};

} // namespace nlm

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
