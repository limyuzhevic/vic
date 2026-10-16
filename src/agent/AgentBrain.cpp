#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <chrono>

namespace nlm {

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
    , explorationStrategy_(ExplorationStrategy::EpsilonGreedy)
    , curiosityMax_(1.0f)
    , curiosityDecay_(0.95f)
    , explorationEventTriggered_(false)
    , lastExplorationEventTime_(0.0)
    , stepsWithoutExploration_(0)
    , totalExplorationBonus_(0.0f)
{
    // Initialize motor and sensory neuron groups
    if (brain_) {
        initializeNeuronGroups();
        actionStats_ = std::make_unique<ActionStatistics>(getMotorOutputSize());
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initializeNeuronGroups() {
    setupMotorNeuronGroups();
    setupSensoryNeuronGroups();
}

void AgentBrain::setupMotorNeuronGroups() {
    // Clear existing groups
    motorForward_.clear();
    motorBackward_.clear();
    motorTurnLeft_.clear();
    motorTurnRight_.clear();
    motorInteract_.clear();
    motorWait_.clear();
    
    if (!brain_) return;
    
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            
            if (type == NeuronType::Motor) {
                for (Neuron* n : pop->getNeurons()) {
                    // Distribute motor neurons to different action groups using deterministic but varied distribution
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
                    // Distribute sensory neurons using deterministic but varied distribution
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

void AgentBrain::setupSensoryNeuronGroups() {
    // Sensory groups are already set up in initializeNeuronGroups
}

void AgentBrain::initialize(const SimpleWorld& world) {
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    actionStats_->actionValues.assign(actionStats_->actionValues.size(), 0.0f);
    actionStats_->actionCounts.assign(actionStats_->actionCounts.size(), 0);
    actionStats_->actionRewards.assign(actionStats_->actionRewards.size(), 0.0f);
    actionStats_->explorationBonus = 0.0f;
    
    resetCuriosity();
    stepsWithoutExploration_ = 0;
    
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
            sensoryProcessor_.processVisionSignal(vision[i], sensoryVision_[i]);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            sensoryProcessor_.processTouchSignal(touch[i], sensoryTouch_[i]);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            sensoryProcessor_.processInternalSignal(intern[i], sensoryInternal_[i]);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            sensoryProcessor_.processProprioceptionSignal(proprio[i], sensoryProprioception_[i]);
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
    updateCuriosityState(1.0);
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > motorSelectionParams_.noveltyThreshold) {
        decoded = selectWithCuriosity(decoded);
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
    
    // Update action statistics for exploration
    int actionIndex = static_cast<int>(best);
    if (actionIndex >= 0 && actionIndex < static_cast<int>(actionStats_->actionCounts.size())) {
        actionStats_->actionCounts[actionIndex]++;
        actionStats_->actionValues[actionIndex] = bestActivity;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Get current activities for all motor groups
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
    
    std::vector<MotorCommand> commands = {
        MotorCommand::MoveForward, MotorCommand::MoveBackward,
        MotorCommand::TurnLeft, MotorCommand::TurnRight,
        MotorCommand::Interact, MotorCommand::Wait
    };
    
    std::vector<float> activities = {forwardAct, backwardAct, leftAct, rightAct, interactAct, waitAct};
    
    // Apply exploration strategy
    MotorCommand selectedCommand = defaultCmd;
    
    switch (explorationStrategy_) {
        case ExplorationStrategy::None:
            selectedCommand = defaultCmd;
            break;
            
        case ExplorationStrategy::Random:
            selectedCommand = exploreRandom();
            break;
            
        case ExplorationStrategy::EpsilonGreedy:
            if (curiosityLevel_ > motorSelectionParams_.noveltyThreshold) {
                selectedCommand = exploreRandom();
            } else {
                selectedCommand = defaultCmd;
            }
            break;
            
        case ExplorationStrategy::UCB1:
            selectedCommand = exploreUCB1(commands, activities);
            break;
            
        case ExplorationStrategy::Boltzmann:
            selectedCommand = exploreBoltzmann(commands, activities, 1.0f);
            break;
            
        case ExplorationStrategy::ThompsonSampling:
            // Simplified Thompson sampling - in practice this would require Bayesian inference
            selectedCommand = exploreRandom();
            break;
    }
    
    stepsWithoutExploration_++;
    return selectedCommand;
}

MotorCommand AgentBrain::exploreRandom() {
    int choice = brain_->getRandomGenerator()->uniformInt(0, 7); // 0-7 covers all motor commands
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

MotorCommand AgentBrain::exploreUCB1(const std::vector<MotorCommand>& commands, const std::vector<float>& activities) {
    if (commands.size() != activities.size() || commands.empty()) {
        return MotorCommand::Wait;
    }
    
    float totalVisits = 0.0f;
    for (float count : actionStats_->actionCounts) {
        totalVisits += count;
    }
    
    if (totalVisits == 0.0f) {
        // No visits yet, choose randomly
        return exploreRandom();
    }
    
    float bestScore = -std::numeric_limits<float>::max();
    MotorCommand bestCommand = MotorCommand::Wait;
    
    for (size_t i = 0; i < commands.size(); ++i) {
        float avgReward = actionStats_->actionRewards[i] / std::max(1.0f, actionStats_->actionCounts[i]);
        float explorationBonus = motorSelectionParams_.curiosityWeight * 
                                 std::sqrt(std::log(totalVisits) / std::max(1.0f, actionStats_->actionCounts[i]));
        
        float score = avgReward + actionStats_->explorationBonus + explorationBonus;
        
        if (score > bestScore) {
            bestScore = score;
            bestCommand = commands[i];
        }
    }
    
    return bestCommand;
}

MotorCommand AgentBrain::exploreBoltzmann(const std::vector<MotorCommand>& commands, const std::vector<float>& activities, float temperature) {
    if (commands.size() != activities.size() || commands.empty()) {
        return MotorCommand::Wait;
    }
    
    std::vector<float> probabilities(commands.size(), 0.0f);
    float sum = 0.0f;
    
    for (size_t i = 0; i < commands.size(); ++i) {
        float score = activities[i] + motorSelectionParams_.curiosityWeight * curiosityLevel_;
        probabilities[i] = std::exp(score / temperature);
        sum += probabilities[i];
    }
    
    if (sum <= 0.0f) {
        return exploreRandom();
    }
    
    // Normalize probabilities
    for (float& prob : probabilities) {
        prob /= sum;
    }
    
    // Choose action based on probabilities
    float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
    float cumulative = 0.0f;
    
    for (size_t i = 0; i < commands.size(); ++i) {
        cumulative += probabilities[i];
        if (r <= cumulative) {
            return commands[i];
        }
    }
    
    return commands.back();
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
    
    // Update action rewards for learning
    int defaultAction = static_cast<int>(decodeFromMotorNeurons());
    if (defaultAction >= 0 && defaultAction < static_cast<int>(actionStats_->actionRewards.size())) {
        actionStats_->actionRewards[defaultAction] += reward;
    }
    
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

void AgentBrain::updateNeuromodulationState() {
    // Update curiosity based on novelty and prediction error
    if (curiosityEnabled_) {
        float noveltyInfluence = noveltyLevel_ * motorSelectionParams_.noveltyThreshold;
        float errorInfluence = std::abs(predictionError_) * motorSelectionParams_.errorWeight;
        
        curiosityLevel_ = std::min(curiosityMax_, 
                                   noveltyInfluence + errorInfluence);
        
        // Decay curiosity over time
        curiosityLevel_ *= curiosityDecay_;
        
        // Habituation: reduce curiosity with repeated exposure
        if (stepsWithoutExploration_ > 0) {
            curiosityLevel_ *= (1.0f - motorSelectionParams_.habituationRate / stepsWithoutExploration_);
        }
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

void AgentBrain::triggerExplorationEvent() {
    explorationEventTriggered_ = true;
    lastExplorationEventTime_ = developmentalAge_;
    NLM_LOG_INFO("Exploration event triggered by external stimulus");
}

void AgentBrain::resetCuriosity() {
    curiosityLevel_ = 0.0f;
    explorationEventTriggered_ = false;
    stepsWithoutExploration_ = 0;
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
    
    // Reset action statistics
    if (actionStats_) {
        actionStats_->actionValues.assign(actionStats_->actionValues.size(), 0.0f);
        actionStats_->actionCounts.assign(actionStats_->actionCounts.size(), 0);
        actionStats_->actionRewards.assign(actionStats_->actionRewards.size(), 0.0f);
        actionStats_->explorationBonus = 0.0f;
    }
    
    resetCuriosity();
}

void AgentBrain::updateCuriosityState(double timestep) {
    updateNeuromodulationState();
}

} // namespace nlm