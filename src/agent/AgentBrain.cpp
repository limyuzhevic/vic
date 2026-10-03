#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

// All magic numbers are now defined in constants.hpp
#include "constants.hpp"

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
    , sensoryNoveltyDecay_(MAX_NOVELTY_FACTOR)
{
    NLM_LOG_INFO("AgentBrain: Initializing with brain pointer");
    
    // Initialize motor and sensory neuron groups
    if (brain_) {
        NLM_LOG_INFO("AgentBrain: Processing " + std::to_string(brain_->getRegions().size()) + " brain regions");
        
        for (const auto& region : brain_->getRegions()) {
            if (!region) {
                NLM_LOG_WARNING("AgentBrain: Encountered null region, skipping");
                continue;
            }
            
            const auto& populations = region->getPopulations();
            for (auto& pop : populations) {
                if (!pop) {
                    NLM_LOG_WARNING("AgentBrain: Encountered null population, skipping");
                    continue;
                }
                
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        if (!n) {
                            NLM_LOG_WARNING("AgentBrain: Encountered null motor neuron, skipping");
                            continue;
                        }
                        
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
                        if (!n) {
                            NLM_LOG_WARNING("AgentBrain: Encountered null sensory neuron, skipping");
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
    } else {
        NLM_LOG_WARNING("AgentBrain: Received null brain pointer");
    }
    
    NLM_LOG_INFO("AgentBrain: Initialization complete with " +
                 std::to_string(motorForward_.size()) + " forward, " +
                 std::to_string(motorBackward_.size()) + " backward, " +
                 std::to_string(motorTurnLeft_.size()) + " left, " +
                 std::to_string(motorTurnRight_.size()) + " right, " +
                 std::to_string(motorInteract_.size()) + " interact, " +
                 std::to_string(motorWait_.size()) + " wait motor neurons; " +
                 std::to_string(sensoryVision_.size()) + " vision, " +
                 std::to_string(sensoryTouch_.size()) + " touch, " +
                 std::to_string(sensoryInternal_.size()) + " internal, " +
                 std::to_string(sensoryProprioception_.size()) + " proprioception sensory neurons");
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = PLASTICITY_MODIFIER_INITIAL;
    
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
    if (!brain_) {
        NLM_LOG_WARNING("AgentBrain: Cannot process sensory input without brain");
        return;
    }
    
    // Validate percept
    if (percept.getVision().empty() && percept.getTouch().empty() && 
        percept.getInternal().empty() && percept.getProprioception().empty()) {
        NLM_LOG_DEBUG("AgentBrain: Empty sensory percept, skipping processing");
        return;
    }
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Inject current proportional to vision intensity
            float current = vision[i] * SCALING_FACTOR_VISION;  // Scale factor
            sensoryVision_[i]->injectCurrent(current);
        } else {
            NLM_LOG_WARNING("AgentBrain: Null neuron pointer in sensoryVision_ at index " + std::to_string(i));
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * SCALING_FACTOR_TOUCH;  // Collision signal
            sensoryTouch_[i]->injectCurrent(current);
        } else {
            NLM_LOG_WARNING("AgentBrain: Null neuron pointer in sensoryTouch_ at index " + std::to_string(i));
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * MAX_PLATIVITY_MODIFIER - 1.0f) * SCALING_FACTOR_INTERNAL;  // Center and scale
            sensoryInternal_[i]->injectCurrent(current);
        } else {
            NLM_LOG_WARNING("AgentBrain: Null neuron pointer in sensoryInternal_ at index " + std::to_string(i));
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * MAX_PLATIVITY_MODIFIER - 1.0f) * SCALING_FACTOR_PROPRIOCEPTION;  // Center and scale
            sensoryProprioception_[i]->injectCurrent(current);
        } else {
            NLM_LOG_WARNING("AgentBrain: Null neuron pointer in sensoryProprioception_ at index " + std::to_string(i));
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
        
        NLM_LOG_DEBUG("AgentBrain: Computed novelty level: " + std::to_string(noveltyLevel_));
    }
    
    // Update curiosity based on novelty
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * MAX_PLATIVITY_MODIFIER + std::abs(predictionError_) * MIN_ACTIVITY_THRESHOLD;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > DEFAULT_EXPLORATION_CHANCE) {
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
            if (!n) continue;  // Skip null neurons
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
    
    // Check for any valid motor neuron groups
    if (motorForward_.empty() && motorBackward_.empty() && motorTurnLeft_.empty() && 
        motorTurnRight_.empty() && motorInteract_.empty() && motorWait_.empty()) {
        NLM_LOG_WARNING("AgentBrain: No motor neurons available for decoding");
        return MotorCommand::Wait;
    }
    
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
    if (bestActivity < MIN_ACTIVITY_THRESHOLD) {
        return MotorCommand::Wait;
    }
    
    NLM_LOG_DEBUG("AgentBrain: Selected motor command: " + std::to_string(static_cast<int>(best)) + " with activity " + std::to_string(bestActivity));
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityEnabled_ && curiosityLevel_ > MAX_PLATIVITY_MODIFIER * 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * DEFAULT_EXPLORATION_CHANCE;  // Up to 30% random
        
        if (!brain_ || !brain_->getRandomGenerator()) {
            NLM_LOG_WARNING("AgentBrain: Cannot select random action - brain or random generator is null");
            return defaultCmd;
        }
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command
            int choice = brain_->getRandomGenerator()->uniformInt(0, 6);  // 0-6 inclusive for 7 options
            switch (choice) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::TurnLeft;
                case 3: return MotorCommand::TurnRight;
                case 4: return MotorCommand::LookLeft;
                case 5: return MotorCommand::LookRight;
                case 6: return MotorCommand::Interact;
                default: 
                    NLM_LOG_WARNING("AgentBrain: Invalid random choice in selectWithCuriosity: " + std::to_string(choice));
                    return MotorCommand::Wait;
            }
        }
    }
    
    return defaultCmd;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) {
        return;
    }
    
    // Validate inputs
    if (std::isnan(reward) || std::isnan(predictedReward)) {
        NLM_LOG_WARNING("AgentBrain: Invalid reward values - reward: " + std::to_string(reward) + ", predictedReward: " + std::to_string(predictedReward));
        return;
    }
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = NEUROMODULATION_RANGE * expectedReward_ + (1.0f - NEUROMODULATION_RANGE) * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        if (!region) continue;  // Skip null regions
        
        const auto& synapses = region->getSynapses();
        for (auto* syn : synapses) {
            if (!syn) {
                NLM_LOG_WARNING("AgentBrain: Encountered null synapse, skipping");
                continue;
            }
            
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > ELIGIBILITY_THRESHOLD) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(NEUROMODULATION_RANGE);
            }
        }
    }
    
    // Modulate plasticity based on dopamine
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = PLASTICITY_BASE_FACTOR + PLASTICITY_BASE_FACTOR * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, MIN_PLATIVITY_MODIFIER, MAX_PLATIVITY_MODIFIER);
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(STANP_LTP_FACTOR * plasticityFactor);
        stdp->setLTDWeight(STANP_LTD_FACTOR * plasticityFactor);
    }
    
    NLM_LOG_DEBUG("AgentBrain: Applied reward modulation - reward: " + std::to_string(reward) + ", error: " + std::to_string(predictionError_));
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) {
        NLM_LOG_DEBUG("AgentBrain: Cannot update development - brain disabled or null");
        return;
    }
    
    if (timestep < 0.0) {
        NLM_LOG_WARNING("AgentBrain: Invalid timestep in updateDevelopment: " + std::to_string(timestep));
        return;
    }
    
    developmentalAge_ += timestep;
    
    // Simple developmental stages based on age
    // This is a biologically inspired approximation
    if (developmentalAge_ < MAX_DEVELOPMENT_AGE_1) {  // ~1 minute
        plasticityModifier_ = PLASTICITY_MODIFIER_INITIAL;  // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < MAX_DEVELOPMENT_AGE_2) {  // ~5 minutes
        plasticityModifier_ = PLASTICITY_MODIFIER_CRITICAL;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < MAX_DEVELOPMENT_AGE_3) {  // ~15 minutes
        plasticityModifier_ = PLASTICITY_MODIFIER_MATURE;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = PLASTICITY_MODIFIER_ADULT;  // Adult - more stable
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    NLM_LOG_DEBUG("AgentBrain: Updated development - age: " + std::to_string(developmentalAge_) + ", plasticity: " + std::to_string(plasticityModifier_));
    
    // Structural plasticity changes with development
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // Higher synaptogenesis in early development
            float synRate = SYNAPTOGENESIS_RATE_BASE * plasticityModifier_;
            float pruneRate = PRUNING_RATE_BASE * (MAX_PLATIVITY_MODIFIER - plasticityModifier_);
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
            
            NLM_LOG_DEBUG("AgentBrain: Updated structural plasticity - synaptogenesis: " + std::to_string(synRate) + ", pruning: " + std::to_string(pruneRate));
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
    plasticityModifier_ = PLASTICITY_MODIFIER_INITIAL;
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

} // namespace nlm
