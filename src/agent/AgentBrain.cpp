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
                        // Distribute motor neurons to different action groups with improved logic
                        // Use a more balanced distribution algorithm
                        std::array<std::vector<Neuron*>&, 6> motorGroups = {
                            motorForward_, motorBackward_, motorTurnLeft_, 
                            motorTurnRight_, motorInteract_, motorWait_
                        };
                        
                        // Find the group with the fewest neurons for better balance
                        size_t minSize = motorForward_.size();
                        size_t targetGroup = 0;
                        for (size_t i = 1; i < 6; ++i) {
                            if (motorGroups[i].size() < minSize) {
                                minSize = motorGroups[i].size();
                                targetGroup = i;
                            }
                        }
                        
                        motorGroups[targetGroup].push_back(n);
                    }
                } else if (type == NeuronType::Sensory) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Distribute sensory neurons with better balancing
                        std::array<std::vector<Neuron*>&, 4> sensoryGroups = {
                            sensoryVision_, sensoryTouch_, sensoryInternal_, 
                            sensoryProprioception_
                        };
                        
                        // Target the group with the fewest neurons
                        size_t minSize = sensoryVision_.size();
                        size_t targetGroup = 0;
                        for (size_t i = 1; i < 4; ++i) {
                            if (sensoryGroups[i].size() < minSize) {
                                minSize = sensoryGroups[i].size();
                                targetGroup = i;
                            }
                        }
                        
                        sensoryGroups[targetGroup].push_back(n);
                    }
                }
            }
        }
        
        // Validate that we have meaningful distribution
        NLM_LOG_DEBUG("AgentBrain initialized with " +
                     std::to_string(motorForward_.size()) + " forward, " +
                     std::to_string(motorBackward_.size()) + " backward, " +
                     std::to_string(motorTurnLeft_.size()) + " left, " +
                     std::to_string(motorTurnRight_.size()) + " right, " +
                     std::to_string(motorInteract_.size()) + " interact, " +
                     std::to_string(motorWait_.size()) + " wait motor neurons");
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    if (!brain_) {
        NLM_LOG_ERROR("Cannot initialize AgentBrain: brain pointer is null");
        return;
    }
    
    // Validate world configuration
    size_t expectedVisionSize = world.getVisionWidth() * world.getVisionHeight();
    if (expectedVisionSize == 0) {
        NLM_LOG_ERROR("Cannot initialize AgentBrain: invalid vision dimensions in world");
        return;
    }
    
    previousVision_.resize(expectedVisionSize, 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Log initialization details with comprehensive information
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons, " +
                 std::to_string(sensoryProprioception_.size()) + " proprioception sensory neurons");
    
    NLM_LOG_INFO("World configuration: " + std::to_string(world.getVisionWidth()) + "x" + 
                 std::to_string(world.getVisionHeight()) + " vision, " +
                 std::to_string(world.getMaxEnergy()) + " max energy");
    
    // Validate neural population distribution
    size_t totalMotorNeurons = motorForward_.size() + motorBackward_.size() + 
                              motorTurnLeft_.size() + motorTurnRight_.size() +
                              motorInteract_.size() + motorWait_.size();
    size_t totalSensoryNeurons = sensoryVision_.size() + sensoryTouch_.size() +
                                sensoryInternal_.size() + sensoryProprioception_.size();
    
    if (totalMotorNeurons == 0) {
        NLM_LOG_WARNING("No motor neurons were assigned during initialization");
    }
    
    if (totalSensoryNeurons == 0) {
        NLM_LOG_WARNING("No sensory neurons were assigned during initialization");
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
    if (!brain_) {
        NLM_LOG_ERROR("Cannot process sensory input: brain pointer is null");
        return;
    }
    
    const auto& vision = percept.getVision();
    const auto& touch = percept.getTouch();
    const auto& intern = percept.getInternal();
    const auto& proprio = percept.getProprioception();
    const auto& audio = percept.getAudio();
    
    // Validate sensory data dimensions
    if (!vision.empty()) {
        // Check against our own previous vision storage, not against world
        if (vision.size() != previousVision_.size()) {
            NLM_LOG_WARNING("Vision data size mismatch: expected " + 
                           std::to_string(previousVision_.size()) + " got " + 
                           std::to_string(vision.size()) + ". Adjusting previous vision storage.")
            previousVision_.resize(vision.size(), 0.0f);
        }
    }
    
    // Process vision input with bounds checking and safety
    if (!vision.empty() && !sensoryVision_.empty()) {
        size_t minVision = std::min(vision.size(), sensoryVision_.size());
        for (size_t i = 0; i < minVision; ++i) {
            if (sensoryVision_[i]) {
                // Improved scaling with better range and non-linearity
                float intensity = vision[i];
                // Apply logarithmic scaling for better dynamic range
                intensity = intensity > 0.0f ? std::log(1.0f + intensity * 10.0f) / std::log(11.0f) : 0.0f;
                float current = intensity * 5.0f;  // Scale factor
                current = std::clamp(current, 0.0f, 50.0f); // Limit maximum current
                sensoryVision_[i]->injectCurrent(current);
            }
        }
    }
    
    // Process touch input with improved scaling
    if (!touch.empty() && !sensoryTouch_.empty()) {
        size_t minTouch = std::min(touch.size(), sensoryTouch_.size());
        for (size_t i = 0; i < minTouch; ++i) {
            if (sensoryTouch_[i]) {
                // Improved collision signal with better range
                float collision = touch[i];
                collision = std::min(collision, 1.0f); // Cap at 1.0
                float current = collision * 8.0f;  // Collision signal
                current = std::clamp(current, 0.0f, 20.0f); // Limit maximum current
                sensoryTouch_[i]->injectCurrent(current);
            }
        }
    }
    
    // Process internal signals with better centering
    if (!intern.empty() && !sensoryInternal_.empty()) {
        size_t minIntern = std::min(intern.size(), sensoryInternal_.size());
        for (size_t i = 0; i < minIntern; ++i) {
            if (sensoryInternal_[i]) {
                // Improved centering and scaling
                float internal = intern[i];
                // Map from [-1, 1] to [-5, 5] range with better scaling
                float current = (internal * 5.0f) * 1.0f;  // Center and scale
                current = std::clamp(current, -10.0f, 10.0f); // Reasonable limits
                sensoryInternal_[i]->injectCurrent(current);
            }
        }
    }
    
    // Process proprioception with improved scaling
    if (!proprio.empty() && !sensoryProprioception_.empty()) {
        size_t minProprio = std::min(proprio.size(), sensoryProprioception_.size());
        for (size_t i = 0; i < minProprio; ++i) {
            if (sensoryProprioception_[i]) {
                float proprioception = proprio[i];
                float current = (proprioception * 3.0f - 1.0f) * 3.0f;  // Center and scale
                current = std::clamp(current, -5.0f, 5.0f); // Reasonable limits
                sensoryProprioception_[i]->injectCurrent(current);
            }
        }
    }
    
    // Compute novelty with improved algorithm
    if (!vision.empty()) {
        float totalDiff = 0.0f;
        size_t comparisonSize = std::min(vision.size(), previousVision_.size());
        
        for (size_t i = 0; i < comparisonSize; ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            // Apply logarithmic scaling to emphasize small changes
            totalDiff += std::log(1.0f + diff * 5.0f);
        }
        
        // Normalize with maximum possible difference
        float maxPossibleDiff = comparisonSize * std::log(1.0f + 5.0f);
        noveltyLevel_ = totalDiff / std::max(maxPossibleDiff, 1.0f);
        
        // Apply decay with configurable rate
        noveltyLevel_ *= sensoryNoveltyDecay_;
        
        // Store for next time with bounds checking
        previousVision_.resize(vision.size(), 0.0f);
        std::copy(vision.begin(), vision.begin() + std::min(vision.size(), previousVision_.size()), 
                  previousVision_.begin());
    }
    
    // Update curiosity with improved formula and bounds
    if (curiosityEnabled_) {
        // Enhanced curiosity formula combining novelty, prediction error, and exploration drive
        float noveltyFactor = noveltyLevel_ * 2.0f;
        float predictionFactor = std::abs(predictionError_) * 0.5f;
        float explorationFactor = 0.1f; // Base exploration drive
        
        curiosityLevel_ = noveltyFactor + predictionFactor + explorationFactor;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
        
        // Apply smoothing for stability
        if (curiosityLevel_ > 0.0f) {
            curiosityLevel_ = 0.9f * curiosityLevel_ + 0.1f * 0.5f; // Bias toward moderate curiosity
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
    if (!brain_ || !rewardModulationEnabled_) {
        if (!brain_) {
            NLM_LOG_ERROR("Cannot apply reward modulation: brain pointer is null");
        }
        return;
    }
    
    // Validate input ranges
    reward = std::clamp(reward, -10.0f, 10.0f);
    predictedReward = std::clamp(predictedReward, -10.0f, 10.0f);
    
    // Compute prediction error with clipping
    predictionError_ = reward - predictedReward;
    predictionError_ = std::clamp(predictionError_, -1.0f, 1.0f);
    
    // Update expected reward (exponential moving average) with stability
    expectedReward_ = 0.9f * expectedReward_ + 0.1f * reward;
    expectedReward_ = std::clamp(expectedReward_, -1.0f, 1.0f);
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply to all synapses with eligibility traces
    if (brain_->getRegions().size() > 0) {
        for (const auto& region : brain_->getRegions()) {
            for (auto* syn : region->getSynapses()) {
                float eligibility = syn->getEligibilityTrace();
                
                // Only process synapses with meaningful eligibility traces
                if (std::abs(eligibility) > 0.001f) {
                    // Apply reward-modulated weight change with stability
                    float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                    // Clamp delta to prevent excessive changes
                    delta = std::clamp(delta, -0.1f, 0.1f);
                    syn->addToWeight(delta);
                    
                    // Decay eligibility trace with stability
                    syn->decayEligibilityTrace(0.1f);
                }
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
        
        // Add stability bounds to STDP weights
        float ltp = stdp->getLTPWeight();
        float ltd = stdp->getLTDWeight();
        stdp->setLTPWeight(std::clamp(ltp, 0.001f, 0.05f));
        stdp->setLTDWeight(std::clamp(ltd, 0.001f, 0.06f));
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
