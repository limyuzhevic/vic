#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <unordered_map>

namespace nlm {

// ==================== SOFTMAX ACTION SELECTOR IMPLEMENTATION ====================

SoftmaxActionSelector::SoftmaxActionSelector(float temperature, float explorationBoost)
    : temperature_(temperature), explorationBoost_(explorationBoost), rng_(std::random_device()()) {
}

MotorCommand SoftmaxActionSelector::selectAction(const std::vector<float>& activities,
                                                 const std::vector<float>& preferences,
                                                 float temperature) const {
    if (activities.empty()) return MotorCommand::Wait;
    
    float temp = (temperature > 0.0f) ? temperature : temperature_;
    
    // Convert activities to logits (considering exploration boosts)
    std::vector<float> logits(activities.size());
    for (size_t i = 0; i < activities.size(); ++i) {
        logits[i] = activities[i];
        // Apply exploration boost for certain actions
        if (i < static_cast<size_t>(ActionType::Wait)) {
            ActionType action = static_cast<ActionType>(i);
            auto it = explorationBoosts_.find(action);
            if (it != explorationBoosts_.end()) {
                logits[i] += it->second;
            }
        }
    }
    
    // Apply temperature scaling
    if (temp > 0.0f) {
        for (float& logit : logits) {
            logit /= temp;
        }
    }
    
    // Apply preferences if provided
    if (!preferences.empty() && preferences.size() >= logits.size()) {
        for (size_t i = 0; i < logits.size(); ++i) {
            logits[i] += preferences[i] * explorationBoost_;
        }
    }
    
    // Apply softmax to get probabilities
    std::vector<float> probs(logits.size(), 0.0f);
    float sum = 0.0f;
    for (float logit : logits) {
        float expVal = std::exp(logit);
        probs.push_back(expVal);
        sum += expVal;
    }
    
    // Normalize
    for (float& prob : probs) {
        prob /= sum;
    }
    
    // Sample action based on probabilities
    float threshold = (sum > 0.0f) ? rng_() / static_cast<float>(rng_.max()) * sum : 0.0f;
    float cumulative = 0.0f;
    
    for (size_t i = 0; i < probs.size(); ++i) {
        cumulative += probs[i];
        if (threshold <= cumulative) {
            return static_cast<MotorCommand>(i);
        }
    }
    
    return MotorCommand::Wait;
}

void SoftmaxActionSelector::updateFromNeuromodulators(float dopamine, float curiosity) {
    // Adjust temperature based on neuromodulators
    // High dopamine -> lower temperature (exploitation)
    // High curiosity -> higher temperature (exploration)
    float baseTemp = temperature_;
    float dopamineEffect = std::clamp(-dopamine, 0.5f, 2.0f);
    float curiosityEffect = std::clamp(curiosity * 2.0f, 0.5f, 3.0f);
    
    temperature_ = baseTemp * dopamineEffect * curiosityEffect;
    temperature_ = std::clamp(temperature_, 0.1f, 10.0f);
}

// ==================== TEMPORAL DIFFERENCE LEARNER IMPLEMENTATION ====================

TemporalDifferenceLearner::TemporalDifferenceLearner(float alpha, float gamma)
    : alpha_(alpha), gamma_(gamma) {
}

void TemporalDifferenceLearner::updateActionValue(ActionType action, float reward, float nextValue) {
    auto it = actionValues_.find(action);
    if (it == actionValues_.end()) {
        actionValues_[action] = reward;
    } else {
        // TD(0) update
        float target = reward + gamma_ * nextValue;
        it->second += alpha_ * (target - it->second);
    }
}

float TemporalDifferenceLearner::getActionValue(ActionType action) const {
    auto it = actionValues_.find(action);
    return (it != actionValues_.end()) ? it->second : 0.0f;
}

ActionType TemporalDifferenceLearner::getBestAction() const {
    if (actionValues_.empty()) return ActionType::Wait;
    
    float bestValue = -std::numeric_limits<float>::max();
    ActionType bestAction = ActionType::Wait;
    
    for (const auto& pair : actionValues_) {
        if (pair.second > bestValue) {
            bestValue = pair.second;
            bestAction = pair.first;
        }
    }
    
    return bestAction;
}

void TemporalDifferenceLearner::reset() {
    actionValues_.clear();
}

// ==================== ACTION SEQUENCE IMPLEMENTATION ====================

void ActionSequence::addCommand(MotorCommand cmd, float reward, float duration) {
    commands.push_back(cmd);
    expectedRewards.push_back(reward);
    expectedDurations.push_back(duration);
    totalExpectedReward += reward;
    totalDuration += duration;
}

MotorCommand ActionSequence::executeNext() {
    if (executionIndex_ >= commands.size()) {
        return MotorCommand::Wait;
    }
    return commands[executionIndex_++];
}

// ==================== HIERARCHICAL CONTROLLER IMPLEMENTATION ====================

HierarchicalController::HierarchicalController() : currentPlanIndex_(0) {
}

void HierarchicalController::setGoal(const std::string& goal, 
                                   const std::vector<float>& goalParameters) {
    currentGoal_ = goal;
    goalParameters_ = goalParameters;
    currentPlanIndex_ = 0;
}

std::unique_ptr<ActionSequence> HierarchicalController::planSequence(const std::vector<float>& context) {
    currentSequence_ = std::make_unique<ActionSequence>();
    
    // Simple hierarchical planning based on goal
    if (currentGoal_ == "NavigateToTarget") {
        // Extract target from goal parameters
        float targetX = goalParameters_.size() > 0 ? goalParameters_[0] : 0.0f;
        float targetY = goalParameters_.size() > 1 ? goalParameters_[1] : 0.0f;
        
        // Simple navigation sequence
        currentSequence_->addCommand(MotorCommand::MoveForward, 0.1f, 1.0f);
        currentSequence_->addCommand(MotorCommand::TurnRight, 0.05f, 0.5f);
        currentSequence_->goalDescription = "Navigate to target at (" + 
                                          std::to_string(targetX) + ", " + std::to_string(targetY) + ")";
    } else if (currentGoal_ == "ExploreArea") {
        currentSequence_->addCommand(MotorCommand::TurnLeft, 0.1f, 0.5f);
        currentSequence_->addCommand(MotorCommand::MoveForward, 0.2f, 1.5f);
        currentSequence_->addCommand(MotorCommand::TurnRight, 0.1f, 0.5f);
        currentSequence_->goalDescription = "Explore surrounding area";
    } else {
        // Default behavior
        currentSequence_->addCommand(MotorCommand::MoveForward, 0.1f, 1.0f);
        currentSequence_->goalDescription = "Generic goal: " + currentGoal_;
    }
    
    currentPlanIndex_ = 0;
    return currentSequence_;
}

MotorCommand HierarchicalController::executeCurrentAction() {
    return currentSequence_ ? currentSequence_->executeNext() : MotorCommand::Wait;
}

bool HierarchicalController::isGoalAchieved() const {
    // Simple goal achievement check based on sequence completion
    return currentSequence_ && currentSequence_->isComplete();
}

// ==================== Q-LEARNER IMPLEMENTATION ====================

QLearner::QLearner(float alpha, float gamma, float epsilon)
    : alpha_(alpha), gamma_(gamma), epsilon_(epsilon) {
}

ActionType QLearner::selectAction(const std::vector<ActionType>& availableActions,
                                 const std::unordered_map<ActionType, float>& qValues) {
    if (availableActions.empty()) return ActionType::Wait;
    
    // Epsilon-greedy action selection
    float random = static_cast<float>(rand()) / RAND_MAX;
    
    if (random < epsilon_) {
        // Explore: random action
        return availableActions[rand() % availableActions.size()];
    } else {
        // Exploit: best Q-value action
        float bestValue = -std::numeric_limits<float>::max();
        ActionType bestAction = ActionType::Wait;
        
        for (ActionType action : availableActions) {
            float value = (qValues.find(action) != qValues.end()) ? qValues.at(action) : 0.0f;
            if (value > bestValue) {
                bestValue = value;
                bestAction = action;
            }
        }
        
        return bestAction;
    }
}

void QLearner::updateQValue(ActionType action, float reward, ActionType nextAction, float maxNextQ) {
    float currentQ = 0.0f;
    // In a real implementation, you'd retrieve current Q-value from Q-table
    
    float target = reward + gamma_ * maxNextQ;
    float newQ = currentQ + alpha_ * (target - currentQ);
    
    // Update Q-table (would be stored externally)
}

// ==================== POLICY GRADIENT LEARNER IMPLEMENTATION ====================

PolicyGradientLearner::PolicyGradientLearner(float learningRate, float entropyCoeff)
    : learningRate_(learningRate), entropyCoeff_(entropyCoeff) {
}

void PolicyGradientLearner::updatePolicy(const std::vector<ActionType>& actions,
                                        const std::vector<float>& advantages,
                                        const std::vector<float>& logProbabilities) {
    if (actions.size() != advantages.size() || actions.size() != logProbabilities.size()) {
        return;
    }
    
    // Policy gradient update: ∇θ J(θ) = E[∇ log π(a|s) * A(s,a)]
    for (size_t i = 0; i < actions.size(); ++i) {
        float gradient = advantages[i] * logProbabilities[i];
        // Update policy parameters (would be stored externally)
    }
}

float PolicyGradientLearner::getActionProbability(const std::vector<ActionType>& actions, 
                                                 ActionType action) const {
    // Compute probability of action using current policy
    // This would depend on the current policy parameters
    return 1.0f / static_cast<float>(actions.size());  // Placeholder
}

// ==================== ACTOR-CRITIC LEARNER IMPLEMENTATION ====================

ActorCriticLearner::ActorCriticLearner(float actorAlpha, float criticAlpha,
                                       float gamma, float tau)
    : actorAlpha_(actorAlpha), criticAlpha_(criticAlpha), gamma_(gamma), tau_(tau) {
}

void ActorCriticLearner::update(const std::vector<ActionType>& states,
                               const std::vector<ActionType>& actions,
                               const std::vector<float>& rewards,
                               const std::vector<ActionType>& nextStates,
                               const std::vector<float>& dones) {
    if (states.size() != actions.size() || states.size() != rewards.size() ||
        states.size() != nextStates.size() || states.size() != dones.size()) {
        return;
    }
    
    // Simple temporal difference error computation
    for (size_t i = 0; i < states.size(); ++i) {
        float currentValue = getValue(std::vector<float>(states[i]));  // Would extract features
        float nextValue = 0.0f;
        
        if (!dones[i]) {
            nextValue = getValue(std::vector<float>(nextStates[i]));  // Would extract features
        }
        
        float tdError = rewards[i] + gamma_ * nextValue - currentValue;
        
        // Update critic (value function)
        // criticGradient = -tdError * ∇V(s)
        
        // Update actor (policy)
        // actorGradient = tdError * ∇ log π(a|s)
    }
}

ActionType ActorCriticLearner::selectAction(const std::vector<ActionType>& availableActions,
                                           const std::vector<float>& stateFeatures) {
    if (availableActions.empty()) return ActionType::Wait;
    
    // Softmax action selection based on current policy
    // This would use the actor network parameters
    float totalProb = 0.0f;
    std::vector<float> probs;
    
    for (ActionType action : availableActions) {
        float prob = getActionProbability(availableActions, action);  // Would compute from policy
        probs.push_back(prob);
        totalProb += prob;
    }
    
    if (totalProb <= 0.0f) {
        return availableActions[0];
    }
    
    // Normalize probabilities
    for (float& prob : probs) {
        prob /= totalProb;
    }
    
    // Sample action
    float threshold = static_cast<float>(rand()) / RAND_MAX * totalProb;
    float cumulative = 0.0f;
    
    for (size_t i = 0; i < probs.size(); ++i) {
        cumulative += probs[i];
        if (threshold <= cumulative) {
            return availableActions[i];
        }
    }
    
    return availableActions[0];
}

float ActorCriticLearner::getValue(const std::vector<float>& stateFeatures) const {
    // Evaluate state using critic network
    // Would use current critic parameters
    return 0.0f;  // Placeholder
}

// ==================== DOPAMINE MODULATOR IMPLEMENTATION ====================

DopamineModulator::DopamineModulator(float learningRate, float predictionErrorDecay)
    : learningRate_(learningRate), predictionErrorDecay_(predictionErrorDecay),
      dopamineLevel_(0.0f), predictionError_(0.0f), predictedReward_(0.0f) {
}

void DopamineModulator::update(float reward, float predictedReward, float learningRate) {
    float lr = (learningRate > 0.0f) ? learningRate : learningRate_;
    
    // Compute prediction error
    predictionError_ = reward - predictedReward_;
    
    // Update predicted reward with decay
    predictedReward_ = predictionErrorDecay_ * predictedReward_ + 
                       (1.0f - predictionErrorDecay_) * reward;
    
    // Update dopamine level based on prediction error
    dopamineLevel_ = predictionError_ * lr;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
}

void DopamineModulator::adaptLearningRate(float environmentalStability) {
    // Adapt learning rate based on environmental stability
    // High stability -> higher learning rate
    // Low stability -> lower learning rate
    float stabilityFactor = std::clamp(environmentalStability, 0.1f, 1.0f);
    learningRate_ = 0.01f * stabilityFactor;
}

// ==================== HEBBIAN SKILL LEARNER IMPLEMENTATION ====================

HebbianSkillLearner::HebbianSkillLearner(float learningRate, float consolidationRate)
    : learningRate_(learningRate), consolidationRate_(consolidationRate), rng_(std::random_device()()) {
}

void HebbianSkillLearner::learnAssociation(const std::vector<float>& sensoryInput,
                                          MotorCommand motorOutput,
                                          float reward) {
    // Create hash for sensory input pattern
    size_t inputHash = 0;
    for (float val : sensoryInput) {
        // Simple hash function
        inputHash = (inputHash * 31 + static_cast<size_t>(val * 100.0f)) % 1000000;
    }
    
    auto& actionMap = associations_[inputHash];
    float& strength = actionMap[motorOutput];
    
    // Hebbian learning: strengthen connection based on reward
    strength += learningRate_ * reward;
    
    // Apply consolidation decay
    strength *= (1.0f - consolidationRate_);
    
    // Clamp strength to reasonable range
    strength = std::clamp(strength, 0.0f, 1.0f);
}

MotorCommand HebbianSkillLearner::retrieveMotorResponse(const std::vector<float>& sensoryInput) const {
    if (associations_.empty()) return MotorCommand::Wait;
    
    // Create hash for sensory input pattern
    size_t inputHash = 0;
    for (float val : sensoryInput) {
        inputHash = (inputHash * 31 + static_cast<size_t>(val * 100.0f)) % 1000000;
    }
    
    auto it = associations_.find(inputHash);
    if (it == associations_.end()) return MotorCommand::Wait;
    
    // Select motor output with highest strength
    float maxStrength = -1.0f;
    MotorCommand bestCommand = MotorCommand::Wait;
    
    for (const auto& pair : it->second) {
        if (pair.second > maxStrength) {
            maxStrength = pair.second;
            bestCommand = pair.first;
        }
    }
    
    return bestCommand;
}

void HebbianSkillLearner::consolidateSkills() {
    // Apply consolidation to all associations
    for (auto& inputMap : associations_) {
        for (auto& actionPair : inputMap.second) {
            actionPair.second *= (1.0f - consolidationRate_);
        }
    }
}

float HebbianSkillLearner::getSkillStrength(const std::vector<float>& sensoryInput,
                                           MotorCommand motorOutput) const {
    size_t inputHash = 0;
    for (float val : sensoryInput) {
        inputHash = (inputHash * 31 + static_cast<size_t>(val * 100.0f)) % 1000000;
    }
    
    auto it = associations_.find(inputHash);
    if (it == associations_.end()) return 0.0f;
    
    auto actionIt = it->second.find(motorOutput);
    return (actionIt != it->second.end()) ? actionIt->second : 0.0f;
}

// ==================== MOTOR MEMORY IMPLEMENTATION ====================

MotorMemory::MotorMemory(size_t capacity) : capacity_(capacity), rng_(std::random_device()()) {
}

void MotorMemory::storeMotorPattern(const std::vector<MotorCommand>& pattern,
                                   const std::vector<float>& sensoryContext,
                                   float reward) {
    // Trim memory if at capacity
    if (patterns_.size() >= capacity_) {
        // Remove lowest usage patterns (simple approach)
        std::sort(patterns_.begin(), patterns_.end(),
                  [](const MotorPattern& a, const MotorPattern& b) {
                      return a.usageCount < b.usageCount;
                  });
        patterns_.erase(patterns_.begin(), patterns_.begin() + (capacity_ / 4));
    }
    
    // Create new pattern
    MotorPattern newPattern;
    newPattern.commands = pattern;
    newPattern.context = sensoryContext;
    newPattern.reward = reward;
    newPattern.usageCount = 1;
    newPattern.lastUsed = 0.0f;
    
    patterns_.push_back(newPattern);
}

std::unique_ptr<ActionSequence> MotorMemory::retrievePattern(const std::vector<float>& sensoryContext,
                                                           float similarityThreshold) {
    if (patterns_.empty()) return nullptr;
    
    std::vector<MotorPattern*> bestCandidates;
    float bestSimilarity = 0.0f;
    
    for (auto& pattern : patterns_) {
        // Compute similarity between contexts
        float similarity = 0.0f;
        if (pattern.context.size() == sensoryContext.size()) {
            float sum = 0.0f;
            for (size_t i = 0; i < pattern.context.size(); ++i) {
                similarity += 1.0f - std::abs(pattern.context[i] - sensoryContext[i]);
            }
            similarity /= pattern.context.size();
        }
        
        if (similarity >= similarityThreshold && similarity > bestSimilarity) {
            bestSimilarity = similarity;
            bestCandidates.clear();
            bestCandidates.push_back(&pattern);
        } else if (similarity >= similarityThreshold && similarity == bestSimilarity) {
            bestCandidates.push_back(&pattern);
        }
    }
    
    if (bestCandidates.empty()) return nullptr;
    
    // Select the pattern with highest reward among best matches
    MotorPattern* selectedPattern = bestCandidates[0];
    for (MotorPattern* candidate : bestCandidates) {
        if (candidate->reward > selectedPattern->reward) {
            selectedPattern = candidate;
        }
    }
    
    // Create action sequence from pattern
    auto sequence = std::make_unique<ActionSequence>();
    for (MotorCommand cmd : selectedPattern->commands) {
        sequence->addCommand(cmd, 0.1f, 1.0f);  // Default values
    }
    sequence->goalDescription = "Retrieved motor pattern";
    
    return sequence;
}

void MotorMemory::updatePatternUsage(const std::vector<MotorCommand>& pattern) {
    for (auto& storedPattern : patterns_) {
        if (storedPattern.commands == pattern) {
            storedPattern.usageCount++;
            storedPattern.lastUsed = 0.0f;  // Would use current time
            break;
        }
    }
}

float MotorMemory::getPatternUsageCount(const std::vector<MotorCommand>& pattern) const {
    for (const auto& storedPattern : patterns_) {
        if (storedPattern.commands == pattern) {
            return static_cast<float>(storedPattern.usageCount);
        }
    }
    return 0.0f;
}

// ==================== FORWARD MODEL IMPLEMENTATION ====================

ForwardModel::ForwardModel() : rng_(std::random_device()()) {
}

void ForwardModel::learnActionOutcome(MotorCommand action, const std::vector<float>& expectedSensory) {
    actionOutcomes_[action] = expectedSensory;
}

std::vector<float> ForwardModel::predictSensory(MotorCommand action) const {
    auto it = actionOutcomes_.find(action);
    if (it != actionOutcomes_.end()) {
        return it->second;
    }
    
    // Return zero vector if no prediction available
    return std::vector<float>(256, 0.0f);  // Assuming 256 vision values
}

float ForwardModel::getPredictionError(MotorCommand action, const std::vector<float>& actualSensory) const {
    std::vector<float> predicted = predictSensory(action);
    
    if (predicted.size() != actualSensory.size()) {
        return 1.0f;  // Maximum error if sizes differ
    }
    
    float error = 0.0f;
    for (size_t i = 0; i < predicted.size(); ++i) {
        error += std::abs(predicted[i] - actualSensory[i]);
    }
    
    return error / predicted.size();
}

// ==================== INTERNAL MODEL IMPLEMENTATION ====================

InternalModel::InternalModel() {
    // Initialize with default state
    internalState_.resize(64, 0.0f);  // 64-dimensional state space
    stateTransitions_.resize(64, std::vector<float>(64, 0.0f));
}

void InternalModel::updateWorldModel(const std::vector<float>& sensoryInput, MotorCommand action) {
    // Update internal state based on sensory input and action
    // This is a simplified implementation
    
    // Encode sensory input into internal representation
    for (size_t i = 0; i < std::min(sensoryInput.size(), internalState_.size()); ++i) {
        internalState_[i] = sensoryInput[i] * 0.1f;  // Scale down
    }
    
    // Update transition model (simplified)
    size_t actionIndex = static_cast<size_t>(action);
    if (actionIndex < stateTransitions_.size()) {
        // Simple state update
        size_t currentState = 0;  // Would compute from internal state
        size_t nextState = (currentState + actionIndex) % stateTransitions_.size();
        
        if (nextState < stateTransitions_[currentState].size()) {
            stateTransitions_[currentState][nextState] += 0.1f;
        }
    }
}

std::vector<float> InternalModel::predictFutureState(const std::vector<float>& action,
                                                     size_t steps) const {
    // Predict future state after taking action for given steps
    std::vector<float> futureState = internalState_;
    
    // Simple deterministic prediction
    for (size_t i = 0; i < steps; ++i) {
        if (futureState.empty()) break;
        
        // Apply action effect (simplified)
        size_t actionIndex = 0;
        if (!action.empty()) {
            actionIndex = static_cast<size_t>(action[0]);
        }
        
        // Update future state based on internal dynamics
        for (size_t j = 0; j < futureState.size(); ++j) {
            float noise = ((static_cast<float>(rand()) / RAND_MAX) - 0.5f) * 0.01f;
            futureState[j] = futureState[j] * 0.9f + noise;
        }
    }
    
    return futureState;
}

} // namespace nlm

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(brain)
    , dopamineLevel_(0.0f)
    , noveltyLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , predictionError_(0.0f)
    , expectedReward_(0.0f)
    , developmentalAge_(0.0)
    , plasticityModifier_(1.0f)
    , rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , curiosityEnabled_(true)
    , advancedMotorDecodingEnabled_(true)
    , motorPlanningEnabled_(true)
    , reinforcementLearningEnabled_(true)
    , skillLearningEnabled_(true)
    , sensoryMotorIntegrationEnabled_(true)
    , maxSequenceLength_(10)
    , hierarchicalControlEnabled_(true)
    , discountFactor_(0.99f)
    , explorationRate_(0.1f)
    , predictionModelEnabled_(true)
    , internalModelEnabled_(true)
    , sensoryNoveltyDecay_(0.99f)
{
    // Initialize motor and sensory neuron groups (legacy)
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Distribute motor neurons to different action groups
                        size_t idx = motorForward_.size() + motorBackward_.size() + 
                                     motorTurnLeft_.size() + motorTurnRight_.size() +
                                     motorInteract_.size() + motorWait_.size();
                        
                        switch (idx % 6) {
                            case 0: motorForward_.push_back(n); break;
                            case 1: motorBackward_.push_back(n); break;
                            case 2: motorTurnLeft_.push_back(n); break;
                            case 3: motorTurnRight_.push_back(n); break;
                            case 4: motorInteract_.push_back(n); break;
                            case 5: motorWait_.push_back(n); break;
                        }
                    }
                } else if (type == NeuronType::Sensory) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Distribute sensory neurons
                        size_t idx = sensoryVision_.size() + sensoryTouch_.size() +
                                     sensoryInternal_.size() + sensoryProprioception_.size();
                        
                        switch (idx % 4) {
                            case 0: sensoryVision_.push_back(n); break;
                            case 1: sensoryTouch_.push_back(n); break;
                            case 2: sensoryInternal_.push_back(n); break;
                            case 3: sensoryProprioception_.push_back(n); break;
                        }
                    }
                }
            }
        }
    }
    
    // Initialize advanced components
    initializeAdvancedComponents();
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
}

void AgentBrain::initializeAdvancedComponents() {
    // Set up advanced component parameters
    softmaxSelector_.setTemperature(1.0f);
    tdLearner_.setAlpha(0.01f);
    tdLearner_.setGamma(0.99f);
    
    qLearner_.setEpsilon(0.1f);
    qLearner_.setAlpha(0.1f);
    qLearner_.setGamma(0.99f);
    
    policyLearner_.setLearningRate(0.001f);
    policyLearner_.setEntropyCoeff(0.01f);
    
    actorCriticLearner_.setActorAlpha(0.001f);
    actorCriticLearner_.setCriticAlpha(0.01f);
    actorCriticLearner_.setGamma(0.99f);
    
    dopamineModulator_.adaptLearningRate(0.5f);
    
    hebbianLearner_.setLearningRate(0.001f);
    motorMemory_.setCapacity(100);
}

size_t AgentBrain::getSensoryInputSize() const {
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
    return 256 + 8 + 4 + 6;
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action
    return 6;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Inject current proportional to vision intensity
            float current = vision[i] * 5.0f;  // Scale factor
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * 8.0f;  // Collision signal
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
    
    // Compute novelty (difference from previous vision)
    if (!vision.empty()) {
        float totalDiff = 0.0f;
        for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Normalize
        noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
        
        // Decay and update
        noveltyLevel_ *= sensoryNoveltyDecay_;
        
        // Store for next time
        previousVision_ = vision;
    }
    
    // Update curiosity based on novelty
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
    
    // Update advanced components
    if (sensoryMotorIntegrationEnabled_) {
        // Update forward model prediction error
        std::vector<float> predictedSensory;
        for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
            if (sensoryVision_[i]) {
                predictedSensory.push_back(vision[i]);
            }
        }
        
        // Compute prediction error
        if (predictedSensory.size() == vision.size()) {
            float predError = forwardModel_.getPredictionError(MotorCommand::Wait, vision);
            predictionError_ = predError;
        }
    }
    
    // Update internal model
    if (internalModelEnabled_) {
        internalModel_.updateWorldModel(vision, MotorCommand::Wait);
    }
    
    // Update skill learning based on experience
    if (skillLearningEnabled_) {
        // Learn from current sensory pattern
        // Would extract features for Hebbian learning
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded;
    
    if (advancedMotorDecodingEnabled_) {
        // Enhanced motor decoding using advanced systems
        decoded = getAdvancedMotorCommand(calculateNeuralActivities());
        
        // Apply motor planning if enabled
        if (motorPlanningEnabled_) {
            updateActionSequence();
            if (currentSequence_ && !currentSequence_->isComplete()) {
                MotorCommand plannedCmd = currentSequence_->executeNext();
                // Blend enhanced decoding with planned sequence
                decoded = blendCommands(decoded, plannedCmd);
            }
        }
        
        // Apply curiosity-based exploration
        if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
            decoded = selectWithCuriosity(decoded);
        }
    } else {
        // Legacy motor decoding
        decoded = decodeFromMotorNeurons();
        
        if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
            decoded = selectWithCuriosity(decoded);
        }
    }
    
    return decoded;
}

std::vector<float> AgentBrain::calculateNeuralActivities() const {
    std::vector<float> activities(6, 0.0f);  // One per motor command
    
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            // Use membrane potential deviation from rest as activity measure
            sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
        return sum / neurons.size();
    };
    
    activities[0] = calcActivity(motorForward_);      // MoveForward
    activities[1] = calcActivity(motorBackward_);     // MoveBackward
    activities[2] = calcActivity(motorTurnLeft_);     // TurnLeft
    activities[3] = calcActivity(motorTurnRight_);    // TurnRight
    activities[4] = calcActivity(motorInteract_);     // Interact
    activities[5] = calcActivity(motorWait_);         // Wait
    
    return activities;
}

MotorCommand AgentBrain::getAdvancedMotorCommand(const std::vector<float>& neuralActivity) {
    // Use softmax action selector for advanced decoding
    std::vector<ActionType> actionTypes = {
        ActionType::MoveForward, ActionType::MoveBackward,
        ActionType::MoveLeft, ActionType::MoveRight,
        ActionType::Look, ActionType::Interact, ActionType::Wait
    };
    
    // Convert to float activities for softmax
    std::vector<float> floatActivities = neuralActivity;
    while (floatActivities.size() < actionTypes.size()) {
        floatActivities.push_back(0.0f);
    }
    
    // Use softmax selector
    MotorCommand motorCmd = softmaxSelector_.selectAction(floatActivities);
    
    // Update with RL values if enabled
    if (reinforcementLearningEnabled_) {
        float bestValue = -std::numeric_limits<float>::max();
        ActionType bestAction = ActionType::Wait;
        
        for (size_t i = 0; i < actionTypes.size(); ++i) {
            float value = tdLearner_.getActionValue(actionTypes[i]);
            if (value > bestValue) {
                bestValue = value;
                bestAction = actionTypes[i];
            }
        }
        
        // Blend softmax choice with Q-learning if exploration is needed
        if (curiosityLevel_ > 0.5f) {
            // High curiosity -> prefer exploration over exploitation
            float randomFactor = curiosityLevel_ * 0.5f;
            if ((static_cast<float>(rand()) / RAND_MAX) < randomFactor) {
                // Sample from softmax distribution
                return motorCmd;
            }
        }
    }
    
    return motorCmd;
}

MotorCommand AgentBrain::blendCommands(MotorCommand primary, MotorCommand secondary) const {
    // Blend two motor commands based on confidence
    // Simple averaging for now
    float primaryActivity = getCommandActivity(primary);
    float secondaryActivity = getCommandActivity(secondary);
    
    float totalActivity = primaryActivity + secondaryActivity;
    if (totalActivity <= 0.0f) return primary;
    
    float primaryWeight = primaryActivity / totalActivity;
    
    // Choose based on confidence
    return (primaryWeight > 0.6f) ? primary : secondary;
}

float AgentBrain::getCommandActivity(MotorCommand cmd) const {
    switch (cmd) {
        case MotorCommand::MoveForward: return calcActivity(motorForward_);
        case MotorCommand::MoveBackward: return calcActivity(motorBackward_);
        case MotorCommand::TurnLeft: return calcActivity(motorTurnLeft_);
        case MotorCommand::TurnRight: return calcActivity(motorTurnRight_);
        case MotorCommand::LookLeft: return calcActivity(motorTurnLeft_);
        case MotorCommand::LookRight: return calcActivity(motorTurnRight_);
        case MotorCommand::Interact: return calcActivity(motorInteract_);
        case MotorCommand::Wait: return calcActivity(motorWait_);
        default: return 0.0f;
    }
}

void AgentBrain::updateActionSequence() {
    if (!hierarchicalControlEnabled_) return;
    
    // Get current context from internal model
    std::vector<float> context = internalModel_.getInternalState();
    
    // Plan new sequence if none exists or current is complete
    if (!currentSequence_ || currentSequence_->isComplete()) {
        if (brain_->getDevelopmentSystem()) {
            // Get current goal from development system if available
            std::string goal = "ExploreArea";  // Default goal
            currentSequence_ = hierarchicalController_.planSequence(context);
        }
    }
}

std::unique_ptr<ActionSequence> AgentBrain::getCurrentActionSequence() const {
    if (currentSequence_) {
        return std::make_unique<ActionSequence>(*currentSequence_);
    }
    return nullptr;
}

bool AgentBrain::isActionSequenceComplete() const {
    return !currentSequence_ || currentSequence_->isComplete();
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Original legacy implementation (from previous version)
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            // Use membrane potential deviation from rest as activity measure
            sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
        return sum / neurons.size();
    };
    
    float forwardAct = calcActivity(motorForward_);
    float backwardAct = calcActivity(motorBackward_);
    float leftAct = calcActivity(motorTurnLeft_);
    float rightAct = calcActivity(motorTurnRight_);
    float interactAct = calcActivity(motorInteract_);
    float waitAct = calcActivity(motorWait_);
    
    // Find maximum activity
    struct { MotorCommand cmd; float activity; } commands[] = {
        {MotorCommand::MoveForward, forwardAct},
        {MotorCommand::MoveBackward, backwardAct},
        {MotorCommand::TurnLeft, leftAct},
        {MotorCommand::TurnRight, rightAct},
        {MotorCommand::LookLeft, leftAct},
        {MotorCommand::LookRight, rightAct},
        {MotorCommand::Interact, interactAct},
        {MotorCommand::Wait, waitAct}
    };
    
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct;  // Default to wait if nothing stronger
    
    for (const auto& c : commands) {
        if (c.activity > bestActivity) {
            bestActivity = c.activity;
            best = c.cmd;
        }
    }
    
    // Only act if there's meaningful activity
    if (bestActivity < 0.5f) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Enhanced curiosity-based exploration
    if (curiosityLevel_ > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * 0.3f;  // Up to 30% random
        
        float r = static_cast<float>(rand()) / RAND_MAX;
        if (r < exploreChance) {
            // Random motor command
            int choice = rand() % 8;
            switch (choice) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::TurnLeft;
                case 3: return MotorCommand::TurnRight;
                case 4: return MotorCommand::LookLeft;
                case 5: return MotorCommand::LookRight;
                case 6: return MotorCommand::Interact;
                default: return MotorCommand::Wait;
            }
        }
    }
    
    return defaultCmd;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Advanced dopamine-based learning
    if (reinforcementLearningEnabled_) {
        dopamineModulator_.update(reward, predictedReward);
    } else {
        // Original dopamine-like signal (based on prediction error)
        dopamineLevel_ = predictionError_;
    }
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
    
    // Modulate plasticity based on dopamine
    if (reinforcementLearningEnabled_) {
        // Advanced plasticity modulation
        float plasticityFactor = 0.5f + 0.5f * dopamineModulator_.getDopamineLevel();
        plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
        
        plasticityModifier_ = plasticityFactor;
    } else {
        // Original plasticity modulation
        float plasticityFactor = 0.5f + 0.5f * dopamineLevel_;
        plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
        
        // Apply to STDP
        auto* stdp = brain_->getSTDP();
        if (stdp) {
            stdp->setLTPWeight(0.01f * plasticityFactor);
            stdp->setLTDWeight(0.012f * plasticityFactor);
        }
    }
    
    // Update reinforcement learning if enabled
    if (reinforcementLearningEnabled_) {
        // TD learning update
        tdLearner_.updateActionValue(
            ActionType::Wait,  // Would get from current action
            reward,
            0.0f  // Would get from next state value
        );
        
        // Actor-critic update (would need more state information)
        // actorCriticLearner_.update(...);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // Simple developmental stages based on age
    // This is a biologically inspired approximation
    if (developmentalAge_ < 60.0) {  // ~1 minute
        plasticityModifier_ = 1.0f;  // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < 300.0) {  // ~5 minutes
        plasticityModifier_ = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < 900.0) {  // ~15 minutes
        plasticityModifier_ = 0.5f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = 0.2f;  // Adult - more stable
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Structural plasticity changes with development
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // Higher synaptogenesis in early development
            float synRate = 0.0001f * plasticityModifier_;
            float pruneRate = 0.00001f * (2.0f - plasticityModifier_);
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
    }
    
    // Update skill learning consolidation
    if (skillLearningEnabled_) {
        hebbianLearner_.consolidateSkills();
    }
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!brain_) return DevelopmentalStage::Initial;
    return brain_->getDevelopmentalStage();
}

float AgentBrain::getNeuromodulationLevel() const {
    return dopamineLevel_;
}

float AgentBrain::getCuriosityLevel() const {
    return curiosityLevel_;
}

float AgentBrain::getNoveltyLevel() const {
    return noveltyLevel_;
}

float AgentBrain::getPredictionError() const {
    return predictionError_;
}

void AgentBrain::setGoal(const std::string& goal, const std::vector<float>& parameters) {
    if (hierarchicalControlEnabled_) {
        hierarchicalController_.setGoal(goal, parameters);
        updateActionSequence();
    }
}

float AgentBrain::getSkillStrength(const std::vector<float>& sensoryInput,
                                  MotorCommand motorOutput) const {
    if (skillLearningEnabled_) {
        return hebbianLearner_.getSkillStrength(sensoryInput, motorOutput);
    }
    return 0.0f;
}

std::vector<float> AgentBrain::getPredictedSensory(MotorCommand action) const {
    if (predictionModelEnabled_) {
        return forwardModel_.predictSensory(action);
    }
    return std::vector<float>();
}

void AgentBrain::reset() {
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Reset advanced components
    tdLearner_.reset();
    dopamineModulator_.reset();
    hebbianLearner_.resetAssociations();
    motorMemory_.clear();
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    
    // Clear current sequence
    currentSequence_.reset();
}

void AgentBrain::setQLearningParameters(float alpha, float gamma, float epsilon) {
    if (reinforcementLearningEnabled_) {
        qLearner_.setAlpha(alpha);
        qLearner_.setGamma(gamma);
        qLearner_.setEpsilon(epsilon);
    }
}

void AgentBrain::setPolicyGradientParameters(float learningRate, float entropyCoeff) {
    if (reinforcementLearningEnabled_) {
        policyLearner_.setLearningRate(learningRate);
        policyLearner_.setEntropyCoeff(entropyCoeff);
    }
}

// Helper methods for legacy support
ActionType AgentBrain::getLegacyMotorCommandAsAction(MotorCommand cmd) const {
    switch (cmd) {
        case MotorCommand::MoveForward: return ActionType::MoveForward;
        case MotorCommand::MoveBackward: return ActionType::MoveBackward;
        case MotorCommand::TurnLeft: return ActionType::TurnLeft;
        case MotorCommand::TurnRight: return ActionType::TurnRight;
        case MotorCommand::LookLeft: return ActionType::MoveLeft;
        case MotorCommand::LookRight: return ActionType::MoveRight;
        case MotorCommand::Interact: return ActionType::Interact;
        case MotorCommand::Wait: return ActionType::Wait;
        default: return ActionType::Wait;
    }
}

} // namespace nlm