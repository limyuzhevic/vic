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
    if (!brain_) {
        NLM_LOG_WARNING("AgentBrain initialized with null brain pointer");
        return;
    }
    
    for (const auto& region : brain_->getRegions()) {
        if (!region) {
            NLM_LOG_WARNING("Skipping null region pointer");
            continue;
        }
        
        for (auto& pop : region->getPopulations()) {
            if (!pop) {
                NLM_LOG_WARNING("Skipping null population pointer");
                continue;
            }
            
            NeuronType type = pop->getNeuronType();
            
            if (type == NeuronType::Motor) {
                const auto& neurons = pop->getNeurons();
                for (Neuron* n : neurons) {
                    if (!n) {
                        NLM_LOG_WARNING("Skipping null neuron pointer");
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
                const auto& neurons = pop->getNeurons();
                for (Neuron* n : neurons) {
                    if (!n) {
                        NLM_LOG_WARNING("Skipping null neuron pointer");
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

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    // Pre-allocate vectors with known maximum sizes to avoid reallocations
    previousVision_.reserve(world.getVisionWidth() * world.getVisionHeight());
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Log initialization with performance info
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
        NLM_LOG_WARNING("Cannot process sensory input - brain is null");
        return;
    }
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    size_t visionSize = vision.size();
    if (visionSize > 0) {
        size_t minVisionSize = std::min(sensoryVision_.size(), visionSize);
        for (size_t i = 0; i < minVisionSize; ++i) {
            Neuron* neuron = sensoryVision_[i];
            if (neuron) {
                // Inject current proportional to vision intensity
                float current = vision[i] * 5.0f;  // Scale factor
                neuron->injectCurrent(current);
            }
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    size_t touchSize = touch.size();
    if (touchSize > 0) {
        size_t minTouchSize = std::min(sensoryTouch_.size(), touchSize);
        for (size_t i = 0; i < minTouchSize; ++i) {
            Neuron* neuron = sensoryTouch_[i];
            if (neuron) {
                float current = touch[i] * 8.0f;  // Collision signal
                neuron->injectCurrent(current);
            }
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    size_t internSize = intern.size();
    if (internSize > 0) {
        size_t minInternSize = std::min(sensoryInternal_.size(), internSize);
        for (size_t i = 0; i < minInternSize; ++i) {
            Neuron* neuron = sensoryInternal_[i];
            if (neuron) {
                float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
                neuron->injectCurrent(current);
            }
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    size_t proprioSize = proprio.size();
    if (proprioSize > 0) {
        size_t minProprioSize = std::min(sensoryProprioception_.size(), proprioSize);
        for (size_t i = 0; i < minProprioSize; ++i) {
            Neuron* neuron = sensoryProprioception_[i];
            if (neuron) {
                float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
                neuron->injectCurrent(current);
            }
        }
    }
    
    // Compute novelty (difference from previous vision) - optimized
    if (!vision.empty() && !previousVision_.empty()) {
        size_t minVisionSize = std::min(vision.size(), previousVision_.size());
        float totalDiff = 0.0f;
        
        // Unroll loop for better performance (small fixed size)
        if (minVisionSize >= 4) {
            size_t unrolledCount = minVisionSize - (minVisionSize % 4);
            for (size_t i = 0; i < unrolledCount; i += 4) {
                totalDiff += std::abs(vision[i] - previousVision_[i]);
                totalDiff += std::abs(vision[i + 1] - previousVision_[i + 1]);
                totalDiff += std::abs(vision[i + 2] - previousVision_[i + 2]);
                totalDiff += std::abs(vision[i + 3] - previousVision_[i + 3]);
            }
            for (size_t i = minVisionSize - (minVisionSize % 4); i < minVisionSize; ++i) {
                totalDiff += std::abs(vision[i] - previousVision_[i]);
            }
        } else {
            for (size_t i = 0; i < minVisionSize; ++i) {
                totalDiff += std::abs(vision[i] - previousVision_[i]);
            }
        }
        
        // Normalize
        noveltyLevel_ = totalDiff / static_cast<float>(visionSize);
        
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
    // Optimized: use pre-calculated values and early exits
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        
        float sum = 0.0f;
        size_t validNeurons = 0;
        
        for (Neuron* n : neurons) {
            if (!n) continue;  // Skip null neurons
            validNeurons++;
            // Use membrane potential deviation from rest as activity measure
            sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
        
        return validNeurons > 0 ? sum / static_cast<float>(validNeurons) : 0.0f;
    };
    
    // Pre-calculate activities
    const std::vector<Neuron*>& forwardGroup = motorForward_;
    const std::vector<Neuron*>& backwardGroup = motorBackward_;
    const std::vector<Neuron*>& leftGroup = motorTurnLeft_;
    const std::vector<Neuron*>& rightGroup = motorTurnRight_;
    const std::vector<Neuron*>& interactGroup = motorInteract_;
    const std::vector<Neuron*>& waitGroup = motorWait_;
    
    float forwardAct = calcActivity(forwardGroup);
    float backwardAct = calcActivity(backwardGroup);
    float leftAct = calcActivity(leftGroup);
    float rightAct = calcActivity(rightGroup);
    float interactAct = calcActivity(interactGroup);
    float waitAct = calcActivity(waitGroup);
    
    // Find maximum activity - optimized for common case (wait)
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct;  // Default to wait if nothing stronger
    
    // Check only if we have meaningful activity
    if (forwardAct >= 0.5f || backwardAct >= 0.5f || leftAct >= 0.5f || 
        rightAct >= 0.5f || interactAct >= 0.5f) {
        
        struct { MotorCommand cmd; float activity; } commands[] = {
            {MotorCommand::MoveForward, forwardAct},
            {MotorCommand::MoveBackward, backwardAct},
            {MotorCommand::TurnLeft, leftAct},
            {MotorCommand::TurnRight, rightAct},
            {MotorCommand::Interact, interactAct},
            {MotorCommand::Wait, waitAct}
        };
        
        for (const auto& c : commands) {
            if (c.activity > bestActivity) {
                bestActivity = c.activity;
                best = c.cmd;
            }
        }
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > 0.5f && brain_) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * 0.3f;  // Up to 30% random
        
        // Check if random generator exists
        auto* rng = brain_->getRandomGenerator();
        if (!rng) {
            NLM_LOG_WARNING("Cannot select with curiosity - random generator is null");
            return defaultCmd;
        }
        
        float r = rng->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command
            int choice = rng->uniformInt(0, 7);
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
    if (!brain_ || !rewardModulationEnabled_) {
        if (!brain_) {
            NLM_LOG_WARNING("Cannot apply reward modulation - brain is null");
        }
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
        if (!region) {
            NLM_LOG_WARNING("Skipping null region in reward modulation");
            continue;
        }
        
        const auto& synapses = region->getSynapses();
        for (auto* syn : synapses) {
            if (!syn) {
                NLM_LOG_WARNING("Skipping null synapse in reward modulation");
                continue;
            }
            
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
    if (!stdp) {
        NLM_LOG_WARNING("Cannot apply plasticity - STDP system is null");
    } else {
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) {
        if (!brain_) {
            NLM_LOG_WARNING("Cannot update development - brain is null");
        }
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
        if (!sp) {
            NLM_LOG_WARNING("Cannot update structural plasticity - system is null");
        } else {
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

} // namespace nlm
