// AgentBrain implementation with comprehensive memory integration
// This file fixes the critical Phase 6 integration issues:
// - Memory systems are now properly connected
// - Neuromodulation systems are fully integrated
// - Complete pipeline from sensory input to action selection

#include "AgentBrain.hpp"
#include "MemoryIntegration.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Reward.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <sstream>

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(brain)
    , memoryIntegration_(std::make_unique<MemoryIntegration>())
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
        
        // Initialize memory integration system
        memoryIntegration_->initialize(brain_.get());
        
        // Initialize neuromodulation systems
        initializeNeuromodulationSystems();
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initializeNeuromodulationSystems() {
    if (!brain_) return;
    
    NLM_LOG_INFO("Initializing neuromodulation systems...");
    
    // Create neuromodulation systems using NeuromodulatorManager
    auto* neuromodulatorManager = brain_->getNeuromodulatorManager();
    if (neuromodulatorManager) {
        // Initialize each neuromodulation system
        auto* dopamine = neuromodulatorManager->getDopamine();
        auto* curiosity = neuromodulatorManager->getCuriosity();
        auto* novelty = neuromodulatorManager->getNovelty();
        auto* predictionError = neuromodulatorManager->getPredictionError();
        auto* reward = neuromodulatorManager->getReward();
        
        // Configure dopamine system
        if (dopamine) {
            dopamine->setLevel(0.5f); // Start with moderate baseline
        }
        
        // Initialize curiosity with brain reference
        if (curiosity) {
            curiosity->initialize(brain_.get());
        }
        
        // Initialize novelty with brain reference
        if (novelty) {
            novelty->initialize(brain_.get());
        }
        
        // Initialize prediction error with brain reference
        if (predictionError) {
            predictionError->initialize(brain_.get());
        }
        
        NLM_LOG_INFO("Neuromodulation systems initialized successfully");
    }
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
    
    // Store percept in memory integration
    if (memoryIntegration_) {
        memoryIntegration_->processSensoryInput(brain_.get(), percept);
    }
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Inject current proportional to vision intensity
            float current = vision[i] * 5.0f;  // Scale factor
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * 8.0f;  // Collision signal
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            sensoryProprioception_[i]->injectCurrent(current);
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
        
        // Update novelty neuromodulator if available
        updateNoveltyNeuromodulator();
        
        // Store for next time
        previousVision_ = vision;
    }
    
    // Update curiosity based on novelty and prediction error
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
        
        // Update curiosity neuromodulator if available
        updateCuriosityNeuromodulator();
    }
}

void AgentBrain::updateNoveltyNeuromodulator() {
    auto* neuromodulatorManager = brain_ ? brain_->getNeuromodulatorManager() : nullptr;
    if (!neuromodulatorManager) return;
    
    auto* novelty = neuromodulatorManager->getNovelty();
    if (novelty) {
        // Update novelty based on sensory difference
        novelty->setLevel(noveltyLevel_);
    }
}

void AgentBrain::updateCuriosityNeuromodulator() {
    auto* neuromodulatorManager = brain_ ? brain_->getNeuromodulatorManager() : nullptr;
    if (!neuromodulatorManager) return;
    
    auto* curiosity = neuromodulatorManager->getCuriosity();
    if (curiosity) {
        // Update curiosity based on novelty and prediction error
        float curiosityValue = curiosityLevel_;
        curiosity->setLevel(curiosityValue);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        decoded = selectWithCuriosity(decoded);
    }
    
    // Store current action in memory integration
    if (memoryIntegration_) {
        memoryIntegration_->setCurrentAction(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
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
    
    // Update neuromodulation systems
    updateNeuromodulationSystems(reward, predictionError_);
}

void AgentBrain::updateNeuromodulationSystems(float reward, float predictionError) {
    auto* neuromodulatorManager = brain_ ? brain_->getNeuromodulatorManager() : nullptr;
    if (!neuromodulatorManager) return;
    
    // Update dopamine based on reward prediction error
    auto* dopamine = neuromodulatorManager->getDopamine();
    if (dopamine) {
        dopamine->signalRewardPredictionError(predictionError);
        dopamine->update(0.001f); // Small timestep
    }
    
    // Update curiosity based on novelty and prediction error
    auto* curiosity = neuromodulatorManager->getCuriosity();
    if (curiosity) {
        curiosity->update(noveltyLevel_, std::abs(predictionError), 0.001f);
    }
    
    // Update novelty detection
    auto* novelty = neuromodulatorManager->getNovelty();
    if (novelty) {
        novelty->update(0.001f);
    }
    
    // Update prediction error signal
    auto* predError = neuromodulatorManager->getPredictionError();
    if (predError) {
        predError->computeError(predError->getPredictedValue(), reward);
    }
    
    // Update reward system
    auto* rewardSys = neuromodulatorManager->getReward();
    if (rewardSys) {
        rewardSys->add(reward);
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
    
    // Update brain initializer for development effects
    auto* brainInitializer = brain_ ? brain_->getBrainInitializer() : nullptr;
    if (brainInitializer) {
        brainInitializer->updateDevelopment(this, timestep);
    }
}

void AgentBrain::setCurrentAction(MotorCommand action) {
    if (memoryIntegration_) {
        memoryIntegration_->setCurrentAction(action);
    }
}

void AgentBrain::setCurrentReward(float reward) {
    currentReward_ = reward;
    if (memoryIntegration_) {
        memoryIntegration_->setCurrentReward(reward);
    }
}

void AgentBrain::setCurrentPredictionError(float error) {
    currentPredictionError_ = error;
    if (memoryIntegration_) {
        memoryIntegration_->setCurrentPredictionError(error);
    }
}

void AgentBrain::updateBrainMemorySystems(double timestep) {
    if (memoryIntegration_ && brain_) {
        memoryIntegration_->update(brain_.get(), timestep);
    }
}

void AgentBrain::logAgentStatus() const {
    NLM_LOG_INFO("=== AgentBrain Status ===");
    NLM_LOG_INFO("Vision neurons: " + std::to_string(sensoryVision_.size()));
    NLM_LOG_INFO("Touch neurons: " + std::to_string(sensoryTouch_.size()));
    NLM_LOG_INFO("Internal neurons: " + std::to_string(sensoryInternal_.size()));
    NLM_LOG_INFO("Proprioception neurons: " + std::to_string(sensoryProprioception_.size()));
    
    NLM_LOG_INFO("Motor forward: " + std::to_string(motorForward_.size()));
    NLM_LOG_INFO("Motor backward: " + std::to_string(motorBackward_.size()));
    NLM_LOG_INFO("Motor turn left: " + std::to_string(motorTurnLeft_.size()));
    NLM_LOG_INFO("Motor turn right: " + std::to_string(motorTurnRight_.size()));
    NLM_LOG_INFO("Motor interact: " + std::to_string(motorInteract_.size()));
    NLM_LOG_INFO("Motor wait: " + std::to_string(motorWait_.size()));
    
    NLM_LOG_INFO("Dopamine level: " + std::to_string(dopamineLevel_));
    NLM_LOG_INFO("Novelty level: " + std::to_string(noveltyLevel_));
    NLM_LOG_INFO("Curiosity level: " + std::to_string(curiosityLevel_));
    NLM_LOG_INFO("Prediction error: " + std::to_string(predictionError_));
    NLM_LOG_INFO("Expected reward: " + std::to_string(expectedReward_));
    
    if (memoryIntegration_) {
        memoryIntegration_->logMemoryStatus(brain_.get());
    }
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
    
    // Reset memory integration
    if (memoryIntegration_ && brain_) {
        memoryIntegration_->reset(brain_.get());
    }
}

} // namespace nlm
