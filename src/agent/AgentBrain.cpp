#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

/**
 * Motor command configuration implementation
 */
void AgentBrain::applyMotorConfiguration() {
    // Clear existing groups
    motorForward_.clear();
    motorBackward_.clear();
    motorTurnLeft_.clear();
    motorTurnRight_.clear();
    motorInteract_.clear();
    motorWait_.clear();
    
    if (!brain_) return;
    
    // Get all motor neurons
    std::vector<Neuron*> allMotorNeurons;
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Motor) {
                for (Neuron* n : pop->getNeurons()) {
                    allMotorNeurons.push_back(n);
                }
            }
        }
    }
    
    if (allMotorNeurons.empty()) {
        NLM_LOG_WARN("No motor neurons found for configuration");
        return;
    }
    
    // Apply motor configuration
    size_t motorCount = allMotorNeurons.size();
    size_t actionCount = motorConfig_.numMotorActions;
    
    if (actionCount == 0) {
        NLM_LOG_WARN("No motor actions configured, using default");
        actionCount = 6; // Default
    }
    
    // Check if custom mapping is provided
    if (!motorConfig_.customActionMapping.empty() && 
        motorConfig_.customActionMapping.size() >= motorCount) {
        
        // Use custom mapping
        for (size_t i = 0; i < motorCount; ++i) {
            int actionIndex = motorConfig_.customActionMapping[i];
            if (actionIndex >= 0 && actionIndex < static_cast<int>(actionCount)) {
                if (actionIndex < 6) { // Default actions
                    switch (actionIndex) {
                        case 0:
                            if (motorConfig_.enableForward) motorForward_.push_back(allMotorNeurons[i]);
                            break;
                        case 1:
                            if (motorConfig_.enableBackward) motorBackward_.push_back(allMotorNeurons[i]);
                            break;
                        case 2:
                            if (motorConfig_.enableTurnLeft) motorTurnLeft_.push_back(allMotorNeurons[i]);
                            break;
                        case 3:
                            if (motorConfig_.enableTurnRight) motorTurnRight_.push_back(allMotorNeurons[i]);
                            break;
                        case 4:
                            if (motorConfig_.enableInteract) motorInteract_.push_back(allMotorNeurons[i]);
                            break;
                        case 5:
                            if (motorConfig_.enableWait) motorWait_.push_back(allMotorNeurons[i]);
                            break;
                    }
                }
            }
        }
    } else {
        // Use distribution-based allocation
        std::vector<size_t> actionCounts(actionCount, 0);
        
        // For each motor neuron, decide which action group it belongs to
        // based on action ratios and random assignment for distribution
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        
        for (size_t i = 0; i < motorCount; ++i) {
            float r = dist(gen);
            float cumulative = 0.0f;
            size_t selectedAction = 0;
            
            // Find action based on cumulative distribution
            for (size_t j = 0; j < actionCount; ++j) {
                if (j < motorConfig_.actionRatios.size()) {
                    cumulative += motorConfig_.actionRatios[j];
                    if (r <= cumulative) {
                        selectedAction = j;
                        break;
                    }
                } else {
                    cumulative += 1.0f / actionCount;
                    if (r <= cumulative) {
                        selectedAction = j;
                        break;
                    }
                }
            }
            
            actionCounts[selectedAction]++;
        }
        
        // Allocate neurons to action groups based on counts
        size_t currentNeuron = 0;
        for (size_t action = 0; action < actionCount; ++action) {
            size_t count = actionCounts[action];
            
            // Map action index to default action types (first 6 actions)
            if (action < 6) {
                switch (action) {
                    case 0: // Forward
                        if (motorConfig_.enableForward) {
                            for (size_t i = 0; i < count && currentNeuron < motorCount; ++i) {
                                motorForward_.push_back(allMotorNeurons[currentNeuron++]);
                            }
                        }
                        break;
                    case 1: // Backward
                        if (motorConfig_.enableBackward) {
                            for (size_t i = 0; i < count && currentNeuron < motorCount; ++i) {
                                motorBackward_.push_back(allMotorNeurons[currentNeuron++]);
                            }
                        }
                        break;
                    case 2: // TurnLeft
                        if (motorConfig_.enableTurnLeft) {
                            for (size_t i = 0; i < count && currentNeuron < motorCount; ++i) {
                                motorTurnLeft_.push_back(allMotorNeurons[currentNeuron++]);
                            }
                        }
                        break;
                    case 3: // TurnRight
                        if (motorConfig_.enableTurnRight) {
                            for (size_t i = 0; i < count && currentNeuron < motorCount; ++i) {
                                motorTurnRight_.push_back(allMotorNeurons[currentNeuron++]);
                            }
                        }
                        break;
                    case 4: // Interact
                        if (motorConfig_.enableInteract) {
                            for (size_t i = 0; i < count && currentNeuron < motorCount; ++i) {
                                motorInteract_.push_back(allMotorNeurons[currentNeuron++]);
                            }
                        }
                        break;
                    case 5: // Wait
                        if (motorConfig_.enableWait) {
                            for (size_t i = 0; i < count && currentNeuron < motorCount; ++i) {
                                motorWait_.push_back(allMotorNeurons[currentNeuron++]);
                            }
                        }
                        break;
                }
            } else {
                // Additional actions beyond the standard 6
                // Create new generic action groups
                std::string actionName = "Action" + std::to_string(action);
                // Note: Additional actions beyond the standard 6 aren't implemented
                // as they would require extending the MotorCommand enum
            }
        }
    }
    
    NLM_LOG_INFO("Motor configuration applied:");
    NLM_LOG_INFO("  Total motor neurons: " + std::to_string(motorCount));
    NLM_LOG_INFO("  Motor action groups: " + std::to_string(actionCount));
    NLM_LOG_INFO("  Forward: " + std::to_string(motorForward_.size()));
    NLM_LOG_INFO("  Backward: " + std::to_string(motorBackward_.size()));
    NLM_LOG_INFO("  TurnLeft: " + std::to_string(motorTurnLeft_.size()));
    NLM_LOG_INFO("  TurnRight: " + std::to_string(motorTurnRight_.size()));
    NLM_LOG_INFO("  Interact: " + std::to_string(motorInteract_.size()));
    NLM_LOG_INFO("  Wait: " + std::to_string(motorWait_.size()));
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
{
    // Initialize with default motor configuration
    applyMotorConfiguration();
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
