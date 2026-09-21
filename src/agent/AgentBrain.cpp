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
    , motorActivityCache_{}
    , totalStepsProcessed_(0)
    , sensoryInputsProcessed_(0)
    , motorCommandsGenerated_(0)
    , cumulativeReward_(0.0f)
    , episodeCount_(0)
    , enableActivityCaching_(true)
    , enableStatisticsTracking_(true)
    , activityThreshold_(0.5f)
    , curiosityThreshold_(0.3f)
    , noveltyThreshold_(0.2f)
    , adaptationRate_(0.01f)
    , environmentalStability_(1.0f)
    , habitatComplexity_(1.0f)
    , learningRate_(0.01f)
    , explorationRate_(0.1f)
    , exploitationRate_(0.9f)
    , enableSocialLearning_(false)
    , socialLearningRate_(0.05f)
    , stressLevel_(0.0f)
    , recoveryRate_(0.01f)
    , stressThreshold_(0.8f)
    , isSleeping_(false)
    , sleepDebt_(0.0f)
    , consolidationEfficiency_(0.8f)
    , enableMetaLearning_(false)
    , metaLearningRate_(0.001f)
    , energyBudget_(100.0f)
    , energyConsumptionRate_(0.1f)
    , energyRegenerationRate_(0.05f)
    , mutationRate_(0.001f)
    , selectionPressure_(1.0f)
    , populationDiversity_(1.0f)
    , maxDebugLogSize_(100)
    , developmentalPlasticityRate_(1.0f)
    , maturationProgress_(0.0f)
    , synapticGrowthRate_(0.001f)
{
    // Validate input brain pointer
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain constructor: Invalid brain pointer - cannot be null");
        // The brain pointer is shared_ptr, so this will throw if null
        // but we add defensive logging for debugging
    }

    // Initialize motor and sensory neuron groups
    if (brain_) {
        // Pre-allocate vectors based on configuration for better performance
        size_t totalMotorNeurons = 0, totalSensoryNeurons = 0;
        
        // First pass: count neurons to pre-allocate
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                if (type == NeuronType::Motor) {
                    totalMotorNeurons += pop->getNeuronCount();
                } else if (type == NeuronType::Sensory) {
                    totalSensoryNeurons += pop->getNeuronCount();
                }
            }
        }
        
        // Reserve memory
        motorForward_.reserve(totalMotorNeurons / 6 + 1);
        motorBackward_.reserve(totalMotorNeurons / 6 + 1);
        motorTurnLeft_.reserve(totalMotorNeurons / 6 + 1);
        motorTurnRight_.reserve(totalMotorNeurons / 6 + 1);
        motorInteract_.reserve(totalMotorNeurons / 6 + 1);
        motorWait_.reserve(totalMotorNeurons / 6 + 1);
        
        sensoryVision_.reserve(totalSensoryNeurons / 4 + 1);
        sensoryTouch_.reserve(totalSensoryNeurons / 4 + 1);
        sensoryInternal_.reserve(totalSensoryNeurons / 4 + 1);
        sensoryProprioception_.reserve(totalSensoryNeurons / 4 + 1);
        
        // Second pass: distribute neurons
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        if (!n) {
                            NLM_LOG_ERROR("AgentBrain constructor warning: Null neuron found");
                            continue;
                        }
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
                        if (!n) {
                            NLM_LOG_ERROR("AgentBrain constructor warning: Null neuron found");
                            continue;
                        }
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
        
        // Track statistics if enabled
        if (enableStatisticsTracking_) {
            sensoryInputsProcessed_++;
            if (curiosityLevel_ > 0.5f) {
                // High curiosity exploration
                exploreChance = curiosityLevel_ * 0.3f;
            }
        }
    }
    
    // Apply exploration/exploitation based on curiosity and statistics
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f && enableStatisticsTracking_) {
        // More sophisticated exploration based on statistics
        if (exploreChance > 0.0f) {
            float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
            if (r < exploreChance) {
                // Statistics-informed exploration
                int choice = brain_->getRandomGenerator()->uniformInt(0, 7);
                
                // Apply environmental adaptation
                float adaptation = environmentalStability_ * 0.1f;
                if (brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f) < adaptation) {
                    // Explore with some adaptation to environment
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
                } else {
                    // Pure random exploration
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
        }
    }
    
    // Update energy management
    if (enableStatisticsTracking_) {
        // Energy management
        float energyConsumed = 0.1f; // Base energy cost per step
        if (isSleeping_) {
            energyConsumed *= 0.1f; // Reduced cost during sleep
        }
        
        energyBudget_ -= energyConsumed;
        if (energyBudget_ < 0.0f) {
            // Enter sleep state if energy is depleted
            isSleeping_ = true;
            sleepDebt_ += std::abs(energyBudget_);
            energyBudget_ = 10.0f; // Reset with sleep
            NLM_LOG_INFO("AgentBrain: Energy depleted - entering sleep state");
        }
        
        // Regenerate energy
        if (!isSleeping_) {
            energyBudget_ += energyRegenerationRate_;
        }
        
        // Apply stress based on energy level
        if (energyBudget_ < stressThreshold_ * energyBudget_) {
            stressLevel_ = std::min(1.0f, stressLevel_ + 0.1f);
        } else {
            stressLevel_ = std::max(0.0f, stressLevel_ - recoveryRate_);
        }
        
        // Apply stress effects on learning
        if (stressLevel_ > 0.8f) {
            // High stress reduces learning efficiency
            learningRate_ *= 0.5f;
        } else {
            // Recovery phase
            learningRate_ = std::min(0.01f, learningRate_ * 1.1f);
        }
    }
