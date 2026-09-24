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
    // Initialize neuron groups with null safety
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
    
    if (brain_) {
        NLM_LOG_INFO("Initializing AgentBrain with " + std::to_string(brain_->getTotalNeuronCount()) + " neurons");
        
        // Initialize motor and sensory neuron groups with safety checks
        for (const auto& region : brain_->getRegions()) {
            if (!region) continue;
                
            for (auto& pop : region->getPopulations()) {
                if (!pop) continue;
                    
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        if (!n) continue;
                        
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
                        if (!n) continue;
                        
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
        
        NLM_LOG_INFO("AgentBrain setup complete: " +
                    std::to_string(motorForward_.size()) + " forward, " +
                    std::to_string(motorBackward_.size()) + " backward, " +
                    std::to_string(motorTurnLeft_.size()) + " left, " +
                    std::to_string(motorTurnRight_.size()) + " right, " +
                    std::to_string(motorInteract_.size()) + " interact, " +
                    std::to_string(motorWait_.size()) + " wait; " +
                    std::to_string(sensoryVision_.size()) + " vision, " +
                    std::to_string(sensoryTouch_.size()) + " touch, " +
                    std::to_string(sensoryInternal_.size()) + " internal, " +
                    std::to_string(sensoryProprioception_.size()) + " proprioception sensory neurons");
    } else {
        NLM_LOG_ERROR("AgentBrain initialized with null brain pointer");
    }
}

AgentBrain::~AgentBrain() = default;

static float calculateNeuronActivity(const std::vector<Neuron*>& neurons) {
    if (neurons.empty()) return 0.0f;
    float sum = 0.0f;
    for (const Neuron* n : neurons) {
        if (!n) continue;
        // Use membrane potential deviation from rest as activity measure
        sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
    }
    return sum / neurons.size();
}

static float calculateCuriosityLevel(float novelty, float predictionError) {
    return std::clamp(novelty * 2.0f + std::abs(predictionError) * 0.5f, 0.0f, 1.0f);
}

static float calculatePlasticityFactor(float dopamineLevel) {
    return std::clamp(0.5f + 0.5f * dopamineLevel, 0.1f, 2.0f);
}

size_t AgentBrain::getSensoryInputSize() const {
    if (!brain_) {
        NLM_LOG_WARNING("Cannot get sensory input size: brain is null");
        return 0;
    }
    
    // Calculate expected input sizes based on actual neuron group sizes
    // Vision input expected by the world (standard size)
    size_t visionSize = 256;  // 16x16 vision grid
    
    // Touch input expected by the world (standard size) 
    size_t touchSize = 8;    // Number of touch sensors
    
    // Internal signals expected by the brain (standard size)
    size_t internalSize = 4; // Number of internal state signals
    
    // Proprioception expected by the brain (standard size)
    size_t proprioceptionSize = 6; // Number of proprioception signals
    
    return visionSize + touchSize + internalSize + proprioceptionSize;
}

size_t AgentBrain::getMotorOutputSize() const {
    if (!brain_) {
        NLM_LOG_WARNING("Cannot get motor output size: brain is null");
        return 0;
    }
    
    // Calculate expected motor output size based on actual motor neuron groups
    // Each motor neuron group produces one motor command
    size_t motorGroups = 6;  // forward, backward, left, right, interact, wait
    return motorGroups;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) {
        NLM_LOG_ERROR("Cannot process sensory input: brain is null");
        return;
    }
    
    // Check if we have any sensory neurons to process input
    bool hasSensoryNeurons = !sensoryVision_.empty() || !sensoryTouch_.empty() || 
                             !sensoryInternal_.empty() || !sensoryProprioception_.empty();
    if (!hasSensoryNeurons) {
        NLM_LOG_WARNING("Processing sensory input but no sensory neurons available");
        return;
    }
    
    // Process vision input
    processVisionInput(percept);
    
    // Process touch input
    processTouchInput(percept);
    
    // Process internal signals
    processInternalInput(percept);
    
    // Process proprioception
    processProprioceptionInput(percept);
    
    // Update novelty and curiosity
    updateNoveltyAndCuriosity(percept);
}

// Helper methods for sensory processing

void AgentBrain::processVisionInput(const SensoryPercept& percept) {
    const auto& vision = percept.getVision();
    if (vision.empty() || sensoryVision_.empty()) return;
    
    const float VISION_SCALE_FACTOR = 5.0f;
    size_t minSize = std::min(sensoryVision_.size(), vision.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        if (sensoryVision_[i] && sensoryVision_[i]->isFunctional()) {
            float current = vision[i] * VISION_SCALE_FACTOR;
            sensoryVision_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processTouchInput(const SensoryPercept& percept) {
    const auto& touch = percept.getTouch();
    if (touch.empty() || sensoryTouch_.empty()) return;
    
    const float TOUCH_SCALE_FACTOR = 8.0f;
    size_t minSize = std::min(sensoryTouch_.size(), touch.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        if (sensoryTouch_[i] && sensoryTouch_[i]->isFunctional()) {
            float current = touch[i] * TOUCH_SCALE_FACTOR;
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processInternalInput(const SensoryPercept& percept) {
    const auto& intern = percept.getInternal();
    if (intern.empty() || sensoryInternal_.empty()) return;
    
    const float INTERNAL_SCALE_FACTOR = 5.0f;
    const float INTERNAL_CENTER = 1.0f;
    size_t minSize = std::min(sensoryInternal_.size(), intern.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        if (sensoryInternal_[i] && sensoryInternal_[i]->isFunctional()) {
            float current = (intern[i] * 2.0f - INTERNAL_CENTER) * INTERNAL_SCALE_FACTOR;
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processProprioceptionInput(const SensoryPercept& percept) {
    const auto& proprio = percept.getProprioception();
    if (proprio.empty() || sensoryProprioception_.empty()) return;
    
    const float PROPRIOCEPTION_SCALE_FACTOR = 3.0f;
    const float PROPRIOCEPTION_CENTER = 1.0f;
    size_t minSize = std::min(sensoryProprioception_.size(), proprio.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        if (sensoryProprioception_[i] && sensoryProprioception_[i]->isFunctional()) {
            float current = (proprio[i] * 2.0f - PROPRIOCEPTION_CENTER) * PROPRIOCEPTION_SCALE_FACTOR;
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::updateNoveltyAndCuriosity(const SensoryPercept& percept) {
    const auto& vision = percept.getVision();
    if (vision.empty()) return;
    
    // Compute novelty (difference from previous vision)
    float totalDiff = 0.0f;
    size_t minSize = std::min(vision.size(), previousVision_.size());
    for (size_t i = 0; i < minSize; ++i) {
        float diff = std::abs(vision[i] - previousVision_[i]);
        totalDiff += diff;
    }
    
    // Normalize
    noveltyLevel_ = totalDiff / std::max<size_t>(minSize, 1);
    
    // Decay and update
    noveltyLevel_ *= sensoryNoveltyDecay_;
    
    // Store for next time
    previousVision_ = vision;
    
    // Update curiosity based on novelty
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) {
        NLM_LOG_ERROR("Cannot decode motor command: brain is null");
        return MotorCommand::Wait;
    }
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    if (!brain_) {
        NLM_LOG_ERROR("Cannot decode motor commands: brain is null");
        return MotorCommand::Wait;
    }
    
    // Calculate average activity in each motor group
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            if (!n) continue;
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
    if (bestActivity < 0.5f) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    if (!brain_) {
        NLM_LOG_ERROR("Cannot select with curiosity: brain is null");
        return defaultCmd;
    }
    
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * 0.3f;  // Up to 30% random
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command - ensure we don't exceed valid enum values
            int choice = brain_->getRandomGenerator()->uniformInt(0, 6);  // 0-6, 7 values
            switch (choice) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::TurnLeft;
                case 3: return MotorCommand::TurnRight;
                case 4: return MotorCommand::LookLeft;
                case 5: return MotorCommand::LookRight;
                case 6: return MotorCommand::Interact;
            }
        }
    }
    
    return defaultCmd;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) {
        if (!brain_) {
            NLM_LOG_ERROR("Cannot apply reward modulation: brain is null");
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
    
    NLM_LOG_DEBUG("Reward modulation: reward=" + std::to_string(reward) + 
                 " predicted=" + std::to_string(predictedReward) + 
                 " error=" + std::to_string(predictionError_) + 
                 " dopamine=" + std::to_string(dopamineLevel_));
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        if (!region) continue;
            
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
    
    NLM_LOG_DEBUG("Plasticity factor modulated to: " + std::to_string(plasticityFactor));
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) {
        if (!brain_) {
            NLM_LOG_ERROR("Cannot update development: brain is null");
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
    
    NLM_LOG_DEBUG("Development updated: age=" + std::to_string(developmentalAge_) + 
                 " stage=" + std::to_string(static_cast<int>(brain_->getDevelopmentalStage())) +
                 " plasticity=" + std::to_string(plasticityModifier_));
    
    // Structural plasticity changes with development
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // Higher synaptogenesis in early development
            float synRate = 0.0001f * plasticityModifier_;
            float pruneRate = 0.00001f * (2.0f - plasticityModifier_);
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
            
            NLM_LOG_DEBUG("Structural plasticity rates updated: synaptogenesis=" + std::to_string(synRate) +
                         " pruning=" + std::to_string(pruneRate));
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
