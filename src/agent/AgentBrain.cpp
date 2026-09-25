#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Constants.hpp"
#include "../core/Utils.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(brain)
    , dopamine_level_(0.0f)
    , novelty_level_(0.0f)
    , curiosity_level_(0.0f)
    , prediction_error_(0.0f)
    , expected_reward_(0.0f)
    , developmental_age_(0.0)
    , plasticity_modifier_(1.0f)
    , reward_modulation_enabled_(true)
    , structural_plasticity_enabled_(true)
    , development_enabled_(true)
    , curiosity_enabled_(true)
    , sensoryNoveltyDecay_(Constants::DEFAULT_NOVELTY_DECAY)
{
    // Initialize motor and sensory neuron groups
    if (brain_) {
        for (auto& region : brain_->getRegions()) {
            NeuronType neuron_type = region->getNeuronType();
            
            if (neuron_type == NeuronType::Motor) {
                for (const auto& shared_neuron : region->getNeurons()) {
                    Neuron* n = shared_neuron.get();
                    // Distribute motor neurons to different action groups
                    size_t idx = motorForward_.size() + motorBackward_.size() + 
                                motorTurnLeft_.size() + motorTurnRight_.size() +
                                motorInteract_.size() + motorWait_.size();
                    
                    switch (idx % Constants::NUM_MOTOR_NEURONS) {
                        case 0: motorForward_.push_back(n); break;
                        case 1: motorBackward_.push_back(n); break;
                        case 2: motorTurnLeft_.push_back(n); break;
                        case 3: motorTurnRight_.push_back(n); break;
                        case 4: motorInteract_.push_back(n); break;
                        case 5: motorWait_.push_back(n); break;
                    }
                }
            } else if (neuron_type == NeuronType::Sensory) {
                for (const auto& shared_neuron : region->getNeurons()) {
                    Neuron* n = shared_neuron.get();
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

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmental_age_ = 0.0;
    plasticity_modifier_ = 1.0f;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
}

size_t AgentBrain::getSensoryInputSize() const {
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
    return Constants::NUM_VISION_NEURONS + Constants::NUM_TOUCH_NEURONS + 
           Constants::NUM_INTERNAL_NEURONS + Constants::NUM_PROPRIOCEPTION_NEURONS;
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action
    return Constants::NUM_MOTOR_NEURONS;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Inject current proportional to vision intensity
            float current = vision[i] * Constants::EXCITATORY_INPUT_SCALE;  // Scale factor
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * Constants::TOUCH_INPUT_SCALE;  // Collision signal
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * Constants::INTERNAL_INPUT_SCALE - 1.0f) * Constants::INTERNAL_INPUT_SCALE;  // Center and scale
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * Constants::PROPRIOCEPTION_SCALE - 1.0f) * Constants::PROPRIOCEPTION_SCALE;  // Center and scale
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
    
    // Compute novelty (difference from previous vision)
    if (!vision.empty()) {
        float total_diff = 0.0f;
        for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            total_diff += diff;
        }
        
        // Normalize
        novelty_level_ = total_diff / std::max<size_t>(vision.size(), 1);
        
        // Decay and update
        novelty_level_ *= sensoryNoveltyDecay_;
        
        // Store for next time
        previousVision_ = vision;
    }
    
    // Update curiosity based on novelty
    if (curiosity_enabled_) {
        curiosity_level_ = novelty_level_ * Constants::ADAPTATION_EFFECT + std::abs(prediction_error_) * 0.5f;
        curiosity_level_ = std::clamp(curiosity_level_, 0.0f, 1.0f);
    }
}
}
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosity_enabled_ && curiosity_level_ > Constants::ACTIVITY_THRESHOLD) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate average activity in each motor group
    auto calc_activity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            // Use membrane potential deviation from rest as activity measure
            sum += std::abs(n->getState().membrane_potential - n->getState().resting_potential);
        }
        return sum / neurons.size();
    };
    
    float forward_act = calc_activity(motorForward_);
    float backward_act = calc_activity(motorBackward_);
    float left_act = calc_activity(motorTurnLeft_);
    float right_act = calc_activity(motorTurnRight_);
    float interact_act = calc_activity(motorInteract_);
    float wait_act = calc_activity(motorWait_);
    
    // Find maximum activity
    struct { MotorCommand cmd; float activity; } commands[] = {
        {MotorCommand::MoveForward, forward_act},
        {MotorCommand::MoveBackward, backward_act},
        {MotorCommand::TurnLeft, left_act},
        {MotorCommand::TurnRight, right_act},
        {MotorCommand::Interact, interact_act},
        {MotorCommand::Wait, wait_act}
    };
    
    MotorCommand best = MotorCommand::Wait;
    float best_activity = wait_act;  // Default to wait if nothing stronger
    
    for (const auto& c : commands) {
        if (c.activity > best_activity) {
            best_activity = c.activity;
            best = c.cmd;
        }
    }
    
    // Only act if there's meaningful activity
    if (best_activity < Constants::ACTIVITY_THRESHOLD) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand default_cmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosity_level_ > Constants::ACTIVITY_THRESHOLD) {
        // Higher curiosity = more exploration
        float explore_chance = curiosity_level_ * Constants::EXPLORATION_CHANCE_MAX;  // Up to 30% random
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < explore_chance) {
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
    
    return default_cmd;
}

void AgentBrain::applyRewardModulation(float reward, float predicted_reward) {
    if (!brain_ || !reward_modulation_enabled_) return;
    
    // Compute prediction error
    prediction_error_ = reward - predicted_reward;
    
    // Update expected reward (exponential moving average)
    expected_reward_ = Constants::REWARD_PREDICTION_DECAY * expected_reward_ + 
                      0.05f * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamine_level_ = prediction_error_;
    
    // Clamp to reasonable range
    dopamine_level_ = std::clamp(dopamine_level_, Constants::REWARD_CLAMP_MIN, Constants::REWARD_CLAMP_MAX);
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > Constants::EPSILON_FOR_NORMALS) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamine_level_ * plasticity_modifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(Constants::REWARD_CLAMP_MIN);
            }
        }
    }
    
    // Modulate plasticity based on dopamine
    // Positive dopamine increases plasticity, negative decreases
    float plasticity_factor = Constants::PLASTICITY_RANGE_MIN + Constants::PLASTICITY_RANGE_MAX * dopamine_level_;
    plasticity_factor = std::clamp(plasticity_factor, Constants::PLASTICITY_RANGE_MIN, Constants::PLASTICITY_RANGE_MAX);
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(Constants::STDP_LTP_WEIGHT * plasticity_factor);
        stdp->setLTDWeight(Constants::STDP_LTD_WEIGHT * plasticity_factor);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !development_enabled_) return;
    
    developmental_age_ += timestep;
    
    // Simple developmental stages based on age
    // This is a biologically inspired approximation
    if (developmental_age_ < Constants::INITIAL_STAGE_MS) {  // ~1 minute
        plasticity_modifier_ = 1.0f;  // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmental_age_ < Constants::CRITICAL_PERIOD_MS) {  // ~5 minutes
        plasticity_modifier_ = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmental_age_ < Constants::MATURATION_MS) {  // ~15 minutes
        plasticity_modifier_ = 0.5f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticity_modifier_ = 0.2f;  // Adult - more stable
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Structural plasticity changes with development
    if (structural_plasticity_enabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // Higher synaptogenesis in early development
            float syn_rate = Constants::SYNAPTOGENESIS_RATE_BASE * plasticity_modifier_;
            float prune_rate = Constants::PRUNING_RATE_BASE * (Constants::PLASTICITY_RANGE_MAX - plasticity_modifier_);
            sp->setSynaptogenesisRate(syn_rate);
            sp->setPruningRate(prune_rate);
        }
    }
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!brain_) return DevelopmentalStage::Initial;
    return brain_->getDevelopmentalStage();
}

float AgentBrain::getNeuromodulationLevel() const {
    return dopamine_level_;
}

float AgentBrain::getCuriosityLevel() const {
    return curiosity_level_;
}

float AgentBrain::getNoveltyLevel() const {
    return novelty_level_;
}

float AgentBrain::getPredictionError() const {
    return prediction_error_;
}

void AgentBrain::reset() {
    dopamine_level_ = 0.0f;
    novelty_level_ = 0.0f;
    curiosity_level_ = 0.0f;
// AgentBrain implementation continues with additional helper methods and documentation
// This section provides comprehensive algorithm documentation for AgentBrain operations
