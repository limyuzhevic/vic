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
                        if (n) {
                            std::shared_ptr<Neuron> neuronPtr(n, [](Neuron*) {}); // Wrap raw ptr to weak_ptr
                            size_t idx = motorForward_.size() + motorBackward_.size() + 
                                        motorTurnLeft_.size() + motorTurnRight_.size() +
                                        motorInteract_.size() + motorWait_.size();
                            
                            switch (idx % 6) {
                                case 0: motorForward_.push_back(std::weak_ptr<Neuron>(neuronPtr)); break;
                                case 1: motorBackward_.push_back(std::weak_ptr<Neuron>(neuronPtr)); break;
                                case 2: motorTurnLeft_.push_back(std::weak_ptr<Neuron>(neuronPtr)); break;
                                case 3: motorTurnRight_.push_back(std::weak_ptr<Neuron>(neuronPtr)); break;
                                case 4: motorInteract_.push_back(std::weak_ptr<Neuron>(neuronPtr)); break;
                                case 5: motorWait_.push_back(std::weak_ptr<Neuron>(neuronPtr)); break;
                            }
                        }
                    }
                } else if (type == NeuronType::Sensory) {
                    for (Neuron* n : pop->getNeurons()) {
                        if (n) {
                            std::shared_ptr<Neuron> neuronPtr(n, [](Neuron*) {}); // Wrap raw ptr to weak_ptr
                            size_t idx = sensoryVision_.size() + sensoryTouch_.size() +
                                        sensoryInternal_.size() + sensoryProprioception_.size();
                            
                            switch (idx % 4) {
                                case 0: sensoryVision_.push_back(std::weak_ptr<Neuron>(neuronPtr)); break;
                                case 1: sensoryTouch_.push_back(std::weak_ptr<Neuron>(neuronPtr)); break;
                                case 2: sensoryInternal_.push_back(std::weak_ptr<Neuron>(neuronPtr)); break;
                                case 3: sensoryProprioception_.push_back(std::weak_ptr<Neuron>(neuronPtr)); break;
                            }
                        }
                    }
                }
            }
        }
    }
}

AgentBrain::~AgentBrain() = default;

// Helper methods to get locked neuron pointers
std::vector<Neuron*> AgentBrain::getVisionNeurons() const {
    std::vector<Neuron*> locked;
    for (const auto& weakPtr : sensoryVision_) {
        if (auto lockedPtr = weakPtr.lock()) {
            locked.push_back(lockedPtr.get());
        }
    }
    return locked;
}

std::vector<Neuron*> AgentBrain::getTouchNeurons() const {
    std::vector<Neuron*> locked;
    for (const auto& weakPtr : sensoryTouch_) {
        if (auto lockedPtr = weakPtr.lock()) {
            locked.push_back(lockedPtr.get());
        }
    }
    return locked;
}

std::vector<Neuron*> AgentBrain::getInternalNeurons() const {
    std::vector<Neuron*> locked;
    for (const auto& weakPtr : sensoryInternal_) {
        if (auto lockedPtr = weakPtr.lock()) {
            locked.push_back(lockedPtr.get());
        }
    }
    return locked;
}

std::vector<Neuron*> AgentBrain::getProprioceptionNeurons() const {
    std::vector<Neuron*> locked;
    for (const auto& weakPtr : sensoryProprioception_) {
        if (auto lockedPtr = weakPtr.lock()) {
            locked.push_back(lockedPtr.get());
        }
    }
    return locked;
}

std::vector<Neuron*> AgentBrain::getForwardNeurons() const {
    std::vector<Neuron*> locked;
    for (const auto& weakPtr : motorForward_) {
        if (auto lockedPtr = weakPtr.lock()) {
            locked.push_back(lockedPtr.get());
        }
    }
    return locked;
}

std::vector<Neuron*> AgentBrain::getBackwardNeurons() const {
    std::vector<Neuron*> locked;
    for (const auto& weakPtr : motorBackward_) {
        if (auto lockedPtr = weakPtr.lock()) {
            locked.push_back(lockedPtr.get());
        }
    }
    return locked;
}

std::vector<Neuron*> AgentBrain::getLeftNeurons() const {
    std::vector<Neuron*> locked;
    for (const auto& weakPtr : motorTurnLeft_) {
        if (auto lockedPtr = weakPtr.lock()) {
            locked.push_back(lockedPtr.get());
        }
    }
    return locked;
}

std::vector<Neuron*> AgentBrain::getRightNeurons() const {
    std::vector<Neuron*> locked;
    for (const auto& weakPtr : motorTurnRight_) {
        if (auto lockedPtr = weakPtr.lock()) {
            locked.push_back(lockedPtr.get());
        }
    }
    return locked;
}

std::vector<Neuron*> AgentBrain::getInteractNeurons() const {
    std::vector<Neuron*> locked;
    for (const auto& weakPtr : motorInteract_) {
        if (auto lockedPtr = weakPtr.lock()) {
            locked.push_back(lockedPtr.get());
        }
    }
    return locked;
}

std::vector<Neuron*> AgentBrain::getWaitNeurons() const {
    std::vector<Neuron*> locked;
    for (const auto& weakPtr : motorWait_) {
        if (auto lockedPtr = weakPtr.lock()) {
            locked.push_back(lockedPtr.get());
        }
    }
    return locked;
}

// Helper methods to get locked neuron pointers (using the helper function)
std::vector<Neuron*> AgentBrain::getLockedVisionNeurons() const {
    return getNeuronsFromVector(sensoryVision_);
}

std::vector<Neuron*> AgentBrain::getLockedTouchNeurons() const {
    return getNeuronsFromVector(sensoryTouch_);
}

std::vector<Neuron*> AgentBrain::getLockedInternalNeurons() const {
    return getNeuronsFromVector(sensoryInternal_);
}

std::vector<Neuron*> AgentBrain::getLockedProprioceptionNeurons() const {
    return getNeuronsFromVector(sensoryProprioception_);
}

std::vector<Neuron*> AgentBrain::getLockedForwardNeurons() const {
    return getNeuronsFromVector(motorForward_);
}

// Helper methods to get neurons from any weak vector
std::vector<Neuron*> AgentBrain::getNeuronsFromVector(const std::vector<std::weak_ptr<Neuron>>& weakVec) const {
    std::vector<Neuron*> locked;
    for (const auto& weakPtr : weakVec) {
        if (auto lockedPtr = weakPtr.lock()) {
            locked.push_back(lockedPtr.get());
        }
    }
    return locked;
}

void AgentBrain::initialize(const SimpleWorld& world) {
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(getVisionNeurons().size()) + " vision sensory neurons, " +
                 std::to_string(getTouchNeurons().size()) + " touch sensory neurons, " +
                 std::to_string(getInternalNeurons().size()) + " internal sensory neurons");
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
    
    // Get locked neuron pointers (safe access)
    auto visionNeurons = getVisionNeurons();
    auto touchNeurons = getTouchNeurons();
    auto internalNeurons = getInternalNeurons();
    auto proprioNeurons = getProprioceptionNeurons();
    
    // Vision input (256 values -> vision neurons)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < visionNeurons.size() && i < vision.size(); ++i) {
        if (visionNeurons[i]) {
            // Inject current proportional to vision intensity
            float current = vision[i] * 5.0f;  // Scale factor
            visionNeurons[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> touch neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < touchNeurons.size() && i < touch.size(); ++i) {
        if (touchNeurons[i]) {
            float current = touch[i] * 8.0f;  // Collision signal
            touchNeurons[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> internal neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < internalNeurons.size() && i < intern.size(); ++i) {
        if (internalNeurons[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            internalNeurons[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> proprioception neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < proprioNeurons.size() && i < proprio.size(); ++i) {
        if (proprioNeurons[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            proprioNeurons[i]->injectCurrent(current);
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
    // Get locked neuron pointers
    auto forwardNeurons = getLockedForwardNeurons();
    auto backwardNeurons = getLockedNeuronsFromVector(motorBackward_);
    auto leftNeurons = getLockedNeuronsFromVector(motorTurnLeft_);
    auto rightNeurons = getLockedNeuronsFromVector(motorTurnRight_);
    auto interactNeurons = getLockedNeuronsFromVector(motorInteract_);
    auto waitNeurons = getLockedNeuronsFromVector(motorWait_);
    
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
    
    float forwardAct = calcActivity(forwardNeurons);
    float backwardAct = calcActivity(backwardNeurons);
    float leftAct = calcActivity(leftNeurons);
    float rightAct = calcActivity(rightNeurons);
    float interactAct = calcActivity(interactNeurons);
    float waitAct = calcActivity(waitNeurons);
    
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
