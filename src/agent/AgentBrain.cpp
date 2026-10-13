#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <numeric>

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
    // Advanced exploration features
    , explorationMode_(ExplorationMode::EPSILON_GREEDY)
    , epsilon_(0.1f)
    , temperature_(1.0f)
    , explorationBonusWeight_(0.5f)
    , intrinsicMotivationEnabled_(true)
    , curiosityMode_(CuriosityMode::PURE_NOVELTY)
    , learningRate_(0.01f)
    , discountFactor_(0.95f)
    , eligibilityTraceDecay_(0.5f)
    , rewardSmoothing_(0.1f)
    , parallelProcessingEnabled_(false)
    , maxConcurrentExplorations_(4)
    , adaptiveExploration_(true)
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
                                    motorInteract_.size() + motorWait_.size() +
                                    motorLookLeft_.size() + motorLookRight_.size() +
                                    motorLookUp_.size() + motorLookDown_.size() +
                                    motorEat_.size() + motorDrink_.size() +
                                    motorRest_.size();
                        
                        switch (idx % 12) {
                            case 0: motorForward_.push_back(n); break;
                            case 1: motorBackward_.push_back(n); break;
                            case 2: motorTurnLeft_.push_back(n); break;
                            case 3: motorTurnRight_.push_back(n); break;
                            case 4: motorInteract_.push_back(n); break;
                            case 5: motorWait_.push_back(n); break;
                            case 6: motorLookLeft_.push_back(n); break;
                            case 7: motorLookRight_.push_back(n); break;
                            case 8: motorLookUp_.push_back(n); break;
                            case 9: motorLookDown_.push_back(n); break;
                            case 10: motorEat_.push_back(n); break;
                            case 11: motorDrink_.push_back(n); break;
                            case 12: motorRest_.push_back(n); break;
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
    
    // Initialize exploration parameters
    updateExplorationParameters();
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Initialize eligibility traces for each synapse
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (auto* syn : region->getSynapses()) {
                syn->initializeEligibilityTrace();
            }
        }
    }
    
    // Initialize exploration state
    explorationHistory_.clear();
    rewardHistory_.clear();
    curiosityHistory_.clear();
    
    NLM_LOG_INFO("AgentBrain initialized with advanced exploration systems");
    NLM_LOG_INFO("  Motor neurons: " + std::to_string(motorForward_.size() + motorBackward_.size() + 
                                                 motorTurnLeft_.size() + motorTurnRight_.size() +
                                                 motorInteract_.size() + motorWait_.size() +
                                                 motorLookLeft_.size() + motorLookRight_.size() +
                                                 motorLookUp_.size() + motorLookDown_.size() +
                                                 motorEat_.size() + motorDrink_.size() +
                                                 motorRest_.size()));
    NLM_LOG_INFO("  Sensory neurons: " + std::to_string(sensoryVision_.size() + sensoryTouch_.size() +
                                                  sensoryInternal_.size() + sensoryProprioception_.size()));
    NLM_LOG_INFO("  Exploration mode: " + std::to_string(static_cast<int>(explorationMode_)));
}

void AgentBrain::updateExplorationParameters() {
    // Adapt exploration parameters based on developmental stage and experience
    if (adaptiveExploration_) {
        // Adjust epsilon based on performance
        if (developmentalAge_ > 300.0) {  // Adult phase
            epsilon_ = 0.05f;  // Less exploration, more exploitation
        } else if (developmentalAge_ > 60.0) {  // Critical period
            epsilon_ = 0.15f;  // Moderate exploration
        } else {  // Initial phase
            epsilon_ = 0.25f;  // High exploration
        }
        
        // Adjust temperature based on curiosity
        temperature_ = 1.0f + curiosityLevel_ * 2.0f;
        
        // Adjust exploration bonus weight based on novelty
        explorationBonusWeight_ = 0.3f + noveltyLevel_ * 0.4f;
    }
}

size_t AgentBrain::getSensoryInputSize() const {
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6) + audio (4) + novelty
    return 256 + 8 + 4 + 6 + 4 + 1;
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action
    return 12;  // All new motor groups
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
    
    // Audio input (4 values)
    const auto& audio = percept.getAudio();
    for (size_t i = 0; i < audio.size() && i < 4; ++i) {
        // Add audio input to vision neurons for integrated processing
        if (i < sensoryVision_.size() && sensoryVision_[i]) {
            float current = audio[i] * 2.0f;
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Compute novelty and curiosity with advanced features
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
    
    // Update curiosity based on multiple factors
    if (curiosityEnabled_) {
        float noveltyCuriosity = noveltyLevel_ * explorationBonusWeight_;
        float predictionCuriosity = std::abs(predictionError_) * 0.5f;
        float intrinsicCuriosity = (noveltyCuriosity + predictionCuriosity) / 2.0f;
        
        // Apply curiosity mode
        switch (curiosityMode_) {
            case CuriosityMode::PURE_NOVELTY:
                curiosityLevel_ = noveltyCuriosity;
                break;
            case CuriosityMode::PREDICTION_ERROR:
                curiosityLevel_ = predictionCuriosity;
                break;
            case CuriosityMode::COMBINED:
                curiosityLevel_ = (noveltyCuriosity + predictionCuriosity) / 2.0f;
                break;
            case CuriosityMode::ADAPTIVE:
                curiosityLevel_ = intrinsicCuriosity * (1.0f + developmentalAge_ / 1000.0f);
                break;
        }
        
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
    
    // Update intrinsic motivation
    if (intrinsicMotivationEnabled_) {
        float intrinsicMotiv = curiosityLevel_ * learningRate_;
        
        // Apply to neuromodulation
        dopamineLevel_ += intrinsicMotiv * 0.1f;
        dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    }
    
    // Update exploration state
    ExplorationState state;
    state.novelty = noveltyLevel_;
    state.curiosity = curiosityLevel_;
    state.dopamine = dopamineLevel_;
    state.prediction_error = predictionError_;
    state.developmental_stage = getDevelopmentalStage();
    
    explorationHistory_.push_back(state);
    if (explorationHistory_.size() > 100) {
        explorationHistory_.erase(explorationHistory_.begin());
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply exploration based on current mode
    decoded = applyExploration(decoded);
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        decoded = selectWithCuriosity(decoded);
    }
    
    // Apply parallel processing if enabled
    if (parallelProcessingEnabled_) {
        decoded = applyParallelProcessing(decoded);
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
    float lookLeftAct = calcActivity(motorLookLeft_);
    float lookRightAct = calcActivity(motorLookRight_);
    float lookUpAct = calcActivity(motorLookUp_);
    float lookDownAct = calcActivity(motorLookDown_);
    float eatAct = calcActivity(motorEat_);
    float drinkAct = calcActivity(motorDrink_);
    float restAct = calcActivity(motorRest_);
    
    // Find maximum activity
    struct { MotorCommand cmd; float activity; } commands[] = {
        {MotorCommand::MoveForward, forwardAct},
        {MotorCommand::MoveBackward, backwardAct},
        {MotorCommand::TurnLeft, leftAct},
        {MotorCommand::TurnRight, rightAct},
        {MotorCommand::Interact, interactAct},
        {MotorCommand::Wait, waitAct},
        {MotorCommand::LookLeft, lookLeftAct},
        {MotorCommand::LookRight, lookRightAct},
        {MotorCommand::Look, lookUpAct},
        {MotorCommand::LookUp, lookUpAct},
        {MotorCommand::LookDown, lookDownAct},
        {MotorCommand::Eat, eatAct},
        {MotorCommand::Drink, drinkAct},
        {MotorCommand::Rest, restAct}
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

MotorCommand AgentBrain::applyExploration(MotorCommand defaultCmd) {
    switch (explorationMode_) {
        case ExplorationMode::EPSILON_GREEDY:
            return applyEpsilonGreedy(defaultCmd);
        case ExplorationMode::BOLTZMANN:
            return applyBoltzmannExploration(defaultCmd);
        case ExplorationMode::THOMPSON_SAMPLING:
            return applyThompsonSampling(defaultCmd);
        case ExplorationMode::UPPER_CONFIDENCE_BOUND:
            return applyUCBExploration(defaultCmd);
        case ExplorationMode::NOISE_INJECTION:
            return applyNoiseInjection(defaultCmd);
        case ExplorationMode::MULTI_ARM:
            return applyMultiArmedBandit(defaultCmd);
        default:
            return defaultCmd;
    }
}

MotorCommand AgentBrain::applyEpsilonGreedy(MotorCommand defaultCmd) {
    // Simple epsilon-greedy: with probability epsilon, explore randomly
    float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
    if (r < epsilon_) {
        // Random exploration across all actions
        int choice = brain_->getRandomGenerator()->uniformInt(0, 13);
        switch (choice) {
            case 0: return MotorCommand::MoveForward;
            case 1: return MotorCommand::MoveBackward;
            case 2: return MotorCommand::TurnLeft;
            case 3: return MotorCommand::TurnRight;
            case 4: return MotorCommand::LookLeft;
            case 5: return MotorCommand::LookRight;
            case 6: return MotorCommand::LookUp;
            case 7: return MotorCommand::LookDown;
            case 8: return MotorCommand::Interact;
            case 9: return MotorCommand::Eat;
            case 10: return MotorCommand::Drink;
            case 11: return MotorCommand::Rest;
            default: return MotorCommand::Wait;
        }
    }
    return defaultCmd;
}

MotorCommand AgentBrain::applyBoltzmannExploration(MotorCommand defaultCmd) {
    // Boltzmann (softmax) exploration: select actions with probability proportional to exp(Q/temperature)
    // For now, use curiosity as Q-value and temperature as exploration parameter
    std::vector<std::pair<MotorCommand, float>> actionValues;
    
    // Assign values based on curiosity and default command preference
    if (defaultCmd != MotorCommand::Wait) {
        actionValues.push_back({defaultCmd, curiosityLevel_ * temperature_});
    }
    
    // Add exploration noise for other actions
    float explorationNoise = temperature_ * 0.5f;
    for (int i = 0; i < 5; ++i) {  // Sample some exploration actions
        MotorCommand action = static_cast<MotorCommand>(i % 5 + 1);  // Avoid Wait
        actionValues.push_back({action, explorationNoise});
    }
    
    if (actionValues.empty()) return defaultCmd;
    
    // Calculate probabilities
    float totalValue = 0.0f;
    for (const auto& pair : actionValues) {
        totalValue += std::exp(pair.second / temperature_);
    }
    
    // Select action based on probabilities
    float r = brain_->getRandomGenerator()->uniformReal(0.0f, totalValue);
    float cumulative = 0.0f;
    for (const auto& pair : actionValues) {
        cumulative += std::exp(pair.second / temperature_);
        if (r <= cumulative) {
            return pair.first;
        }
    }
    
    return defaultCmd;
}

MotorCommand AgentBrain::applyThompsonSampling(MotorCommand defaultCmd) {
    // Thompson sampling: maintain distribution over action values
    // For simplicity, use curiosity as probability of selecting default action
    float curiosityProbability = curiosityLevel_;
    
    float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
    if (r < curiosityProbability) {
        return defaultCmd;
    }
    
    // Otherwise, explore based on novelty
    return applyNoiseInjection(defaultCmd);
}

MotorCommand AgentBrain::applyUCBExploration(MotorCommand defaultCmd) {
    // Upper Confidence Bound: balance exploration and exploitation
    float explorationBonus = std::sqrt(std::log(explorationHistory_.size() + 1.0f) / 
                                      (explorationHistory_.size() + 1.0f));
    
    float ucbScore = curiosityLevel_ + explorationBonus * temperature_;
    
    float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
    if (r < ucbScore * 0.1f) {  // Scale for reasonable exploration
        return applyNoiseInjection(defaultCmd);
    }
    
    return defaultCmd;
}

MotorCommand AgentBrain::applyNoiseInjection(MotorCommand defaultCmd) {
    // Add Gaussian noise to action selection
    float noise = brain_->getRandomGenerator()->gaussianReal(0.0f, 1.0f);
    
    // Sometimes the noise leads to different action
    if (std::abs(noise) > 1.5f) {
        // Large noise - explore
        int choice = brain_->getRandomGenerator()->uniformInt(0, 8);
        switch (choice) {
            case 0: return MotorCommand::MoveForward;
            case 1: return MotorCommand::MoveBackward;
            case 2: return MotorCommand::TurnLeft;
            case 3: return MotorCommand::TurnRight;
            case 4: return MotorCommand::LookLeft;
            case 5: return MotorCommand::LookRight;
            case 6: return MotorCommand::LookUp;
            case 7: return MotorCommand::LookDown;
            default: return MotorCommand::Wait;
        }
    }
    
    return defaultCmd;
}

MotorCommand AgentBrain::applyMultiArmedBandit(MotorCommand defaultCmd) {
    // Multi-armed bandit approach: maintain separate statistics for each action
    // Simplified: use exploration bonus based on recent novelty
    float explorationBonus = noveltyLevel_ * (1.0f - developmentalAge_ / 1000.0f);
    
    float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
    if (r < explorationBonus * 0.2f) {
        // Explore based on multi-armed bandit heuristic
        return applyNoiseInjection(defaultCmd);
    }
    
    return defaultCmd;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Enhanced exploration when curiosity is high
    if (curiosityLevel_ > 0.7f) {
        // High curiosity = high exploration
        float exploreChance = curiosityLevel_ * 0.4f;  // Up to 40% random
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command
            int choice = brain_->getRandomGenerator()->uniformInt(0, 13);
            switch (choice) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::TurnLeft;
                case 3: return MotorCommand::TurnRight;
                case 4: return MotorCommand::LookLeft;
                case 5: return MotorCommand::LookRight;
                case 6: return MotorCommand::LookUp;
                case 7: return MotorCommand::LookDown;
                case 8: return MotorCommand::Interact;
                case 9: return MotorCommand::Eat;
                case 10: return MotorCommand::Drink;
                case 11: return MotorCommand::Rest;
                default: return MotorCommand::Wait;
            }
        }
    }
    
    return defaultCmd;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Apply reward smoothing
    if (rewardSmoothing_ > 0.0f) {
        reward = rewardSmoothing_ * reward + (1.0f - rewardSmoothing_) * expectedReward_;
    }
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = discountFactor_ * expectedReward_ + (1.0f - discountFactor_) * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply reward modulation to eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel_ * plasticityModifier_ * learningRate_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(eligibilityTraceDecay_);
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
    
    // Update reward history
    rewardHistory_.push_back(reward);
    if (rewardHistory_.size() > 100) {
        rewardHistory_.erase(rewardHistory_.begin());
    }
}

MotorCommand AgentBrain::applyParallelProcessing(MotorCommand defaultCmd) {
    // Simplified parallel processing: sometimes override default action
    if (maxConcurrentExplorations_ > 1) {
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < 0.1f) {  // 10% chance of parallel exploration
            // Sometimes choose a different action for exploration
            int choice = brain_->getRandomGenerator()->uniformInt(0, 6);
            switch (choice) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::TurnLeft;
                case 2: return MotorCommand::TurnRight;
                case 3: return MotorCommand::LookLeft;
                case 4: return MotorCommand::LookRight;
                case 5: return MotorCommand::Wait;
                default: return defaultCmd;
            }
        }
    }
    
    return defaultCmd;
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // Update exploration parameters based on development
    updateExplorationParameters();
    
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
    
    // Clear exploration history
    explorationHistory_.clear();
    rewardHistory_.clear();
    curiosityHistory_.clear();
    
    // Reset eligibility traces
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (auto* syn : region->getSynapses()) {
                syn->resetEligibilityTrace();
            }
        }
    }
}

// Setters for exploration configuration
void AgentBrain::setExplorationMode(ExplorationMode mode) {
    explorationMode_ = mode;
    updateExplorationParameters();
}

void AgentBrain::setExplorationRate(float epsilon) {
    epsilon_ = std::clamp(epsilon, 0.0f, 1.0f);
}

void AgentBrain::setCuriosityMode(CuriosityMode mode) {
    curiosityMode_ = mode;
}

void AgentBrain::setLearningRate(float rate) {
    learningRate_ = std::clamp(rate, 0.0f, 1.0f);
}

void AgentBrain::setParallelProcessing(bool enabled, int maxConcurrent) {
    parallelProcessingEnabled_ = enabled;
    maxConcurrentExplorations_ = std::max(1, maxConcurrent);
}

// Getters for exploration configuration
ExplorationMode AgentBrain::getExplorationMode() const {
    return explorationMode_;
}

float AgentBrain::getExplorationRate() const {
    return epsilon_;
}

CuriosityMode AgentBrain::getCuriosityMode() const {
    return curiosityMode_;
}

bool AgentBrain::isParallelProcessingEnabled() const {
    return parallelProcessingEnabled_;
}

int AgentBrain::getMaxConcurrentExplorations() const {
    return maxConcurrentExplorations_;
}

// Get exploration statistics
float AgentBrain::getAverageCuriosity() const {
    if (explorationHistory_.empty()) return 0.0f;
    float sum = 0.0f;
    for (const auto& state : explorationHistory_) {
        sum += state.curiosity;
    }
    return sum / explorationHistory_.size();
}

float AgentBrain::getAverageNovelty() const {
    if (explorationHistory_.empty()) return 0.0f;
    float sum = 0.0f;
    for (const auto& state : explorationHistory_) {
        sum += state.novelty;
    }
    return sum / explorationHistory_.size();
}

float AgentBrain::getAverageReward() const {
    if (rewardHistory_.empty()) return 0.0f;
    float sum = 0.0f;
    for (float reward : rewardHistory_) {
        sum += reward;
    }
    return sum / rewardHistory_.size();
}

// Get development statistics
double AgentBrain::getDevelopmentalAge() const {
    return developmentalAge_;
}

float AgentBrain::getPlasticityModifier() const {
    return plasticityModifier_;
}

// Get exploration history (for analysis)
const std::vector<ExplorationState>& AgentBrain::getExplorationHistory() const {
    return explorationHistory_;
}

const std::vector<float>& AgentBrain::getRewardHistory() const {
    return rewardHistory_;
}

const std::vector<float>& AgentBrain::getCuriosityHistory() const {
    return curiosityHistory_;
}

} // namespace nlm
