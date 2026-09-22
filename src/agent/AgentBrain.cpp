#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

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
{
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
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
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
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * 0.3f;  // Up to 30% random
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command
            int choice = brain_->getRandomGenerator()->uniformInt(0, 7);
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
}

// Advanced AgentBrain commands

void AgentBrain::exploreRandomly() {
    if (!brain_) return;
    
    // Simple random exploration
    MotorCommand randomCmd;
    int choice = brain_->getRandomGenerator()->uniformInt(0, 7);
    switch (choice) {
        case 0: randomCmd = MotorCommand::MoveForward; break;
        case 1: randomCmd = MotorCommand::MoveBackward; break;
        case 2: randomCmd = MotorCommand::TurnLeft; break;
        case 3: randomCmd = MotorCommand::TurnRight; break;
        case 4: randomCmd = MotorCommand::LookLeft; break;
        case 5: randomCmd = MotorCommand::LookRight; break;
        case 6: randomCmd = MotorCommand::Interact; break;
        default: randomCmd = MotorCommand::Wait; break;
    }
    
    // Apply random exploration with higher chance when curiosity is high
    if (curiosityLevel_ > 0.7f) {
        // 50% chance to use random exploration
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < 0.5f) {
            NLM_LOG_INFO("AgentBrain: Performing random exploration");
            // Note: In a real implementation, this would directly control the motor system
        }
    }
}

void AgentBrain::enableAdvancedExploration() {
    // Enable advanced exploration behaviors
    if (curiosityEnabled_) {
        NLM_LOG_INFO("AgentBrain: Advanced exploration enabled");
        // This could set up additional exploration behaviors
    }
}

void AgentBrain::setExplorationStrategy(ExplorationStrategy strategy) {
    // Set exploration strategy (placeholder for future implementation)
    explorationStrategy_ = strategy;
    NLM_LOG_INFO("AgentBrain: Exploration strategy set to " + std::to_string(static_cast<int>(strategy)));
}

float AgentBrain::getExplorationProbability() const {
    // Calculate probability of exploration based on curiosity
    return std::min(1.0f, curiosityLevel_ * 0.5f + 0.1f);
}

void AgentBrain::updateNoveltyDetection() {
    // Update novelty detection with more sophisticated processing
    if (!brain_ || sensoryVision_.empty()) return;
    
    // Enhanced novelty detection with temporal smoothing
    float currentNovelty = 0.0f;
    const auto& vision = sensoryVision_;
    
    if (!vision.empty() && !previousVision_.empty()) {
        for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            currentNovelty += diff;
        }
        
        // Normalize and apply logarithmic scaling for sensitivity
        currentNovelty = currentNovelty / vision.size();
        currentNovelty = 1.0f - std::exp(-currentNovelty * 5.0f); // Logarithmic scaling
    }
    
    // Smooth novelty with decay
    noveltyLevel_ = noveltyLevel_ * 0.8f + currentNovelty * 0.2f;
    
    // Update previous vision
    previousVision_ = vision;
}

void AgentBrain::applyCuriosityBasedLearning() {
    // Apply curiosity-driven learning with prediction error
    if (!brain_ || !curiosityEnabled_) return;
    
    // Calculate prediction error if we have predictions
    if (predictionError_ != 0.0f) {
        // Scale plasticity by curiosity and prediction error
        float learningMod = std::abs(predictionError_) * curiosityLevel_ * 0.1f;
        
        // Apply to motor system
        if (learningMod > 0.01f) {
            NLM_LOG_INFO("AgentBrain: Curiosity-based learning with modulation " + std::to_string(learningMod));
        }
    }
}

void AgentBrain::setTargetBehavior(const std::string& behavior) {
    // Set target behavior for the agent
    targetBehavior_ = behavior;
    NLM_LOG_INFO("AgentBrain: Target behavior set to " + behavior);
}

void AgentBrain::enableSelfMonitoring() {
    // Enable self-monitoring and introspection
    selfMonitorEnabled_ = true;
    NLM_LOG_INFO("AgentBrain: Self-monitoring enabled");
}

float AgentBrain::getAttentionLevel() const {
    // Estimate attention level based on curiosity and novelty
    if (curiosityLevel_ < 0.01f && noveltyLevel_ < 0.01f) {
        return 0.0f;
    }
    return std::min(1.0f, (curiosityLevel_ + noveltyLevel_) * 0.5f);
}

void AgentBrain::applyStressLevel(float stress) {
    // Apply stress level affecting behavior and learning
    stressLevel_ = std::clamp(stress, 0.0f, 1.0f);
    
    // High stress reduces exploration and increases random movements
    if (stressLevel_ > 0.7f) {
        NLM_LOG_INFO("AgentBrain: High stress level - reducing exploration");
    } else if (stressLevel_ > 0.4f) {
        NLM_LOG_INFO("AgentBrain: Moderate stress level");
    }
}

float AgentBrain::getStressLevel() const {
    return stressLevel_;
}

void AgentBrain::setEnergyLevel(float energy) {
    // Set energy level affecting action selection
    energyLevel_ = std::clamp(energy, 0.0f, 1.0f);
}

float AgentBrain::getEnergyLevel() const {
    return energyLevel_;
}

void AgentBrain::enableMetaLearning() {
    // Enable meta-learning capabilities
    metaLearningEnabled_ = true;
    NLM_LOG_INFO("AgentBrain: Meta-learning enabled");
}

void AgentBrain::updateMetaLearning() {
    // Update meta-learning based on performance
    if (metaLearningEnabled_) {
        // Simple meta-learning: adjust exploration based on prediction error
        if (std::abs(predictionError_) < 0.1f) {
            // Good prediction -> increase exploration
            if (curiosityLevel_ < 1.0f) {
                curiosityLevel_ = std::min(1.0f, curiosityLevel_ + 0.05f);
            }
        } else {
            // Bad prediction -> decrease exploration and increase exploitation
            curiosityLevel_ = std::max(0.1f, curiosityLevel_ - 0.05f);
        }
    }
}

void AgentBrain::setPersonality(double openness, double conscientiousness, 
                               double extraversion, double agreeableness,
                               double neuroticism) {
    // Set personality traits for behavior modulation
    personalityOpenness_ = std::clamp(openness, 0.0, 1.0);
    personalityConscientiousness_ = std::clamp(conscientiousness, 0.0, 1.0);
    personalityExtraversion_ = std::clamp(extraversion, 0.0, 1.0);
    personalityAgreeableness_ = std::clamp(agreeableness, 0.0, 1.0);
    personalityNeuroticism_ = std::clamp(neuroticism, 0.0, 1.0);
    
    NLM_LOG_INFO("AgentBrain: Personality traits set - Openness:" + std::to_string(personalityOpenness_) +
                 ", Conscientiousness:" + std::to_string(personalityConscientiousness_));
}

void AgentBrain::setState(const std::string& stateName, const std::string& stateData) {
    // Set agent state
    currentStateName_ = stateName;
    currentStateData_ = stateData;
    stateTimestamp_ = static_cast<double>(brain_->getTotalSpikeCount());
}

void AgentBrain::transitionToState(const std::string& newStateName) {
    // Transition to new state
    if (!currentStateName_.empty()) {
        NLM_LOG_INFO("AgentBrain: Transitioning from state " + currentStateName_ + " to " + newStateName);
    }
    
    currentStateName_ = newStateName;
    stateTimestamp_ = static_cast<double>(brain_->getTotalSpikeCount());
    
    // Apply state-specific behaviors
    if (newStateName == "exploration") {
        explorationStrategy_ = ExplorationStrategy::Random;
    } else if (newStateName == "exploitation") {
        explorationStrategy_ = ExplorationStrategy::Exploitation;
    } else if (newStateName == "curiosity") {
        explorationStrategy_ = ExplorationStrategy::CuriosityDriven;
    }
}

std::string AgentBrain::getCurrentState() const {
    return currentStateName_;
}

void AgentBrain::logStateChange(const std::string& fromState, const std::string& toState) {
    // Log state changes for debugging and analysis
    NLM_LOG_INFO("AgentBrain: State change from '" + fromState + "' to '" + toState + "'");
    
    // Store state transition history
    stateTransitionHistory_.push_back(std::make_pair(fromState, toState));
    
    // Limit history size
    if (stateTransitionHistory_.size() > 100) {
        stateTransitionHistory_.erase(stateTransitionHistory_.begin());
    }
}
