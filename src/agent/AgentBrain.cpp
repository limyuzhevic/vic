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
                        // Distribute motor neurons to different action groups with bounds checking
                        size_t groupIndex = 0;
                        if (motorForward_.size() > groupIndex) {
                            motorForward_.push_back(n);
                            ++groupIndex;
                        } else if (motorBackward_.size() > groupIndex) {
                            motorBackward_.push_back(n);
                            ++groupIndex;
                        } else if (motorTurnLeft_.size() > groupIndex) {
                            motorTurnLeft_.push_back(n);
                            ++groupIndex;
                        } else if (motorTurnRight_.size() > groupIndex) {
                            motorTurnRight_.push_back(n);
                            ++groupIndex;
                        } else if (motorInteract_.size() > groupIndex) {
                            motorInteract_.push_back(n);
                            ++groupIndex;
                        } else {
                            motorWait_.push_back(n);
                        }
                    }
                } else if (type == NeuronType::Sensory) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Distribute sensory neurons with bounds checking
                        size_t groupIndex = 0;
                        if (sensoryVision_.size() > groupIndex) {
                            sensoryVision_.push_back(n);
                            ++groupIndex;
                        } else if (sensoryTouch_.size() > groupIndex) {
                            sensoryTouch_.push_back(n);
                            ++groupIndex;
                        } else if (sensoryInternal_.size() > groupIndex) {
                            sensoryInternal_.push_back(n);
                            ++groupIndex;
                        } else {
                            sensoryProprioception_.push_back(n);
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
    // Calculate average activity in each motor group with safety checks
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            if (!n) continue;  // Safety check
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
            // Random motor command selection (compatible with MotorCommand enum)
            int choice = brain_->getRandomGenerator()->uniformInt(0, 6);  // 0-6 for Wait, MoveForward, MoveBackward, TurnLeft, TurnRight, Interact
            switch (choice) {
                case 0: return MotorCommand::Wait;
                case 1: return MotorCommand::MoveForward;
                case 2: return MotorCommand::MoveBackward;
                case 3: return MotorCommand::TurnLeft;
                case 4: return MotorCommand::TurnRight;
                case 5: return MotorCommand::Interact;
                default: return MotorCommand::Wait;  // Should never happen
            }
        }
    }
    
    return defaultCmd;
}

// Apply neuromodulatory signals to brain plasticity
    void applyRewardModulation(float reward, float predictedReward) {
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
    curiosityThreshold_ = 0.5f;
    explorationRate_ = 0.1f;
    developmentSpeed_ = 1.0f;
    developmentPaused_ = false;
    
    // Clear neuromodulation events
    neuromodulationEvents_.clear();
}

// ========== ADVANCED FEATURE IMPLEMENTATIONS ==========

void AgentBrain::setCuriosityThreshold(float threshold) {
    curiosityThreshold_ = std::clamp(threshold, 0.0f, 1.0f);
}

void AgentBrain::setExplorationRate(float rate) {
    explorationRate_ = std::clamp(rate, 0.0f, 1.0f);
}

void AgentBrain::applyCustomNeuromodulation(float level, float duration) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Apply custom neuromodulation signal
    dopamineLevel_ = level;
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Schedule event if duration > 0
    if (duration > 0.0f) {
        NeuromodulationEvent event;
        event.time = developmentalAge_ + duration;
        event.level = dopamineLevel_;
        event.type = NeuromodulatorType::Dopamine;
        event.duration = duration;
        neuromodulationEvents_.push_back(event);
    }
}

void AgentBrain::scheduleNeuromodulationEvent(float time, float level, NeuromodulatorType type) {
    NeuromodulationEvent event;
    event.time = time;
    event.level = level;
    event.type = type;
    event.duration = 0.0f;
    neuromodulationEvents_.push_back(event);
}

float AgentBrain::getMemoryUsage() const {
    if (!brain_) return 0.0f;
    
    float memory = static_cast<float>(brain_->getTotalNeuronCount()) * 32.0f;  // ~32 bytes per neuron
    memory += static_cast<float>(brain_->getTotalSynapseCount()) * 24.0f;      // ~24 bytes per synapse
    memory += static_cast<float>(sensoryVision_.size()) * 8.0f;               // Pointer storage
    memory += static_cast<float>(motorForward_.size()) * 8.0f;
    memory += static_cast<float>(previousVision_.size()) * 4.0f;             // Float storage
    
    return memory;
}

float AgentBrain::getFiringRate() const {
    if (!brain_) return 0.0f;
    
    size_t totalNeurons = brain_->getTotalNeuronCount();
    if (totalNeurons == 0) return 0.0f;
    
    return static_cast<float>(brain_->getFiringNeuronCount()) / totalNeurons;
}

float AgentBrain::getLearningRate() const {
    // Calculate an approximation of current learning rate based on neuromodulation
    float learningRate = 0.0f;
    
    if (rewardModulationEnabled_) {
        learningRate += std::abs(dopamineLevel_) * 0.1f;
    }
    
    if (structuralPlasticityEnabled_) {
        learningRate += 0.05f;
    }
    
    if (developmentEnabled_) {
        learningRate += plasticityModifier_ * 0.05f;
    }
    
    if (curiosityEnabled_) {
        learningRate += curiosityLevel_ * 0.1f;
    }
    
    return std::min(learningRate, 1.0f);
}

size_t AgentBrain::getActiveNeuronCount() const {
    if (!brain_) return 0;
    return brain_->getActiveNeuronCount();
}

size_t AgentBrain::getTotalSynapseCount() const {
    if (!brain_) return 0;
    return brain_->getTotalSynapseCount();
}

std::vector<float> AgentBrain::analyzeSensoryInput() const {
    std::vector<float> analysis;
    
    if (previousVision_.empty()) {
        return analysis;
    }
    
    // Calculate basic statistics of vision input
    float sum = 0.0f;
    float min = 1.0f;
    float max = 0.0f;
    float variance = 0.0f;
    
    for (float val : previousVision_) {
        sum += val;
        min = std::min(min, val);
        max = std::max(max, val);
    }
    
    size_t count = previousVision_.size();
    float mean = count > 0 ? sum / count : 0.0f;
    
    for (float val : previousVision_) {
        float diff = val - mean;
        variance += diff * diff;
    }
    float stdDev = count > 0 ? std::sqrt(variance / count) : 0.0f;
    
    // Add statistics to analysis
    analysis.push_back(mean);      // Mean brightness
    analysis.push_back(min);       // Minimum brightness
    analysis.push_back(max);       // Maximum brightness
    analysis.push_back(stdDev);     // Standard deviation (texture)
    analysis.push_back(noveltyLevel_); // Novelty
    analysis.push_back(curiosityLevel_); // Curiosity
    
    return analysis;
}

MotorCommand AgentBrain::predictNextAction() const {
    // Simple prediction based on current motor neuron activity
    if (!brain_) return MotorCommand::Wait;
    
    auto* region = brain_->getRegion(RegionId(1));
    if (!region) return MotorCommand::Wait;
    
    // Check which motor neuron group has the highest activity
    float bestActivity = 0.0f;
    MotorCommand bestCommand = MotorCommand::Wait;
    
    for (size_t i = 0; i < motorForward_.size(); ++i) {
        if (motorForward_[i]) {
            float activity = std::abs(motorForward_[i]->getState().membranePotential - 
                                    motorForward_[i]->getState().restingPotential);
            if (activity > bestActivity) {
                bestActivity = activity;
                bestCommand = MotorCommand::MoveForward;
            }
        }
    }
    
    for (size_t i = 0; i < motorBackward_.size(); ++i) {
        if (motorBackward_[i]) {
            float activity = std::abs(motorBackward_[i]->getState().membranePotential - 
                                    motorBackward_[i]->getState().restingPotential);
            if (activity > bestActivity) {
                bestActivity = activity;
                bestCommand = MotorCommand::MoveBackward;
            }
        }
    }
    
    for (size_t i = 0; i < motorTurnLeft_.size(); ++i) {
        if (motorTurnLeft_[i]) {
            float activity = std::abs(motorTurnLeft_[i]->getState().membranePotential - 
                                    motorTurnLeft_[i]->getState().restingPotential);
            if (activity > bestActivity) {
                bestActivity = activity;
                bestCommand = MotorCommand::TurnLeft;
            }
        }
    }
    
    for (size_t i = 0; i < motorTurnRight_.size(); ++i) {
        if (motorTurnRight_[i]) {
            float activity = std::abs(motorTurnRight_[i]->getState().membranePotential - 
                                    motorTurnRight_[i]->getState().restingPotential);
            if (activity > bestActivity) {
                bestActivity = activity;
                bestCommand = MotorCommand::TurnRight;
            }
        }
    }
    
    for (size_t i = 0; i < motorInteract_.size(); ++i) {
        if (motorInteract_[i]) {
            float activity = std::abs(motorInteract_[i]->getState().membranePotential - 
                                    motorInteract_[i]->getState().restingPotential);
            if (activity > bestActivity) {
                bestActivity = activity;
                bestCommand = MotorCommand::Interact;
            }
        }
    }
    
    return bestCommand;
}

void AgentBrain::setDevelopmentSpeed(float speed) {
    developmentSpeed_ = std::max(0.1f, speed);
}

void AgentBrain::accelerateDevelopment(float factor) {
    developmentSpeed_ *= factor;
}

void AgentBrain::pauseDevelopment(bool pause) {
    developmentPaused_ = pause;
}

} // namespace nlm
