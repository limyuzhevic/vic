// Refactored AgentBrain.cpp with bug fixes and improvements

#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>
#include <memory>
#include <vector>
#include <stdexcept>
#include "AgentBrainCommon.hpp"

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(std::move(brain))
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
    if (brain_) {
        initializeNeuronGroups(motorForward_, motorBackward_, motorTurnLeft_, 
                              motorTurnRight_, motorInteract_, motorWait_,
                              sensoryVision_, sensoryTouch_, sensoryInternal_, 
                              sensoryProprioception_, brain_.get());
    } else {
        NLM_LOG_WARNING("AgentBrain initialized with null brain pointer");
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    // Validate inputs
    if (!brain_) {
        NLM_LOG_ERROR("Cannot initialize AgentBrain: brain pointer is null");
        return;
    }
    
    // Safely resize previous vision with bounds checking
    size_t visionSize = world.getVisionWidth() * world.getVisionHeight();
    previousVision_.resize(visionSize, 0.0f);
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
    if (!validateBrainPtr(brain_, "processSensoryInput")) {
        return;
    }
    
    // Vision input processing with bounds checking
    const auto& vision = percept.getVision();
    size_t minVisionSize = std::min(sensoryVision_.size(), vision.size());
    
    for (size_t i = 0; i < minVisionSize; ++i) {
        Neuron* neuron = getNeuronSafely(sensoryVision_, i);
        if (neuron) {
            // Inject current proportional to vision intensity
            float current = vision[i] * 5.0f;  // Scale factor
            neuron->injectCurrent(current);
        }
    }
    
    // Touch input processing
    const auto& touch = percept.getTouch();
    size_t minTouchSize = std::min(sensoryTouch_.size(), touch.size());
    
    for (size_t i = 0; i < minTouchSize; ++i) {
        Neuron* neuron = getNeuronSafely(sensoryTouch_, i);
        if (neuron) {
            float current = touch[i] * 8.0f;  // Collision signal
            neuron->injectCurrent(current);
        }
    }
    
    // Internal signals processing
    const auto& intern = percept.getInternal();
    size_t minInternalSize = std::min(sensoryInternal_.size(), intern.size());
    
    for (size_t i = 0; i < minInternalSize; ++i) {
        Neuron* neuron = getNeuronSafely(sensoryInternal_, i);
        if (neuron) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            neuron->injectCurrent(current);
        }
    }
    
    // Proprioception processing
    const auto& proprio = percept.getProprioception();
    size_t minProprioSize = std::min(sensoryProprioception_.size(), proprio.size());
    
    for (size_t i = 0; i < minProprioSize; ++i) {
        Neuron* neuron = getNeuronSafely(sensoryProprioception_, i);
        if (neuron) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            neuron->injectCurrent(current);
        }
    }
    
    // Compute novelty (difference from previous vision) with safe bounds checking
    if (!vision.empty()) {
        float totalDiff = 0.0f;
        size_t visionLimit = std::min(vision.size(), previousVision_.size());
        
        for (size_t i = 0; i < visionLimit; ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Normalize
        noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
        
        // Decay and update
        noveltyLevel_ *= sensoryNoveltyDecay_;
        
        // Store for next time (safely)
        size_t copyLimit = std::min(vision.size(), previousVision_.size());
        for (size_t i = 0; i < copyLimit; ++i) {
            previousVision_[i] = vision[i];
        }
    }
    
    // Update curiosity based on novelty (safe calculation)
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!validateBrainPtr(brain_, "decodeMotorCommand")) {
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
    // Calculate average activity in each motor group using helper function
    float forwardAct = calculateGroupActivity(motorForward_);
    float backwardAct = calculateGroupActivity(motorBackward_);
    float leftAct = calculateGroupActivity(motorTurnLeft_);
    float rightAct = calculateGroupActivity(motorTurnRight_);
    float interactAct = calculateGroupActivity(motorInteract_);
    float waitAct = calculateGroupActivity(motorWait_);
    
    // Find maximum activity using safe structure
    struct CommandActivity {
        MotorCommand cmd;
        float activity;
    };
    
    CommandActivity commands[] = {
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
    
    // Only act if there's meaningful activity (threshold)
    if (bestActivity < 0.5f) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    if (!validateBrainPtr(brain_, "selectWithCuriosity")) {
        return defaultCmd;
    }
    
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityEnabled_ && curiosityLevel_ > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * 0.3f;  // Up to 30% random
        
        auto* rng = brain_->getRandomGenerator();
        if (rng) {
            float r = rng->uniformReal(0.0f, 1.0f);
            if (r < exploreChance) {
                // Random motor command selection - use complete ActionType enum
                const std::vector<MotorCommand> validCommands = {
                    MotorCommand::MoveForward,
                    MotorCommand::MoveBackward,
                    MotorCommand::TurnLeft,
                    MotorCommand::TurnRight,
                    MotorCommand::LookLeft,
                    MotorCommand::LookRight,
                    MotorCommand::Interact,
                    MotorCommand::Wait
                };
                
                MotorCommand randomCmd = exploreWithCuriosity(rng, 1.0f, validCommands);
                if (randomCmd != MotorCommand::Wait) {
                    return randomCmd;
                }
            }
        }
    }
    
    return defaultCmd;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!validateBrainPtr(brain_, "applyRewardModulation") || !rewardModulationEnabled_) {
        return;
    }
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    dopamineLevel_ = clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply to all synapses with eligibility traces (safe access)
    auto* rng = brain_->getRandomGenerator();
    if (!rng) {
        NLM_LOG_ERROR("Random generator is null during reward modulation");
        return;
    }
    
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
    plasticityFactor = clamp(plasticityFactor, 0.1f, 2.0f);
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!validateBrainPtr(brain_, "updateDevelopment") || !developmentEnabled_) {
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
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

} // namespace nlm
