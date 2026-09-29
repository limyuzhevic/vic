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
    , sensoryNoveltyDecay_(AgentBrainConfig::NOVELTY_DECAY_RATE)
{
    // Initialize motor and sensory neuron groups
    if (brain_) {
        // Pre-calculate distribution counts for efficiency
        size_t motorCount = 0, visionCount = 0, touchCount = 0, internalCount = 0, proprioCount = 0;
        
        // First pass: count neurons by type
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                if (type == NeuronType::Motor) {
                    motorCount += pop->getNeurons().size();
                } else if (type == NeuronType::Sensory) {
                    visionCount += pop->getNeurons().size();
                    touchCount += pop->getNeurons().size();
                    internalCount += pop->getNeurons().size();
                    proprioCount += pop->getNeurons().size();
                }
            }
        }
        
        // Reserve memory to avoid reallocations
        motorForward_.reserve((motorCount + AgentBrainConfig::MOTOR_GROUP_SIZE - 1) / 
                               AgentBrainConfig::MOTOR_GROUP_SIZE);
        motorBackward_.reserve((motorCount + AgentBrainConfig::MOTOR_GROUP_SIZE - 1) / 
                                AgentBrainConfig::MOTOR_GROUP_SIZE);
        motorTurnLeft_.reserve((motorCount + AgentBrainConfig::MOTOR_GROUP_SIZE - 1) / 
                               AgentBrainConfig::MOTOR_GROUP_SIZE);
        motorTurnRight_.reserve((motorCount + AgentBrainConfig::MOTOR_GROUP_SIZE - 1) / 
                                AgentBrainConfig::MOTOR_GROUP_SIZE);
        motorInteract_.reserve((motorCount + AgentBrainConfig::MOTOR_GROUP_SIZE - 1) / 
                               AgentBrainConfig::MOTOR_GROUP_SIZE);
        motorWait_.reserve((motorCount + AgentBrainConfig::MOTOR_GROUP_SIZE - 1) / 
                           AgentBrainConfig::MOTOR_GROUP_SIZE);
        
        sensoryVision_.reserve((visionCount + AgentBrainConfig::SENSOR_GROUP_SIZE_VISION - 1) / 
                               AgentBrainConfig::SENSOR_GROUP_SIZE_VISION);
        sensoryTouch_.reserve((touchCount + AgentBrainConfig::SENSOR_GROUP_SIZE_TOUCH - 1) / 
                              AgentBrainConfig::SENSOR_GROUP_SIZE_TOUCH);
        sensoryInternal_.reserve((internalCount + AgentBrainConfig::SENSOR_GROUP_SIZE_INTERNAL - 1) / 
                                 AgentBrainConfig::SENSOR_GROUP_SIZE_INTERNAL);
        sensoryProprioception_.reserve((proprioCount + AgentBrainConfig::SENSOR_GROUP_SIZE_PROPRIO - 1) / 
                                       AgentBrainConfig::SENSOR_GROUP_SIZE_PROPRIO);
        
        // Second pass: distribute neurons to groups
        size_t motorForwardCount = 0, motorBackwardCount = 0, motorTurnLeftCount = 0,
               motorTurnRightCount = 0, motorInteractCount = 0, motorWaitCount = 0;
        size_t visionCountDist = 0, touchCountDist = 0, internalCountDist = 0, proprioCountDist = 0;
        
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Distribute motor neurons to different action groups
                        switch (motorForwardCount % AgentBrainConfig::MOTOR_GROUP_SIZE) {
                            case 0: motorForward_.push_back(n); break;
                            case 1: motorBackward_.push_back(n); break;
                            case 2: motorTurnLeft_.push_back(n); break;
                            case 3: motorTurnRight_.push_back(n); break;
                            case 4: motorInteract_.push_back(n); break;
                            case 5: motorWait_.push_back(n); break;
                        }
                        motorForwardCount++;
                    }
                } else if (type == NeuronType::Sensory) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Distribute sensory neurons
                        if (visionCountDist % AgentBrainConfig::SENSOR_GROUP_SIZE_VISION == 0) {
                            sensoryVision_.push_back(n);
                        } else if (visionCountDist % AgentBrainConfig::SENSOR_GROUP_SIZE_VISION == 1) {
                            sensoryTouch_.push_back(n);
                        } else if (visionCountDist % AgentBrainConfig::SENSOR_GROUP_SIZE_VISION == 2) {
                            sensoryInternal_.push_back(n);
                        } else {
                            sensoryProprioception_.push_back(n);
                        }
                        visionCountDist++;
                    }
                }
            }
        }
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    // Use configuration constants for better maintainability
    size_t visionWidth = AgentBrainConfig::VISION_WIDTH;
    size_t visionHeight = AgentBrainConfig::VISION_HEIGHT;
    size_t visionTotal = visionWidth * visionHeight;
    
    previousVision_.resize(visionTotal, 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
}

size_t AgentBrain::getSensoryInputSize() const {
    // Use configuration constants for sensory input size
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
    return AgentBrainConfig::VISION_TOTAL + 
           AgentBrainConfig::TOUCH_TOTAL + 
           AgentBrainConfig::INTERNAL_TOTAL + 
           AgentBrainConfig::PROPRIOCEPTION_TOTAL;
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action
    return 6;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) {
        NLM_LOG_ERROR("Cannot process sensory input: brain is null");
        return;
    }
    
    const auto& vision = percept.getVision();
    const auto& touch = percept.getTouch();
    const auto& intern = percept.getInternal();
    const auto& proprio = percept.getProprioception();
    
    // Input validation
    if (vision.size() > AgentBrainConfig::VISION_TOTAL) {
        NLM_LOG_WARNING("Vision input size (" + std::to_string(vision.size()) + ") exceeds expected size (" + 
                       std::to_string(AgentBrainConfig::VISION_TOTAL) + ")");
    }
    
    if (touch.size() > AgentBrainConfig::TOUCH_TOTAL) {
        NLM_LOG_WARNING("Touch input size (" + std::to_string(touch.size()) + ") exceeds expected size (" + 
                       std::to_string(AgentBrainConfig::TOUCH_TOTAL) + ")");
    }
    
    if (intern.size() > AgentBrainConfig::INTERNAL_TOTAL) {
        NLM_LOG_WARNING("Internal input size (" + std::to_string(intern.size()) + ") exceeds expected size (" + 
                       std::to_string(AgentBrainConfig::INTERNAL_TOTAL) + ")");
    }
    
    if (proprio.size() > AgentBrainConfig::PROPRIOCEPTION_TOTAL) {
        NLM_LOG_WARNING("Proprioception input size (" + std::to_string(proprio.size()) + ") exceeds expected size (" + 
                       std::to_string(AgentBrainConfig::PROPRIOCEPTION_TOTAL) + ")");
    }
    
    // Vision input (256 values -> sensoryVision_ neurons)
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Inject current proportional to vision intensity
            float current = vision[i] * AgentBrainConfig::VISION_SCALE_FACTOR;  // Scale factor
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * AgentBrainConfig::TOUCH_SCALE_FACTOR;  // Collision signal
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            // Center and scale the input
            float current = (intern[i] * AgentBrainConfig::INTERNAL_SCALE_FACTOR - 
                           AgentBrainConfig::INTERNAL_SCALE_CENTER) * 
                          AgentBrainConfig::INTERNAL_SCALE_FACTOR;
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            // Center and scale the input
            float current = (proprio[i] * AgentBrainConfig::PROPRIOCEPTION_SCALE_FACTOR - 
                           AgentBrainConfig::PROPRIOCEPTION_SCALE_CENTER) * 
                          AgentBrainConfig::PROPRIOCEPTION_SCALE_FACTOR;
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
    
    // Compute novelty (difference from previous vision)
    if (!vision.empty() && !previousVision_.empty()) {
        float totalDiff = 0.0f;
        size_t minSize = std::min(vision.size(), previousVision_.size());
        
        for (size_t i = 0; i < minSize; ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Normalize
        noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
        
        // Decay and update
        noveltyLevel_ *= sensoryNoveltyDecay_;
        
        // Store for next time (using std::copy for efficiency)
        if (vision.size() <= previousVision_.size()) {
            std::copy(vision.begin(), vision.end(), previousVision_.begin());
        } else {
            previousVision_.resize(vision.size());
            std::copy(vision.begin(), vision.end(), previousVision_.begin());
        }
    }
    
    // Update curiosity based on novelty
    if (curiosityEnabled_) {
        curiosityLevel_ = AgentBrainConfig::CURIOSITY_NOVELTY_WEIGHT * noveltyLevel_ + 
                         std::abs(predictionError_) * AgentBrainConfig::CURIOSITY_PREDICTION_ERROR_WEIGHT;
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
    struct MotorCommandWithActivity {
        MotorCommand cmd;
        float activity;
    };
    
    MotorCommandWithActivity commands[] = {
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
    if (bestActivity < AgentBrainConfig::MOTOR_ACTIVITY_THRESHOLD) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityEnabled_ && curiosityLevel_ > AgentBrainConfig::CURIOSITY_HIGH_THRESHOLD) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * AgentBrainConfig::CURIOSITY_EXPLORATION_RATE;  // Up to 30% random
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command
            int choice = brain_->getRandomGenerator()->uniformInt(0, AgentBrainConfig::MOTOR_COMMANDS_TOTAL - 1);
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
    expectedReward_ = AgentBrainConfig::REWARD_EXPONENTIAL_DECAY * expectedReward_ + 
                      AgentBrainConfig::REWARD_EXPONENTIAL_INCREMENT * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, AgentBrainConfig::DOPAMINE_CLAMP_MIN, 
                                AgentBrainConfig::DOPAMINE_CLAMP_MAX);
    
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
    float plasticityFactor = AgentBrainConfig::PLASTICITY_BASE + 
                             AgentBrainConfig::PLASTICITY_DOPAMINE_COEFF * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, AgentBrainConfig::PLASTICITY_MIN, 
                                  AgentBrainConfig::PLASTICITY_MAX);
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(AgentBrainConfig::LTP_BASE * plasticityFactor);
        stdp->setLTDWeight(AgentBrainConfig::LTD_BASE * plasticityFactor);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // Simple developmental stages based on age
    // This is a biologically inspired approximation
    if (developmentalAge_ < AgentBrainConfig::DEVELOPMENTAL_INITIAL) {  // ~1 minute
        plasticityModifier_ = 1.0f;  // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < AgentBrainConfig::DEVELOPMENTAL_CRITICAL) {  // ~5 minutes
        plasticityModifier_ = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < AgentBrainConfig::DEVELOPMENTAL_MATURATION) {  // ~15 minutes
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
            float synRate = AgentBrainConfig::SYNAPTOGENESIS_BASE * plasticityModifier_;
            float pruneRate = AgentBrainConfig::PRUNING_BASE * (2.0f - plasticityModifier_);
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
