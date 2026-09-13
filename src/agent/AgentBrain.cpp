#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

namespace nlm {

// Constants for scaling and thresholds
namespace AgentBrainConstants {
    // Sensory scaling factors
    constexpr float VISION_SCALE = 5.0f;
    constexpr float TOUCH_SCALE = 8.0f;
    constexpr float INTERNAL_SCALE = 5.0f;
    constexpr float PROPRIOCEPTION_SCALE = 3.0f;
    
    // Activity and exploration thresholds
    constexpr float MIN_ACTIVITY_THRESHOLD = 0.5f;
    constexpr float CURIOUSITY_EXPLORATION_THRESHOLD = 0.3f;
    constexpr float HIGH_CURIOUSITY_THRESHOLD = 0.5f;
    constexpr float EXPLORATION_CHANCE_MULTIPLIER = 0.3f;
    
    // Neuromodulation ranges
    constexpr float DOPAMINE_CLAMP_MIN = -1.0f;
    constexpr float DOPAMINE_CLAMP_MAX = 1.0f;
    constexpr float NEUROTIC_DECAY_FACTOR = 0.99f;
    
    // Development stages
    constexpr double INITIAL_STAGE_TIME = 60.0;       // ~1 minute
    constexpr double CRITICAL_PERIOD_TIME = 300.0;   // ~5 minutes
    constexpr double MATURATION_TIME = 900.0;        // ~15 minutes
    constexpr float ADULT_PLASTICITY_MODIFIER = 0.2f;
    
    // Memory system parameters
    constexpr float ELIGIBILITY_TRACE_THRESHOLD = 0.001f;
    constexpr float ELIGIBILITY_TRACE_DECAY = 0.1f;
    constexpr float PLASTICITY_BASE_FACTOR = 0.5f;
    constexpr float PLASTICITY_MIN_FACTOR = 0.1f;
    constexpr float PLASTICITY_MAX_FACTOR = 2.0f;
    
    // STDP parameters
    constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.01f;
    constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.012f;
    constexpr float STRUCTURAL_SYNAPTOGENESIS_RATE = 0.0001f;
    constexpr float STRUCTURAL_PRUNING_RATE = 0.00001f;
    
    // Motor command distribution
    constexpr int MOTOR_COMMAND_COUNT = 6;           // Forward, Backward, Left, Right, Interact, Wait
    constexpr int LOOK_COMMANDS_START = 4;           // LookLeft, LookRight (for curiosity exploration)
    constexpr int LOOK_COMMANDS_COUNT = 2;
    constexpr int MAX_RANDOM_CHOICE = 7;              // 0-7 inclusive for curiosity exploration
}

// Helper function to calculate distribution index safely
static size_t calculateDistributionIndex(const std::vector<Neuron*>& neurons, 
                                       size_t accumulatedSize) {
    // Check for potential overflow
    if (accumulatedSize > std::numeric_limits<size_t>::max() / AgentBrainConstants::MOTOR_COMMAND_COUNT) {
        NLM_LOG_WARNING("Distribution index overflow detected in AgentBrain neuron distribution");
        return 0;
    }
    return accumulatedSize % AgentBrainConstants::MOTOR_COMMAND_COUNT;
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
    , sensoryNoveltyDecay_(AgentBrainConstants::NEUROTIC_DECAY_FACTOR)
{
    // Initialize motor and sensory neuron groups
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Distribute motor neurons to different action groups
                        size_t idx = calculateDistributionIndex(motorForward_, 
                                                              motorBackward_.size() + 
                                                              motorTurnLeft_.size() + 
                                                              motorTurnRight_.size() + 
                                                              motorInteract_.size() + 
                                                              motorWait_.size());
                        
                        switch (idx) {
                            case 0: motorForward_.push_back(n); break;
                            case 1: motorBackward_.push_back(n); break;
                            case 2: motorTurnLeft_.push_back(n); break;
                            case 3: motorTurnRight_.push_back(n); break;
                            case 4: motorInteract_.push_back(n); break;
                            case 5: motorWait_.push_back(n); break;
                            default: 
                                NLM_LOG_WARNING("Unexpected motor distribution index: " + 
                                               std::to_string(idx));
                                break;
                        }
                    }
                } else if (type == NeuronType::Sensory) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Distribute sensory neurons
                        size_t idx = calculateDistributionIndex(sensoryVision_,
                                                              sensoryTouch_.size() + 
                                                              sensoryInternal_.size() + 
                                                              sensoryProprioception_.size());
                        
                        switch (idx) {
                            case 0: sensoryVision_.push_back(n); break;
                            case 1: sensoryTouch_.push_back(n); break;
                            case 2: sensoryInternal_.push_back(n); break;
                            case 3: sensoryProprioception_.push_back(n); break;
                            default: 
                                NLM_LOG_WARNING("Unexpected sensory distribution index: " + 
                                               std::to_string(idx));
                                break;
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
    
    // Debug logging for initialization
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons, " +
                 std::to_string(sensoryProprioception_.size()) + " proprioception sensory neurons");
    
    NLM_LOG_INFO("Initialized with " + std::to_string(motorForward_.size()) + " forward, " +
                 std::to_string(motorBackward_.size()) + " backward, " +
                 std::to_string(motorTurnLeft_.size()) + " left, " +
                 std::to_string(motorTurnRight_.size()) + " right, " +
                 std::to_string(motorInteract_.size()) + " interact, " +
                 std::to_string(motorWait_.size()) + " wait motor neurons");
}

size_t AgentBrain::getSensoryInputSize() const {
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
    return 256 + 8 + 4 + 6;
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action
    return 6;
}

size_t AgentBrain::getSensoryInputSize() const {
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
    return constants_.visionScale * constants_.visionScale +  // vision
           constants_.touchScale * 1.0f +                     // touch
           constants_.internalScale * 1.0f +                  // internal
           constants_.proprioceptionScale * 1.0f;            // proprioception
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) {
        NLM_LOG_WARNING("Cannot process sensory input: Brain is not initialized");
        return;
    }
    
    // Validate percept data
    const auto& vision = percept.getVision();
    if (vision.empty()) {
        NLM_LOG_WARNING("Received empty vision percept - check world configuration");
    }
    
    // Vision input processing
    size_t minVisionSize = std::min(sensoryVision_.size(), vision.size());
    if (minVisionSize > 0) {
        NLM_LOG_DEBUG("Processing " + std::to_string(minVisionSize) + " vision values " +
                     "(" + std::to_string(sensoryVision_.size()) + " neurons)");
        
        for (size_t i = 0; i < minVisionSize; ++i) {
            if (sensoryVision_[i]) {
                float current = vision[i] * constants_.visionScale;
                sensoryVision_[i]->injectCurrent(current);
            }
        }
    }
    
    // Touch input processing
    const auto& touch = percept.getTouch();
    size_t minTouchSize = std::min(sensoryTouch_.size(), touch.size());
    if (minTouchSize > 0) {
        for (size_t i = 0; i < minTouchSize; ++i) {
            if (sensoryTouch_[i]) {
                float current = touch[i] * constants_.touchScale;
                sensoryTouch_[i]->injectCurrent(current);
            }
        }
    }
    
    // Internal signals processing
    const auto& intern = percept.getInternal();
    size_t minInternalSize = std::min(sensoryInternal_.size(), intern.size());
    if (minInternalSize > 0) {
        for (size_t i = 0; i < minInternalSize; ++i) {
            if (sensoryInternal_[i]) {
                float current = (intern[i] * 2.0f - 1.0f) * constants_.internalScale;
                sensoryInternal_[i]->injectCurrent(current);
            }
        }
    }
    
    // Proprioception processing
    const auto& proprio = percept.getProprioception();
    size_t minProprioSize = std::min(sensoryProprioception_.size(), proprio.size());
    if (minProprioSize > 0) {
        for (size_t i = 0; i < minProprioSize; ++i) {
            if (sensoryProprioception_[i]) {
                float current = (proprio[i] * 2.0f - 1.0f) * constants_.proprioceptionScale;
                sensoryProprioception_[i]->injectCurrent(current);
            }
        }
    }
    
    // Compute novelty (difference from previous vision)
    if (!vision.empty()) {
        float totalDiff = 0.0f;
        size_t compareSize = std::min(vision.size(), previousVision_.size());
        for (size_t i = 0; i < compareSize; ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Normalize
        noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
        
        // Apply novelty decay
        noveltyLevel_ *= constants_.noveltyDecayFactor;
        
        // Store for next time
        if (previousVision_.size() < vision.size()) {
            previousVision_.resize(vision.size(), 0.0f);
        }
        previousVision_ = vision;
        
        // Debug log for significant novelty
        if (noveltyLevel_ > 0.1f) {
            NLM_LOG_DEBUG("High novelty detected: " + std::to_string(noveltyLevel_));
        }
    }
    
    // Update curiosity based on novelty and prediction error
    if (curiosityEnabled_) {
        // Use std::abs since predictionError_ can be negative
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
        
        if (curiosityLevel_ > constants_.highCuriosityThreshold) {
            NLM_LOG_DEBUG("High curiosity level: " + std::to_string(curiosityLevel_));
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
