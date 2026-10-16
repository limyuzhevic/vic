#include "BasalGanglia.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <unordered_map>
#include <queue>

namespace nlm {

// Global random number generator for basal ganglia
static std::mt19937 rng(std::random_device{}());
static std::uniform_real_distribution<float> uniformDist(0.0f, 1.0f);
static std::normal_distribution<float> normalDist(0.0f, 1.0f);

// Basal ganglia implementation

BasalGanglia::BasalGanglia() {
    // Initialize default parameters
    learningRate_ = 0.1f;
    explorationRate_ = 0.1f;
    habitFormationRate_ = 0.05f;
    actionWindowSize_ = 50;
    
    actionSelectionEnabled_ = true;
    habitFormationEnabled_ = true;
    valueLearningEnabled_ = true;
    inhibitionEnabled_ = true;
    
    brain_ = nullptr;
    dopamine_ = nullptr;
    
    totalActionsSelected_ = 0;
    cumulativeReward_ = 0.0f;
    explorationCount_ = 0;
}

BasalGanglia::~BasalGanglia() = default;

void BasalGanglia::initialize(Brain* brain, Neuromodulator* dopamine) {
    brain_ = brain;
    dopamine_ = dopamine;
    
    // Initialize striatal pathways
    directPath_.clear();
    indirectPath_.clear();
    
    // Initialize output nuclei
    gpi_.clear();
    stn_.clear();
    
    // Set up action selection
    actionValues_.clear();
    activeActions_.clear();
    recentlySelectedActions_.clear();
    
    // Set up motor patterns
    activePatterns_.clear();
    
    // Configure neural connectivity
    setupStriatalConnectivity();
    setupOutputConnections();
    
    // Log initialization
    NLM_LOG_INFO("Basal ganglia initialized with dopamine modulation");
}

void BasalGanglia::update(TimestepDuration dt) {
    if (!brain_ || !dopamine_) return;
    
    // Update dopamine levels based on prediction errors
    updateDopamineLevels(dt);
    
    // Update striatal pathways
    if (actionSelectionEnabled_) {
        updateDirectPathway();
        updateIndirectPathway();
        updateActionSelection(dt);
    }
    
    // Update output structures
    if (inhibitionEnabled_) {
        updateOutputStructures();
    }
    
    // Update motor patterns
    updateMotorPatterns();
    
    // Clean up old actions
    cleanupOldActions();
}

void BasalGanglia::processRewardPredictionError(float reward, float predictedReward) {
    if (!valueLearningEnabled_) return;
    
    float rpe = reward - predictedReward;
    
    // Apply dopaminergic modulation
    if (dopamine_) {
        dopamine_->applyModulation(rpe);
    }
    
    // Update learning statistics
    cumulativeReward_ += reward;
    totalActionsSelected_++;
    
    // Trigger value learning for recently selected actions
    temporalDifferenceLearning(rpe);
}

void BasalGanglia::addAction(const ActionValue& action) {
    // Check if action already exists
    auto it = std::find_if(actionValues_.begin(), actionValues_.end(),
                          [&action](const ActionValue& a) { return a.actionId == action.actionId; });n    
    if (it == actionValues_.end()) {
        // Add new action
        actionValues_.push_back(action);
        
        // Initialize synaptic connections for this action
        initializeActionSynapticConnections(action.actionId);
        
        NLM_LOG_INFO("Added action " + std::to_string(action.actionId.index()) + ": " + action.actionName);
    } else {
        NLM_LOG_INFO("Action already exists: " + std::to_string(action.actionId.index()));
    }
}

void BasalGanglia::removeAction(uint64_t actionId) {
    // Remove action from selection pool
    auto it = std::remove_if(actionValues_.begin(), actionValues_.end(),
                           [&actionId](const ActionValue& a) { return a.actionId == actionId; });n    
    if (it != actionValues_.end()) {
        actionValues_.erase(it, actionValues_.end());
        
        // Clean up synaptic connections
        cleanupActionSynapticConnections(actionId);
        
        NLM_LOG_INFO("Removed action " + std::to_string(actionId.index()));
    }
}

ActionValue BasalGanglia::selectAction() {
    if (!actionSelectionEnabled_ || actionValues_.empty()) {
        return ActionValue();
    }
    
    // Use appropriate action selection algorithm
    ActionValue selectedAction;
    
    if (actionValues_.size() <= 10) {
        // For small action spaces, use softmax
        selectedAction = softmaxActionSelection();
    } else if (explorationRate_ > 0.3f) {
        // For high exploration, use epsilon-greedy
        selectedAction = epsilonGreedyActionSelection();
    } else {
        // For normal operation, use UCB
        selectedAction = UCBActionSelection();
    }
    
    // Add to recently selected actions
    recentlySelectedActions_.push_back(selectedAction.actionId);
    if (recentlySelectedActions_.size() > actionWindowSize_) {
        recentlySelectedActions_.pop_front();
    }
    
    // Update selection statistics
    totalActionsSelected_++;
    if (selectedAction.explorationValue > 0.5f) {
        explorationCount_++;
    }
    
    // Mark as selected
    selectedAction.selected = true;
    
    // Add to active actions
    activeActions_.push_back(selectedAction);
    
    // Log action selection
    NLM_LOG_INFO("Selected action " + selectedAction.actionName + 
               " (value: " + std::to_string(selectedAction.expectedValue) + 
               ", exploration: " + std::to_string(selectedAction.explorationValue) + ")");
    
    return selectedAction;
}

void BasalGanglia::updateActionValues(const ActionResult& result) {
    if (!valueLearningEnabled_) return;
    
    // Find the most recently selected action
    if (recentlySelectedActions_.empty()) {
        return;
    }
    
    uint64_t lastActionId = recentlySelectedActions_.back();
    
    // Update the action's expected value
    for (auto& action : actionValues_) {
        if (action.actionId == lastActionId) {
            // Update expected value based on result
            float valueChange = (result.reward - action.expectedValue) * learningRate_;
            action.expectedValue += valueChange;
            
            // Update past rewards
            action.pastRewards.push_back(result.reward);
            if (action.pastRewards.size() > 10) {
                action.pastRewards.erase(action.pastRewards.begin());
            }
            
            // Calculate average past value
            float avgPastValue = std::accumulate(action.pastRewards.begin(), 
                                               action.pastRewards.end(), 0.0f) / 
                               static_cast<float>(action.pastRewards.size());
            action.learnedPolicy = softmax(avgPastValue);
            
            // Log value update
            NLM_LOG_INFO("Updated action " + std::to_string(action.actionId.index()) +
                       " value to " + std::to_string(action.expectedValue));
            
            break;
        }
    }
}

void BasalGanglia::formHabit(uint64_t actionId, float successThreshold) {
    if (!habitFormationEnabled_) return;
    
    // Find the action
    for (auto& action : actionValues_) {
        if (action.actionId == actionId) {
            // Calculate success rate
            float successRate = calculateSuccessRate(actionId);
            
            if (successRate >= successThreshold) {
                // Form habit
                action.habitStrength += habitFormationRate_;
                action.habitStrength = std::min(1.0f, action.habitStrength);
                
                // Adjust learned policy to reflect habit strength
                action.learnedPolicy = 0.7f + action.habitStrength * 0.3f;
                
                // Increase urgency for habit actions
                action.urgency = std::min(1.0f, action.urgency + 0.1f);
                
                NLM_LOG_INFO("Habit formed for action " + std::to_string(actionId.index()) +
                           " with strength " + std::to_string(action.habitStrength) +
                           " (success rate: " + std::to_string(successRate) + ")");
            }
            
            break;
        }
    }
}

std::vector<MotorCommand> BasalGanglia::generateMotorCommands(const ActionValue& action) {
    // Convert action to motor commands
    std::vector<MotorCommand> commands;
    
    // Base commands from action type
    if (action.actionName.find("MoveForward") != std::string::npos) {
        commands.push_back(MotorCommand::MoveForward);
    } else if (action.actionName.find("MoveBackward") != std::string::npos) {
        commands.push_back(MotorCommand::MoveBackward);
    } else if (action.actionName.find("TurnLeft") != std::string::npos) {
        commands.push_back(MotorCommand::TurnLeft);
    } else if (action.actionName.find("TurnRight") != std::string::npos) {
        commands.push_back(MotorCommand::TurnRight);
    } else if (action.actionName.find("Look") != std::string::npos) {
        commands.push_back(MotorCommand::Look);
    } else if (action.actionName.find("LookUp") != std::string::npos) {
        commands.push_back(MotorCommand::LookUp);
    } else if (action.actionName.find("LookDown") != std::string::npos) {
        commands.push_back(MotorCommand::LookDown);
    } else if (action.actionName.find("Interact") != std::string::npos) {
        commands.push_back(MotorCommand::Interact);
    } else if (action.actionName.find("Eat") != std::string::npos) {
        commands.push_back(MotorCommand::Eat);
    } else if (action.actionName.find("Drink") != std::string::npos) {
        commands.push_back(MotorCommand::Drink);
    } else if (action.actionName.find("Rest") != std::string::npos) {
        commands.push_back(MotorCommand::Rest);
    } else {
        commands.push_back(MotorCommand::Wait);
    }
    
    // Add context-dependent variations
    addContextualVariations(commands, action);
    
    // Add habit-based variations
    addHabitBasedVariations(commands, action);
    
    // Add exploration-based variations
    if (uniformDist(rng) < action.explorationValue) {
        addExplorationVariations(commands);
    }
    
    return commands;
}

float BasalGanglia::getActionSelectionEntropy() const {
    if (actionValues_.empty()) {
        return 0.0f;
    }
    
    std::vector<float> probabilities;
    float totalValue = 0.0f;
    
    for (const auto& action : actionValues_) {
        float value = action.expectedValue + action.explorationValue * explorationRate_;
        totalValue += value;
        probabilities.push_back(value);
    }
    
    // Convert to probabilities using softmax
    float sumProb = 0.0f;
    for (float value : probabilities) {
        float prob = softmax(value / totalValue);
        sumProb += prob;
    }
    
    // Calculate entropy
    float entropy = 0.0f;
    for (float prob : probabilities) {
        if (prob > 0.0f) {
            entropy -= prob * std::log(prob);
        }
    }
    
    // Normalize by maximum possible entropy
    float maxEntropy = std::log(static_cast<float>(actionValues_.size()));
    if (maxEntropy > 0.0f) {
        entropy /= maxEntropy;
    }
    
    return entropy;
}

float BasalGanglia::getExplorationRate() const {
    return explorationCount_ / std::max(1.0f, static_cast<float>(totalActionsSelected_));
}

float BasalGanglia::getHabitFormationRate() const {
    float totalHabitStrength = 0.0f;
    for (const auto& action : actionValues_) {
        totalHabitStrength += action.habitStrength;
    }
    
    return actionValues_.empty() ? 0.0f : totalHabitStrength / actionValues_.size();
}

size_t BasalGanglia::getActionCount() const {
    return actionValues_.size();
}

float BasalGanglia::getTotalActionValue() const {
    float totalValue = 0.0f;
    for (const auto& action : actionValues_) {
        totalValue += action.expectedValue;
    }
    return totalValue;
}

// Private helper methods

void BasalGanglia::updateDirectPathway() {
    // Update direct pathway (D1) neurons
    for (auto& neuron : directPath_) {
        // Update firing based on cortical input
        float corticalInput = getCorticalInput(neuron);
        neuron.updateFiring(corticalInput);
        
        // Apply dopaminergic modulation
        if (dopamine_) {
            float dopamineLevel = dopamine_->getLevel();
            neuron.applyDopamineModulation(dopamineLevel);
        }
    }
}

void BasalGanglia::updateIndirectPathway() {
    // Update indirect pathway (D2) neurons
    for (auto& neuron : indirectPath_) {
        // Update firing based on cortical input
        float corticalInput = getCorticalInput(neuron);
        neuron.updateFiring(corticalInput);
        
        // Apply dopaminergic modulation
        if (dopamine_) {
            float dopamineLevel = dopamine_->getLevel();
            neuron.applyDopamineModulation(dopamineLevel);
        }
    }
}

void BasalGanglia::updateOutputStructures() {
    // Update globus pallidus and subthalamic nucleus
    float directActivity = calculateDirectPathwayActivity();
    float indirectActivity = calculateIndirectPathwayActivity();
    
    // Update GPi inhibition
    float gpiInhibition = calculateGPIInhibition(directActivity, indirectActivity);
    for (auto& effector : gpi_) {
        effector.updateInhibition(gpiInhibition);
    }
    
    // Update STN facilitation
    float stnExcitability = calculateSTNExcitability(directActivity, indirectActivity);
    for (auto& nucleus : stn_) {
        nucleus.updateExcitability(stnExcitability);
    }
}

void BasalGanglia::updateActionSelection(TimestepDuration dt) {
    // Update action selection based on striatal activity
    std::vector<float> actionProbabilities;
    
    // Calculate action selection probabilities based on pathway activity
    for (const auto& action : actionValues_) {
        float pathwayActivity = calculateActionPathwayActivity(action.actionId);
        actionProbabilities.push_back(pathwayActivity);
    }
    
    // Normalize probabilities
    float sumProb = std::accumulate(actionProbabilities.begin(), actionProbabilities.end(), 0.0f);
    if (sumProb > 0.0f) {
        for (auto& prob : actionProbabilities) {
            prob /= sumProb;
        }
    }
    
    // Select actions based on probabilities
    activeActions_.clear();
    
    std::vector<size_t> indices(actionValues_.size());
    std::iota(indices.begin(), indices.end(), 0);
    
    std::shuffle(indices.begin(), indices.end(), rng);
    
    float cumulativeProb = 0.0f;
    for (size_t idx : indices) {
        cumulativeProb += actionProbabilities[idx];
        if (uniformDist(rng) < cumulativeProb) {
            activeActions_.push_back(actionValues_[idx]);
        }
    }
}

void BasalGanglia::updateMotorPatterns() {
    // Update motor patterns based on action selection
    activePatterns_.clear();
    
    for (const auto& action : activeActions_) {
        MotorPattern pattern;
        pattern.patternId = generatePatternId();
        pattern.patternName = action.actionName;
        pattern.predictedReward = action.expectedValue;
        pattern.explorationValue = action.explorationValue;
        pattern.urgency = action.urgency;
        pattern.habitStrength = action.habitStrength;
        pattern.executionProbability = action.learnedPolicy;
        pattern.riskLevel = action.riskLevel;
        
        pattern.commands = generateMotorCommands(action);
        pattern.neuralPattern = createNeuralPattern(action);
        pattern.activationLevels = generateActivationLevels(action);
        
        activePatterns_.push_back(pattern);
    }
}

void BasalGanglia::cleanupOldActions() {
    // Remove actions that haven't been used recently
    auto cutoffTime = getCurrentTime() - 1000.0f;  // Remove actions not used in last 1000ms
    
    actionValues_.erase(std::remove_if(actionValues_.begin(), actionValues_.end(),
                                      [cutoffTime](const ActionValue& action) {
                                          return action.lastUsedTime < cutoffTime;
                                      }),
                       actionValues_.end());
}

void BasalGanglia::setupStriatalConnectivity() {
    // Set up striatal neural connectivity
    size_t striatumSize = 100;
    
    // Initialize direct pathway (D1) neurons
    directPath_.clear();
    directPath_.reserve(striatumSize / 2);
    
    for (size_t i = 0; i < striatumSize / 2; ++i) {
        MediumSpinyNeuron neuron;
        neuron.id = NeuronId(i + 1);
        neuron.pathway = MediumSpinyNeuron::EXCITATORY_GLUTAMATE;
        neuron.firingRate = 0.0f;
        neuron.eligibilityTrace = 0.0f;
        
        directPath_.push_back(neuron);
    }
    
    // Initialize indirect pathway (D2) neurons
    indirectPath_.clear();
    indirectPath_.reserve(striatumSize / 2);
    
    for (size_t i = 0; i < striatumSize / 2; ++i) {
        MediumSpinyNeuron neuron;
        neuron.id = NeuronId(striatumSize / 2 + i + 1);
        neuron.pathway = MediumSpinyNeuron::EXCITATORY_GLUTAMATE;
        neuron.firingRate = 0.0f;
        neuron.eligibilityTrace = 0.0f;
        
        indirectPath_.push_back(neuron);
    }
}

void BasalGanglia::setupOutputConnections() {
    // Set up output nucleus connections
    size_t gpiSize = 50;
    size_t stnSize = 50;
    
    gpi_.clear();
    gpi_.reserve(gpiSize);
    
    for (size_t i = 0; i < gpiSize; ++i) {
        GoliEffector effector;
        effector.id = NeuronId(gpiSize + i + 1);
        effector.baselineInhibition = 0.5f;
        effector.plasticInhibition = 0.0f;
        effector.burstInhibition = 0.0f;
        effector.currentInhibition = effector.baselineInhibition;
        
        gpi_.push_back(effector);
    }
    
    stn_.clear();
    stn_.reserve(stnSize);
    
    for (size_t i = 0; i < stnSize; ++i) {
        SubthalamicNucleus nucleus;
        nucleus.id = NeuronId(gpiSize + stnSize + i + 1);
        nucleus.baselineExcitability = 0.5f;
        nucleus.plasticExcitability = 0.0f;
        nucleus.burstFacilitation = 0.0f;
        nucleus.currentExcitability = nucleus.baselineExcitability;
        
        stn_.push_back(nucleus);
    }
}

void BasalGanglia::initializeActionSynapticConnections(uint64_t actionId) {
    // Initialize synaptic connections for a new action
    std::vector<MediumSpinyNeuron*> striatalNeurons;
    
    // Connect to striatal neurons
    for (auto& neuron : directPath_) {
        if (neuron.pathway == MediumSpinyNeuron::EXCITATORY_GLUTAMATE) {
            striatalNeurons.push_back(&neuron);
        }
    }
    
    for (auto& neuron : indirectPath_) {
        if (neuron.pathway == MediumSpinyNeuron::EXCITATORY_GLUTAMATE) {
            striatalNeurons.push_back(&neuron);
        }
    }
    
    // Set up synaptic weights
    for (auto& neuron : striatalNeurons) {
        // Initialize synaptic weight based on action properties
        float weight = calculateActionSynapticWeight(actionId, neuron->id);
        // neuron->addSynapticConnection(weight);  // Would need to be implemented
    }
}

void BasalGanglia::cleanupActionSynapticConnections(uint64_t actionId) {
    // Clean up synaptic connections for removed action
    // Implementation depends on specific synaptic connection management
}

void BasalGanglia::updateDopamineLevels(TimestepDuration dt) {
    if (!dopamine_) return;
    
    // Calculate reward prediction error
    float rpe = calculateRewardPredictionError();
    
    // Update dopamine levels based on RPE
    float currentDopamine = dopamine_->getLevel();
    float dopamineChange = learningRate_ * rpe;
    dopamine_->setLevel(currentDopamine + dopamineChange);
    
    // Apply bounded update
    dopamine_->setLevel(std::clamp(dopamine_->getLevel(), -1.0f, 1.0f));
}

void BasalGanglia::temporalDifferenceLearning(float tdError) {
    if (!valueLearningEnabled_) return;
    
    // Update action values based on temporal difference error
    for (auto& action : actionValues_) {
        // Update eligibility trace
        action.eligibilityTrace *= 0.9f;  // Trace decay
        action.eligibilityTrace += 1.0f;  // Add new eligibility
        
        // Update action value based on TD error
        float valueUpdate = learningRate_ * tdError * action.eligibilityTrace;
        action.expectedValue += valueUpdate;
        
        // Update selection statistics
        action.selectionWeight += std::abs(valueUpdate);
    }
}

float BasalGanglia::calculateSuccessRate(uint64_t actionId) const {
    // Calculate success rate for an action
    const auto& action = findAction(actionId);
    if (!action) {
        return 0.0f;
    }
    
    if (action->pastRewards.empty()) {
        return 0.0f;
    }
    
    float totalReward = std::accumulate(action->pastRewards.begin(),
                                      action->pastRewards.end(), 0.0f);
    float avgReward = totalReward / action->pastRewards.size();
    
    // Normalize to [0, 1] range
    return std::max(0.0f, std::min(1.0f, avgReward * 2.0f - 0.5f));
}

const ActionValue* BasalGanglia::findAction(uint64_t actionId) const {
    for (const auto& action : actionValues_) {
        if (action.actionId == actionId) {
            return &action;
        }
    }
    return nullptr;
}

float BasalGanglia::calculateRewardPredictionError() const {
    if (recentlySelectedActions_.empty()) {
        return 0.0f;
    }
    
    uint64_t lastActionId = recentlySelectedActions_.back();
    const auto& action = findAction(lastActionId);
    
    if (!action) {
        return 0.0f;
    }
    
    // Calculate actual reward based on action outcome
    float actualReward = getActualRewardForAction(*action);
    
    return actualReward - action->expectedValue;
}

float BasalGanglia::getActualRewardForAction(const ActionValue& action) const {
    // Calculate actual reward based on action properties and context
    float reward = 0.0f;
    
    // Base reward from action value
    reward += action.expectedValue * 0.5f;
    
    // Add exploration bonus
    reward += action.explorationValue * 0.1f;
    
    // Add habit bonus
    reward += action.habitStrength * 0.2f;
    
    // Add urgency bonus
    reward += action.urgency * 0.1f;
    
    // Add contextual bonus
    float contextualBonus = 0.0f;
    for (const auto& label : action.contextLabels) {
        if (isContextRelevant(label)) {
            contextualBonus += 0.1f;
        }
    }
    reward += contextualBonus;
    
    // Add risk-adjusted reward
    float riskAdjustment = 1.0f - action.riskLevel * 0.3f;
    reward *= riskAdjustment;
    
    return std::max(0.0f, reward);
}

bool BasalGanglia::isContextRelevant(const std::string& label) const {
    // Check if context label is relevant to current situation
    // Implementation depends on specific context management
    return false;  // Placeholder
}

float BasalGanglia::softmax(float x) const {
    return std::exp(x) / (1.0f + std::exp(x));
}

float BasalGanglia::getCurrentTime() const {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    return static_cast<float>(duration.count() / 1000000.0);
}

uint64_t BasalGanglia::generatePatternId() {
    static uint64_t patternCounter = 0;
    return ++patternCounter;
}

std::vector<float> BasalGanglia::generateActivationLevels(const ActionValue& action) const {
    std::vector<float> levels;
    
    // Generate activation levels based on action properties
    for (size_t i = 0; i < action.commands.size(); ++i) {
        float level = action.urgency * action.habitStrength;
        level += (i + 1) * 0.1f;  // Add sequence effect
        levels.push_back(level);
    }
    
    return levels;
}

std::vector<NeuronId> BasalGanglia::createNeuralPattern(const ActionValue& action) const {
    // Create neural pattern representation for action
    std::vector<NeuronId> pattern;
    
    // Generate random neural pattern based on action
    size_t patternSize = std::min<size_t>(100, action.actionName.size() * 2);
    for (size_t i = 0; i < patternSize; ++i) {
        pattern.push_back(NeuronId(i + 1));
    }
    
    return pattern;
}

void BasalGanglia::addContextualVariations(std::vector<MotorCommand>& commands,
                                         const ActionValue& action) const {
    // Add context-dependent variations to motor commands
    if (uniformDist(rng) < 0.3f) {
        // Add small variation
        int variation = static_cast<int>(uniformDist(rng) * commands.size());
        if (variation < commands.size()) {
            // Modify command based on context
            commands[variation] = modifyCommand(commands[variation]);
        }
    }
}

void BasalGanglia::addHabitBasedVariations(std::vector<MotorCommand>& commands,
                                          const ActionValue& action) const {
    // Add habit-based variations
    if (action.habitStrength > 0.5f) {
        // Add stable habit pattern
        for (size_t i = 0; i < commands.size(); ++i) {
            if (uniformDist(rng) < action.habitStrength * 0.2f) {
                commands[i] = addHabitVariation(commands[i]);
            }
        }
    }
}

void BasalGanglia::addExplorationVariations(std::vector<MotorCommand>& commands) const {
    // Add exploration-based variations
    for (size_t i = 0; i < commands.size(); ++i) {
        if (uniformDist(rng) < 0.2f) {
            commands[i] = addExplorationVariation(commands[i]);
        }
    }
}

MotorCommand BasalGanglia::modifyCommand(MotorCommand original) const {
    // Modify a motor command based on context
    switch (uniformDist(rng) * 6) {
        case 0: return MotorCommand::MoveForward;
        case 1: return MotorCommand::MoveBackward;
        case 2: return MotorCommand::TurnLeft;
        case 3: return MotorCommand::TurnRight;
        case 4: return MotorCommand::Look;
        default: return MotorCommand::Wait;
    }
}

MotorCommand BasalGanglia::addHabitVariation(MotorCommand original) const {
    // Add habit-based variation
    return original;  // Placeholder
}

MotorCommand BasalGanglia::addExplorationVariation(MotorCommand original) const {
    // Add exploration-based variation
    return original;  // Placeholder
}

float BasalGanglia::calculateDirectPathwayActivity() const {
    float totalActivity = 0.0f;
    for (const auto& neuron : directPath_) {
        totalActivity += neuron.firingRate;
    }
    return totalActivity / std::max(1.0f, static_cast<float>(directPath_.size()));
}

float BasalGanglia::calculateIndirectPathwayActivity() const {
    float totalActivity = 0.0f;
    for (const auto& neuron : indirectPath_) {
        totalActivity += neuron.firingRate;
    }
    return totalActivity / std::max(1.0f, static_cast<float>(indirectPath_.size()));
}

float BasalGanglia::calculateGPIInhibition(float directActivity, float indirectActivity) const {
    // Calculate globus pallidus internal inhibition
    float inhibition = indirectActivity - directActivity;
    return std::max(0.0f, std::min(1.0f, inhibition));
}

float BasalGanglia::calculateSTNExcitability(float directActivity, float indirectActivity) const {
    // Calculate subthalamic nucleus excitability
    float excitability = directActivity + indirectActivity;
    return std::max(0.0f, std::min(1.0f, excitability));
}

float BasalGanglia::calculateActionPathwayActivity(uint64_t actionId) const {
    // Calculate pathway activity for an action
    float directInfluence = calculateDirectInfluence(actionId);
    float indirectInfluence = calculateIndirectInfluence(actionId);
    
    return directInfluence - indirectInfluence;
}

float BasalGanglia::calculateDirectInfluence(uint64_t actionId) const {
    // Calculate influence on direct pathway
    float influence = 0.0f;
    
    const auto& action = findAction(actionId);
    if (!action) {
        return influence;
    }
    
    // Calculate based on action value and habit strength
    influence = action->expectedValue * action->habitStrength;
    
    return influence;
}

float BasalGanglia::calculateIndirectInfluence(uint64_t actionId) const {
    // Calculate influence on indirect pathway
    float influence = 0.0f;
    
    const auto& action = findAction(actionId);
    if (!action) {
        return influence;
    }
    
    // Calculate based on action exploration value
    influence = action->explorationValue * (1.0f - action->habitStrength);
    
    return influence;
}

float BasalGanglia::calculatePatternCompetitionScore(const MotorPattern& pattern) const {
    // Calculate competition score for a motor pattern
    float score = 0.0f;
    
    // Calculate based on pattern strength and competition
    score = pattern.patternStrength * (1.0f - pattern.patternInhibition);
    
    return score;
}

std::vector<float> BasalGanglia::calculateSelectionProbabilities() const {
    // Calculate selection probabilities for all actions
    std::vector<float> probabilities;
    float totalValue = 0.0f;
    
    for (const auto& action : actionValues_) {
        float value = action.expectedValue + action.explorationValue * explorationRate_;
        totalValue += value;
        probabilities.push_back(value);
    }
    
    // Convert to probabilities
    std::vector<float> actionProbabilities;
    float sumProb = 0.0f;
    
    for (float value : probabilities) {
        float prob = softmax(value / totalValue);
        actionProbabilities.push_back(prob);
        sumProb += prob;
    }
    
    // Normalize
    for (auto& prob : actionProbabilities) {
        prob /= sumProb;
    }
    
    return actionProbabilities;
}

const ActionValue* BasalGanglia::findAction(uint64_t actionId) const {
    for (const auto& action : actionValues_) {
        if (action.actionId == actionId) {
            return &action;
        }
    }
    return nullptr;
}

std::vector<const EpisodicMemory*> BasalGanglia::selectEpisodes(
    const std::vector<const EpisodicMemory*>& episodes,
    const std::vector<float>& weights,
    size_t count) {
    
    std::vector<const EpisodicMemory*> selected;
    selected.reserve(count);
    
    if (episodes.empty()) {
        return selected;
    }
    
    // Weighted random selection
    std::vector<float> cumulativeWeights;
    float totalWeight = 0.0f;
    
    for (size_t i = 0; i < weights.size(); ++i) {
        totalWeight += weights[i];
        cumulativeWeights.push_back(totalWeight);
    }
    
    std::vector<size_t> indices(episodes.size());
    std::iota(indices.begin(), indices.end(), 0);
    
    std::shuffle(indices.begin(), indices.end(), rng);
    
    size_t selectedCount = std::min(count, episodes.size());
    for (size_t i = 0; i < selectedCount; ++i) {
        selected.push_back(episodes[indices[i]]);
    }
    
    return selected;
}

void BasalGanglia::setupStriatalConnectivity() {
    // Set up striatal neural connectivity
    size_t striatumSize = 100;
    
    directPath_.clear();
    directPath_.reserve(striatumSize / 2);
    
    for (size_t i = 0; i < striatumSize / 2; ++i) {
        MediumSpinyNeuron neuron;
        neuron.id = NeuronId(i + 1);
        neuron.pathway = MediumSpinyNeuron::EXCITATORY_GLUTAMATE;
        neuron.firingRate = 0.0f;
        neuron.eligibilityTrace = 0.0f;
        
        directPath_.push_back(neuron);
    }
    
    indirectPath_.clear();
    indirectPath_.reserve(striatumSize / 2);
    
    for (size_t i = 0; i < striatumSize / 2; ++i) {
        MediumSpinyNeuron neuron;
        neuron.id = NeuronId(striatumSize / 2 + i + 1);
        neuron.pathway = MediumSpinyNeuron::EXCITATORY_GLUTAMATE;
        neuron.firingRate = 0.0f;
        neuron.eligibilityTrace = 0.0f;
        
        indirectPath_.push_back(neuron);
    }
}

void BasalGanglia::setupOutputConnections() {
    // Set up output nucleus connections
    size_t gpiSize = 50;
    size_t stnSize = 50;
    
    gpi_.clear();
    gpi_.reserve(gpiSize);
    
    for (size_t i = 0; i < gpiSize; ++i) {
        GoliEffector effector;
        effector.id = NeuronId(gpiSize + i + 1);
        effector.baselineInhibition = 0.5f;
        effector.plasticInhibition = 0.0f;
        effector.burstInhibition = 0.0f;
        effector.currentInhibition = effector.baselineInhibition;
        
        gpi_.push_back(effector);
    }
    
    stn_.clear();
    stn_.reserve(stnSize);
    
    for (size_t i = 0; i < stnSize; ++i) {
        SubthalamicNucleus nucleus;
        nucleus.id = NeuronId(gpiSize + stnSize + i + 1);
        nucleus.baselineExcitability = 0.5f;
        nucleus.plasticExcitability = 0.0f;
        nucleus.burstFacilitation = 0.0f;
        nucleus.currentExcitability = nucleus.baselineExcitability;
        
        stn_.push_back(nucleus);
    }
}

} // namespace nlm

// Integration with NLM
namespace nlm {
namespace memory {

void integrateBasalGangliaWithNLM(BasalGanglia* basalGanglia) {
    if (!basalGanglia || !basalGanglia->getBrain()) {
        return;
    }
    
    // Get brain reference
    Brain* brain = basalGanglia->getBrain();
    
    // Add some example actions
    ActionValue moveForwardAction;
    moveForwardAction.actionId = 1;
    moveForwardAction.actionName = "MoveForward";
    moveForwardAction.expectedValue = 0.8f;
    moveForwardAction.explorationValue = 0.1f;
    moveForwardAction.urgency = 0.9f;
    moveForwardAction.habitStrength = 0.3f;
    moveForwardAction.learnedPolicy = 0.8f;
    moveForwardAction.riskLevel = 0.2f;
    moveForwardAction.contextLabels = {"navigation", "movement"};
    
    basalGanglia->addAction(moveForwardAction);
    
    ActionValue turnLeftAction;
    turnLeftAction.actionId = 2;
    turnLeftAction.actionName = "TurnLeft";
    turnLeftAction.expectedValue = 0.6f;
    turnLeftAction.explorationValue = 0.2f;
    turnLeftAction.urgency = 0.7f;
    turnLeftAction.habitStrength = 0.4f;
    turnLeftAction.learnedPolicy = 0.6f;
    turnLeftAction.riskLevel = 0.3f;
    turnLeftAction.contextLabels = {"navigation", "turning"};
    
    basalGanglia->addAction(turnLeftAction);
    
    ActionValue lookAction;
    lookAction.actionId = 3;
    lookAction.actionName = "Look";
    lookAction.expectedValue = 0.4f;
    lookAction.explorationValue = 0.5f;
    lookAction.urgency = 0.5f;
    lookAction.habitStrength = 0.2f;
    lookAction.learnedPolicy = 0.4f;
    lookAction.riskLevel = 0.4f;
    lookAction.contextLabels = {"perception", "sensing"};
    
    basalGanglia->addAction(lookAction);
}

} // namespace memory
} // namespace nlm