#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

void AgentBrain::distributeMotorNeurons() {
    // Helper function to distribute motor neurons
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Motor) {
                for (Neuron* n : pop->getNeurons()) {
                    // Distribute motor neurons to different action groups
                    // Note: MotorCommand enum has 8 values but we only have 6 groups
                    // LookLeft and LookRight are handled separately
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
            }
        }
    }
}

void AgentBrain::distributeSensoryNeurons() {
    // Helper function to distribute sensory neurons
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Sensory) {
                for (Neuron* n : pop->getNeurons()) {
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
    , visionScale_(5.0f)
    , touchScale_(8.0f)
    , internalScale_(5.0f)
    , proprioceptionScale_(3.0f)
    , curiosityThreshold_(0.3f)
    , explorationRate_(0.3f)
    , noveltyThreshold_(0.5f)
    , noveltyDecay_(0.99f)
    , totalSensoryEvents_(0)
    , totalMotorCommandsIssued_(0)
    , totalProcessingTime_(0.0)
{
    // Initialize motor and sensory neuron groups
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
    
    // Only proceed if brain is valid
    if (!brain_) {
        return;
    }
    
    // Distribute neurons using helper functions
    distributeMotorNeurons();
    distributeSensoryNeurons();
}

AgentBrain::~AgentBrain() {
    // Clear all neuron group vectors to ensure proper cleanup
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
    
    previousVision_.clear();
}

void AgentBrain::initialize(const SimpleWorld& world) {
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
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
    if (!brain_) return;
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    if (!vision.empty() && !sensoryVision_.empty()) {
        for (size_t i = 0; i < std::min(sensoryVision_.size(), vision.size()); ++i) {
            if (sensoryVision_[i]) {
                // Inject current proportional to vision intensity with configurable scale
                float current = vision[i] * visionScale_;
                sensoryVision_[i]->injectCurrent(current);
            }
        }
        totalSensoryEvents_++;
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    if (!touch.empty() && !sensoryTouch_.empty()) {
        for (size_t i = 0; i < std::min(sensoryTouch_.size(), touch.size()); ++i) {
            if (sensoryTouch_[i]) {
                float current = touch[i] * touchScale_;
                sensoryTouch_[i]->injectCurrent(current);
            }
        }
        totalSensoryEvents_++;
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    if (!intern.empty() && !sensoryInternal_.empty()) {
        for (size_t i = 0; i < std::min(sensoryInternal_.size(), intern.size()); ++i) {
            if (sensoryInternal_[i]) {
                float current = (intern[i] * 2.0f - 1.0f) * internalScale_;
                sensoryInternal_[i]->injectCurrent(current);
            }
        }
        totalSensoryEvents_++;
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    if (!proprio.empty() && !sensoryProprioception_.empty()) {
        for (size_t i = 0; i < std::min(sensoryProprioception_.size(), proprio.size()); ++i) {
            if (sensoryProprioception_[i]) {
                float current = (proprio[i] * 2.0f - 1.0f) * proprioceptionScale_;
                sensoryProprioception_[i]->injectCurrent(current);
            }
        }
        totalSensoryEvents_++;
    }
    
    // Compute novelty (difference from previous vision)
    if (!vision.empty()) {
        float totalDiff = 0.0f;
        for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Normalize - avoid division by zero
        if (!vision.empty()) {
            noveltyLevel_ = totalDiff / static_cast<float>(vision.size());
        }
        
        // Apply novelty threshold
        if (noveltyLevel_ > noveltyThreshold_) {
            noveltyLevel_ = 1.0f; // Clamp to max if exceeds threshold
        }
        
        // Decay and update
        noveltyLevel_ *= noveltyDecay_;
        
        // Store for next time
        previousVision_ = vision;
    }
    
    // Update curiosity based on novelty
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
        
        // Apply curiosity threshold
        if (curiosityLevel_ > curiosityThreshold_) {
            curiosityLevel_ = 1.0f; // Maximum exploration
        }
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        decoded = selectWithCuriosity(decoded);
    }
    
    totalMotorCommandsIssued_++;
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Safety check: ensure brain_ is valid before accessing
    if (!brain_) return MotorCommand::Wait;
    
    // Calculate average activity in each motor group
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            // Safety check: ensure neuron pointer is valid
            if (n) {
                // Use membrane potential deviation from rest as activity measure
                sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
            }
        }
        return sum / neurons.size();
    };
    
    // Ensure all motor neuron groups are initialized before use
    float forwardAct = motorForward_.empty() ? 0.0f : calcActivity(motorForward_);
    float backwardAct = motorBackward_.empty() ? 0.0f : calcActivity(motorBackward_);
    float leftAct = motorTurnLeft_.empty() ? 0.0f : calcActivity(motorTurnLeft_);
    float rightAct = motorTurnRight_.empty() ? 0.0f : calcActivity(motorTurnRight_);
    float interactAct = motorInteract_.empty() ? 0.0f : calcActivity(motorInteract_);
    float waitAct = motorWait_.empty() ? 0.0f : calcActivity(motorWait_);
    
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
    if (curiosityLevel_ > curiosityThreshold_) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * explorationRate_;  // Configurable
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command from full set (including LookLeft/LookRight)
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
    // Critical null checks - ensure brain_ is valid
    if (!brain_) {
        return; // Cannot apply modulation without a valid brain
    }
    
    if (!rewardModulationEnabled_) return;
    
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
            // Safety check: ensure synapse pointer is valid
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

bool AgentBrain::validateNeuronPointers() const {
    bool valid = true;
    // Check motor neuron groups
    for (const auto& n : motorForward_) if (!n) valid = false;
    for (const auto& n : motorBackward_) if (!n) valid = false;
    for (const auto& n : motorTurnLeft_) if (!n) valid = false;
    for (const auto& n : motorTurnRight_) if (!n) valid = false;
    for (const auto& n : motorInteract_) if (!n) valid = false;
    for (const auto& n : motorWait_) if (!n) valid = false;
    
    // Check sensory neuron groups
    for (const auto& n : sensoryVision_) if (!n) valid = false;
    for (const auto& n : sensoryTouch_) if (!n) valid = false;
    for (const auto& n : sensoryInternal_) if (!n) valid = false;
    for (const auto& n : sensoryProprioception_) if (!n) valid = false;
    
    return valid;
}

bool AgentBrain::validateBrainConnectivity() const {
    if (!brain_) return false;
    
    bool regionsExist = !brain_->getRegions().empty();
    bool synapsesExist = false;
    for (const auto& region : brain_->getRegions()) {
        if (!region->getSynapses().empty()) {
            synapsesExist = true;
            break;
        }
    }
    
    return regionsExist && synapsesExist;
}

} // namespace nlm
