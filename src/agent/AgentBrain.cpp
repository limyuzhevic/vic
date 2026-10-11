#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
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
    , explorationEnabled_(true)
    , sensoryNoveltyDecay_(Defaults::NOVELTY_DECAY_RATE)
{
    // Initialize motor and sensory neuron groups
    if (validateBrainState()) {
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Distribute motor neurons to different action groups
                        size_t idx = motorForward_.size() + motorBackward_.size() + 
                                    motorTurnLeft_.size() + motorTurnRight_.size() +
                                    motorInteract_.size() + motorWait_.size();
                        
                        switch (idx % MOTOR_GROUP_COUNT) {
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
                        
                        switch (idx % SENSORY_GROUP_COUNT) {
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
    developmentalAge_ = Defaults::DEVELOPMENT_INITIAL_AGE;
    plasticityModifier_ = PLASTICITY_MODIFIER_INITIAL;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
}

size_t AgentBrain::getSensoryInputSize() const {
    // Use constants for sensory input sizes
    return Behavioral::VISION_INPUT_WIDTH * Behavioral::VISION_INPUT_HEIGHT +
           Behavioral::TOUCH_INPUT_COUNT +
           Behavioral::INTERNAL_INPUT_COUNT +
           Behavioral::PROPRIOCEPTION_INPUT_COUNT;
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action
    return MOTOR_GROUP_COUNT;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!validateBrainState()) {
        NLM_LOG_WARNING("Cannot process sensory input: invalid brain state");
        return;
    }
    
    validateSensoryConsistency(percept);
    
    // Vision input
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Use constant for scaling factor
            float current = vision[i] * NEURAL_INJECTION_SCALE;
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * COLLISION_SCALE;
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * INTERNAL_SCALE;
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * PROPRIOCEPTION_SCALE;
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
        
        // Clamp to valid range
        noveltyLevel_ = std::clamp(noveltyLevel_, Behavioral::NOVELTY_CLAMP_MIN, Behavioral::NOVELTY_CLAMP_MAX);
        
        // Store for next time
        previousVision_ = vision;
    }
    
    // Update curiosity based on novelty
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * NOVELTY_WEIGHT + std::abs(predictionError_) * PREDICTION_ERROR_WEIGHT;
        curiosityLevel_ = std::clamp(curiosityLevel_, Behavioral::CURIOSITY_CLAMP_MIN, Behavioral::CURIOSITY_CLAMP_MAX);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!validateBrainState()) {
        NLM_LOG_WARNING("Cannot decode motor command: invalid brain state");
        return MotorCommand::Wait;
    }
    
    validateMotorCommandSelection();
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > CURIOUSITY_THRESHOLD) {
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
            if (n) {
                sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
            }
        }
        return neurons.empty() ? 0.0f : sum / neurons.size();
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
    if (bestActivity < MIN_ACTIVITY_FOR_ACTION) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > EXPLORATION_THRESHOLD) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * EXPLORATION_CHANCE;  // Up to 30% random
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command
            int choice = brain_->getRandomGenerator()->uniformInt(0, MAX_EXPLORATION_ACTIONS - 1);
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
    if (!validateBrainState() || !rewardModulationEnabled_) {
        return;
    }
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = expectedReward_ * REWARD_PREDICTION_DECAY + reward * REWARD_LEARNING_RATE;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, DOPAMINE_CLAMP_MIN, DOPAMINE_CLAMP_MAX);
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            if (!syn) continue;
            
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > ELIGIBILITY_TRACE_THRESHOLD) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(ELIGIBILITY_TRACE_DECAY);
            }
        }
    }
    
    // Modulate plasticity based on dopamine
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, PLASTICITY_FACTOR_MIN, PLASTICITY_FACTOR_MAX);
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(STDP_LTP_BASE_WEIGHT * plasticityFactor);
        stdp->setLTDWeight(STDP_LTD_BASE_WEIGHT * plasticityFactor);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!validateBrainState() || !developmentEnabled_) {
        return;
    }
    
    developmentalAge_ += timestep;
    
    // Simple developmental stages based on age
    if (developmentalAge_ < DEVELOPMMENT_STAGE_INITIAL) {  // ~1 minute
        plasticityModifier_ = PLASTICITY_MODIFIER_INITIAL;  // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < DEVELOPMMENT_STAGE_CRITICAL) {  // ~5 minutes
        plasticityModifier_ = PLASTICITY_MODIFIER_CRITICAL;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < DEVELOPMMENT_STAGE_MATURATION) {  // ~15 minutes
        plasticityModifier_ = PLASTICITY_MODIFIER_MATURATION;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = PLASTICITY_MODIFIER_ADULT;  // Adult - more stable
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Structural plasticity changes with development
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // Higher synaptogenesis in early development
            float synRate = SYNAPTOGENESIS_BASE_RATE * plasticityModifier_;
            float pruneRate = PRUNING_BASE_RATE * (2.0f - plasticityModifier_);
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
    }
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!validateBrainState()) return DevelopmentalStage::Initial;
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
    developmentalAge_ = Defaults::DEVELOPMENT_INITIAL_AGE;
    plasticityModifier_ = PLASTICITY_MODIFIER_INITIAL;
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

// === VALIDATION METHODS ===
void AgentBrain::validateSensoryConsistency(const SensoryPercept& percept) const {
    // Validate sensory data consistency
    const auto& vision = percept.getVision();
    const auto& touch = percept.getTouch();
    const auto& internal = percept.getInternal();
    const auto& proprio = percept.getProprioception();
    
    if (vision.size() > 0 && sensoryVision_.size() == 0) {
        NLM_LOG_WARNING("Vision percept has data but no vision sensory neurons available");
    }
}

void AgentBrain::validateMotorCommandSelection() const {
    // Validate motor command selection
    bool hasActiveMotorNeurons = !motorForward_.empty() || !motorBackward_.empty() || 
                                 !motorTurnLeft_.empty() || !motorTurnRight_.empty() ||
                                 !motorInteract_.empty() || !motorWait_.empty();
    
    if (!hasActiveMotorNeurons) {
        NLM_LOG_DEBUG("No active motor neurons available for command selection");
    }
}

} // namespace nlm