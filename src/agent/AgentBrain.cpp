#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Pimpl implementation details
struct AgentBrain::Impl {
    std::shared_ptr<Brain> brain_;
    
    // Motor neuron groups
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Sensory neuron groups
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    // Neuromodulation state
    float dopamineLevel_ = 0.0f;
    float noveltyLevel_ = 0.0f;
    float curiosityLevel_ = 0.0f;
    float predictionError_ = 0.0f;
    float expectedReward_ = 0.0f;
    
    // Development state
    double developmentalAge_ = 0.0;
    float plasticityModifier_ = 1.0f;
    
    // Configuration flags
    bool rewardModulationEnabled_ = true;
    bool structuralPlasticityEnabled_ = true;
    bool developmentEnabled_ = true;
    bool curiosityEnabled_ = true;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_ = 0.99f;
};

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(brain)
{
    pImpl = std::make_unique<Impl>();
    pImpl->brain_ = brain;
    
    // Initialize motor and sensory neuron groups
    if (pImpl->brain_) {
        for (const auto& region : pImpl->brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Distribute motor neurons to different action groups
                        size_t idx = pImpl->motorForward_.size() + pImpl->motorBackward_.size() + 
                                    pImpl->motorTurnLeft_.size() + pImpl->motorTurnRight_.size() +
                                    pImpl->motorInteract_.size() + pImpl->motorWait_.size();
                        
                        switch (idx % 6) {
                            case 0: pImpl->motorForward_.push_back(n); break;
                            case 1: pImpl->motorBackward_.push_back(n); break;
                            case 2: pImpl->motorTurnLeft_.push_back(n); break;
                            case 3: pImpl->motorTurnRight_.push_back(n); break;
                            case 4: pImpl->motorInteract_.push_back(n); break;
                            case 5: pImpl->motorWait_.push_back(n); break;
                        }
                    }
                } else if (type == NeuronType::Sensory) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Distribute sensory neurons
                        size_t idx = pImpl->sensoryVision_.size() + pImpl->sensoryTouch_.size() +
                                    pImpl->sensoryInternal_.size() + pImpl->sensoryProprioception_.size();
                        
                        switch (idx % 4) {
                            case 0: pImpl->sensoryVision_.push_back(n); break;
                            case 1: pImpl->sensoryTouch_.push_back(n); break;
                            case 2: pImpl->sensoryInternal_.push_back(n); break;
                            case 3: pImpl->sensoryProprioception_.push_back(n); break;
                        }
                    }
                }
            }
        }
    }
}

AgentBrain::AgentBrain(AgentBrain&& other) noexcept
    : pImpl(std::move(other.pImpl))
    , brain_(std::move(other.brain_))
{
    // Clear other pointers to prevent double-free
    other.brain_ = nullptr;
}

AgentBrain& AgentBrain::operator=(AgentBrain&& other) noexcept {
    if (this != &other) {
        // Clean up current resources
        pImpl.reset();
        brain_ = nullptr;
        
        // Move resources from other
        pImpl = std::move(other.pImpl);
        brain_ = std::move(other.brain_);
        
        // Clear other pointers
        other.brain_ = nullptr;
    }
    return *this;
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    if (!pImpl) return;
    
    pImpl->previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    pImpl->developmentalAge_ = 0.0;
    pImpl->plasticityModifier_ = 1.0f;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(pImpl->sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(pImpl->sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(pImpl->sensoryInternal_.size()) + " internal sensory neurons");
}

size_t AgentBrain::getSensoryInputSize() const {
    return 256 + 8 + 4 + 6;  // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
}

size_t AgentBrain::getMotorOutputSize() const {
    return 6;  // One motor neuron per action
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!pImpl || !pImpl->brain_) return;
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < pImpl->sensoryVision_.size() && i < vision.size(); ++i) {
        if (pImpl->sensoryVision_[i]) {
            // Inject current proportional to vision intensity
            float current = vision[i] * 5.0f;  // Scale factor
            pImpl->sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < pImpl->sensoryTouch_.size() && i < touch.size(); ++i) {
        if (pImpl->sensoryTouch_[i]) {
            float current = touch[i] * 8.0f;  // Collision signal
            pImpl->sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < pImpl->sensoryInternal_.size() && i < intern.size(); ++i) {
        if (pImpl->sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            pImpl->sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < pImpl->sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (pImpl->sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            pImpl->sensoryProprioception_[i]->injectCurrent(current);
        }
    }
    
    // Compute novelty (difference from previous vision)
    if (!vision.empty()) {
        float totalDiff = 0.0f;
        for (size_t i = 0; i < vision.size() && i < pImpl->previousVision_.size(); ++i) {
            float diff = std::abs(vision[i] - pImpl->previousVision_[i]);
            totalDiff += diff;
        }
        
        // Normalize
        pImpl->noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
        
        // Decay and update
        pImpl->noveltyLevel_ *= pImpl->sensoryNoveltyDecay_;
        
        // Store for next time
        pImpl->previousVision_ = vision;
    }
    
    // Update curiosity based on novelty
    if (pImpl->curiosityEnabled_) {
        pImpl->curiosityLevel_ = pImpl->noveltyLevel_ * 2.0f + std::abs(pImpl->predictionError_) * 0.5f;
        pImpl->curiosityLevel_ = std::clamp(pImpl->curiosityLevel_, 0.0f, 1.0f);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!pImpl || !pImpl->brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (pImpl->curiosityEnabled_ && pImpl->curiosityLevel_ > 0.3f) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    if (!pImpl) return MotorCommand::Wait;
    
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
    
    float forwardAct = calcActivity(pImpl->motorForward_);
    float backwardAct = calcActivity(pImpl->motorBackward_);
    float leftAct = calcActivity(pImpl->motorTurnLeft_);
    float rightAct = calcActivity(pImpl->motorTurnRight_);
    float interactAct = calcActivity(pImpl->motorInteract_);
    float waitAct = calcActivity(pImpl->motorWait_);
    
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
    if (!pImpl) return defaultCmd;
    
    // Exploration: occasionally choose random action when curiosity is high
    if (pImpl->curiosityLevel_ > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = pImpl->curiosityLevel_ * 0.3f;  // Up to 30% random
        
        float r = pImpl->brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command
            int choice = pImpl->brain_->getRandomGenerator()->uniformInt(0, 7);
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
    if (!pImpl || !pImpl->brain_ || !pImpl->rewardModulationEnabled_) return;
    
    // Compute prediction error
    pImpl->predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    pImpl->expectedReward_ = 0.95f * pImpl->expectedReward_ + 0.05f * reward;
    
    // Dopamine-like signal (based on prediction error)
    pImpl->dopamineLevel_ = pImpl->predictionError_;
    
    // Clamp to reasonable range
    pImpl->dopamineLevel_ = std::clamp(pImpl->dopamineLevel_, -1.0f, 1.0f);
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : pImpl->brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Apply reward-modulated weight change
                float delta = eligibility * pImpl->dopamineLevel_ * pImpl->plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
    
    // Modulate plasticity based on dopamine
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = 0.5f + 0.5f * pImpl->dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    // Apply to STDP
    auto* stdp = pImpl->brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!pImpl || !pImpl->brain_ || !pImpl->developmentEnabled_) return;
    
    pImpl->developmentalAge_ += timestep;
    
    // Simple developmental stages based on age
    // This is a biologically inspired approximation
    if (pImpl->developmentalAge_ < 60.0) {  // ~1 minute
        pImpl->plasticityModifier_ = 1.0f;  // High plasticity
        pImpl->brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (pImpl->developmentalAge_ < 300.0) {  // ~5 minutes
        pImpl->plasticityModifier_ = 0.8f;
        pImpl->brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (pImpl->developmentalAge_ < 900.0) {  // ~15 minutes
        pImpl->plasticityModifier_ = 0.5f;
        pImpl->brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        pImpl->plasticityModifier_ = 0.2f;  // Adult - more stable
        pImpl->brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Structural plasticity changes with development
    if (pImpl->structuralPlasticityEnabled_) {
        auto* sp = pImpl->brain_->getStructuralPlasticity();
        if (sp) {
            // Higher synaptogenesis in early development
            float synRate = 0.0001f * pImpl->plasticityModifier_;
            float pruneRate = 0.00001f * (2.0f - pImpl->plasticityModifier_);
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
    }
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!pImpl || !pImpl->brain_) return DevelopmentalStage::Initial;
    return pImpl->brain_->getDevelopmentalStage();
}

float AgentBrain::getNeuromodulationLevel() const {
    return pImpl ? pImpl->dopamineLevel_ : 0.0f;
}

float AgentBrain::getCuriosityLevel() const {
    return pImpl ? pImpl->curiosityLevel_ : 0.0f;
}

float AgentBrain::getNoveltyLevel() const {
    return pImpl ? pImpl->noveltyLevel_ : 0.0f;
}

float AgentBrain::getPredictionError() const {
    return pImpl ? pImpl->predictionError_ : 0.0f;
}

void AgentBrain::reset() {
    if (!pImpl) return;
    
    pImpl->dopamineLevel_ = 0.0f;
    pImpl->noveltyLevel_ = 0.0f;
    pImpl->curiosityLevel_ = 0.0f;
    pImpl->predictionError_ = 0.0f;
    pImpl->expectedReward_ = 0.0f;
    pImpl->developmentalAge_ = 0.0;
    pImpl->plasticityModifier_ = 1.0f;
    
    // Clear previous vision cache
    pImpl->previousVision_.clear();
}

AgentBrain* AgentBrain::getBrain() {
    return pImpl && pImpl->brain_ ? pImpl->brain_.get() : nullptr;
}

const AgentBrain* AgentBrain::getBrain() const {
    return pImpl && pImpl->brain_ ? pImpl->brain_.get() : nullptr;
}

void AgentBrain::enableRewardModulation(bool enable) {
    if (pImpl) pImpl->rewardModulationEnabled_ = enable;
}

void AgentBrain::enableStructuralPlasticity(bool enable) {
    if (pImpl) pImpl->structuralPlasticityEnabled_ = enable;
}

void AgentBrain::enableDevelopment(bool enable) {
    if (pImpl) pImpl->developmentEnabled_ = enable;
}

void AgentBrain::enableCuriosity(bool enable) {
    if (pImpl) pImpl->curiosityEnabled_ = enable;
}

bool AgentBrain::isRewardModulationEnabled() const {
    return pImpl ? pImpl->rewardModulationEnabled_ : false;
}

bool AgentBrain::isStructuralPlasticityEnabled() const {
    return pImpl ? pImpl->structuralPlasticityEnabled_ : false;
}

bool AgentBrain::isDevelopmentEnabled() const {
    return pImpl ? pImpl->developmentEnabled_ : false;
}

bool AgentBrain::isCuriosityEnabled() const {
    return pImpl ? pImpl->curiosityEnabled_ : false;
}

} // namespace nlm