#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>

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
    , curiosityThreshold_(0.3f)
    , explorationRate_(0.1f)
    , noveltyDecayRate_(0.99f)
    , visionScale_(5.0f)
    , touchScale_(8.0f)
    , internalScale_(5.0f)
    , proprioceptionScale_(3.0f)
    , motorDecodingMode_(0)
    , simulationTime_(0.0)
    , advancedLearningEnabled_(false)
    , selfRepairEnabled_(false)
    , metaLearningEnabled_(false)
    , plasticityRate_(1.0f)
    , memoryConsolidationRate_(0.5f)
    , attentionStrength_(0.5f)
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
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    simulationTime_ = 0.0;
    
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
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Inject current proportional to vision intensity with scaling
            float current = vision[i] * visionScale_;
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * touchScale_;
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * internalScale_;
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * proprioceptionScale_;
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
        
        // Apply decay
        noveltyLevel_ *= sensoryNoveltyDecay_;
        
        // Store for next time
        previousVision_ = vision;
    }
    
    // Update curiosity based on novelty
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
    
    // Advanced learning updates
    if (advancedLearningEnabled_) {
        // Meta-learning: track how curiosity affects learning
        if (predictionError_ > 0.1f && curiosityLevel_ < curiosityThreshold_) {
            // Agent needs more exploration to learn
            explorationRate_ = std::min(0.5f, explorationRate_ * 1.1f);
        }
        
        // Self-repair: detect and compensate for performance issues
        if (getAverageFiringRate() < 0.5f) {
            if (selfRepairEnabled_) {
                // Inject recovery current to restore activity
                brain_->injectCurrentToNeurons(NeuronType::Sensory, 20.0f);
            }
        }
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > curiosityThreshold_) {
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
    
    // Find maximum activity based on decoding mode
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct;
    
    switch (motorDecodingMode_) {
        case 1: // Weighted
        {
            struct { MotorCommand cmd; float activity; } commands[] = {
                {MotorCommand::MoveForward, forwardAct * 1.2f},
                {MotorCommand::MoveBackward, backwardAct * 1.2f},
                {MotorCommand::TurnLeft, leftAct * 1.0f},
                {MotorCommand::TurnRight, rightAct * 1.0f},
                {MotorCommand::Interact, interactAct * 1.5f},
                {MotorCommand::Wait, waitAct * 0.5f}
            };
            for (const auto& c : commands) {
                if (c.activity > bestActivity) {
                    bestActivity = c.activity;
                    best = c.cmd;
                }
            }
            break;
        }
        case 2: // Fuzzy logic (combine multiple factors)
        {
            // Higher threshold for fuzzy logic
            if (std::max({forwardAct, backwardAct, leftAct, rightAct, interactAct}) > 0.3f) {
                // Use activity differences to resolve ties
                if (forwardAct > backwardAct && forwardAct > interactAct) {
                    best = MotorCommand::MoveForward;
                    bestActivity = forwardAct;
                } else if (backwardAct > leftAct && backwardAct > rightAct) {
                    best = MotorCommand::MoveBackward;
                    bestActivity = backwardAct;
                } else if (leftAct > rightAct) {
                    best = MotorCommand::TurnLeft;
                    bestActivity = leftAct;
                } else if (rightAct > leftAct) {
                    best = MotorCommand::TurnRight;
                    bestActivity = rightAct;
                } else if (interactAct > 0.2f) {
                    best = MotorCommand::Interact;
                    bestActivity = interactAct;
                }
            }
            break;
        }
        default: // Threshold (original mode)
        {
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
            break;
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
        float exploreChance = curiosityLevel_ * explorationRate_;
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command with weighted preference for interesting actions
            float r2 = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
            if (r2 < 0.3f) {
                // Prefer exploration over waiting
                return MotorCommand::MoveForward;
            } else if (r2 < 0.6f) {
                return MotorCommand::TurnLeft;
            } else {
                return MotorCommand::Interact;
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
                // Apply reward-modulated weight change with plasticity rate
                float delta = eligibility * dopamineLevel_ * plasticityModifier_ * plasticityRate_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
    
    // Modulate plasticity based on dopamine
    if (advancedLearningEnabled_) {
        // Positive dopamine increases plasticity, negative decreases
        float plasticityFactor = 0.5f + 0.5f * dopamineLevel_;
        plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
        
        // Apply to STDP with meta-learning adjustments
        auto* stdp = brain_->getSTDP();
        if (stdp) {
            stdp->setLTPWeight(0.01f * plasticityFactor);
            stdp->setLTDWeight(0.012f * plasticityFactor);
            
            // Meta-learning: adjust based on learning progress
            if (metaLearningEnabled_) {
                float learningProgress = getLearningProgress();
                if (learningProgress < 0.3f) {
                    // More exploration needed
                    stdp->setLTPWeight(0.015f * plasticityFactor);
                } else if (learningProgress > 0.7f) {
                    // Fine-tune existing connections
                    stdp->setLTPWeight(0.008f * plasticityFactor);
                }
            }
        }
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    simulationTime_ += timestep;
    developmentalAge_ += timestep;
    
    // Simple developmental stages based on age
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
            float synRate = 0.0001f * plasticityModifier_ * plasticityRate_;
            float pruneRate = 0.00001f * (2.0f - plasticityModifier_) * (2.0f - plasticityModifier_);
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
            
            // Memory consolidation rate
            sp->setConsolidationRate(memoryConsolidationRate_);
        }
    }
    
    // Attention modulation based on development
    if (advancedLearningEnabled_) {
        auto* attention = brain_->getAttentionSystem();
        if (attention) {
            // Attention strength increases with development
            float attentionFactor = 0.5f + 0.5f * std::min(1.0f, developmentalAge_ / 600.0f);
            attention->setStrength(attentionStrength_ * attentionFactor);
        }
    }
}

bool AgentBrain::isHealthy() const {
    if (!brain_) return false;
    
    // Check multiple health metrics
    float firingRate = getAverageFiringRate();
    float actionDiversity = getActionDiversity();
    float survivalScore = getSurvivalScore();
    
    // Health criteria: reasonable activity, some diversity, basic survival
    bool healthy = (firingRate > 0.1f && firingRate < 10.0f &&
                   actionDiversity > 0.1f && actionDiversity < 0.9f &&
                   survivalScore > 0.5f);
    
    return healthy;
}

std::string AgentBrain::getStatus() const {
    std::stringstream ss;
    ss << "=== AgentBrain Status ===" << std::endl;
    ss << "Developmental Stage: " << static_cast<int>(getDevelopmentalStage()) << std::endl;
    ss << "Average Firing Rate: " << getAverageFiringRate() << " Hz" << std::endl;
    ss << "Curiosity Level: " << curiosityLevel_ << std::endl;
    ss << "Novelty Level: " << noveltyLevel_ << std::endl;
    ss << "Prediction Error: " << predictionError_ << std::endl;
    ss << "Neuromodulation Level: " << dopamineLevel_ << std::endl;
    ss << "Plasticity Rate: " << plasticityRate_ << std::endl;
    ss << "Advanced Learning: " << (advancedLearningEnabled_ ? "Enabled" : "Disabled") << std::endl;
    ss << "Self Repair: " << (selfRepairEnabled_ ? "Enabled" : "Disabled") << std::endl;
    ss << "Meta Learning: " << (metaLearningEnabled_ ? "Enabled" : "Disabled") << std::endl;
    ss << "Healthy: " << (isHealthy() ? "Yes" : "No") << std::endl;
    ss << "====================================" << std::endl;
    return ss.str();
}

double AgentBrain::getSimulationTime() const {
    return simulationTime_;
}

float AgentBrain::getAverageFiringRate() const {
    if (!brain_) return 0.0f;
    
    float totalSpikes = brain_->getTotalSpikeCount();
    double time = brain_->getSimulationClock()->getCurrentTime();
    if (time <= 0.0) return 0.0f;
    
    // Rough estimate in Hz
    return totalSpikes / static_cast<float>(time * 1000.0);  // Convert ms to s
}

float AgentBrain::getSensoryActivations(const std::string& modality) const {
    if (modality == "vision") {
        float total = 0.0f;
        for (const auto* n : sensoryVision_) {
            if (n) total += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
        return sensoryVision_.empty() ? 0.0f : total / sensoryVision_.size();
    } else if (modality == "touch") {
        float total = 0.0f;
        for (const auto* n : sensoryTouch_) {
            if (n) total += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
        return sensoryTouch_.empty() ? 0.0f : total / sensoryTouch_.size();
    } else if (modality == "internal") {
        float total = 0.0f;
        for (const auto* n : sensoryInternal_) {
            if (n) total += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
        return sensoryInternal_.empty() ? 0.0f : total / sensoryInternal_.size();
    } else if (modality == "proprioception") {
        float total = 0.0f;
        for (const auto* n : sensoryProprioception_) {
            if (n) total += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
        return sensoryProprioception_.empty() ? 0.0f : total / sensoryProprioception_.size();
    }
    return 0.0f;
}

float AgentBrain::getMotorCommandProbabilities(const std::string& command) const {
    // Calculate how often this command would be selected
    if (command == "forward") {
        return decodeFromMotorNeurons() == MotorCommand::MoveForward ? 1.0f : 0.0f;
    } else if (command == "backward") {
        return decodeFromMotorNeurons() == MotorCommand::MoveBackward ? 1.0f : 0.0f;
    } else if (command == "left") {
        return decodeFromMotorNeurons() == MotorCommand::TurnLeft ? 1.0f : 0.0f;
    } else if (command == "right") {
        return decodeFromMotorNeurons() == MotorCommand::TurnRight ? 1.0f : 0.0f;
    } else if (command == "interact") {
        return decodeFromMotorNeurons() == MotorCommand::Interact ? 1.0f : 0.0f;
    } else {
        return decodeFromMotorNeurons() == MotorCommand::Wait ? 1.0f : 0.0f;
    }
}

void AgentBrain::forceAction(MotorCommand command) {
    // Inject strong current into motor neurons of the desired command
    if (brain_) {
        if (command == MotorCommand::MoveForward && !motorForward_.empty()) {
            for (Neuron* n : motorForward_) {
                n->injectCurrent(100.0f);  // Strong force
            }
        } else if (command == MotorCommand::MoveBackward && !motorBackward_.empty()) {
            for (Neuron* n : motorBackward_) {
                n->injectCurrent(100.0f);
            }
        } else if (command == MotorCommand::TurnLeft && !motorTurnLeft_.empty()) {
            for (Neuron* n : motorTurnLeft_) {
                n->injectCurrent(100.0f);
            }
        } else if (command == MotorCommand::TurnRight && !motorTurnRight_.empty()) {
            for (Neuron* n : motorTurnRight_) {
                n->injectCurrent(100.0f);
            }
        } else if (command == MotorCommand::Interact && !motorInteract_.empty()) {
            for (Neuron* n : motorInteract_) {
                n->injectCurrent(100.0f);
            }
        }
    }
}

void AgentBrain::addSensoryPattern(const std::string& patternName, const std::vector<float>& pattern) {
    // Store pattern for recall
    storedPatterns_[patternName] = pattern;
    NLM_LOG_INFO("Added sensory pattern: " + patternName);
}

void AgentBrain::clearSensoryMemory() {
    storedPatterns_.clear();
    previousVision_.clear();
    NLM_LOG_INFO("Cleared sensory memory");
}

void AgentBrain::setCuriosityThreshold(float threshold) {
    curiosityThreshold_ = std::clamp(threshold, 0.0f, 1.0f);
}

void AgentBrain::setExplorationRate(float rate) {
    explorationRate_ = std::clamp(rate, 0.0f, 1.0f);
}

void AgentBrain::setNoveltyDecayRate(float decay) {
    sensoryNoveltyDecay_ = std::clamp(decay, 0.0f, 1.0f);
}

void AgentBrain::setSensoryScaling(float vision, float touch, float internal, float proprio) {
    visionScale_ = std::max(0.0f, vision);
    touchScale_ = std::max(0.0f, touch);
    internalScale_ = std::max(0.0f, internal);
    proprioceptionScale_ = std::max(0.0f, proprio);
}

void AgentBrain::setMotorDecodingMode(int mode) {
    motorDecodingMode_ = std::clamp(mode, 0, 2);
}

void AgentBrain::setPlasticityRate(float rate) {
    plasticityRate_ = std::max(0.0f, rate);
}

void AgentBrain::setMemoryConsolidationRate(float rate) {
    memoryConsolidationRate_ = std::clamp(rate, 0.0f, 1.0f);
}

void AgentBrain::setAttentionStrength(float strength) {
    attentionStrength_ = std::clamp(strength, 0.0f, 1.0f);
}

float AgentBrain::getActionDiversity() const {
    // Count how many different actions the agent has taken recently
    // This is a simplified version - in practice you'd need to track action history
    // For now, return based on current neuromodulation state
    return dopamineLevel_ > 0.0f ? 0.5f : 0.2f;
}

float AgentBrain::getLearningProgress() const {
    // Simple learning progress metric based on plasticity and development
    return std::min(1.0f, developmentalAge_ / 1000.0f);
}

float AgentBrain::getSurvivalScore() const {
    // Simplified survival score based on health and activity
    float healthScore = isHealthy() ? 1.0f : 0.0f;
    float activityScore = getAverageFiringRate() > 0.1f ? 1.0f : 0.0f;
    return (healthScore + activityScore) / 2.0f;
}

// Private helper to get attention system (placeholder)
AttentionSystem* AgentBrain::getAttentionSystem() {
    // This would need to be implemented in Brain class
    return nullptr;
}

} // namespace nlm
