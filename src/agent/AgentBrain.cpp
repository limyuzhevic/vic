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
    // Initialize motor and sensory neuron groups with proper error handling
    if (brain_) {
        const auto& regions = brain_->getRegions();
        if (!regions.empty()) {
            for (const auto& region : regions) {
                if (!region) continue;
                
                const auto& populations = region->getPopulations();
                if (!populations.empty()) {
                    for (const auto& pop : populations) {
                        if (!pop) continue;
                        
                        NeuronType type = pop->getNeuronType();
                        
                        if (type == NeuronType::Motor) {
                            const auto& neurons = pop->getNeurons();
                            if (!neurons.empty()) {
                                for (Neuron* n : neurons) {
                                    if (!n) continue;  // Safety check
                                    
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
                            }
                        } else if (type == NeuronType::Sensory) {
                            const auto& neurons = pop->getNeurons();
                            if (!neurons.empty()) {
                                for (Neuron* n : neurons) {
                                    if (!n) continue;  // Safety check
                                    
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
        }
    }
    
    // Log initialization status
    NLM_LOG_INFO("AgentBrain initialized with " +
                 std::to_string(motorForward_.size() + motorBackward_.size() + motorTurnLeft_.size() +
                               motorTurnRight_.size() + motorInteract_.size() + motorWait_.size()) +
                 " motor neurons, " +
                 std::to_string(sensoryVision_.size() + sensoryTouch_.size() + 
                               sensoryInternal_.size() + sensoryProprioception_.size()) +
                 " sensory neurons");
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    // Validate input parameter
    // Note: SimpleWorld doesn't have an is_valid() method, so we'll check if the world is properly configured
    if (world.getWidth() <= 0 || world.getHeight() <= 0) {
        NLM_LOG_ERROR("AgentBrain::initialize: Invalid world dimensions provided");
        return;
    }
    
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Log initialization status with detailed information
    size_t totalMotorNeurons = motorForward_.size() + motorBackward_.size() + 
                              motorTurnLeft_.size() + motorTurnRight_.size() +
                              motorInteract_.size() + motorWait_.size();
    size_t totalSensoryNeurons = sensoryVision_.size() + sensoryTouch_.size() + 
                                sensoryInternal_.size() + sensoryProprioception_.size();
    
    NLM_LOG_INFO("AgentBrain initialized with " + std::to_string(totalMotorNeurons) + " motor neurons, " +
                 std::to_string(totalSensoryNeurons) + " sensory neurons, " +
                 std::to_string(sensoryVision_.size()) + " vision, " +
                 std::to_string(sensoryTouch_.size()) + " touch, " +
                 std::to_string(sensoryInternal_.size()) + " internal, " +
                 std::to_string(sensoryProprioception_.size()) + " proprioception neurons");
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
    // Validate input parameters
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain::processSensoryInput: No brain available");
        return;
    }
    
    if (!brain_->getBrain()) {
        NLM_LOG_ERROR("AgentBrain::processSensoryInput: Invalid brain object");
        return;
    }
    
    if (!percept.getVisionWidth() || !percept.getVisionHeight()) {
        NLM_LOG_ERROR("AgentBrain::processSensoryInput: Invalid vision dimensions");
        return;
    }
    
    // Log processing start if debug logging is enabled
    NLM_LOG_DEBUG("AgentBrain::processSensoryInput: Processing perceptual input");
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        Neuron* neuron = sensoryVision_[i];
        if (neuron && neuron->getState().firingState != FiringState::Refractory) {
            // Validate neuron is in a valid state for input
            if (neuron->getState().membranePotential > neuron->getState().threshold) {
                NLM_LOG_WARNING("AgentBrain::processSensoryInput: Neuron already above threshold, skipping input");
                continue;
            }
            
            // Inject current proportional to vision intensity
            float current = vision[i] * 5.0f;  // Scale factor
            if (std::abs(current) > 1000.0f) {  // Safety check for extreme values
                NLM_LOG_WARNING("AgentBrain::processSensoryInput: Extreme current value detected, clamping: " + 
                              std::to_string(current));
                current = std::clamp(current, -1000.0f, 1000.0f);
            }
            
            neuron->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        Neuron* neuron = sensoryTouch_[i];
        if (neuron && neuron->getState().firingState != FiringState::Refractory) {
            float current = touch[i] * 8.0f;  // Collision signal
            if (std::abs(current) > 100.0f) {
                NLM_LOG_WARNING("AgentBrain::processSensoryInput: Extreme touch current detected, clamping: " + 
                              std::to_string(current));
                current = std::clamp(current, -100.0f, 100.0f);
            }
            neuron->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        Neuron* neuron = sensoryInternal_[i];
        if (neuron && neuron->getState().firingState != FiringState::Refractory) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            if (std::abs(current) > 200.0f) {
                NLM_LOG_WARNING("AgentBrain::processSensoryInput: Extreme internal current detected, clamping: " + 
                              std::to_string(current));
                current = std::clamp(current, -200.0f, 200.0f);
            }
            neuron->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        Neuron* neuron = sensoryProprioception_[i];
        if (neuron && neuron->getState().firingState != FiringState::Refractory) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            if (std::abs(current) > 150.0f) {
                NLM_LOG_WARNING("AgentBrain::processSensoryInput: Extreme proprioception current detected, clamping: " + 
                              std::to_string(current));
                current = std::clamp(current, -150.0f, 150.0f);
            }
            neuron->injectCurrent(current);
        }
    }
    
    // Compute novelty (difference from previous vision)
    if (!vision.empty() && !vision.empty()) {
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
        
        // Log novelty if it's significant
        if (noveltyLevel_ > 0.1f) {
            NLM_LOG_INFO("AgentBrain::processSensoryInput: High novelty detected: " + std::to_string(noveltyLevel_));
        }
    }
    
    // Update curiosity based on novelty and prediction error
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
    
    NLM_LOG_DEBUG("AgentBrain::processSensoryInput: Processing completed");
}

MotorCommand AgentBrain::decodeMotorCommand() {
    // Validate brain availability
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain::decodeMotorCommand: No brain available");
        return MotorCommand::Wait;
    }
    
    if (!brain_->getBrain()) {
        NLM_LOG_ERROR("AgentBrain::decodeMotorCommand: Invalid brain object");
        return MotorCommand::Wait;
    }
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration with validation
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        // Validate curiosity level before using it
        if (curiosityLevel_ < 0.0f || curiosityLevel_ > 1.0f) {
            NLM_LOG_WARNING("AgentBrain::decodeMotorCommand: Invalid curiosity level, clamping: " + 
                          std::to_string(curiosityLevel_));
            curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
        }
        
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
    // Validate state before curiosity-based exploration
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain::selectWithCuriosity: No brain available");
        return defaultCmd;
    }
    
    if (!brain_->getBrain()) {
        NLM_LOG_ERROR("AgentBrain::selectWithCuriosity: Invalid brain object");
        return defaultCmd;
    }
    
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > 0.5f) {
        // Validate curiosity level
        if (curiosityLevel_ < 0.0f || curiosityLevel_ > 1.0f) {
            NLM_LOG_WARNING("AgentBrain::selectWithCuriosity: Invalid curiosity level, clamping: " + 
                          std::to_string(curiosityLevel_));
            curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
        }
        
        // Calculate explore chance with bounds checking
        float exploreChance = curiosityLevel_ * 0.3f;  // Up to 30% random
        exploreChance = std::clamp(exploreChance, 0.0f, 1.0f);
        
        // Get random generator safely
        auto* rng = brain_->getRandomGenerator();
        if (!rng) {
            NLM_LOG_ERROR("AgentBrain::selectWithCuriosity: No random generator available");
            return defaultCmd;
        }
        
        float r = rng->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command
            int choice = rng->uniformInt(0, 7);
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
    // Validate input parameters
    if (!brain_ || !rewardModulationEnabled_) {
        return;
    }
    
    if (!brain_->getBrain()) {
        NLM_LOG_ERROR("AgentBrain::applyRewardModulation: Invalid brain object");
        return;
    }
    
    // Validate reward values
    if (std::isnan(reward) || std::isinf(reward)) {
        NLM_LOG_WARNING("AgentBrain::applyRewardModulation: Invalid reward value, ignoring: " + 
                      std::to_string(reward));
        return;
    }
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Validate prediction error
    if (std::isnan(predictionError_) || std::isinf(predictionError_)) {
        NLM_LOG_WARNING("AgentBrain::applyRewardModulation: Invalid prediction error, clamping: " + 
                      std::to_string(predictionError_));
        predictionError_ = std::clamp(predictionError_, -1.0f, 1.0f);
    }
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Validate expected reward
    if (std::isnan(expectedReward_) || std::isinf(expectedReward_)) {
        NLM_LOG_WARNING("AgentBrain::applyRewardModulation: Invalid expected reward, resetting: " + 
                      std::to_string(expectedReward_));
        expectedReward_ = 0.0f;
    }
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply to all synapses with eligibility traces
    const auto& regions = brain_->getRegions();
    if (!regions.empty()) {
        for (const auto& region : regions) {
            if (!region) continue;
            
            const auto& synapses = region->getSynapses();
            if (!synapses.empty()) {
                for (auto* syn : synapses) {
                    if (!syn) continue;
                    
                    float eligibility = syn->getEligibilityTrace();
                    
                    if (std::abs(eligibility) > 0.001f) {
                        // Apply reward-modulated weight change
                        float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                        
                        // Validate delta
                        if (std::isnan(delta) || std::isinf(delta)) {
                            NLM_LOG_WARNING("AgentBrain::applyRewardModulation: Invalid delta, skipping: " + 
                                          std::to_string(delta));
                            continue;
                        }
                        
                        syn->addToWeight(delta);
                        
                        // Decay eligibility trace
                        syn->decayEligibilityTrace(0.1f);
                    }
                }
            }
        }
    }
    
    // Modulate plasticity based on dopamine
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    // Apply to STDP with validation
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        float ltpWeight = 0.01f * plasticityFactor;
        float ltdWeight = 0.012f * plasticityFactor;
        
        // Validate STDP parameters
        if (std::isnan(ltpWeight) || std::isinf(ltpWeight) || 
            std::isnan(ltdWeight) || std::isinf(ltdWeight)) {
            NLM_LOG_WARNING("AgentBrain::applyRewardModulation: Invalid STDP parameters, skipping update");
        } else {
            stdp->setLTPWeight(ltpWeight);
            stdp->setLTDWeight(ltdWeight);
        }
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    // Validate input parameters
    if (!brain_ || !developmentEnabled_) {
        return;
    }
    
    if (timestep < 0.0) {
        NLM_LOG_WARNING("AgentBrain::updateDevelopment: Negative timestep, using absolute value: " + 
                      std::to_string(timestep));
        timestep = std::abs(timestep);
    }
    
    if (std::isnan(timestep) || std::isinf(timestep)) {
        NLM_LOG_WARNING("AgentBrain::updateDevelopment: Invalid timestep, skipping: " + 
                      std::to_string(timestep));
        return;
    }
    
    developmentalAge_ += timestep;
    
    // Validate developmental age
    if (developmentalAge_ < 0.0) {
        NLM_LOG_WARNING("AgentBrain::updateDevelopment: Negative developmental age, resetting: " + 
                      std::to_string(developmentalAge_));
        developmentalAge_ = 0.0;
    }
    
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
    
    // Validate plasticity modifier
    if (plasticityModifier_ < 0.0f || plasticityModifier_ > 2.0f) {
        NLM_LOG_WARNING("AgentBrain::updateDevelopment: Invalid plasticity modifier, clamping: " + 
                      std::to_string(plasticityModifier_));
        plasticityModifier_ = std::clamp(plasticityModifier_, 0.1f, 2.0f);
    }
    
    // Structural plasticity changes with development
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // Higher synaptogenesis in early development
            float synRate = 0.0001f * plasticityModifier_;
            float pruneRate = 0.00001f * (2.0f - plasticityModifier_);
            
            // Validate structural plasticity rates
            if (synRate < 0.0f || synRate > 0.01f || pruneRate < 0.0f || pruneRate > 0.001f) {
                NLM_LOG_WARNING("AgentBrain::updateDevelopment: Invalid structural plasticity rates, using defaults");
                synRate = 0.0001f;
                pruneRate = 0.00001f;
            }
            
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
    }
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    // Validate brain availability
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain::getDevelopmentalStage: No brain available");
        return DevelopmentalStage::Initial;
    }
    
    if (!brain_->getBrain()) {
        NLM_LOG_ERROR("AgentBrain::getDevelopmentalStage: Invalid brain object");
        return DevelopmentalStage::Initial;
    }
    
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
    // Reset all internal state variables
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Clear previous vision data
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    
    // Log reset operation
    NLM_LOG_INFO("AgentBrain::reset: Complete reset of agent state performed");
}

} // namespace nlm
