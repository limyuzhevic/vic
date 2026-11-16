// Enhanced Reward System Implementation
// 
// This file implements the enhanced reward system and motor control
// for Phase 6 integration experiments.

#include "experiments/EnhancedRewardSystem.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>

using namespace nlm;

EnhancedRewardSystem::EnhancedRewardSystem(std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent)
    : brain_(brain), agent_(agent), runningSum_(0.0f), runningSumOfSquares_(0.0f),
      predictedReward_(0.0f), rewardPredictionError_(0.0f), predictionErrorAccumulator_(0.0f),
      dopamineLevel_(0.0f), curiosityLevel_(0.0f), noveltyLevel_(0.0f), learning_(0.0f),
      explorationRate_(0.0f) {
    
    // Initialize reward history
    rewardHistory_.reserve(config_.rewardHistorySize);
    actionHistory_.reserve(1000);
    
    // Reset statistics
    stats_ = RewardStats();
}

EnhancedRewardSystem::~EnhancedRewardSystem() {}

float EnhancedRewardSystem::computeEnhancedReward(const ActionResult& actionResult,
                                                 const SensoryPercept& percept,
                                                 const MotorCommand& command) {
    // Compute base reward from action result
    float baseReward = computeBaseReward(actionResult);
    
    // Compute novelty reward
    float noveltyReward = computeNoveltyReward(percept);
    
    // Compute curiosity reward
    float curiosityReward = computeCuriosityReward(command, percept);
    
    // Apply neuromodulation weights
    float totalReward = baseReward * config_.resourceRewardWeight +
                       noveltyReward * config_.noveltyRewardWeight +
                       curiosityReward * config_.curiosityRewardWeight;
    
    // Apply learning-based adjustments
    if (learning_ > 0.0f) {
        // Scale reward based on learning state
        totalReward *= (1.0f + learning_ * 0.5f);
        
        // Apply exploration bonus
        if (explorationRate_ > 0.3f) {
            totalReward *= (1.0f + explorationRate_ * 0.2f);
        }
    }
    
    // Update reward prediction
    updateRewardPrediction(totalReward, predictedReward_);
    
    // Update statistics and history
    updateStatistics(totalReward);
    
    return totalReward;
}

void EnhancedRewardSystem::updateRewardPrediction(float actualReward, float predictedReward) {
    // Calculate prediction error
    float error = actualReward - predictedReward;
    rewardPredictionError_ = error;
    
    // Accumulate prediction error for learning
    predictionErrorAccumulator_ += std::abs(error) * config_.rewardPredictionLearningRate;
    
    // Update prediction based on actual reward (reward prediction learning)
    predictedReward_ = predictedReward_ + config_.rewardPredictionLearningRate * error;
    
    // Decay prediction error
    rewardPredictionError_ *= config_.rewardPredictionErrorDecay;
    
    // Update learning state based on prediction error
    learning_ = std::min(1.0f, learning_ + predictionErrorAccumulator_ * 0.01f);
    learning_ *= config_.dopamineDecayRate;
}

void EnhancedRewardSystem::updateNeuromodulation(float reward, float predictionError) {
    // Update dopamine level based on reward prediction error
    float rewardSignal = reward * 0.5f;
    float errorSignal = predictionError * 0.3f;
    
    dopamineLevel_ = config_.dopamineLearningRate * rewardSignal +
                    config_.dopamineLearningRate * errorSignal;
    dopamineLevel_ = std::max(0.0f, std::min(1.0f, dopamineLevel_));
    
    // Update curiosity level based on novelty and exploration
    float noveltyContribution = noveltyLevel_ * 0.7f;
    float explorationContribution = explorationRate_ * 0.3f;
    curiosityLevel_ = noveltyContribution + explorationContribution;
    curiosityLevel_ = std::max(0.0f, std::min(1.0f, curiosityLevel_));
    
    // Apply dopamine effects on plasticity
    if (dopamineLevel_ > 0.3f) {
        // Enhance plasticity when dopamine is high
        if (brain_ && brain_->getSTDP()) {
            brain_->getSTDP()->setLTPWeight(config_.dopamineLearningRate);
            brain_->getSTDP()->setLTDWeight(config_.dopamineLearningRate);
        }
    }
}

void EnhancedRewardSystem::updateStatistics(float reward) {
    // Update running statistics
    runningSum_ += reward;
    runningSumOfSquares_ += reward * reward;
    
    size_t count = rewardHistory_.size();
    
    // Add to history
    rewardHistory_.push_back(reward);
    if (rewardHistory_.size() > config_.rewardHistorySize) {
        rewardHistory_.erase(rewardHistory_.begin());
        runningSum_ -= rewardHistory_.front();  // Remove oldest contribution
    }
    
    // Update statistics
    if (count > 0) {
        stats_.totalSteps = count;
        stats_.meanReward = runningSum_ / count;
        
        if (count > 1) {
            float variance = (runningSumOfSquares_ / count) - (stats_.meanReward * stats_.meanReward);
            stats_.rewardVariance = std::max(0.0f, variance);
            stats_.maxReward = *std::max_element(rewardHistory_.begin(), rewardHistory_.end());
            stats_.minReward = *std::min_element(rewardHistory_.begin(), rewardHistory_.end());
        }
        
        // Calculate recent average (weighted towards recent rewards)
        float recentSum = 0.0f;
        int recentCount = 0;
        for (int i = std::max(0, static_cast<int>(count) - 100); i < static_cast<int>(count); ++i) {
            recentSum += rewardHistory_[static_cast<size_t>(i)];
            recentCount++;
        }
        stats_.recentAverage = recentCount > 0 ? recentSum / recentCount : stats_.meanReward;
    }
}

float EnhancedRewardSystem::computeBaseReward(const ActionResult& actionResult) const {
    // Simple base reward from action result
    return actionResult.reward;
}

float EnhancedRewardSystem::computeNoveltyReward(const SensoryPercept& percept) const {
    // Calculate novelty based on prediction error
    float totalChange = 0.0f;
    for (float v : percept.getVision()) {
        totalChange += std::abs(v - predictedVision_[v]);
    }
    
    // Normalize and scale
    float novelty = totalChange / std::max(1.0f, static_cast<float>(percept.getVision().size()));
    novelty = std::min(1.0f, novelty * 2.0f);  // Cap at 1.0
    
    return novelty;
}

float EnhancedRewardSystem::computeCuriosityReward(const MotorCommand& command,
                                                  const SensoryPercept& percept) const {
    // Calculate curiosity based on action diversity and exploration
    float explorationBonus = 0.0f;
    
    // Add bonus for exploring new actions
    MotorCommand lastAction = lastAction_;
    if (command != lastAction_) {
        explorationBonus += config_.curiosityExplorationBonus;
    }
    
    // Add bonus for novel sensory input
    float noveltyLevel = computeNoveltyReward(percept);
    explorationBonus += noveltyLevel * config_.curiosityRewardWeight;
    
    // Apply curiosity decay
    explorationBonus *= 0.9f;
    
    return explorationBonus;
}

EnhancedMotorControl::EnhancedMotorControl(std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent)
    : brain_(brain), agent_(agent), currentStrategy_(EXPLORATION_DRIVEN),
      strategyName_("ExplorationDriven") {
    
    actionHistory_.reserve(1000);
    actionStats_ = ActionStats();
}

EnhancedMotorControl::~EnhancedMotorControl() {}

MotorCommand EnhancedMotorControl::selectAction(const SensoryPercept& percept,
                                                float curiosityLevel,
                                                float noveltyLevel) {
    // Select action based on current strategy
    MotorCommand action;
    
    switch (currentStrategy_) {
        case SIMPLE_MAXIMUM:
            action = selectMaxFiring(percept);
            break;
        case EXPLORATION_DRIVEN:
            action = selectExploration(percept, curiosityLevel, noveltyLevel);
            break;
        case REWARD_PREDICTION:
            action = selectRewardPrediction(percept, curiosityLevel);
            break;
        case MULTI_STRATEGY:
            action = selectMultiStrategy(percept, curiosityLevel, noveltyLevel);
            break;
        default:
            action = selectMaxFiring(percept);
    }
    
    // Record action for learning
    actionHistory_.push_back(std::make_pair(action, curiosityLevel));
    if (actionHistory_.size() > 1000) {
        actionHistory_.erase(actionHistory_.begin());
    }
    
    // Apply exploration bonus
    action = addExplorationBonus(action, percept);
    
    // Update action statistics
    actionStats_ = computeActionScores(percept, curiosityLevel, noveltyLevel);
    
    return action;
}

MotorCommand EnhancedMotorControl::selectMaxFiring(const SensoryPercept& percept) {
    // Select action based on maximum motor neuron firing rate
    float maxScore = -1.0f;
    MotorCommand bestAction = MotorCommand::Wait;
    
    for (MotorCommand cmd : {MotorCommand::MoveForward, MotorCommand::MoveBackward,
                            MotorCommand::TurnLeft, MotorCommand::TurnRight,
                            MotorCommand::Interact, MotorCommand::Wait}) {
        float score = getActionScore(cmd);
        if (score > maxScore) {
            maxScore = score;
            bestAction = cmd;
        }
    }
    
    return bestAction;
}

MotorCommand EnhancedMotorControl::selectExploration(const SensoryPercept& percept,
                                                      float curiosityLevel,
                                                      float noveltyLevel) {
    // Select action using exploration strategy
    ActionStats scores = computeActionScores(percept, curiosityLevel, noveltyLevel);
    
    // Calculate exploration bonus for each action
    MotorCommand bestAction = MotorCommand::Wait;
    float maxScore = -1.0f;
    
    // Add exploration bonus to less common actions
    std::vector<MotorCommand> actions = {
        MotorCommand::MoveForward, MotorCommand::MoveBackward,
        MotorCommand::TurnLeft, MotorCommand::TurnRight,
        MotorCommand::Interact, MotorCommand::Wait
    };
    
    // Shuffle based on curiosity level
    if (curiosityLevel > 0.5f) {
        // Higher curiosity = more random exploration
        std::random_shuffle(actions.begin(), actions.end());
    }
    
    // Select best action
    for (MotorCommand cmd : actions) {
        float score = getActionScore(cmd);
        
        // Add exploration bonus for actions not recently used
        float explorationBonus = 0.0f;
        if (curiosityLevel > 0.3f) {
            explorationBonus = curiosityLevel * 0.2f * (1.0f - getActionFrequency(cmd));
        }
        
        float totalScore = score + explorationBonus;
        if (totalScore > maxScore) {
            maxScore = totalScore;
            bestAction = cmd;
        }
    }
    
    return bestAction;
}

MotorCommand EnhancedMotorControl::selectRewardPrediction(const SensoryPercept& percept,
                                                         float curiosityLevel) {
    // Select action using reward prediction strategy
    float maxScore = -1.0f;
    MotorCommand bestAction = MotorCommand::Wait;
    
    for (MotorCommand cmd : {MotorCommand::MoveForward, MotorCommand::MoveBackward,
                            MotorCommand::TurnLeft, MotorCommand::TurnRight,
                            MotorCommand::Interact, MotorCommand::Wait}) {
        float score = getActionScore(cmd);
        
        // Adjust score based on curiosity and prediction
        if (curiosityLevel > 0.5f) {
            // Higher curiosity = prefer novel actions
            score *= (1.0f + (curiosityLevel - 0.5f) * 0.5f);
        }
        
        // Add prediction bonus for actions with high confidence
        float confidence = getActionConfidence(cmd);
        if (confidence > 0.7f) {
            score *= (1.0f + confidence * 0.3f);
        }
        
        if (score > maxScore) {
            maxScore = score;
            bestAction = cmd;
        }
    }
    
    return bestAction;
}

MotorCommand EnhancedMotorControl::selectMultiStrategy(const SensoryPercept& percept,
                                                      float curiosityLevel,
                                                      float noveltyLevel) {
    // Select action using multiple strategies
    std::vector<MotorCommand> actions = {
        MotorCommand::MoveForward, MotorCommand::MoveBackward,
        MotorCommand::TurnLeft, MotorCommand::TurnRight,
        MotorCommand::Interact, MotorCommand::Wait
    };
    
    // Calculate scores for each strategy
    std::vector<float> strategyScores(actions.size(), 0.0f);
    
    for (size_t i = 0; i < actions.size(); ++i) {
        MotorCommand cmd = actions[i];
        
        // Maximum firing strategy
        float maxScore = getActionScore(cmd);
        
        // Exploration strategy
        ActionStats currentStats = computeActionScores(percept, curiosityLevel, noveltyLevel);
        float explorationBonus = 0.0f;
        if (curiosityLevel > 0.3f) {
            explorationBonus = curiosityLevel * 0.2f * (1.0f - getActionFrequency(cmd));
        }
        
        // Reward prediction strategy
        float confidence = getActionConfidence(cmd);
        float predictionBonus = confidence * 0.3f;
        
        // Combine strategies
        float totalScore = maxScore + explorationBonus + predictionBonus;
        strategyScores[i] = totalScore;
    }
    
    // Select best action
    size_t bestIndex = std::distance(strategyScores.begin(),
                                   std::max_element(strategyScores.begin(), strategyScores.end()));
    return actions[bestIndex];
}

ActionStats EnhancedMotorControl::computeActionScores(const SensoryPercept& percept,
                                                     float curiosityLevel,
                                                     float noveltyLevel) {
    // Calculate scores for each action based on multiple criteria
    ActionStats stats;
    
    for (MotorCommand cmd : {MotorCommand::MoveForward, MotorCommand::MoveBackward,
                            MotorCommand::TurnLeft, MotorCommand::TurnRight,
                            MotorCommand::Interact, MotorCommand::Wait}) {
        // Base score from motor neuron activity
        float baseScore = getActionScore(cmd);
        
        // Exploration bonus
        float explorationBonus = 0.0f;
        if (curiosityLevel > 0.3f) {
            explorationBonus = curiosityLevel * 0.2f * (1.0f - getActionFrequency(cmd));
        }
        
        // Novelty bonus
        float noveltyBonus = 0.0f;
        if (noveltyLevel > 0.5f) {
            noveltyBonus = noveltyLevel * 0.15f * (1.0f - getActionFrequency(cmd));
        }
        
        // Confidence bonus
        float confidenceBonus = getActionConfidence(cmd) * 0.25f;
        
        // Total score
        float totalScore = baseScore + explorationBonus + noveltyBonus + confidenceBonus;
        
        // Store in action-specific statistics
        switch (cmd) {
            case MotorCommand::MoveForward: stats.moveForwardScore = totalScore; break;
            case MotorCommand::MoveBackward: stats.moveBackwardScore = totalScore; break;
            case MotorCommand::TurnLeft: stats.turnLeftScore = totalScore; break;
            case MotorCommand::TurnRight: stats.turnRightScore = totalScore; break;
            case MotorCommand::Interact: stats.interactScore = totalScore; break;
            case MotorCommand::Wait: stats.waitScore = totalScore; break;
        }
    }
    
    return stats;
}

float EnhancedMotorControl::getActionScore(MotorCommand command) const {
    // Get action score based on motor neuron group activity
    size_t neuronCount = 0;
    
    switch (command) {
        case MotorCommand::MoveForward: neuronCount = motorForward_.size(); break;
        case MotorCommand::MoveBackward: neuronCount = motorBackward_.size(); break;
        case MotorCommand::TurnLeft: neuronCount = motorTurnLeft_.size(); break;
        case MotorCommand::TurnRight: neuronCount = motorTurnRight_.size(); break;
        case MotorCommand::Interact: neuronCount = motorInteract_.size(); break;
        case MotorCommand::Wait: neuronCount = motorWait_.size(); break;
    }
    
    // Normalize to [0, 1] range
    float maxNeurons = std::max({motorForward_.size(), motorBackward_.size(),
                               motorTurnLeft_.size(), motorTurnRight_.size(),
                               motorInteract_.size(), motorWait_.size()});
    
    return maxNeurons > 0 ? static_cast<float>(neuronCount) / maxNeurons : 0.0f;
}

float EnhancedMotorControl::getActionConfidence(MotorCommand command) const {
    // Calculate confidence based on action consistency and history
    float consistency = 1.0f - getActionFrequency(command);
    return consistency * 0.5f + 0.5f;  // Normalize to [0, 1]
}

float EnhancedMotorControl::getActionFrequency(MotorCommand command) const {
    // Calculate frequency of action in history
    if (actionHistory_.empty()) return 0.0f;
    
    size_t count = 0;
    for (const auto& history : actionHistory_) {
        if (history.first == command) {
            count++;
        }
    }
    
    return static_cast<float>(count) / actionHistory_.size();
}

MotorCommand EnhancedMotorControl::addExplorationBonus(const MotorCommand& baseAction,
                                                       const SensoryPercept& percept) const {
    // Add exploration bonus to base action
    MotorCommand bonusAction = baseAction;
    
    // Check if we should explore
    bool shouldExplore = false;
    
    // Explore if curiosity is high
    if (curiosityLevel_ > 0.6f) {
        shouldExplore = true;
    }
    
    // Explore if action hasn't been used recently
    if (getActionFrequency(baseAction) < 0.2f) {
        shouldExplore = true;
    }
    
    // Explore if novelty is high
    if (noveltyLevel_ > 0.8f) {
        shouldExplore = true;
    }
    
    if (shouldExplore) {
        // Select a different action
        std::vector<MotorCommand> actions = {
            MotorCommand::MoveForward, MotorCommand::MoveBackward,
            MotorCommand::TurnLeft, MotorCommand::TurnRight,
            MotorCommand::Interact, MotorCommand::Wait
        };
        
        // Remove base action from list
        auto it = std::find(actions.begin(), actions.end(), baseAction);
        if (it != actions.end()) {
            actions.erase(it);
        }
        
        if (!actions.empty()) {
            // Select random different action
            size_t randomIndex = std::rand() % actions.size();
            bonusAction = actions[randomIndex];
        }
    }
    
    return bonusAction;
}

Phase6EnhancedExperiment::Phase6EnhancedExperiment() : curiosityLevel_(0.0f), noveltyLevel_(0.0f) {
    brain_ = std::make_unique<Brain>();
    agent_ = std::make_unique<AgentBrain>(brain_);
    world_ = std::make_unique<SimpleWorld>();
    rewardSystem_ = std::make_unique<EnhancedRewardSystem>(brain_, agent_);
    motorControl_ = std::make_unique<EnhancedMotorControl>(brain_, agent_);
}

Phase6EnhancedExperiment::~Phase6EnhancedExperiment() {}

bool Phase6EnhancedExperiment::run(const EnhancedRewardConfig& rewardConfig) {
    // Initialize experiment
    if (!initialize()) {
        return false;
    }
    
    // Configure world
    world_->configure(20.0f, 20.0f, 8, 8);
    world_->reset();
    
    // Configure agent
    agent_->initialize(*world_);
    agent_->enableRewardModulation(true);
    agent_->enableCuriosity(true);
    
    // Set enhanced motor control strategy
    motorControl_->setStrategy("MultiStrategy");
    
    // Run enhanced simulation
    runEnhancedSimulation();
    
    // Collect results
    collectResults();
    
    return true;
}

void Phase6EnhancedExperiment::runEnhancedSimulation() {
    // Enhanced simulation loop with improved reward and motor control
    float totalReward = 0.0f;
    float totalFiringRate = 0.0f;
    size_t successfulActions = 0;
    size_t totalActions = 0;
    
    for (uint64_t step = 0; step < 2000; ++step) {
        // Get observation
        world_->update(0.1);  // Longer timestep for better world interaction
        SensoryPercept percept = world_->getSensoryPercept();
        
        // Process sensory input
        agent_->processSensoryInput(percept);
        
        // Get motor command using enhanced control
        MotorCommand cmd = motorControl_->selectAction(
            percept, rewardSystem_->getCuriosityLevel(), 
            rewardSystem_->getNoveltyLevel()
        );
        
        // Apply action to world
        ActionResult actionResult = world_->applyMotorCommand(cmd, step * 0.1);
        
        // Compute enhanced reward
        float reward = rewardSystem_->computeEnhancedReward(actionResult, percept, cmd);
        totalReward += reward;
        
        // Update reward prediction
        rewardSystem_->updateRewardPrediction(reward, rewardSystem_->getRewardPredictionError());
        
        // Apply reward modulation
        agent_->applyRewardModulation(reward, rewardSystem_->getRewardPredictionError());
        
        // Update development
        agent_->updateDevelopment(0.1);
        
        // Collect metrics
        totalFiringRate += brain_->getAverageFiringRate();
        if (brain_->getFiringNeuronCount() > 0) {
            successfulActions++;
        }
        totalActions++;
        
        // Update neuromodulation
        rewardSystem_->updateNeuromodulation(reward, rewardSystem_->getRewardPredictionError());
        
        // Log progress periodically
        if (step % 500 == 0) {
            std::cout << "Step " << step 
                      << " | Reward: " << reward
                      << " | Total: " << totalReward / (step + 1)
                      << " | Firing: " << brain_->getAverageFiringRate()
                      << " | Learning: " << rewardSystem_->isLearning()
                      << " | Curiosity: " << rewardSystem_->getCuriosityLevel()
                      << " | Dopamine: " << rewardSystem_->getDopamineLevel() << std::endl;
        }
    }
    
    // Store results
    results_.totalReward = totalReward;
    results_.avgFiringRate = totalFiringRate / 2000;
    results_.successfulActions = successfulActions;
    results_.dopamineLevel = rewardSystem_->getDopamineLevel();
    results_.curiosityLevel = rewardSystem_->getCuriosityLevel();
    results_.noveltyLevel = rewardSystem_->getNoveltyLevel();
    results_.rewardPredictionError = rewardSystem_->getRewardPredictionError();
    results_.learningProgress = rewardSystem_->isLearning() ? 1.0f : 0.0f;
    results_.actionEfficiency = totalActions > 0 ? static_cast<float>(successfulActions) / totalActions : 0.0f;
    results_.explorationRate = 1.0f - getMostFrequentActionFrequency();
}

void Phase6EnhancedExperiment::collectResults() {
    // Collect final experiment results
    auto rewardStats = rewardSystem_->getRewardStats();
    
    results_.totalReward = rewardStats.meanReward;
    results_.avgFiringRate = brain_->getAverageFiringRate();
    results_.rewardPredictionError = rewardStats.rewardVariance;
    results_.dopamineLevel = rewardSystem_->getDopamineLevel();
    results_.curiosityLevel = rewardSystem_->getCuriosityLevel();
    results_.noveltyLevel = rewardSystem_->getNoveltyLevel();
    results_.learningProgress = rewardSystem_->isLearning() ? 1.0f : 0.0f;
    results_.actionEfficiency = calculateActionEfficiency();
    results_.explorationRate = calculateExplorationRate();
}

void Phase6EnhancedExperiment::printSummary() const {
    std::cout << "===========================================\n";
    std::cout << "      PHASE 6 ENHANCED EXPERIMENT SUMMARY\n";
    std::cout << "===========================================\n\n";
    
    std::cout << "Enhanced Reward System:\n";
    std::cout << "  Mean Reward: " << results_.totalReward << "\n";
    std::cout << "  Reward Variance: " << results_.rewardPredictionError << "\n";
    std::cout << "  Dopamine Level: " << results_.dopamineLevel << "\n";
    std::cout << "  Curiosity Level: " << results_.curiosityLevel << "\n";
    std::cout << "  Novelty Level: " << results_.noveltyLevel << "\n";
    std::cout << "  Learning Progress: " << results_.learningProgress << "\n\n";
    
    std::cout << "Enhanced Motor Control:\n";
    std::cout << "  Average Firing Rate: " << results_.avgFiringRate << " Hz\n";
    std::cout << "  Action Efficiency: " << results_.actionEfficiency * 100 << "%\n";
    std::cout << "  Exploration Rate: " << results_.explorationRate * 100 << "%\n";
    std::cout << "  Successful Actions: " << results_.successfulActions << "\n\n";
    
    std::cout << "Integration Status:\n";
    std::cout << "  Reward Prediction: " << (results_.rewardPredictionError < 0.1f ? "GOOD" : "NEEDS IMPROVEMENT") << "\n";
    std::cout << "  Neuromodulation: " << (results_.dopamineLevel > 0.2f ? "ACTIVE" : "WEAK") << "\n";
    std::cout << "  Exploration: " << (results_.explorationRate > 0.3f ? "ACTIVE" : "LIMITED") << "\n";
    std::cout << "  Learning: " << (results_.learningProgress > 0.5f ? "PROGRESSING" : "STALLED") << "\n\n";
    
    if (results_.learningProgress > 0.5f && results_.actionEfficiency > 0.3f) {
        std::cout << "✓ ENHANCED EXPERIMENT SUCCESSFUL\n";
        std::cout << "  The enhanced reward system and motor control show promising results.\n";
        std::cout << "  Phase 6 integration is working with improved learning and exploration.\n";
    } else {
        std::cout << "✗ ENHANCED EXPERIMENT NEEDS IMPROVEMENT\n";
        std::cout << "  The reward system and motor control need further enhancements.\n";
    }
}

bool Phase6EnhancedExperiment::initialize() {
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 500);
    config->set("region_count", 2);
    config->set("connection_probability", 0.1f);
    config->set("random_seed", static_cast<int64_t>(42));
    
    // Initialize brain
    if (!brain_->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return false;
    }
    
    return true;
}

float Phase6EnhancedExperiment::calculateActionEfficiency() const {
    // Calculate action efficiency based on successful actions
    if (results_.successfulActions == 0) return 0.0f;
    return static_cast<float>(results_.successfulActions) / 2000.0f;
}

float Phase6EnhancedExperiment::calculateExplorationRate() const {
    // Calculate exploration rate based on action variety
    return 1.0f - getMostFrequentActionFrequency();
}

float Phase6EnhancedExperiment::getMostFrequentActionFrequency() const {
    // Get the most frequent action from motor control
    return motorControl_->getActionFrequency(MotorCommand::Wait); // Default
}