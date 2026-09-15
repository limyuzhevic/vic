#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain(brain)
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
    , sensory_novelty_decay_(0.99f)
{
    // Initialize motor and sensory neuron groups
    if (brain) {
        for (const auto& region : brain->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Distribute motor neurons to different action groups
                        size_t idx = motor_forward_.size() + motor_backward_.size() + 
                                    motor_turn_left_.size() + motor_turn_right_.size() +
                                    motor_interact_.size() + motor_wait_.size();
                        
                        switch (idx % 6) {
                            case 0: motor_forward_.push_back(n); break;
                            case 1: motor_backward_.push_back(n); break;
                            case 2: motor_turn_left_.push_back(n); break;
                            case 3: motor_turn_right_.push_back(n); break;
                            case 4: motor_interact_.push_back(n); break;
                            case 5: motor_wait_.push_back(n); break;
                        }
                    }
                } else if (type == NeuronType::Sensory) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Distribute sensory neurons
                        size_t idx = sensory_vision_.size() + sensory_touch_.size() +
                                    sensory_internal_.size() + sensory_proprioception_.size();
                        
                        switch (idx % 4) {
                            case 0: sensory_vision_.push_back(n); break;
                            case 1: sensory_touch_.push_back(n); break;
                            case 2: sensory_internal_.push_back(n); break;
                            case 3: sensory_proprioception_.push_back(n); break;
                        }
                    }
                }
            }
        }
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    previous_vision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmental_age_ = 0.0;
    plasticity_modifier_ = 1.0f;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensory_vision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensory_touch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensory_internal_.size()) + " internal sensory neurons");
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
    if (!brain) return;
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensory_vision_.size() && i < vision.size(); ++i) {
        if (sensory_vision_[i]) {
            // Inject current proportional to vision intensity
            float current = vision[i] * 5.0f;  // Scale factor
            sensory_vision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensory_touch_.size() && i < touch.size(); ++i) {
        if (sensory_touch_[i]) {
            float current = touch[i] * 8.0f;  // Collision signal
            sensory_touch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensory_internal_.size() && i < intern.size(); ++i) {
        if (sensory_internal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            sensory_internal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensory_proprioception_.size() && i < proprio.size(); ++i) {
        if (sensory_proprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            sensory_proprioception_[i]->injectCurrent(current);
        }
    }
    
    // Compute novelty (difference from previous vision)
    if (!vision.empty()) {
        float total_diff = 0.0f;
        for (size_t i = 0; i < vision.size() && i < previous_vision_.size(); ++i) {
            float diff = std::abs(vision[i] - previous_vision_[i]);
            total_diff += diff;
        }
        
        // Normalize
        novelty_level_ = total_diff / std::max<size_t>(vision.size(), 1);
        
        // Decay and update
        novelty_level_ *= sensory_novelty_decay_;
        
        // Store for next time
        previous_vision_ = vision;
    }
    
    // Update curiosity based on novelty
    if (curiosity_enabled_) {
        curiosity_level_ = novelty_level_ * 2.0f + std::abs(prediction_error_) * 0.5f;
        curiosity_level_ = std::clamp(curiosity_level_, 0.0f, 1.0f);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain) return MotorCommand::Wait;
    
    MotorCommand decoded = decode_from_motor_neurons();
    
    // Apply curiosity-based exploration
    if (curiosity_enabled_ && curiosity_level_ > 0.3f) {
        decoded = select_with_curiosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decode_from_motor_neurons() {
    // Calculate average activity in each motor group
    auto calc_activity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            // Use membrane potential deviation from rest as activity measure
            sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
        return sum / neurons.size();
    };
    
    float forward_act = calc_activity(motor_forward_);
    float backward_act = calc_activity(motor_backward_);
    float left_act = calc_activity(motor_turn_left_);
    float right_act = calc_activity(motor_turn_right_);
    float interact_act = calc_activity(motor_interact_);
    float wait_act = calc_activity(motor_wait_);
    
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
    if (best_activity < 0.5f) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::select_with_curiosity(MotorCommand default_cmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosity_level_ > 0.5f) {
        // Higher curiosity = more exploration
        float explore_chance = curiosity_level_ * 0.3f;  // Up to 30% random
        
        float r = brain->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < explore_chance) {
            // Random motor command
            int choice = brain->getRandomGenerator()->uniformInt(0, 7);
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

void AgentBrain::apply_reward_modulation(float reward, float predicted_reward) {
    if (!brain || !reward_modulation_enabled_) return;
    
    // Compute prediction error
    prediction_error_ = reward - predicted_reward;
    
    // Update expected reward (exponential moving average)
    expected_reward_ = 0.95f * expected_reward_ + 0.05f * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamine_level_ = prediction_error_;
    
    // Clamp to reasonable range
    dopamine_level_ = std::clamp(dopamine_level_, -1.0f, 1.0f);
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : brain->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamine_level_ * plasticity_modifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
    
    // Modulate plasticity based on dopamine
    // Positive dopamine increases plasticity, negative decreases
    float plasticity_factor = 0.5f + 0.5f * dopamine_level_;
    plasticity_factor = std::clamp(plasticity_factor, 0.1f, 2.0f);
    
    // Apply to STDP
    auto* stdp = brain->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticity_factor);
        stdp->setLTDWeight(0.012f * plasticity_factor);
    }
}

void AgentBrain::update_development(double timestep) {
    if (!brain || !development_enabled_) return;
    
    developmental_age_ += timestep;
    
    // Simple developmental stages based on age
    // This is a biologically inspired approximation
    if (developmental_age_ < 60.0) {  // ~1 minute
        plasticity_modifier_ = 1.0f;  // High plasticity
        brain->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmental_age_ < 300.0) {  // ~5 minutes
        plasticity_modifier_ = 0.8f;
        brain->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmental_age_ < 900.0) {  // ~15 minutes
        plasticity_modifier_ = 0.5f;
        brain->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticity_modifier_ = 0.2f;  // Adult - more stable
        brain->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Structural plasticity changes with development
    if (structural_plasticity_enabled_) {
        auto* sp = brain->getStructuralPlasticity();
        if (sp) {
            // Higher synaptogenesis in early development
            float syn_rate = 0.0001f * plasticity_modifier_;
            float prune_rate = 0.00001f * (2.0f - plasticity_modifier_);
            sp->setSynaptogenesisRate(syn_rate);
            sp->setPruningRate(prune_rate);
        }
    }
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!brain) return DevelopmentalStage::Initial;
    return brain->getDevelopmentalStage();
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
    prediction_error_ = 0.0f;
    expected_reward_ = 0.0f;
    developmental_age_ = 0.0;
    plasticity_modifier_ = 1.0f;
    
    // Clear previous vision
    std::fill(previous_vision_.begin(), previous_vision_.end(), 0.0f);
}

} // namespace nlm
