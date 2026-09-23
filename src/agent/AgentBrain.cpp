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
    
    // Apply curiosity-based exploration with adaptive strategy
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        // Use enhanced curiosity selection based on action value and uncertainty
        decoded = selectWithCuriosityAdvanced(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate average activity in each motor group with adaptive weighting
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            // Use membrane potential deviation from rest as activity measure
            // Add some noise to prevent freezing on equal weights
            float potential = n->getState().membranePotential;
            float resting = n->getState().restingPotential;
            float activity = std::abs(potential - resting);
            // Add small noise for exploration
            activity += (brain_->getRandomGenerator()->uniformReal(-0.1f, 0.1f));
            sum += activity;
        }
        return sum / neurons.size();
    };
    
    // Get activity for each action group
    float forwardAct = calcActivity(motorForward_);
    float backwardAct = calcActivity(motorBackward_);
    float leftAct = calcActivity(motorTurnLeft_);
    float rightAct = calcActivity(motorTurnRight_);
    float interactAct = calcActivity(motorInteract_);
    float waitAct = calcActivity(motorWait_);
    
    // Use softmax-style weighting for more nuanced decision making
    // Higher activity = exponentially higher selection probability
    auto softmax = [](float x, float temp = 2.0f) -> float {
        return std::exp(x / temp);
    };
    
    float f = softmax(forwardAct);
    float b = softmax(backwardAct);
    float l = softmax(leftAct);
    float r = softmax(rightAct);
    float i = softmax(interactAct);
    float w = softmax(waitAct);
    
    float total = f + b + l + r + i + w;
    if (total <= 0.0f) return MotorCommand::Wait;
    
    // Normalize to get selection probabilities
    float forwardProb = f / total;
    float backwardProb = b / total;
    float leftProb = l / total;
    float rightProb = r / total;
    float interactProb = i / total;
    float waitProb = w / total;
    
    // Sample from distribution
    float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
    
    if (r < forwardProb) return MotorCommand::MoveForward;
    r -= forwardProb;
    if (r < backwardProb) return MotorCommand::MoveBackward;
    r -= backwardProb;
    if (r < leftProb) return MotorCommand::TurnLeft;
    r -= leftProb;
    if (r < rightProb) return MotorCommand::TurnRight;
    r -= rightProb;
    if (r < interactProb) return MotorCommand::Interact;
    return MotorCommand::Wait;
}

MotorCommand AgentBrain::selectWithCuriosityAdvanced(MotorCommand defaultCmd) {
    if (!brain_) return defaultCmd;
    
    // Multiple exploration strategies based on curiosity level
    float exploreChance = 0.0f;
    
    if (curiosityLevel_ > 0.8f) {
        // Very high curiosity: random exploration (up to 50%)
        exploreChance = 0.5f;
    } else if (curiosityLevel_ > 0.5f) {
        // High curiosity: balanced exploration (up to 30%)
        exploreChance = curiosityLevel_ * 0.3f;
    } else if (curiosityLevel_ > 0.3f) {
        // Moderate curiosity: selective exploration (up to 15%)
        exploreChance = 0.15f;
    }
    
    if (exploreChance > 0.0f) {
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Enhanced random exploration with context
            return selectRandomActionWithContext();
        }
    }
    
    // Also consider prediction error for exploration
    if (predictionError_ > 0.5f || predictionError_ < -0.5f) {
        // High prediction error suggests novel experiences needed
        float predictionErrorExploreChance = std::min(std::abs(predictionError_) * 0.2f, 0.3f);
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < predictionErrorExploreChance) {
            // Explore based on prediction error
            return selectActionByPredictionError();
        }
    }
    
    return defaultCmd;
}

MotorCommand AgentBrain::selectRandomActionWithContext() {
    if (!brain_) return MotorCommand::Wait;
    
    // Get current state context for better random choices
    bool isHungry = agent_.energy < agent_.maxEnergy * 0.3f;
    bool isInDanger = agent_.health < agent_.maxHealth * 0.3f;
    bool isBored = noveltyLevel_ > 0.7f;
    
    std::vector<MotorCommand> prioritizedActions;
    
    if (isHungry) prioritizedActions.push_back(MotorCommand::Interact);  // Look for food/resources
    if (isInDanger) prioritizedActions.push_back(MotorCommand::LookLeft);  // Check surroundings
    if (isInDanger) prioritizedActions.push_back(MotorCommand::LookRight);
    if (isBored) prioritizedActions.push_back(MotorCommand::TurnLeft);  // Exploration
    if (isBored) prioritizedActions.push_back(MotorCommand::TurnRight);
    
    // Add all actions
    if (prioritizedActions.size() < 6) {
        prioritizedActions.push_back(MotorCommand::MoveForward);
        prioritizedActions.push_back(MotorCommand::MoveBackward);
        prioritizedActions.push_back(MotorCommand::Wait);
    }
    
    if (prioritizedActions.empty()) return MotorCommand::Wait;
    
    // Random choice from prioritized actions
    size_t idx = brain_->getRandomGenerator()->uniformInt(0, prioritizedActions.size() - 1);
    return prioritizedActions[idx];
}

MotorCommand AgentBrain::selectActionByPredictionError() {
    if (!brain_) return MotorCommand::Wait;
    
    // Select action that might reduce prediction error
    // High positive error: action may have produced unexpected reward
    // High negative error: action may have produced unexpected punishment
    
    if (predictionError_ > 0.5f) {
        // Positive prediction error (better than expected): try similar actions
        return MotorCommand::MoveForward;  // Default forward movement
    } else if (predictionError_ < -0.5f) {
        // Negative prediction error (worse than expected): try different actions
        return MotorCommand::TurnLeft;  // Try turning to avoid danger
    }
    
    return MotorCommand::Wait;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Compute prediction error (TD error)
    predictionError_ = reward - predictedReward;
    
    // Update expected reward with exponential moving average
    // More weight on recent rewards for better adaptation
    expectedReward_ = 0.8f * expectedReward_ + 0.2f * reward;
    
    // Enhanced dopamine signal with learning rate modulation
    // Positive error increases dopamine (reward prediction), negative decreases
    float learningRate = 0.1f + 0.1f * std::min(std::abs(predictionError_), 1.0f);
    dopamineLevel_ = predictionError_ * learningRate;
    
    // Clamp to reasonable range for stability
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply reward-modulated weight changes with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Apply reward-modulated weight change with momentum
                float momentum = 0.95f;
                float delta = eligibility * dopamineLevel_ * plasticityModifier_ * momentum;
                
                // Add history-dependent component
                // Long-term rewards have more weight
                if (reward > 0.1f) {
                    delta *= 1.2f;
                }
                
                syn->addToWeight(delta);
                
                // Decay eligibility trace with time constant
                syn->decayEligibilityTrace(0.05f);
            }
        }
    }
    
    // Modulate plasticity based on dopamine with sigmoidal response
    // Creates non-linear relationship between error and plasticity
    float plasticityFactor = 1.0f / (1.0f + std::exp(-2.0f * dopamineLevel_));
    plasticityFactor = 0.1f + 0.9f * plasticityFactor;  // Scale to 0.1-1.0
    
    // Apply to STDP with more dynamic parameters
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        // Adaptive STDP parameters based on dopamine
        float ltpWeight = 0.01f * plasticityFactor;
        float ltdWeight = 0.012f * plasticityFactor;
        
        // Add exploration bonus based on prediction error magnitude
        if (std::abs(predictionError_) > 0.5f) {
            ltpWeight *= 1.5f;  // Enhance LTP for novel experiences
            ltdWeight *= 0.8f;  // Suppress LTD for novel experiences
        }
        
        stdp->setLTPWeight(ltpWeight);
        stdp->setLTDWeight(ltdWeight);
    }
    
    // Update developmental stage based on accumulated reward
    // Positive rewards advance development, negative delays it
    if (reward > 0.0f) {
        developmentalAge_ += 0.1f * reward;
    } else {
        developmentalAge_ -= 0.05f * std::abs(reward);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // More sophisticated developmental stages based on age and experience
    // This is a biologically inspired approximation with better dynamics
    
    if (developmentalAge_ < 30.0) {  // ~15 seconds
        plasticityModifier_ = 1.2f;  // Peak plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
        
        // Higher synaptogenesis in early development
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            float synRate = 0.0002f;  // Higher synaptogenesis rate
            float pruneRate = 0.000005f;
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
        
    } else if (developmentalAge_ < 150.0) {  // ~75 seconds
        plasticityModifier_ = 0.9f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
        
        // Moderate synaptic changes
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            float synRate = 0.0001f;
            float pruneRate = 0.00001f;
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
        
    } else if (developmentalAge_ < 450.0) {  // ~225 seconds
        plasticityModifier_ = 0.6f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
        
        // Lower synaptic changes
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            float synRate = 0.00005f;
            float pruneRate = 0.00002f;
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
        
    } else {
        plasticityModifier_ = 0.3f;  // Adult - more stable
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
        
        // Minimal structural changes in adulthood
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            float synRate = 0.00001f;
            float pruneRate = 0.00005f;  // Pruning still active
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
    }
    
    // Modulate neuromodulators based on development stage
    if (curiosityEnabled_) {
        // Curiosity decreases with development (focused behavior)
        curiosityLevel_ *= 0.98f;
        
        // But increases with negative prediction error
        if (predictionError_ < -0.1f) {
            curiosityLevel_ = std::min(1.0f, curiosityLevel_ + 0.05f * std::abs(predictionError_));
        }
    }
    
    if (sensoryNoveltyDecay_ < 0.98f) {
        // Novelty decay slows with development
        sensoryNoveltyDecay_ = 0.99f - (developmentalAge_ * 0.00001f);
        if (sensoryNoveltyDecay_ < 0.9f) sensoryNoveltyDecay_ = 0.9f;
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

} // namespace nlm
