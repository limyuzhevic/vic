#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <unordered_map>
#include <queue>
#include <numeric>
#include <chrono>

using namespace nlm;

// Advanced action selection strategies
class AdvancedActionSelector {
public:
    struct ActionValue {
        MotorCommand command;
        float value;
        float visitCount;
        float predictionError;
        float timestamp;
        std::vector<float> features;
        
        ActionValue(MotorCommand cmd, float val = 0.0f) 
            : command(cmd), value(val), visitCount(0), predictionError(0.0f), timestamp(0.0f) {}
    };
    
    // Thompson sampling for Bayesian bandits
    class ThompsonSampler {
    public:
        struct BetaDistribution {
            float alpha;  // Successes
            float beta;   // Failures
            float mean() const { return alpha / (alpha + beta); }
            float variance() const { return (alpha * beta) / ((alpha + beta) * (alpha + beta) * (alpha + beta + 1)); }
        };
        
        ThompsonSampler() : rng(std::random_device{}()) {}
        
        float sampleBeta(float alpha, float beta) {
            std::beta_distribution<float> dist(alpha, beta);
            return dist(rng);
        }
        
        void update(int action, bool success) {
            if (success) {
                betaDistributions[action].alpha++;
            } else {
                betaDistributions[action].beta++;
            }
        }
        
        float getProbability(int action) const {
            return betaDistributions[action].mean();
        }
        
        void initialize(int numActions) {
            betaDistributions.resize(numActions, {1.0f, 1.0f});
        }
        
    private:
        std::mt19937 rng;
        std::vector<BetaDistribution> betaDistributions;
    };
    
    // Hierarchical action selection
    struct HierarchicalPolicy {
        MotorCommand highLevelAction;    // Basic category (move, interact, wait)
        MotorCommand lowLevelAction;     // Specific action
        float highLevelProb;
        float lowLevelProb;
        double lastExecutionTime;
        
        HierarchicalPolicy() : highLevelAction(MotorCommand::Wait), 
                              lowLevelAction(MotorCommand::Wait),
                              highLevelProb(0.0f), lowLevelProb(0.0f),
                              lastExecutionTime(0.0) {}
    };
    
    // Episodic memory for action-outcome associations
    struct ActionOutcome {
        MotorCommand action;
        float outcomeValue;
        double timestamp;
        std::vector<float> stateFeatures;
        std::vector<MotorCommand> subsequentActions;
        
        ActionOutcome(MotorCommand cmd, float val, double time)
            : action(cmd), outcomeValue(val), timestamp(time) {}
    };
    
    // Q-learning with experience replay
    struct QLearningAgent {
        std::unordered_map<size_t, std::vector<float>> Q_table;  // state -> action values
        std::vector<MotorCommand> actions;
        float learningRate;
        float discountFactor;
        float explorationRate;
        float maxExperienceReplaySize;
        std::queue<ActionOutcome> experienceReplay;
        
        QLearningAgent() : learningRate(0.1f), discountFactor(0.95f), 
                          explorationRate(0.1f), maxExperienceReplaySize(1000) {
            actions = {MotorCommand::MoveForward, MotorCommand::MoveBackward,
                      MotorCommand::TurnLeft, MotorCommand::TurnRight,
                      MotorCommand::Interact, MotorCommand::Wait,
                      MotorCommand::LookLeft, MotorCommand::LookRight};
        }
        
        size_t hashState(const std::vector<float>& state) {
            std::hash<float> hasher;
            size_t hash = 0;
            for (float val : state) {
                hash ^= hasher(val) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
            }
            return hash;
        }
        
        float getActionValue(const std::vector<float>& state, MotorCommand action) {
            size_t stateHash = hashState(state);
            if (Q_table.find(stateHash) == Q_table.end()) {
                return 0.0f;
            }
            const auto& values = Q_table[stateHash];
            for (size_t i = 0; i < actions.size(); ++i) {
                if (actions[i] == action) {
                    return values[i];
                }
            }
            return 0.0f;
        }
        
        void updateQValue(const std::vector<float>& state, MotorCommand action, 
                         float reward, const std::vector<float>& nextState) {
            size_t stateHash = hashState(state);
            size_t nextHash = hashState(nextState);
            
            float currentValue = getActionValue(state, action);
            float maxNextValue = 0.0f;
            if (Q_table.find(nextHash) != Q_table.end()) {
                const auto& nextValues = Q_table[nextHash];
                for (float val : nextValues) {
                    maxNextValue = std::max(maxNextValue, val);
                }
            }
            
            float target = reward + discountFactor * maxNextValue;
            float newValue = currentValue + learningRate * (target - currentValue);
            
            if (Q_table.find(stateHash) == Q_table.end()) {
                Q_table[stateHash] = std::vector<float>(actions.size(), 0.0f);
            }
            
            for (size_t i = 0; i < actions.size(); ++i) {
                if (actions[i] == action) {
                    Q_table[stateHash][i] = newValue;
                    break;
                }
            }
        }
        
        MotorCommand selectAction(const std::vector<float>& state) {
            // Epsilon-greedy with decaying exploration
            float epsilon = explorationRate * std::exp(-0.001 * experienceReplay.size());
            
            if (std::uniform_real_distribution<float>(0.0f, 1.0f)(rng) < epsilon) {
                return actions[std::uniform_int_distribution<size_t>(0, actions.size() - 1)(rng)];
            }
            
            // Select best action
            float bestValue = std::numeric_limits<float>::lowest();
            MotorCommand bestAction = actions[0];
            
            for (MotorCommand action : actions) {
                float value = getActionValue(state, action);
                if (value > bestValue) {
                    bestValue = value;
                    bestAction = action;
                }
            }
            
            return bestAction;
        }
        
        void addExperience(const ActionOutcome& experience) {
            if (experienceReplay.size() >= maxExperienceReplaySize) {
                experienceReplay.pop();
            }
            experienceReplay.push(experience);
            
            // Sample from experience replay for offline learning
            std::uniform_int_distribution<size_t> dist(0, std::min(size_t(10), experienceReplay.size()) - 1);
            size_t sampleIndex = dist(rng);
            
            auto it = std::next(experienceReplay.begin(), sampleIndex);
            const ActionOutcome& sample = *it;
            
            // Apply Q-learning update
            std::vector<float> dummyState; // Would need to store state in ActionOutcome
            updateQValue(dummyState, sample.action, sample.outcomeValue, dummyState);
        }
        
    private:
        std::mt19937 rng;
    };
    
public:
    AdvancedActionSelector() : curiosityEnabled_(true), rewardModulationEnabled_(true),
                              developmentEnabled_(true), structuralPlasticityEnabled_(true) {}
    
    // Initialize action selector with state and action space
    void initialize(int numHighLevelActions, int numLowLevelActions, int numActionTypes) {
        thompsonSampler_.initialize(numHighLevelActions + numLowLevelActions);
        
        // Initialize action values
        actionValues_.resize(numActionTypes);
        for (auto& action : actionValues_) {
            action.command = MotorCommand::Wait;
            action.value = 0.0f;
            action.visitCount = 0;
            action.predictionError = 0.0f;
        }
        
        // Initialize hierarchical policy
        hierarchicalPolicy_.highLevelAction = MotorCommand::Wait;
        hierarchicalPolicy_.lowLevelAction = MotorCommand::Wait;
        
        // Initialize Q-learning agent
        qLearningAgent_.actions = {MotorCommand::MoveForward, MotorCommand::MoveBackward,
                                   MotorCommand::TurnLeft, MotorCommand::TurnRight,
                                   MotorCommand::Interact, MotorCommand::Wait,
                                   MotorCommand::LookLeft, MotorCommand::LookRight};
    }
    
    // Select action using advanced strategies
    MotorCommand selectAction(const std::vector<float>& stateFeatures,
                            float curiosity, float reward, double timestamp) {
        // Update timestamp
        currentTimestamp_ = timestamp;
        
        // Strategy 1: Thompson sampling for exploration/exploitation
        MotorCommand thompsonAction = thompsonSamplingSelection(stateFeatures);
        
        // Strategy 2: Hierarchical action selection
        MotorCommand hierarchicalAction = hierarchicalSelection(stateFeatures);
        
        // Strategy 3: Q-learning for model-based planning
        MotorCommand qLearningAction = qLearningSelection(stateFeatures);
        
        // Strategy 4: Curiosity-driven exploration
        MotorCommand curiosityAction = curiosityDrivenSelection(curiosity, stateFeatures);
        
        // Combine strategies using weighted voting
        return combineStrategies({thompsonAction, hierarchicalAction, qLearningAction, curiosityAction});
    }
    
    // Update action values based on outcome
    void updateActionValue(MotorCommand action, float outcomeValue, bool success, 
                         const std::vector<float>& stateFeatures) {
        // Update Thompson sampling
        thompsonSampler_.update(static_cast<int>(action), success);
        
        // Update action value
        for (auto& actionVal : actionValues_) {
            if (actionVal.command == action) {
                // Temporal difference learning
                float tdError = outcomeValue - actionVal.value;
                actionVal.predictionError = tdError;
                actionVal.value += 0.1f * tdError;  // Learning rate
                actionVal.visitCount++;
                actionVal.timestamp = currentTimestamp_;
                
                // Update Q-learning
                ActionOutcome experience(action, outcomeValue, currentTimestamp_);
                experience.stateFeatures = stateFeatures;
                qLearningAgent_.addExperience(experience);
                break;
            }
        }
        
        // Update hierarchical policy
        updateHierarchicalPolicy(action, outcomeValue);
    }
    
    // Get most valuable action based on current state
    MotorCommand getBestAction(const std::vector<float>& stateFeatures) {
        MotorCommand bestAction = MotorCommand::Wait;
        float bestValue = std::numeric_limits<float>::lowest();
        
        for (const auto& actionVal : actionValues_) {
            // Consider prediction error as exploration bonus
            float explorationBonus = std::abs(actionVal.predictionError) * 0.5f;
            float totalValue = actionVal.value + explorationBonus;
            
            // Consider recency (more recently visited actions might be more relevant)
            float recencyBonus = (currentTimestamp_ - actionVal.timestamp) * 0.01f;
            
            float adjustedValue = totalValue + recencyBonus;
            
            if (adjustedValue > bestValue) {
                bestValue = adjustedValue;
                bestAction = actionVal.command;
            }
        }
        
        return bestAction;
    }
    
    // Get action statistics for monitoring
    std::string getActionStatistics() const {
        std::stringstream ss;
        ss << "Action Values:";
        for (const auto& action : actionValues_) {
            ss << " " << static_cast<int>(action.command) << ": " 
               << std::fixed << std::setprecision(3) << action.value
               << " (visits: " << action.visitCount << ")";
        }
        return ss.str();
    }
    
    // Reset all action values
    void reset() {
        for (auto& action : actionValues_) {
            action.value = 0.0f;
            action.visitCount = 0;
            action.predictionError = 0.0f;
        }
        thompsonSampler_.initialize(static_cast<int>(MotorCommand::LookRight) + 1);
    }
    
    // Configuration
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    
private:
    MotorCommand thompsonSamplingSelection(const std::vector<float>& stateFeatures) {
        // Simple Thompson sampling for now - can be enhanced with state-dependent priors
        float sample = thompsonSampler_.getProbability(static_cast<int>(MotorCommand::MoveForward));
        
        if (sample > 0.6f) return MotorCommand::MoveForward;
        if (sample > 0.4f) return MotorCommand::MoveBackward;
        if (sample > 0.3f) return MotorCommand::TurnLeft;
        if (sample > 0.2f) return MotorCommand::TurnRight;
        if (sample > 0.1f) return MotorCommand::Interact;
        if (sample > 0.05f) return MotorCommand::LookLeft;
        if (sample > 0.02f) return MotorCommand::LookRight;
        return MotorCommand::Wait;
    }
    
    MotorCommand hierarchicalSelection(const std::vector<float>& stateFeatures) {
        // Determine high-level action category
        float forwardScore = computeForwardScore(stateFeatures);
        float exploreScore = computeExploreScore(stateFeatures);
        float interactScore = computeInteractScore(stateFeatures);
        
        MotorCommand highAction = MotorCommand::Wait;
        if (forwardScore > exploreScore && forwardScore > interactScore) {
            highAction = MotorCommand::MoveForward;
        } else if (exploreScore > interactScore) {
            highAction = MotorCommand::TurnLeft;  // Default exploration action
        } else {
            highAction = MotorCommand::Interact;
        }
        
        // Select low-level action within high-level category
        MotorCommand lowAction = selectLowLevelAction(highAction, stateFeatures);
        
        // For now, return the combined action (simplified)
        return highAction;
    }
    
    MotorCommand qLearningSelection(const std::vector<float>& stateFeatures) {
        return qLearningAgent_.selectAction(stateFeatures);
    }
    
    MotorCommand curiosityDrivenSelection(float curiosity, const std::vector<float>& stateFeatures) {
        if (!curiosityEnabled_ || curiosity < 0.1f) {
            return MotorCommand::Wait;
        }
        
        // Compute novelty of current state
        float novelty = computeNovelty(stateFeatures);
        
        if (novelty > 0.5f && curiosity > 0.3f) {
            // High novelty + high curiosity = exploration
            return getRandomAction();
        }
        
        // Medium novelty = balanced exploration/exploitation
        if (novelty > 0.2f) {
            return getBestAction(stateFeatures);
        }
        
        return MotorCommand::Wait;
    }
    
    MotorCommand combineStrategies(const std::vector<MotorCommand>& strategies) {
        // Simple voting mechanism
        std::unordered_map<MotorCommand, int> votes;
        for (MotorCommand action : strategies) {
            votes[action]++;
        }
        
        // Find action with most votes
        MotorCommand bestAction = MotorCommand::Wait;
        int maxVotes = 0;
        
        for (const auto& pair : votes) {
            if (pair.second > maxVotes) {
                maxVotes = pair.second;
                bestAction = pair.first;
            }
        }
        
        return bestAction;
    }
    
    MotorCommand selectLowLevelAction(MotorCommand highAction, const std::vector<float>& stateFeatures) {
        // Simple mapping based on high-level action
        switch (highAction) {
            case MotorCommand::MoveForward:
                if (stateFeatures.size() > 0 && stateFeatures[0] > 0.7f) {
                    return MotorCommand::MoveForward;
                }
                return MotorCommand::LookForward;  // Placeholder action
            case MotorCommand::TurnLeft:
                return MotorCommand::TurnLeft;
            case MotorCommand::TurnRight:
                return MotorCommand::TurnRight;
            case MotorCommand::Interact:
                return MotorCommand::Interact;
            case MotorCommand::LookLeft:
                return MotorCommand::LookLeft;
            case MotorCommand::LookRight:
                return MotorCommand::LookRight;
            case MotorCommand::MoveBackward:
                return MotorCommand::MoveBackward;
            default:
                return MotorCommand::Wait;
        }
    }
    
    MotorCommand getRandomAction() {
        std::vector<MotorCommand> actions = {
            MotorCommand::MoveForward, MotorCommand::MoveBackward,
            MotorCommand::TurnLeft, MotorCommand::TurnRight,
            MotorCommand::Interact, MotorCommand::Wait,
            MotorCommand::LookLeft, MotorCommand::LookRight
        };
        return actions[std::uniform_int_distribution<size_t>(0, actions.size() - 1)(rng_)];
    }
    
    // Scoring functions for action selection
    float computeForwardScore(const std::vector<float>& stateFeatures) {
        float score = 0.0f;
        if (!stateFeatures.empty()) {
            score = stateFeatures[0];  // First feature represents forward movement potential
        }
        return std::min(1.0f, std::max(0.0f, score));
    }
    
    float computeExploreScore(const std::vector<float>& stateFeatures) {
        // Exploration score based on uncertainty or novelty
        float novelty = computeNovelty(stateFeatures);
        return novelty * 0.5f + 0.1f;  // Base exploration tendency
    }
    
    float computeInteractScore(const std::vector<float>& stateFeatures) {
        float score = 0.0f;
        if (stateFeatures.size() > 1) {
            score = stateFeatures[1];  // Second feature represents interactable objects
        }
        return std::min(1.0f, std::max(0.0f, score));
    }
    
    float computeNovelty(const std::vector<float>& stateFeatures) {
        if (stateFeatures.empty()) return 0.0f;
        
        // Simple novelty computation: distance from mean feature values
        static std::vector<float> featureHistory;
        static size_t historySize = 10;
        
        if (featureHistory.empty()) {
            featureHistory = std::vector<float>(stateFeatures.size(), 0.0f);
        }
        
        // Update history
        for (size_t i = 0; i < stateFeatures.size(); ++i) {
            featureHistory[i] = featureHistory[i] * 0.9f + stateFeatures[i] * 0.1f;
        }
        
        // Compute distance
        float totalDistance = 0.0f;
        for (size_t i = 0; i < stateFeatures.size(); ++i) {
            totalDistance += std::abs(stateFeatures[i] - featureHistory[i]);
        }
        
        return std::min(1.0f, totalDistance / static_cast<float>(stateFeatures.size()));
    }
    
    void updateHierarchicalPolicy(MotorCommand action, float outcomeValue) {
        // Update hierarchical policy based on action outcome
        if (outcomeValue > 0.5f) {
            // Positive outcome: increase probability of this action
            hierarchicalPolicy_.highLevelProb += 0.1f;
            hierarchicalPolicy_.highLevelProb = std::min(1.0f, hierarchicalPolicy_.highLevelProb);
        } else if (outcomeValue < -0.5f) {
            // Negative outcome: decrease probability
            hierarchicalPolicy_.highLevelProb -= 0.05f;
            hierarchicalPolicy_.highLevelProb = std::max(0.0f, hierarchicalPolicy_.highLevelProb);
        }
        
        // Update last execution time
        hierarchicalPolicy_.lastExecutionTime = currentTimestamp_;
    }
    
    // Random number generators
    std::mt19937 rng_;
    std::mt19937 thompsonRng_;
    double currentTimestamp_;
    
    std::vector<ActionValue> actionValues_;
    ThompsonSampler thompsonSampler_;
    HierarchicalPolicy hierarchicalPolicy_;
    QLearningAgent qLearningAgent_;
    
    bool curiosityEnabled_;
    bool rewardModulationEnabled_;
    bool developmentEnabled_;
    bool structuralPlasticityEnabled_;
};

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
    , sensoryNoveltyDecay_(0.99f)
    , advancedActionSelector_(std::make_unique<AdvancedActionSelector>())
{
    // Initialize advanced action selector
    advancedActionSelector_->enableCuriosity(true);
    advancedActionSelector_->enableRewardModulation(true);
    advancedActionSelector_->enableDevelopment(true);
    advancedActionSelector_->enableStructuralPlasticity(true);
    
    // Initialize with action space
    advancedActionSelector_->initialize(6, 6, 8);  // High-level, low-level, total action types
    
    // Initialize motor and sensory neuron groups
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Distribute motor neurons to different action groups
                        size_t idx = motorForward_.size() + motorBackward_.size() + 
                                    motorTurnLeft_.size() + motorTurnRight_.size() +
                                    motorInteract_.size() + motorWait_.size() +
                                    motorLookLeft_.size() + motorLookRight_.size();
                        
                        switch (idx % 8) {
                            case 0: motorForward_.push_back(n); break;
                            case 1: motorBackward_.push_back(n); break;
                            case 2: motorTurnLeft_.push_back(n); break;
                            case 3: motorTurnRight_.push_back(n); break;
                            case 4: motorInteract_.push_back(n); break;
                            case 5: motorWait_.push_back(n); break;
                            case 6: motorLookLeft_.push_back(n); break;
                            case 7: motorLookRight_.push_back(n); break;
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

size_t AgentBrain::getSensoryInputSize() const {
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
    return 256 + 8 + 4 + 6;
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action
    return 8;  // Enhanced with look left/right
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
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    // Get current state features for advanced action selection
    std::vector<float> stateFeatures;
    
    // Extract current brain state as features
    size_t totalNeurons = brain_->getTotalNeuronCount();
    if (totalNeurons > 0) {
        stateFeatures.reserve(16);  // 16 features
        
        // Feature 1: Average firing rate
        float avgFiringRate = brain_->getAverageFiringRate();
        stateFeatures.push_back(avgFiringRate);
        
        // Feature 2: Novelty level
        stateFeatures.push_back(noveltyLevel_);
        
        // Feature 3: Curiosity level
        stateFeatures.push_back(curiosityLevel_);
        
        // Feature 4: Dopamine level
        stateFeatures.push_back(dopamineLevel_);
        
        // Feature 5: Prediction error
        stateFeatures.push_back(predictionError_);
        
        // Feature 6: Developmental stage
        float developmentalStage = static_cast<float>(brain_->getDevelopmentalStage());
        stateFeatures.push_back(developmentalStage);
        
        // Feature 7: Plasticity modifier
        stateFeatures.push_back(plasticityModifier_);
        
        // Feature 8: Working memory activity
        auto* workingMemory = brain_->getWorkingMemory();
        float workingMemoryActivity = workingMemory ? workingMemory->getActiveTraces() : 0.0f;
        stateFeatures.push_back(workingMemoryActivity / 10.0f);  // Normalize
        
        // Feature 9: Reward history
        stateFeatures.push_back(expectedReward_);
        
        // Feature 10: Energy/excitability
        stateFeatures.push_back(1.0f - std::abs(dopamineLevel_) * 0.5f);
        
        // Feature 11: Motor neuron activity
        float motorActivity = 0.0f;
        for (auto* neuron : motorNeurons) {
            if (neuron && neuron->isFiring()) {
                motorActivity += 1.0f;
            }
        }
        stateFeatures.push_back(motorActivity / std::max<size_t>(motorNeurons.size(), 1));
        
        // Feature 12: Sensory input strength
        float sensoryInputStrength = 0.0f;
        for (auto* neuron : sensoryNeurons) {
            if (neuron && neuron->getState().membranePotential > 0.0f) {
                sensoryInputStrength += neuron->getState().membranePotential;
            }
        }
        stateFeatures.push_back(sensoryInputStrength / 10.0f);
        
        // Feature 13: Spiking frequency
        float spikeFrequency = brain_->getTotalSpikeCount() / 100.0f;  // Per 100 steps
        stateFeatures.push_back(std::min(1.0f, spikeFrequency));
        
        // Feature 14: Action repetition penalty
        float actionRepetition = computeActionRepetitionPenalty();
        stateFeatures.push_back(actionRepetition);
        
        // Feature 15: Time since last reward
        float timeSinceReward = getTimeSinceLastReward();
        stateFeatures.push_back(std::min(1.0f, timeSinceReward / 100.0f));  // Normalize
        
        // Feature 16: Context entropy
        float contextEntropy = computeContextEntropy(stateFeatures);
        stateFeatures.push_back(contextEntropy);
    }
    
    // Use advanced action selector for sophisticated decision making
    MotorCommand advancedAction = advancedActionSelector_->selectAction(
        stateFeatures, curiosityLevel_, expectedReward_, developmentalAge_);
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        advancedAction = advancedActionSelector_->selectWithCuriosity(advancedAction);
    }
    
    return advancedAction;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate average activity in each motor group
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
    float lookLeftAct = calcActivity(motorLookLeft_);
    float lookRightAct = calcActivity(motorLookRight_);
    
    // Find maximum activity
    struct { MotorCommand cmd; float activity; } commands[] = {
        {MotorCommand::MoveForward, forwardAct},
        {MotorCommand::MoveBackward, backwardAct},
        {MotorCommand::TurnLeft, leftAct},
        {MotorCommand::TurnRight, rightAct},
        {MotorCommand::Interact, interactAct},
        {MotorCommand::Wait, waitAct},
        {MotorCommand::LookLeft, lookLeftAct},
        {MotorCommand::LookRight, lookRightAct}
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
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command using advanced action selector
            return advancedActionSelector_->getBestAction(std::vector<float>(16, 0.0f));  // Random features
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
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
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
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
    }
    
    // Update advanced action selector with reward information
    if (advancedActionSelector_) {
        advancedActionSelector_->updateActionValue(
            defaultCmd, reward, predictionError_ > 0.0f, std::vector<float>(16, 0.0f)
        );
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // More sophisticated developmental stages based on age
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
    
    // Update advanced action selector with developmental changes
    if (advancedActionSelector_) {
        // Adapt action selection strategies based on development
        if (developmentalAge_ > 300.0 && developmentalAge_ < 600.0) {
            // Critical period: increased exploration
            advancedActionSelector_->enableCuriosity(true);
        } else if (developmentalAge_ > 900.0) {
            // Adult: more exploitation
            advancedActionSelector_->enableCuriosity(false);
        }
    }
}

float AgentBrain::computeActionRepetitionPenalty() {
    // Penalize repetitive actions to encourage exploration
    static std::unordered_map<MotorCommand, double> actionLastTime;
    static double currentTime = 0.0;
    
    double penalty = 0.0f;
    
    // Simple implementation: if same action was used recently, apply penalty
    // In a full implementation, this would track action history more carefully
    
    return static_cast<float>(penalty);
}

double AgentBrain::getTimeSinceLastReward() {
    // Return time since last significant reward
    // This would need to be tracked separately in a full implementation
    return 0.0;
}

float AgentBrain::computeContextEntropy(const std::vector<float>& stateFeatures) {
    // Simple entropy computation based on state feature variance
    if (stateFeatures.size() <= 1) return 0.0f;
    
    float mean = std::accumulate(stateFeatures.begin(), stateFeatures.end(), 0.0f) / stateFeatures.size();
    float variance = 0.0f;
    for (float val : stateFeatures) {
        variance += (val - mean) * (val - mean);
    }
    variance /= stateFeatures.size();
    
    return 1.0f - std::exp(-variance);  // Convert variance to entropy-like measure
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

void AgentBrain::reset() {
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    
    // Reset advanced action selector
    if (advancedActionSelector_) {
        advancedActionSelector_->reset();
    }
}

} // namespace nlm
