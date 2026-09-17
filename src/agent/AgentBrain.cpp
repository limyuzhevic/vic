// AgentBrain.cpp - Advanced Brain Implementation with Enhanced Exploration and Action Selection

#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <map>
#include <deque>
#include <numeric>
#include <unordered_set>
#include <limits>
#include <chrono>

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(brain)
    // Basic neuromodulation state
    , dopamineLevel_(0.0f)
    , noveltyLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , predictionError_(0.0f)
    , expectedReward_(0.0f)
    // Enhanced neuromodulation
    , dopaminePhasic_(0.0f)
    , dopamineTonic_(0.0f)
    , dopamineAdaptation_(1.0f)
    , intrinsicMotivation_(0.0f)
    , rewardPredictionError_(0.0f)
    , noveltySignal_(0.0f)
    , predictionErrorSignal_(0.0f)
    , intrinsicSignal_(0.0f)
    , rewardPredictionSignal_(0.0f)
    // Development state
    , developmentalAge_(0.0)
    , plasticityModifier_(1.0f)
    // Configuration flags
    , rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , curiosityEnabled_(true)
    // Advanced exploration and learning parameters
    , explorationStrategy_(ExplorationStrategy::EpsilonGreedy)
    , actionSelectionMode_(ActionSelectionMode::Hierarchical)
    , explorationRate_(0.1f)
    , temperature_(1.0f)
    , ucbC_(1.5f)
    , entropyWeight_(0.5f)
    , adaptiveLearningRate_(0.01f)
    , hierarchicalLevels_(3)
    , policyGradientEnabled_(true)
    , metaLearningEnabled_(false)
    , metaLearningRate_(0.001f)
    , actorLearningRate_(0.001f)
    , criticLearningRate_(0.01f)
    , sensoryNoveltyDecay_(0.99f)
    , totalSelections_(0)
    , explorationEvents_(0)
{
    // Initialize random number generator
    std::random_device rd;
    rng_ = std::mt19937(rd());
    uniformDist_ = std::uniform_real_distribution<float>(0.0f, 1.0f);
    
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
    
    // Initialize action hierarchies for different levels
    actionHierarchies_.resize(hierarchicalLevels_);
    for (int i = 0; i < hierarchicalLevels_; ++i) {
        actionHierarchies_[i].resize(static_cast<size_t>(MotorCommand::Wait) + 1, MotorCommand::Wait);
    }
    
    // Pre-populate action hierarchies with sensible defaults
    // Level 0: Basic movements
    actionHierarchies_[0][static_cast<size_t>(MotorCommand::MoveForward)] = MotorCommand::MoveForward;
    actionHierarchies_[0][static_cast<size_t>(MotorCommand::MoveBackward)] = MotorCommand::MoveBackward;
    actionHierarchies_[0][static_cast<size_t>(MotorCommand::TurnLeft)] = MotorCommand::TurnLeft;
    actionHierarchies_[0][static_cast<size_t>(MotorCommand::TurnRight)] = MotorCommand::TurnRight;
    
    // Level 1: Complex actions
    actionHierarchies_[1][static_cast<size_t>(MotorCommand::MoveForward)] = MotorCommand::MoveForward;
    actionHierarchies_[1][static_cast<size_t>(MotorCommand::TurnLeft)] = MotorCommand::TurnLeft;
    actionHierarchies_[1][static_cast<size_t>(MotorCommand::Interact)] = MotorCommand::Interact;
    actionHierarchies_[1][static_cast<size_t>(MotorCommand::Wait)] = MotorCommand::Wait;
    
    // Level 2: Higher-level policies
    actionHierarchies_[2][static_cast<size_t>(MotorCommand::MoveForward)] = MotorCommand::MoveForward;
    actionHierarchies_[2][static_cast<size_t>(MotorCommand::Interact)] = MotorCommand::Interact;
    actionHierarchies_[2][static_cast<size_t>(MotorCommand::Wait)] = MotorCommand::Wait;
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
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    // First compute basic motor command from neural activity
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Convert to action value representation for advanced selection
    std::vector<float> actionValues(static_cast<size_t>(MotorCommand::Wait), 0.0f);
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
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
    
    actionValues[static_cast<size_t>(MotorCommand::MoveForward)] = forwardAct;
    actionValues[static_cast<size_t>(MotorCommand::MoveBackward)] = backwardAct;
    actionValues[static_cast<size_t>(MotorCommand::TurnLeft)] = leftAct;
    actionValues[static_cast<size_t>(MotorCommand::TurnRight)] = rightAct;
    actionValues[static_cast<size_t>(MotorCommand::Interact)] = interactAct;
    actionValues[static_cast<size_t>(MotorCommand::Wait)] = waitAct;
    
    // Enhanced action selection based on mode
    MotorCommand selectedCommand = MotorCommand::Wait;
    
    switch (actionSelectionMode_) {
        case ActionSelectionMode::ModelBased:
            selectedCommand = selectModelBasedAction(actionValues);
            break;
        case ActionSelectionMode::ModelFree:
            selectedCommand = selectModelFreeAction(actionValues);
            break;
        case ActionSelectionMode::Hierarchical:
            selectedCommand = selectHierarchicalAction(actionValues);
            break;
        case ActionSelectionMode::Planning:
            selectedCommand = selectPlannedAction(actionValues);
            break;
    }
    
    // Apply exploration if curiosity is enabled
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        // Decay exploration rate over time based on exploration strategy
        if (explorationStrategy_ == ExplorationStrategy::EpsilonGreedy) {
            explorationRate_ *= 0.995f;  // Slow decay
        }
        
        decoded = selectWithCuriosity(decoded, actionValues);
    }
    
    // Record selection for learning
    totalSelections_++;
    if (selectedCommand == MotorCommand::Wait) {
        explorationEvents_++;  // Wait actions count as exploration
    }
    
    return decoded;
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
    
    // Find maximum activity
    struct { MotorCommand cmd; float activity; } commands[] = {
        {MotorCommand::MoveForward, forwardAct},
        {MotorCommand::MoveBackward, backwardAct},
        {MotorCommand::TurnLeft, leftAct},
        {MotorCommand::TurnRight, rightAct},
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

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd, const std::vector<float>& actionValues) {
    switch (explorationStrategy_) {
        case ExplorationStrategy::EpsilonGreedy:
            return epsilonGreedySelection(actionValues);
        case ExplorationStrategy::Softmax:
            return softmaxSelection(actionValues);
        case ExplorationStrategy::UCB:
            // Need visit counts for UCB
            return ucbSelection(actionValues, actionVisits_);
        case ExplorationStrategy::Entropy:
            return entropySelection(actionValues, getEntropyOfActionDistribution());
        case ExplorationStrategy::Adaptive:
            return adaptiveSelection(actionValues);
        case ExplorationStrategy::None:
        default:
            // Simple curiosity-based random exploration
            if (curiosityLevel_ > 0.5f) {
                float exploreChance = curiosityLevel_ * 0.3f;  // Up to 30% random
                float r = uniformDist_(rng_);
                if (r < exploreChance) {
                    // Random motor command
                    int choice = std::uniform_int_distribution<int>(0, 7)(rng_);
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
}

MotorCommand AgentBrain::epsilonGreedySelection(const std::vector<float>& actionValues) {
    if (actionValues.empty()) return MotorCommand::Wait;
    
    // Find best action
    size_t bestAction = 0;
    float bestValue = actionValues[0];
    for (size_t i = 1; i < actionValues.size(); ++i) {
        if (actionValues[i] > bestValue) {
            bestValue = actionValues[i];
            bestAction = i;
        }
    }
    
    // Epsilon-greedy selection
    float r = uniformDist_(rng_);
    if (r < explorationRate_) {
        // Explore: random action
        size_t randomAction = std::uniform_int_distribution<size_t>(0, actionValues.size() - 1)(rng_);
        explorationEvents_++;
        return static_cast<MotorCommand>(randomAction);
    } else {
        // Exploit: best action
        return static_cast<MotorCommand>(bestAction);
    }
}

MotorCommand AgentBrain::softmaxSelection(const std::vector<float>& actionValues) {
    if (actionValues.empty()) return MotorCommand::Wait;
    
    // Compute softmax probabilities
    std::vector<float> probs;
    float sum = 0.0f;
    
    // Shift values for numerical stability
    float maxValue = *std::max_element(actionValues.begin(), actionValues.end());
    
    for (float value : actionValues) {
        float prob = std::exp((value - maxValue) / temperature_);
        probs.push_back(prob);
        sum += prob;
    }
    
    // Normalize
    for (float& prob : probs) {
        prob /= sum;
    }
    
    // Sample from distribution
    float r = uniformDist_(rng_);
    float cumulative = 0.0f;
    
    for (size_t i = 0; i < probs.size(); ++i) {
        cumulative += probs[i];
        if (r <= cumulative) {
            return static_cast<MotorCommand>(i);
        }
    }
    
    // Fallback
    return static_cast<MotorCommand>(probs.size() - 1);
}

MotorCommand AgentBrain::ucbSelection(const std::vector<float>& actionValues, const std::vector<size_t>& visitCounts) {
    if (actionValues.empty()) return MotorCommand::Wait;
    
    // Calculate UCB values
    std::vector<float> ucbValues;
    float totalVisits = 0.0f;
    for (size_t count : visitCounts) {
        totalVisits += count;
    }
    
    for (size_t i = 0; i < actionValues.size(); ++i) {
        float avgReward = actionValues[i];  // In real implementation, this would be average reward
        float explorationTerm = 0.0f;
        
        if (visitCounts[i] > 0) {
            explorationTerm = ucbC_ * std::sqrt(std::log(totalVisits) / visitCounts[i]);
        } else {
            explorationTerm = std::numeric_limits<float>::max();  // Ensure unexplored actions are tried
        }
        
        ucbValues.push_back(avgReward + explorationTerm);
    }
    
    // Find best action
    size_t bestAction = 0;
    float bestValue = ucbValues[0];
    for (size_t i = 1; i < ucbValues.size(); ++i) {
        if (ucbValues[i] > bestValue) {
            bestValue = ucbValues[i];
            bestAction = i;
        }
    }
    
    // Update visit count
    if (bestAction < visitCounts.size()) {
        actionVisits_[bestAction]++;
    }
    
    return static_cast<MotorCommand>(bestAction);
}

MotorCommand AgentBrain::entropySelection(const std::vector<float>& actionValues, float entropy) {
    if (actionValues.empty()) return MotorCommand::Wait;
    
    // Compute entropy-based exploration
    float baseEntropy = getEntropyOfActionDistribution();
    float adjustedEntropy = baseEntropy * (1.0f + entropyWeight_ * entropy);
    
    // Increase temperature for higher entropy
    float adjustedTemp = temperature_ * (1.0f + entropyWeight_ * entropy);
    
    // Use softmax with adjusted temperature
    std::vector<float> probs;
    float sum = 0.0f;
    float maxValue = *std::max_element(actionValues.begin(), actionValues.end());
    
    for (float value : actionValues) {
        float prob = std::exp((value - maxValue) / adjustedTemp);
        probs.push_back(prob);
        sum += prob;
    }
    
    for (float& prob : probs) {
        prob /= sum;
    }
    
    // Sample from distribution
    float r = uniformDist_(rng_);
    float cumulative = 0.0f;
    
    for (size_t i = 0; i < probs.size(); ++i) {
        cumulative += probs[i];
        if (r <= cumulative) {
            return static_cast<MotorCommand>(i);
        }
    }
    
    return static_cast<MotorCommand>(probs.size() - 1);
}

MotorCommand AgentBrain::adaptiveSelection(const std::vector<float>& actionValues) {
    if (actionValues.empty()) return MotorCommand::Wait;
    
    // Adaptive exploration based on prediction error and performance
    float predictionError = getPredictionError();
    float performanceError = 0.0f;  // Would be computed from actual vs expected reward
    
    // Adjust exploration rate based on errors
    float explorationAdjustment = 1.0f;
    
    // High prediction error -> more exploration
    explorationAdjustment *= (1.0f + std::abs(predictionError) * 0.5f);
    
    // High performance error -> more exploration
    explorationAdjustment *= (1.0f + performanceError * 0.3f);
    
    // Decay exploration over time
    explorationRate_ = std::max(0.01f, explorationRate_ * 0.99f);
    
    // Effective epsilon for current context
    float effectiveEpsilon = explorationRate_ * explorationAdjustment;
    effectiveEpsilon = std::clamp(effectiveEpsilon, 0.01f, 0.5f);
    
    // Use epsilon-greedy with adjusted epsilon
    float r = uniformDist_(rng_);
    if (r < effectiveEpsilon) {
        // Explore
        size_t randomAction = std::uniform_int_distribution<size_t>(0, actionValues.size() - 1)(rng_);
        explorationEvents_++;
        return static_cast<MotorCommand>(randomAction);
    } else {
        // Exploit
        size_t bestAction = 0;
        float bestValue = actionValues[0];
        for (size_t i = 1; i < actionValues.size(); ++i) {
            if (actionValues[i] > bestValue) {
                bestValue = actionValues[i];
                bestAction = i;
            }
        }
        return static_cast<MotorCommand>(bestAction);
    }
}

MotorCommand AgentBrain::selectModelBasedAction(const std::vector<float>& state) {
    if (!predictionSystem_) return MotorCommand::Wait;
    
    // Get prediction from model-based system
    // In a full implementation, this would use the prediction system
    // to select actions based on predicted outcomes
    
    // For now, use hierarchical selection with prediction-based weighting
    float predictionError = getPredictionError();
    float confidence = predictionSystem_->getConfidence();
    
    // Modify action selection based on prediction quality
    MotorCommand baseSelection = selectHierarchicalAction(state);
    
    // If prediction is uncertain, explore more
    if (confidence < 0.5f || std::abs(predictionError) > 0.3f) {
        // Randomly explore around base selection
        float r = uniformDist_(rng_);
        if (r < 0.2f) {
            // Try alternative actions
            std::vector<size_t> alternatives;
            for (size_t i = 0; i < state.size(); ++i) {
                if (i != static_cast<size_t>(baseSelection)) {
                    alternatives.push_back(i);
                }
            }
            
            if (!alternatives.empty()) {
                size_t alt = alternatives[std::uniform_int_distribution<size_t>(0, alternatives.size() - 1)(rng_)];
                return static_cast<MotorCommand>(alt);
            }
        }
    }
    
    return baseSelection;
}

MotorCommand AgentBrain::selectModelFreeAction(const std::vector<float>& state) {
    if (actionValues_.empty()) {
        return MotorCommand::Wait;
    }
    
    // Use action value estimates for selection
    // Convert state to action indices (simplified)
    std::vector<size_t> possibleActions;
    for (size_t i = 0; i < state.size(); ++i) {
        possibleActions.push_back(i);
    }
    
    if (possibleActions.empty()) {
        return MotorCommand::Wait;
    }
    
    // Find best action value
    size_t bestAction = 0;
    float bestValue = actionValues_.empty() ? 0.0f : actionValues_.begin()->second;
    
    for (size_t i : possibleActions) {
        float value = 0.0f;
        auto it = actionValues_.find(i);
        if (it != actionValues_.end()) {
            value = it->second;
        }
        
        if (value > bestValue) {
            bestValue = value;
            bestAction = i;
        }
    }
    
    // Apply exploration based on strategy
    float r = uniformDist_(rng_);
    
    switch (explorationStrategy_) {
        case ExplorationStrategy::EpsilonGreedy:
            if (r < explorationRate_) {
                // Explore
                size_t randomAction = possibleActions[std::uniform_int_distribution<size_t>(0, possibleActions.size() - 1)(rng_)];
                explorationEvents_++;
                return static_cast<MotorCommand>(randomAction);
            }
            break;
            
        case ExplorationStrategy::Softmax:
            // Already handled in decodeMotorCommand, but we could apply here
            break;
            
        case ExplorationStrategy::UCB:
            // Already handled in decodeMotorCommand
            break;
            
        case ExplorationStrategy::Entropy:
            // Already handled in decodeMotorCommand
            break;
            
        case ExplorationStrategy::Adaptive:
            // Already handled in decodeMotorCommand
            break;
            
        case ExplorationStrategy::None:
            // Simple curiosity-based exploration
            if (curiosityLevel_ > 0.5f) {
                if (r < curiosityLevel_ * 0.3f) {
                    explorationEvents_++;
                    return static_cast<MotorCommand>(possibleActions[std::uniform_int_distribution<size_t>(0, possibleActions.size() - 1)(rng_)]);
                }
            }
            break;
    }
    
    return static_cast<MotorCommand>(bestAction);
}

MotorCommand AgentBrain::selectHierarchicalAction(const std::vector<float>& state) {
    if (hierarchicalLevels_ <= 0) return MotorCommand::Wait;
    
    // Hierarchical action selection
    // Start from highest level and work down
    for (int level = hierarchicalLevels_ - 1; level >= 0; --level) {
        if (level >= static_cast<int>(actionHierarchies_.size())) continue;
        
        const auto& levelActions = actionHierarchies_[level];
        
        // Find best action at this level based on state
        size_t bestAction = 0;
        float bestValue = 0.0f;
        
        for (size_t i = 0; i < levelActions.size() && i < state.size(); ++i) {
            if (state[i] > bestValue) {
                bestValue = state[i];
                bestAction = i;
            }
        }
        
        MotorCommand selected = levelActions[bestAction];
        
        // If this level provides a meaningful action, use it
        if (selected != MotorCommand::Wait && bestValue > 0.3f) {
            return selected;
        }
    }
    
    // No high-level action found, use lowest-level action
    if (!actionHierarchies_.empty()) {
        const auto& levelActions = actionHierarchies_[0];
        size_t bestAction = 0;
        float bestValue = 0.0f;
        
        for (size_t i = 0; i < levelActions.size() && i < state.size(); ++i) {
            if (state[i] > bestValue) {
                bestValue = state[i];
                bestAction = i;
            }
        }
        
        return levelActions[bestAction];
    }
    
    return MotorCommand::Wait;
}

MotorCommand AgentBrain::selectPlannedAction(const std::vector<float>& state) {
    if (!workingMemory_) return MotorCommand::Wait;
    
    // Planning-based action selection
    // Use working memory to store and retrieve action sequences
    
    // Store current state in working memory
    workingMemory_->store(state, 1.0f);
    
    // Get working memory content
    std::vector<float> memoryContent = workingMemory_->retrieve();
    
    // Simple planning: choose action based on working memory similarity
    float bestScore = -1.0f;
    MotorCommand bestAction = MotorCommand::Wait;
    
    // Evaluate each possible action
    for (size_t i = 0; i < state.size(); ++i) {
        // In a full implementation, this would evaluate action consequences
        // using the prediction system and episodic memory
        
        float actionValue = state[i];
        
        // Modify by episodic memory relevance
        // Would retrieve similar episodes and weight by reward
        
        // For now, simple evaluation
        if (actionValue > bestScore) {
            bestScore = actionValue;
            bestAction = static_cast<MotorCommand>(i);
        }
    }
    
    return bestAction;
}

std::vector<MotorCommand> AgentBrain::planActionSequence(const std::vector<float>& state, size_t sequenceLength) {
    std::vector<MotorCommand> sequence;
    
    if (!workingMemory_ || sequenceLength == 0) {
        return sequence;
    }
    
    // Generate action sequence using working memory and episodic memory
    std::vector<float> currentState = state;
    
    for (size_t step = 0; step < sequenceLength; ++step) {
        // Evaluate all possible actions
        std::vector<std::pair<MotorCommand, float>> actionScores;
        
        for (size_t i = 0; i < currentState.size(); ++i) {
            MotorCommand action = static_cast<MotorCommand>(i);
            float score = evaluateAction(action, currentState);
            actionScores.push_back({action, score});
        }
        
        // Sort by score
        std::sort(actionScores.begin(), actionScores.end(),
                  [](const std::pair<MotorCommand, float>& a, const std::pair<MotorCommand, float>& b) {
                      return a.second > b.second;
                  });
        
        // Add best action to sequence
        if (!actionScores.empty()) {
            sequence.push_back(actionScores[0].first);
            
            // Update current state (simplified)
            // In a full implementation, this would simulate action effects
            currentState[static_cast<size_t>(actionScores[0].first)] *= 0.9f;  // Decay
        }
    }
    
    return sequence;
}

float AgentBrain::evaluateAction(const MotorCommand& action, const std::vector<float>& state) {
    // Evaluate how good an action is given the current state
    // Combines action values, predictions, and episodic memory
    
    float baseValue = 0.0f;
    
    // Get action value if available
    size_t actionIndex = static_cast<size_t>(action);
    auto it = actionValues_.find(actionIndex);
    if (it != actionValues_.end()) {
        baseValue = it->second;
    }
    
    // Get prediction-based evaluation if available
    float predictionBonus = 0.0f;
    if (predictionSystem_) {
        float predictionError = getPredictionError();
        float confidence = predictionSystem_->getConfidence();
        
        // Adjust based on prediction quality
        predictionBonus = baseValue * confidence * (1.0f - std::abs(predictionError));
    }
    
    // Get episodic memory bonus
    float memoryBonus = 0.0f;
    if (episodicMemory_ && !state.empty()) {
        // Retrieve similar episodes
        std::vector<const EpisodicMemoryItem*> similarEpisodes = 
            episodicMemory_->retrieveSimilar(state, 3);
        
        for (const auto* episode : similarEpisodes) {
            memoryBonus += episode->reward * 0.1f;
        }
    }
    
    return baseValue + predictionBonus + memoryBonus;
}

void AgentBrain::learnActorCritic(float reward, const std::vector<float>& state, const std::vector<float>& action) {
    if (!brain_ || !policyGradientEnabled_) return;
    
    // Actor-critic learning with eligibility traces
    
    // Get current action value estimate
    size_t actionIndex = 0;
    if (!action.empty()) {
        actionIndex = static_cast<size_t>(action[0]);
    }
    
    float currentValue = 0.0f;
    auto it = actionValues_.find(actionIndex);
    if (it != actionValues_.end()) {
        currentValue = it->second;
    }
    
    // Estimate value function (critic)
    float estimatedValue = currentValue;  // In full implementation, would use separate value network
    
    // Compute TD error
    float tdError = reward + 0.99f * estimatedValue - currentValue;
    
    // Update action value (actor)
    float actorUpdate = actorLearningRate_ * tdError;
    actionValues_[actionIndex] += actorUpdate;
    
    // Update eligibility traces
    updateEligibilityTraces(action, tdError, 0.9f);
    
    // Record for learning metrics
    actionValueHistory_.push_back(actionValues_[actionIndex]);
    if (actionValueHistory_.size() > 100) {
        actionValueHistory_.erase(actionValueHistory_.begin());
    }
    
    predictionErrorHistory_.push_back(tdError);
    if (predictionErrorHistory_.size() > 100) {
        predictionErrorHistory_.erase(predictionErrorHistory_.begin());
    }
    
    rewardHistory_.push_back(reward);
    if (rewardHistory_.size() > 100) {
        rewardHistory_.erase(rewardHistory_.begin());
    }
    
    // Update neuromodulation
    updateDopamineDynamics(reward, tdError);
}

void AgentBrain::updateEligibilityTraces(const std::vector<float>& action, float tdError, float decay) {
    // Update eligibility traces for action-value learning
    
    if (action.empty()) return;
    
    // Simplified eligibility trace update
    size_t actionIndex = static_cast<size_t>(action[0]);
    
    // In a full implementation, this would maintain traces for all state-action pairs
    // For now, just update the action value directly
    eligibilityTraces_.assign(action.size(), decay * tdError);
}

void AgentBrain::updateDopamineDynamics(float reward, float predictionError) {
    // Enhanced dopamine dynamics with multiple factors
    
    // Phasic component (immediate response to reward prediction error)
    dopaminePhasic_ = predictionError;
    
    // Tonic component (baseline level)
    dopamineTonic_ = 0.1f + 0.3f * (0.5f + predictionError * 0.5f);  // 0.1 to 0.5 range
    dopamineTonic_ = std::clamp(dopamineTonic_, 0.0f, 1.0f);
    
    // Adaptation of dopamine system based on prediction error
    dopamineAdaptation_ = std::max(0.1f, dopamineAdaptation_ * 0.99f + 0.01f * std::abs(predictionError));
    
    // Apply adaptation
    dopamineTonic_ *= dopamineAdaptation_;
    dopaminePhasic_ *= dopamineAdaptation_;
    
    // Apply neuromodulation to brain
    // This would affect synaptic plasticity in the brain
}

void AgentBrain::updateCuriosityMultiFactor(float novelty, float predictionError, float intrinsic, float rewardPredictive) {
    // Multi-factor curiosity system
    
    // Store factors
    noveltySignal_ = novelty;
    predictionErrorSignal_ = predictionError;
    intrinsicSignal_ = intrinsic;
    rewardPredictionSignal_ = rewardPredictive;
    
    // Combine factors for overall curiosity level
    // Novelty and prediction error are primary drivers
    // Intrinsic motivation and reward prediction provide secondary influence
    
    float curiosity = novelty * 0.5f + std::abs(predictionError) * 0.3f + 
                      intrinsic * 0.15f + rewardPredictive * 0.05f;
    
    // Apply adaptive learning rate
    curiosityLevel_ = curiosity * adaptiveLearningRate_;
    curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    
    // Store for metrics
    intrinsicMotivation_ = intrinsic;
    rewardPredictionError_ = rewardPredictive;
}

void AgentBrain::adaptExplorationPolicy(float performanceError) {
    // Meta-learning: adapt exploration policy based on performance
    
    if (!metaLearningEnabled_ || performanceError == 0.0f) return;
    
    // Adjust exploration strategy based on performance error
    float adjustment = metaLearningRate_ * performanceError;
    
    // Modify exploration parameters
    explorationRate_ += adjustment;
    explorationRate_ = std::clamp(explorationRate_, 0.01f, 0.5f);
    
    // Adjust temperature for softmax
    temperature_ += adjustment * 0.5f;
    temperature_ = std::clamp(temperature_, 0.5f, 2.0f);
    
    // Update other exploration parameters
    entropyWeight_ += adjustment * 0.3f;
    entropyWeight_ = std::clamp(entropyWeight_, 0.1f, 1.0f);
    
    // Decay over time
    explorationRate_ *= 0.99f;
    temperature_ *= 0.99f;
}

float AgentBrain::getEntropyOfActionDistribution() const {
    // Calculate entropy of current action distribution
    if (actionValues_.empty()) return 0.0f;
    
    // Convert action values to probabilities (softmax)
    std::vector<float> probs;
    float sum = 0.0f;
    float maxValue = 0.0f;
    
    for (const auto& pair : actionValues_) {
        maxValue = std::max(maxValue, pair.second);
    }
    
    for (const auto& pair : actionValues_) {
        float prob = std::exp((pair.second - maxValue) / temperature_);
        probs.push_back(prob);
        sum += prob;
    }
    
    if (sum == 0.0f) return 0.0f;
    
    // Normalize
    for (float& prob : probs) {
        prob /= sum;
    }
    
    // Calculate entropy
    float entropy = 0.0f;
    for (float prob : probs) {
        if (prob > 0.0f) {
            entropy -= prob * std::log(prob);
        }
    }
    
    return entropy;
}

float AgentBrain::getExplorationExploitationRatio() const {
    // Calculate ratio of exploration vs exploitation events
    if (totalSelections_ == 0) return 0.0f;
    
    float explorationRatio = static_cast<float>(explorationEvents_) / static_cast<float>(totalSelections_);
    return explorationRatio;
}

float AgentBrain::getLearningEfficiency() const {
    // Calculate learning efficiency based on reward vs exploration
    if (rewardHistory_.empty()) return 0.0f;
    
    float totalReward = 0.0f;
    for (float r : rewardHistory_) {
        totalReward += r;
    }
    
    float efficiency = totalReward / std::max<size_t>(totalSelections_, 1);
    
    // Normalize by exploration ratio
    float explorationPenalty = getExplorationExploitationRatio();
    efficiency *= (1.0f - explorationPenalty);
    
    return std::max(0.0f, efficiency);
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
    
    // Clear learning history
    actionValueHistory_.clear();
    predictionErrorHistory_.clear();
    rewardHistory_.clear();
    totalSelections_ = 0;
    explorationEvents_ = 0;
    
    // Reset advanced parameters
    dopaminePhasic_ = 0.0f;
    dopamineTonic_ = 0.0f;
    dopamineAdaptation_ = 1.0f;
    intrinsicMotivation_ = 0.0f;
    rewardPredictionError_ = 0.0f;
    
    // Clear multi-factor signals
    noveltySignal_ = 0.0f;
    predictionErrorSignal_ = 0.0f;
    intrinsicSignal_ = 0.0f;
    rewardPredictionSignal_ = 0.0f;
    
    // Clear action hierarchies (reset to defaults)
    for (int i = 0; i < hierarchicalLevels_; ++i) {
        if (i < static_cast<int>(actionHierarchies_.size())) {
            actionHierarchies_[i].assign(static_cast<size_t>(MotorCommand::Wait) + 1, MotorCommand::Wait);
        }
    }
    
    // Re-initialize basic hierarchies
    if (actionHierarchies_.size() > 0) {
        // Level 0: Basic movements
        actionHierarchies_[0][static_cast<size_t>(MotorCommand::MoveForward)] = MotorCommand::MoveForward;
        actionHierarchies_[0][static_cast<size_t>(MotorCommand::MoveBackward)] = MotorCommand::MoveBackward;
        actionHierarchies_[0][static_cast<size_t>(MotorCommand::TurnLeft)] = MotorCommand::TurnLeft;
        actionHierarchies_[0][static_cast<size_t>(MotorCommand::TurnRight)] = MotorCommand::TurnRight;
    }
    
    if (actionHierarchies_.size() > 1) {
        // Level 1: Complex actions
        actionHierarchies_[1][static_cast<size_t>(MotorCommand::MoveForward)] = MotorCommand::MoveForward;
        actionHierarchies_[1][static_cast<size_t>(MotorCommand::TurnLeft)] = MotorCommand::TurnLeft;
        actionHierarchies_[1][static_cast<size_t>(MotorCommand::Interact)] = MotorCommand::Interact;
        actionHierarchies_[1][static_cast<size_t>(MotorCommand::Wait)] = MotorCommand::Wait;
    }
    
    if (actionHierarchies_.size() > 2) {
        // Level 2: Higher-level policies
        actionHierarchies_[2][static_cast<size_t>(MotorCommand::MoveForward)] = MotorCommand::MoveForward;
        actionHierarchies_[2][static_cast<size_t>(MotorCommand::Interact)] = MotorCommand::Interact;
        actionHierarchies_[2][static_cast<size_t>(MotorCommand::Wait)] = MotorCommand::Wait;
    }
}

} // namespace nlm
