// Complete AgentBrain.cpp with performance optimizations and improvements

#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include "AgentConfig.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <sstream>
#include <unordered_map>

namespace nlm {

// AgentBrain Implementation

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
    , sensoryNoveltyDecay_(AgentConfig::NOVELTY_DECAY)
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
                        
                        switch (idx % AgentConfig::MOTOR_GROUP_COUNT) {
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

// Process command line
bool AgentBrain::processCommand(const std::string& commandLine) {
    return commandHandler_.executeCommand(*this, commandLine);
}

// Register command
void AgentBrain::registerCommand(const std::string& name, std::shared_ptr<AgentCommand> command) {
    commandHandler_.registerCommand(name, command);
}

// Initialize with world
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
    return AgentConfig::VISION_SIZE + AgentConfig::TOUCH_SIZE + 
           AgentConfig::INTERNAL_SIZE + AgentConfig::PROPRIOCEPTION_SIZE;
}

size_t AgentBrain::getMotorOutputSize() const {
    return AgentConfig::MOTOR_GROUP_COUNT;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            float current = vision[i] * 5.0f;
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * 8.0f;
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;
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
        
        noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
        noveltyLevel_ *= sensoryNoveltyDecay_;
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
    
    if (curiosityEnabled_ && curiosityLevel_ > AgentConfig::EXPLORATION_THRESHOLD) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
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
    
    struct { MotorCommand cmd; float activity; } commands[] = {
        {MotorCommand::MoveForward, forwardAct},
        {MotorCommand::MoveBackward, backwardAct},
        {MotorCommand::TurnLeft, leftAct},
        {MotorCommand::TurnRight, rightAct},
        {MotorCommand::Interact, interactAct},
        {MotorCommand::Wait, waitAct}
    };
    
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct;
    
    for (const auto& c : commands) {
        if (c.activity > bestActivity) {
            bestActivity = c.activity;
            best = c.cmd;
        }
    }
    
    if (bestActivity < AgentConfig::ACTIVATION_THRESHOLD) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    if (curiosityLevel_ > AgentConfig::HIGH_EXPLORATION_THRESHOLD) {
        float exploreChance = curiosityLevel_ * AgentConfig::EXPLORATION_THRESHOLD;
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            int choice = brain_->getRandomGenerator()->uniformInt(0, 7);
            switch (choice) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::TurnLeft;
                case 3: return MotorCommand::TurnRight;
                case 4: return MotorCommand::LookLeft;
                case 5: return MotorCommand::LookRight;
                case 6: return MotorCommand::Interact;
                case 7: return MotorCommand::Wait;
            }
        }
    }
    
    return defaultCmd;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    predictionError_ = reward - predictedReward;
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    dopamineLevel_ = predictionError_;
    dopamineLevel_ = std::clamp(dopamineLevel_, AgentConfig::DOPAMINE_CLAMP_MIN, AgentConfig::DOPAMINE_CLAMP_MAX);
    
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > AgentConfig::ELIGIBILITY_THRESHOLD) {
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                syn->addToWeight(delta);
                syn->decayEligibilityTrace(AgentConfig::ELIGIBILITY_DECAY);
            }
        }
    }
    
    float plasticityFactor = AgentConfig::PLASTICITY_BASE + 0.5f * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, AgentConfig::PLASTICITY_MIN, AgentConfig::PLASTICITY_MAX);
    
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    if (developmentalAge_ < AgentConfig::DEVELOPMENT_STAGE_INITIAL) {
        plasticityModifier_ = 1.0f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < AgentConfig::DEVELOPMENT_STAGE_CRITICAL) {
        plasticityModifier_ = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < AgentConfig::DEVELOPMENT_STAGE_MATURATION) {
        plasticityModifier_ = 0.5f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = 0.2f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
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
    
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

// Advanced agent methods
bool AgentBrain::processAdvancedCommand(const std::string& commandLine) {
    return advancedCommandHandler_.executeCommand(*this, commandLine);
}

void AgentBrain::enableFeature(const std::string& featureName) {
    if (featureName == "meta_learning") {
        featureFlags_.enableMetaLearning = true;
    } else if (featureName == "memory_integration") {
        featureFlags_.enableMemoryIntegration = true;
    } else if (featureName == "adaptive_curiosity") {
        featureFlags_.enableAdaptiveCuriosity = true;
    } else if (featureName == "social_learning") {
        featureFlags_.enableSocialLearning = true;
    } else if (featureName == "transfer_learning") {
        featureFlags_.enableTransferLearning = true;
    } else if (featureName == "self_reflection") {
        featureFlags_.enableSelfReflection = true;
    } else if (featureName == "emergent_behaviors") {
        featureFlags_.enableEmergentBehaviors = true;
    } else if (featureName == "goal_directed") {
        featureFlags_.enableGoalDirectedBehavior = true;
    }
}

void AgentBrain::disableFeature(const std::string& featureName) {
    if (featureName == "meta_learning") {
        featureFlags_.enableMetaLearning = false;
    } else if (featureName == "memory_integration") {
        featureFlags_.enableMemoryIntegration = false;
    } else if (featureName == "adaptive_curiosity") {
        featureFlags_.enableAdaptiveCuriosity = false;
    } else if (featureName == "social_learning") {
        featureFlags_.enableSocialLearning = false;
    } else if (featureName == "transfer_learning") {
        featureFlags_.enableTransferLearning = false;
    } else if (featureName == "self_reflection") {
        featureFlags_.enableSelfReflection = false;
    } else if (featureName == "emergent_behaviors") {
        featureFlags_.enableEmergentBehaviors = false;
    } else if (featureName == "goal_directed") {
        featureFlags_.enableGoalDirectedBehavior = false;
    }
}

bool AgentBrain::isFeatureEnabled(const std::string& featureName) const {
    if (featureName == "meta_learning") return featureFlags_.enableMetaLearning;
    if (featureName == "memory_integration") return featureFlags_.enableMemoryIntegration;
    if (featureName == "adaptive_curiosity") return featureFlags_.enableAdaptiveCuriosity;
    if (featureName == "social_learning") return featureFlags_.enableSocialLearning;
    if (featureName == "transfer_learning") return featureFlags_.enableTransferLearning;
    if (featureName == "self_reflection") return featureFlags_.enableSelfReflection;
    if (featureName == "emergent_behaviors") return featureFlags_.enableEmergentBehaviors;
    if (featureName == "goal_directed") return featureFlags_.enableGoalDirectedBehavior;
    return false;
}

void AgentBrain::registerAdvancedCommand(const std::string& name, std::shared_ptr<AgentCommand> command) {
    advancedCommandHandler_.registerCommand(name, command);
}

void AgentBrain::setFeatureFlag(const std::string& featureName, float value) {
    if (featureName == "meta_learning_rate") {
        featureFlags_.metaLearningRate = value;
    } else if (featureName == "memory_integration_weight") {
        featureFlags_.memoryIntegrationWeight = value;
    } else if (featureName == "curiosity_adaptation_rate") {
        featureFlags_.curiosityAdaptationRate = value;
    } else if (featureName == "social_learning_rate") {
        featureFlags_.socialLearningRate = value;
    } else if (featureName == "transfer_learning_threshold") {
        featureFlags_.transferLearningThreshold = value;
    } else if (featureName == "self_reflection_threshold") {
        featureFlags_.selfReflectionThreshold = value;
    }
}

float AgentBrain::getFeatureFlag(const std::string& featureName) const {
    if (featureName == "meta_learning_rate") return featureFlags_.metaLearningRate;
    if (featureName == "memory_integration_weight") return featureFlags_.memoryIntegrationWeight;
    if (featureName == "curiosity_adaptation_rate") return featureFlags_.curiosityAdaptationRate;
    if (featureName == "social_learning_rate") return featureFlags_.socialLearningRate;
    if (featureName == "transfer_learning_threshold") return featureFlags_.transferLearningThreshold;
    if (featureName == "self_reflection_threshold") return featureFlags_.selfReflectionThreshold;
    return 0.0f;
}

// Performance monitoring
void AgentBrain::enablePerformanceMonitoring(bool enable) {
    performanceMonitor_.enable(enable);
}

std::string AgentBrain::getPerformanceReport() const {
    return performanceMonitor_.getReport();
}

bool AgentBrain::executeOptimizedCommand(const std::string& commandLine) {
    return performanceMonitor_.executeCommand(*this, commandLine);
}

std::vector<bool> AgentBrain::executeBatchCommands(const std::vector<std::string>& commandLines) {
    return performanceMonitor_.executeBatch(*this, commandLines);
}

void AgentBrain::clearCommandCache() {
    performanceMonitor_.clearCache();
}

size_t AgentBrain::getCommandCacheSize() const {
    return performanceMonitor_.getCacheSize();
}

} // namespace nlm