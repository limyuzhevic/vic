#include "AgentBrain.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Constants.hpp"
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
    , plasticityModifier_(nlm::PLASTICITY_INITIAL)
    , rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , curiosityEnabled_(true)
    , sensoryNoveltyDecay_(nlm::NOVELTY_DECAY_RATE)
{
    // Validate input parameters
    if (!brain) {
        NLM_LOG_ERROR("AgentBrain constructor called with null brain pointer");
        return;
    }
    
    // Validate brain pointer after construction
    if (!brain_) {
        NLM_LOG_ERROR("Failed to initialize AgentBrain with provided brain pointer");
        return;
    }
    
    // Initialize motor and sensory neuron groups with enhanced bounds checking
    if (brain_) {
        const auto& regions = brain_->getRegions();
        for (const auto& region : regions) {
            if (!region) {
                NLM_LOG_WARNING("Null region pointer encountered during AgentBrain initialization");
                continue;
            }
            
            const auto& populations = region->getPopulations();
            for (const auto& pop : populations) {
                if (!pop) {
                    NLM_LOG_WARNING("Null population pointer encountered during AgentBrain initialization");
                    continue;
                }
                
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    const auto& neurons = pop->getNeurons();
                    for (Neuron* n : neurons) {
                        if (!n) {
                            NLM_LOG_WARNING("Null neuron pointer encountered during AgentBrain initialization");
                            continue;
                        }
                        
                        // Distribute motor neurons to different action groups
                        size_t idx = motorForward_.size() + motorBackward_.size() + 
                                    motorTurnLeft_.size() + motorTurnRight_.size() +
                                    motorInteract_.size() + motorWait_.size();
                        
                        switch (idx % nlm::MOTOR_COMMAND_COUNT) {
                            case 0: motorForward_.push_back(n); break;
                            case 1: motorBackward_.push_back(n); break;
                            case 2: motorTurnLeft_.push_back(n); break;
                            case 3: motorTurnRight_.push_back(n); break;
                            case 4: motorInteract_.push_back(n); break;
                            case 5: motorWait_.push_back(n); break;
                            case 6: break;
                            default: break;
                        }
                    }
                } else if (type == NeuronType::Sensory) {
                    const auto& neurons = pop->getNeurons();
                    for (Neuron* n : neurons) {
                        if (!n) {
                            NLM_LOG_WARNING("Null neuron pointer encountered during AgentBrain initialization");
                            continue;
                        }
                        
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
    // Validate input parameters
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain::initialize called with null brain pointer");
        return;
    }
    
    // Initialize neural state
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = nlm::PLASTICITY_INITIAL;
    
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
    return nlm::MOTOR_COMMAND_COUNT;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    // Validate input parameters
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain::processSensoryInput called with null brain pointer");
        return;
    }
    
    // Process vision input with enhanced bounds checking
    const auto& vision = percept.getVision();
    if (!vision.empty() && !sensoryVision_.empty()) {
        size_t minSize = std::min(sensoryVision_.size(), vision.size());
        for (size_t i = 0; i < minSize; ++i) {
            if (sensoryVision_[i]) {
                // Validate neuron pointer and scale factor
                float current = vision[i] * nlm::VISION_SCALE_FACTOR;
                sensoryVision_[i]->injectCurrent(current);
            }
        }
    }
    
    // Process touch input with enhanced bounds checking
    const auto& touch = percept.getTouch();
    if (!touch.empty() && !sensoryTouch_.empty()) {
        size_t minSize = std::min(sensoryTouch_.size(), touch.size());
        for (size_t i = 0; i < minSize; ++i) {
            if (sensoryTouch_[i]) {
                float current = touch[i] * nlm::TOUCH_SCALE_FACTOR;
                sensoryTouch_[i]->injectCurrent(current);
            }
        }
    }
    
    // Process internal signals with enhanced bounds checking
    const auto& intern = percept.getInternal();
    if (!intern.empty() && !sensoryInternal_.empty()) {
        size_t minSize = std::min(sensoryInternal_.size(), intern.size());
        for (size_t i = 0; i < minSize; ++i) {
            if (sensoryInternal_[i]) {
                float current = (intern[i] * nlm::INTERNAL_CENTER_FACTOR - nlm::INTERNAL_OFFSET) * nlm::INTERNAL_SCALE_FACTOR;
                sensoryInternal_[i]->injectCurrent(current);
            }
        }
    }
    
    // Process proprioception with enhanced bounds checking
    const auto& proprio = percept.getProprioception();
    if (!proprio.empty() && !sensoryProprioception_.empty()) {
        size_t minSize = std::min(sensoryProprioception_.size(), proprio.size());
        for (size_t i = 0; i < minSize; ++i) {
            if (sensoryProprioception_[i]) {
                float current = (proprio[i] * nlm::PROPRIOCEPTION_CENTER_FACTOR - nlm::PROPRIOCEPTION_OFFSET) * nlm::PROPRIOCEPTION_SCALE_FACTOR;
                sensoryProprioception_[i]->injectCurrent(current);
            }
        }
    }
    
    // Compute novelty (difference from previous vision)
    if (!vision.empty() && !previousVision_.empty()) {
        size_t minSize = std::min(vision.size(), previousVision_.size());
        float totalDiff = 0.0f;
        for (size_t i = 0; i < minSize; ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Normalize with safety check
        if (vision.size() > 0) {
            noveltyLevel_ = totalDiff / static_cast<float>(vision.size());
            
            // Decay and update
            noveltyLevel_ *= nlm::NOVELTY_DECAY_RATE;
            
            // Store for next time
            previousVision_ = vision;
        }
    }
    
    // Update curiosity based on novelty
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * nlm::NOVELTY_WEIGHT + std::abs(predictionError_) * nlm::PREDICTION_ERROR_WEIGHT;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, nlm::MAX_CURIOUSITY_LEVEL);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > nlm::HIGH_CURIOUSITY_THRESHOLD) {
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
    const float MIN_ACTIVITY_THRESHOLD = 0.5f;
    if (bestActivity < MIN_ACTIVITY_THRESHOLD) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > nlm::HIGH_CURIOUSITY_THRESHOLD) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * nlm::EXPLORATION_CHANCE_FACTOR;
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command
            int choice = brain_->getRandomGenerator()->uniformInt(nlm::RANDOM_INT_MIN, nlm::MAX_RANDOM_CHOICE);
            switch (choice) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::TurnLeft;
                case 3: return MotorCommand::TurnRight;
                case 4: return MotorCommand::LookLeft;
                case 5: return MotorCommand::LookRight;
                case 6: return MotorCommand::Interact;
                case 7: return MotorCommand::Wait;
                default: return MotorCommand::Wait;  // Safety fallback
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
    expectedReward_ = nlm::REWARD_EXP_AVG_WEIGHT * expectedReward_ + nlm::REWARD_EXP_UPDATE * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, nlm::DOPAMINE_CLAMP_MIN, nlm::DOPAMINE_CLAMP_MAX);
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > nlm::SYNAPTIC_ELIGIBILITY_THRESHOLD) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                
                // Clamp weight delta to prevent explosion
                delta = std::clamp(delta, -nlm::MAX_WEIGHT_DELTA, nlm::MAX_WEIGHT_DELTA);
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(nlm::SYNAPTIC_ELIGIBILITY_DECAY);
            }
        }
    }
    
    // Modulate plasticity based on dopamine
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = nlm::SYNAPTIC_PLASTICITY_MODIFIER_BASE + nlm::SYNAPTIC_PLASTICITY_MODIFIER_FACTOR * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, nlm::SYNAPTIC_PLASTICITY_MIN, nlm::SYNAPTIC_PLASTICITY_MAX);
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        // Validate plasticity factor before applying to STDP
        if (plasticityFactor > 0.0f) {
            stdp->setLTPWeight(nlm::STDP_LTP_DEFAULT_WEIGHT * plasticityFactor);
            stdp->setLTDWeight(nlm::STDP_LTD_DEFAULT_WEIGHT * plasticityFactor);
        } else {
            NLM_LOG_WARNING("Invalid plasticity factor: " + std::to_string(plasticityFactor));
        }
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    // Validate input parameters
    if (!brain_ || !developmentEnabled_) {
        NLM_LOG_ERROR("AgentBrain::updateDevelopment called with invalid parameters");
        return;
    }
    
    // Validate timestep
    if (timestep < 0.0) {
        NLM_LOG_WARNING("Negative timestep provided to updateDevelopment: " + std::to_string(timestep));
        timestep = 0.0;
    }
    
    developmentalAge_ += timestep;
    
    // Simple developmental stages based on age
    // This is a biologically inspired approximation
    if (developmentalAge_ < nlm::DEVELOPMENT_AGE_INITIAL) {
        plasticityModifier_ = nlm::PLASTICITY_INITIAL;  // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < nlm::DEVELOPMENT_AGE_CRITICAL) {
        plasticityModifier_ = nlm::PLASTICITY_CRITICAL;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < nlm::DEVELOPMENT_AGE_MATURATION) {
        plasticityModifier_ = nlm::PLASTICITY_MATURATION;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = nlm::PLASTICITY_ADULT;  // Adult - more stable
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Validate plasticity modifier range
    if (plasticityModifier_ < 0.0f || plasticityModifier_ > nlm::PLASTICITY_MAX) {
        NLM_LOG_WARNING("Plasticity modifier out of valid range: " + std::to_string(plasticityModifier_));
        plasticityModifier_ = nlm::PLASTICITY_INITIAL;
    }
    
    // Structural plasticity changes with development
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // Higher synaptogenesis in early development
            float synRate = nlm::SYNAPTOGENESIS_BASE_RATE * plasticityModifier_;
            float pruneRate = nlm::PRUNING_BASE_RATE * (nlm::SYNAPTOGENESIS_FACTOR - plasticityModifier_);
            
            // Validate synaptic plasticity rates
            if (synRate > nlm::SYNAPTOGENESIS_MAX_RATE) {
                NLM_LOG_WARNING("Synaptogenesis rate too high: " + std::to_string(synRate));
                synRate = nlm::SYNAPTOGENESIS_MAX_RATE;
            }
            
            if (pruneRate > nlm::PRUNING_MAX_RATE) {
                NLM_LOG_WARNING("Pruning rate too high: " + std::to_string(pruneRate));
                pruneRate = nlm::PRUNING_MAX_RATE;
            }
            
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
    // Validate brain pointer before reset
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain::reset called with null brain pointer");
        return;
    }
    
    // Reset neuromodulation state
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    
    // Reset developmental state
    developmentalAge_ = 0.0;
    plasticityModifier_ = nlm::PLASTICITY_INITIAL;
    
    // Clear previous vision with bounds checking
    if (!previousVision_.empty()) {
        std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    }
}

} // namespace nlm
