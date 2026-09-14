// Clean, correct implementation of AgentBrain with all improvements
#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <stdexcept>
#include <cassert>
#include <random>

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
    if (!brain) {
        throw std::invalid_argument("Brain cannot be null");
    }
    
    // Initialize motor and sensory neuron groups
    updateNeuronDistribution();
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::updateNeuronDistribution() {
    // Clear existing weak pointers
    motorForward_.clear();
    motorBackward_.clear();
    motorTurnLeft_.clear();
    motorTurnRight_.clear();
    motorInteract_.clear();
    motorWait_.clear();
    sensoryVision_.clear();
    sensoryTouch_.clear();
    sensoryInternal_.clear();
    sensoryProprioception_.clear();
    
    // Re-populate with weak references using actual neurons from brain
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    // Get actual neurons from the population to distribute
                    const auto& neurons = pop->getNeurons();
                    if (!neurons.empty()) {
                        size_t neuronCount = neurons.size();
                        size_t neuronsAssigned = 0;
                        
                        // Round-robin distribution to different action groups
                        for (size_t i = 0; i < neuronCount; ++i) {
                            size_t idx = neuronsAssigned;
                            
                            if (idx % 6 == 0) {
                                motorForward_.push_back(std::weak_ptr<Neuron>(neurons[i]));
                            } else if (idx % 6 == 1) {
                                motorBackward_.push_back(std::weak_ptr<Neuron>(neurons[i]));
                            } else if (idx % 6 == 2) {
                                motorTurnLeft_.push_back(std::weak_ptr<Neuron>(neurons[i]));
                            } else if (idx % 6 == 3) {
                                motorTurnRight_.push_back(std::weak_ptr<Neuron>(neurons[i]));
                            } else if (idx % 6 == 4) {
                                motorInteract_.push_back(std::weak_ptr<Neuron>(neurons[i]));
                            } else {
                                motorWait_.push_back(std::weak_ptr<Neuron>(neurons[i]));
                            }
                            
                            neuronsAssigned++;
                        }
                    }
                } else if (type == NeuronType::Sensory) {
                    const auto& neurons = pop->getNeurons();
                    if (!neurons.empty()) {
                        size_t neuronCount = neurons.size();
                        size_t neuronsAssigned = 0;
                        
                        // Round-robin distribution to different sensory channels
                        for (size_t i = 0; i < neuronCount; ++i) {
                            size_t idx = neuronsAssigned;
                            
                            if (idx % 4 == 0) {
                                sensoryVision_.push_back(std::weak_ptr<Neuron>(neurons[i]));
                            } else if (idx % 4 == 1) {
                                sensoryTouch_.push_back(std::weak_ptr<Neuron>(neurons[i]));
                            } else if (idx % 4 == 2) {
                                sensoryInternal_.push_back(std::weak_ptr<Neuron>(neurons[i]));
                            } else {
                                sensoryProprioception_.push_back(std::weak_ptr<Neuron>(neurons[i]));
                            }
                            
                            neuronsAssigned++;
                        }
                    }
                }
            }
        }
    }
}

bool AgentBrain::validateBrainState() const {
    return brain_ != nullptr;
}

void AgentBrain::validateSensoryData(const std::vector<float>& vision, 
                                   const std::vector<float>& touch,
                                   const std::vector<float>& internal,
                                   const std::vector<float>& proprio) const {
    // Validate vision data (0.0-1.0 range)
    for (float val : vision) {
        if (val < 0.0f || val > 1.0f) {
            NLM_LOG_WARNING("Vision value out of range: " + std::to_string(val));
        }
    }
    
    // Validate touch data (0.0-1.0 range)
    for (float val : touch) {
        if (val < 0.0f || val > 1.0f) {
            NLM_LOG_WARNING("Touch value out of range: " + std::to_string(val));
        }
    }
    
    // Validate internal signals (-1.0 to 1.0 range)
    for (float val : internal) {
        if (val < -1.0f || val > 1.0f) {
            NLM_LOG_WARNING("Internal signal value out of range: " + std::to_string(val));
        }
    }
    
    // Validate proprioception (-1.0 to 1.0 range)
    for (float val : proprio) {
        if (val < -1.0f || val > 1.0f) {
            NLM_LOG_WARNING("Proprioception value out of range: " + std::to_string(val));
        }
    }
}

void AgentBrain::initialize(const SimpleWorld& world) {
    if (!validateBrainState()) {
        throw std::runtime_error("Cannot initialize AgentBrain: brain not set");
    }
    
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(getSensoryNeuronCount()) + " total sensory neurons, " +
                 std::to_string(getMotorNeuronCount()) + " total motor neurons");
}

size_t AgentBrain::getSensoryInputSize() const {
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
    return 256 + 8 + 4 + 6;
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action
    return 6;
}

size_t AgentBrain::getMotorNeuronCount() const {
    return motorForward_.size() + motorBackward_.size() + 
           motorTurnLeft_.size() + motorTurnRight_.size() +
           motorInteract_.size() + motorWait_.size();
}

size_t AgentBrain::getSensoryNeuronCount() const {
    return sensoryVision_.size() + sensoryTouch_.size() +
           sensoryInternal_.size() + sensoryProprioception_.size();
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!validateBrainState()) {
        NLM_LOG_ERROR("Cannot process sensory input: brain not initialized");
        return;
    }
    
    // Get sensory data with validation
    const auto& vision = percept.getVision();
    const auto& touch = percept.getTouch();
    const auto& internal = percept.getInternal();
    const auto& proprio = percept.getProprioception();
    
    // Validate sensory data ranges
    validateSensoryData(vision, touch, internal, proprio);
    
    // Vision input (256 values -> sensoryVision_ neurons)
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        auto neuron = sensoryVision_[i].lock();
        if (neuron) {
            // Inject current proportional to vision intensity
            float current = vision[i] * 5.0f;  // Scale factor
            neuron->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        auto neuron = sensoryTouch_[i].lock();
        if (neuron) {
            float current = touch[i] * 8.0f;  // Collision signal
            neuron->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    for (size_t i = 0; i < sensoryInternal_.size() && i < internal.size(); ++i) {
        auto neuron = sensoryInternal_[i].lock();
        if (neuron) {
            float current = (internal[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            neuron->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        auto neuron = sensoryProprioception_[i].lock();
        if (neuron) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            neuron->injectCurrent(current);
        }
    }
    
    // Compute novelty (difference from previous vision) - OPTIMIZED VERSION
    if (!vision.empty() && !previousVision_.empty()) {
        // Use efficient computation for better performance
        float totalDiff = 0.0f;
        size_t validPairs = 0;
        
        size_t minSize = std::min(vision.size(), previousVision_.size());
        for (size_t i = 0; i < minSize; ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
            validPairs++;
        }
        
        // Normalize
        noveltyLevel_ = validPairs > 0 ? (totalDiff / validPairs) : 0.0f;
        
        // Decay and update
        noveltyLevel_ *= sensoryNoveltyDecay_;
        
        // Store for next time - efficient assignment
        previousVision_.assign(vision.begin(), vision.end());
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
    auto calcActivity = [](const std::vector<std::weak_ptr<Neuron>>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        size_t validCount = 0;
        for (const auto& weakNeuron : neurons) {
            auto neuron = weakNeuron.lock();
            if (neuron) {
                sum += std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
                validCount++;
            }
        }
        return validCount > 0 ? (sum / validCount) : 0.0f;
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
        
        if (brain_->getRandomGenerator()) {
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
    }
    
    return defaultCmd;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Validate input
    if (std::isnan(reward) || std::isnan(predictedReward)) {
        NLM_LOG_WARNING("Invalid reward or predicted reward values");
        return;
    }
    
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
            if (!syn) continue;
            
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
    
    // Validate timestep
    if (std::isnan(timestep) || timestep < 0.0) {
        return;
    }
    
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
    previousVision_.clear();
}

} // namespace nlm