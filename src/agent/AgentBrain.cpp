#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <memory>

namespace nlm {

// Add member initializers for neuron vectors
    : brain_(brain)
    , motorForward_()
    , motorBackward_()
    , motorTurnLeft_()
    , motorTurnRight_()
    , motorInteract_()
    , motorWait_()
    , sensoryVision_()
    , sensoryTouch_()
    , sensoryInternal_()
    , sensoryProprioception_()
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
                            case 0: motorForward_.push_back(std::weak_ptr<Neuron>(n)); break;
                            case 1: motorBackward_.push_back(std::weak_ptr<Neuron>(n)); break;
                            case 2: motorTurnLeft_.push_back(std::weak_ptr<Neuron>(n)); break;
                            case 3: motorTurnRight_.push_back(std::weak_ptr<Neuron>(n)); break;
                            case 4: motorInteract_.push_back(std::weak_ptr<Neuron>(n)); break;
                            case 5: motorWait_.push_back(std::weak_ptr<Neuron>(n)); break;
                        }
                    }
                } else if (type == NeuronType::Sensory) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Distribute sensory neurons
                        size_t idx = sensoryVision_.size() + sensoryTouch_.size() +
                                    sensoryInternal_.size() + sensoryProprioception_.size();
                        
                        switch (idx % 4) {
                            case 0: sensoryVision_.push_back(std::weak_ptr<Neuron>(n)); break;
                            case 1: sensoryTouch_.push_back(std::weak_ptr<Neuron>(n)); break;
                            case 2: sensoryInternal_.push_back(std::weak_ptr<Neuron>(n)); break;
                            case 3: sensoryProprioception_.push_back(std::weak_ptr<Neuron>(n)); break;
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
    
    // Resolve weak pointers to ensure neurons are still valid
    auto resolveNeuron = [](std::vector<std::weak_ptr<Neuron>>& weakNeurons) {
        std::vector<Neuron*> resolvedNeurons;
        for (auto& weakNeuron : weakNeurons) {
            auto neuron = weakNeuron.lock();
            if (neuron) resolvedNeurons.push_back(neuron.get());
        }
        weakNeurons.clear();
        weakNeurons.reserve(resolvedNeurons.size());
        for (Neuron* neuron : resolvedNeurons) {
            weakNeurons.push_back(std::weak_ptr<Neuron>(neuron));
        }
        return resolvedNeurons;
    };
    
    // Resolve motor neuron groups
    std::vector<Neuron*> motorForwardRes = resolveNeuron(motorForward_);
    std::vector<Neuron*> motorBackwardRes = resolveNeuron(motorBackward_);
    std::vector<Neuron*> motorTurnLeftRes = resolveNeuron(motorTurnLeft_);
    std::vector<Neuron*> motorTurnRightRes = resolveNeuron(motorTurnRight_);
    std::vector<Neuron*> motorInteractRes = resolveNeuron(motorInteract_);
    std::vector<Neuron*> motorWaitRes = resolveNeuron(motorWait_);
    
    // Resolve sensory neuron groups
    std::vector<Neuron*> sensoryVisionRes = resolveNeuron(sensoryVision_);
    std::vector<Neuron*> sensoryTouchRes = resolveNeuron(sensoryTouch_);
    std::vector<Neuron*> sensoryInternalRes = resolveNeuron(sensoryInternal_);
    std::vector<Neuron*> sensoryProprioceptionRes = resolveNeuron(sensoryProprioception_);
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVisionRes.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouchRes.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternalRes.size()) + " internal sensory neurons");
    
    // Update the weak vectors with resolved neurons
    motorForward_ = std::vector<std::weak_ptr<Neuron>>(motorForwardRes.begin(), motorForwardRes.end());
    motorBackward_ = std::vector<std::weak_ptr<Neuron>>(motorBackwardRes.begin(), motorBackwardRes.end());
    motorTurnLeft_ = std::vector<std::weak_ptr<Neuron>>(motorTurnLeftRes.begin(), motorTurnLeftRes.end());
    motorTurnRight_ = std::vector<std::weak_ptr<Neuron>>(motorTurnRightRes.begin(), motorTurnRightRes.end());
    motorInteract_ = std::vector<std::weak_ptr<Neuron>>(motorInteractRes.begin(), motorInteractRes.end());
    motorWait_ = std::vector<std::weak_ptr<Neuron>>(motorWaitRes.begin(), motorWaitRes.end());
    
    sensoryVision_ = std::vector<std::weak_ptr<Neuron>>(sensoryVisionRes.begin(), sensoryVisionRes.end());
    sensoryTouch_ = std::vector<std::weak_ptr<Neuron>>(sensoryTouchRes.begin(), sensoryTouchRes.end());
    sensoryInternal_ = std::vector<std::weak_ptr<Neuron>>(sensoryInternalRes.begin(), sensoryInternalRes.end());
    sensoryProprioception_ = std::vector<std::weak_ptr<Neuron>>(sensoryProprioceptionRes.begin(), sensoryProprioceptionRes.end());
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
    
    // Resolve weak pointers to current neurons
    auto resolveNeurons = [](const std::vector<std::weak_ptr<Neuron>>& weakNeurons) {
        std::vector<Neuron*> resolved;
        for (const auto& weakNeuron : weakNeurons) {
            auto neuron = weakNeuron.lock();
            if (neuron) resolved.push_back(neuron.get());
        }
        return resolved;
    };
    
    // Resolve all neuron groups
    std::vector<Neuron*> sensoryVisionRes = resolveNeurons(sensoryVision_);
    std::vector<Neuron*> sensoryTouchRes = resolveNeurons(sensoryTouch_);
    std::vector<Neuron*> sensoryInternalRes = resolveNeurons(sensoryInternal_);
    std::vector<Neuron*> sensoryProprioceptionRes = resolveNeurons(sensoryProprioception_);
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVisionRes.size() && i < vision.size(); ++i) {
        Neuron* neuron = sensoryVisionRes[i];
        if (neuron) {
            // Inject current proportional to vision intensity
            float current = vision[i] * 5.0f;  // Scale factor
            neuron->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouchRes.size() && i < touch.size(); ++i) {
        Neuron* neuron = sensoryTouchRes[i];
        if (neuron) {
            float current = touch[i] * 8.0f;  // Collision signal
            neuron->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternalRes.size() && i < intern.size(); ++i) {
        Neuron* neuron = sensoryInternalRes[i];
        if (neuron) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            neuron->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioceptionRes.size() && i < proprio.size(); ++i) {
        Neuron* neuron = sensoryProprioceptionRes[i];
        if (neuron) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            neuron->injectCurrent(current);
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
    // Resolve weak pointers to current neurons
    auto resolveNeurons = [](const std::vector<std::weak_ptr<Neuron>>& weakNeurons) {
        std::vector<Neuron*> resolved;
        for (const auto& weakNeuron : weakNeurons) {
            auto neuron = weakNeuron.lock();
            if (neuron) resolved.push_back(neuron.get());
        }
        return resolved;
    };
    
    // Resolve all motor neuron groups
    std::vector<Neuron*> motorForwardRes = resolveNeurons(motorForward_);
    std::vector<Neuron*> motorBackwardRes = resolveNeurons(motorBackward_);
    std::vector<Neuron*> motorTurnLeftRes = resolveNeurons(motorTurnLeft_);
    std::vector<Neuron*> motorTurnRightRes = resolveNeurons(motorTurnRight_);
    std::vector<Neuron*> motorInteractRes = resolveNeurons(motorInteract_);
    std::vector<Neuron*> motorWaitRes = resolveNeurons(motorWait_);
    
    // Calculate average activity in each motor group
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            // Use membrane potential deviation from rest as activity measure
            if (n) sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
        return neurons.empty() ? 0.0f : sum / neurons.size();
    };
    
    float forwardAct = calcActivity(motorForwardRes);
    float backwardAct = calcActivity(motorBackwardRes);
    float leftAct = calcActivity(motorTurnLeftRes);
    float rightAct = calcActivity(motorTurnRightRes);
    float interactAct = calcActivity(motorInteractRes);
    float waitAct = calcActivity(motorWaitRes);
    
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
    
    // Resolve weak pointers to current neurons and synapses
    auto resolveNeurons = [](const std::vector<std::weak_ptr<Neuron>>& weakNeurons) {
        std::vector<Neuron*> resolved;
        for (const auto& weakNeuron : weakNeurons) {
            auto neuron = weakNeuron.lock();
            if (neuron) resolved.push_back(neuron.get());
        }
        return resolved;
    };
    
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

} // namespace nlm
