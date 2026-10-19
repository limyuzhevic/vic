#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <chrono>
#include <iostream>
#include <fstream>

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : AgentBrain(brain, AgentBrainConfig()) {
}

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain, const AgentBrainConfig& config)
    : brain_(brain)
    , config_(config)
    , dopamineLevel_(0.0f)
    , noveltyLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , predictionError_(0.0f)
    , expectedReward_(0.0f)
    , developmentalAge_(0.0)
    , plasticityModifier_(1.0f)
    , sensoryNoveltyDecay_(0.99f)
    , totalNeuronsProcessed_(0)
    , totalStepsProcessed_(0)
    , totalProcessingTime_(0.0)
    , lastStepTime_(0.0) {
    
    // Validate configuration
    if (!config_.validate()) {
        std::cerr << "AgentBrain: Configuration validation failed!" << std::endl;
        return;
    }
    
    // Initialize neuron groups
    initializeNeuronGroups();
    
    if (config_.enableDebugLogging) {
        logDebugInfo("AgentBrain initialized with enhanced configuration");
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    initialize(world, config_);
}

void AgentBrain::initialize(const SimpleWorld& world, const AgentBrainConfig& config) {
    // Update configuration
    config_ = config;
    
    // Resize previous vision based on world dimensions
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Reset performance metrics
    totalNeuronsProcessed_ = 0;
    totalStepsProcessed_ = 0;
    totalProcessingTime_ = 0.0;
    lastStepTime_ = 0.0;
    
    // Re-initialize neuron groups with new config
    initializeNeuronGroups();
    
    if (config_.enableDebugLogging) {
        logDebugInfo(std::string("AgentBrain re-initialized with config: ") + config_.toString());
    }
}

size_t AgentBrain::getSensoryInputSize() const {
    // Calculate based on configuration
    size_t visionSize = config_.sensoryVisionWidth * config_.sensoryVisionHeight;
    return visionSize + config_.sensoryTouchCount + config_.sensoryInternalCount + config_.sensoryProprioceptionCount;
}

size_t AgentBrain::getMotorOutputSize() const {
    // Count enabled motor groups
    size_t motorGroups = 0;
    if (config_.enableMotorForward) motorGroups++;
    if (config_.enableMotorBackward) motorGroups++;
    if (config_.enableMotorTurnLeft) motorGroups++;
    if (config_.enableMotorTurnRight) motorGroups++;
    if (config_.enableMotorInteract) motorGroups++;
    if (config_.enableMotorWait) motorGroups++;
    
    return motorGroups;
}

void AgentBrain::initializeNeuronGroups() {
    motorForward_.clear();
    motorBackward_.clear();
    motorTurnLeft_.clear();
    motorTurnRight_.clear();
    motorInteract_.clear();
    motorWait_.clear();
    
    sensoryVision_.clear();
    sensoryTouch_.clear();
    sensoryInternal_.clear();
    sensoryProprioception_.clear();
    
    if (!brain_) return;
    
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            
            if (type == NeuronType::Motor) {
                for (Neuron* n : pop->getNeurons()) {
                    distributeNeurons("motor", {n});
                }
            } else if (type == NeuronType::Sensory) {
                for (Neuron* n : pop->getNeurons()) {
                    distributeNeurons("sensory", {n});
                }
            }
        }
    }
    
    totalNeuronsProcessed_ = motorForward_.size() + motorBackward_.size() + 
                            motorTurnLeft_.size() + motorTurnRight_.size() +
                            motorInteract_.size() + motorWait_.size() +
                            sensoryVision_.size() + sensoryTouch_.size() +
                            sensoryInternal_.size() + sensoryProprioception_.size();
    
    if (config_.enableDebugLogging) {
        logDebugInfo(std::string("Neuron groups initialized: ") + 
                    std::to_string(motorForward_.size()) + " forward, " +
                    std_to_string(motorBackward_.size()) + " backward, " +
                    std::to_string(motorTurnLeft_.size()) + " left, " +
                    std::to_string(motorTurnRight_.size()) + " right, " +
                    std::to_string(motorInteract_.size()) + " interact, " +
                    std::to_string(motorWait_.size()) + " wait");
    }
}

void AgentBrain::distributeNeurons(const std::string& type, const std::vector<Neuron*>& neurons) {
    if (type == "motor") {
        if (!config_.enableMotorForward || neurons.empty()) return;
        
        size_t idx = motorForward_.size() + motorBackward_.size() + 
                    motorTurnLeft_.size() + motorTurnRight_.size() +
                    motorInteract_.size() + motorWait_.size();
        
        float weight = getGroupWeight("forward");
        
        switch (idx % 6) {
            case 0: motorForward_.insert(motorForward_.end(), neurons.begin(), neurons.end()); break;
            case 1: motorBackward_.insert(motorBackward_.end(), neurons.begin(), neurons.end()); break;
            case 2: motorTurnLeft_.insert(motorTurnLeft_.end(), neurons.begin(), neurons.end()); break;
            case 3: motorTurnRight_.insert(motorTurnRight_.end(), neurons.begin(), neurons.end()); break;
            case 4: motorInteract_.insert(motorInteract_.end(), neurons.begin(), neurons.end()); break;
            case 5: motorWait_.insert(motorWait_.end(), neurons.begin(), neurons.end()); break;
        }
    } else if (type == "sensory") {
        if (config_.sensoryVisionWidth * config_.sensoryVisionHeight == 0) return;
        
        size_t idx = sensoryVision_.size() + sensoryTouch_.size() +
                    sensoryInternal_.size() + sensoryProprioception_.size();
        
        float weight = getGroupWeight("vision");
        
        switch (idx % 4) {
            case 0: sensoryVision_.insert(sensoryVision_.end(), neurons.begin(), neurons.end()); break;
            case 1: sensoryTouch_.insert(sensoryTouch_.end(), neurons.begin(), neurons.end()); break;
            case 2: sensoryInternal_.insert(sensoryInternal_.end(), neurons.begin(), neurons.end()); break;
            case 3: sensoryProprioception_.insert(sensoryProprioception_.end(), neurons.begin(), neurons.end()); break;
        }
    }
}

float AgentBrain::getGroupWeight(const std::string& group) const {
    auto it = config_.neuronGroupWeights.find(group);
    if (it != config_.neuronGroupWeights.end()) {
        return it->second;
    }
    
    // Default weights based on group
    if (group == "forward") return 0.2f;
    if (group == "backward") return 0.2f;
    if (group == "left") return 0.2f;
    if (group == "right") return 0.2f;
    if (group == "interact") return 0.1f;
    if (group == "wait") return 0.3f;
    if (group == "vision") return 0.4f;
    if (group == "touch") return 0.3f;
    if (group == "internal") return 0.2f;
    if (group == "proprioception") return 0.1f;
    
    return 0.1f; // Default
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    if (!brain_) return;
    
    // Process vision input
    if (config_.enableDebugLogging) {
        logDebugInfo("Processing vision input: " + std::to_string(percept.getVision().size()) + " values");
    }
    
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            float current = vision[i] * config_.visionScaleFactor;
            if (validateNeuralActivity(current)) {
                sensoryVision_[i]->injectCurrent(current);
            }
        }
    }
    
    // Process touch input
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * config_.touchScaleFactor;
            if (validateNeuralActivity(current)) {
                sensoryTouch_[i]->injectCurrent(current);
            }
        }
    }
    
    // Process internal signals
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * config_.internalScaleFactor;
            if (validateNeuralActivity(current)) {
                sensoryInternal_[i]->injectCurrent(current);
            }
        }
    }
    
    // Process proprioception
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * config_.proprioceptionScaleFactor;
            if (validateNeuralActivity(current)) {
                sensoryProprioception_[i]->injectCurrent(current);
            }
        }
    }
    
    // Compute novelty
    if (!vision.empty()) {
        float totalDiff = 0.0f;
        for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
        noveltyLevel_ *= config_.noveltyDecayFactor;
        
        // Store for next time
        previousVision_ = vision;
    }
    
    // Update curiosity based on novelty
    if (config_.enableCuriosity) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
    
    // Update performance metrics
    totalStepsProcessed_++;
    auto endTime = std::chrono::high_resolution_clock::now();
    totalProcessingTime_ += std::chrono::duration<double>(endTime - startTime).count() * 1000.0; // Convert to milliseconds
    
    if (config_.enableStepByStepLogging && totalStepsProcessed_ % 10 == 0) {
        logDebugInfo(std::string("Step " + std::to_string(totalStepsProcessed_) + 
                               ": novelty=" + std::to_string(noveltyLevel_) +
                               ", curiosity=" + std::to_string(curiosityLevel_) +
                               ", prediction_error=" + std::to_string(predictionError_)));
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    return decodeMotorCommandWithAdvancedOptions();
}

MotorCommand AgentBrain::decodeMotorCommandWithAdvancedOptions() const {
    // Use the most appropriate decoding method based on enabled features
    if (config_.enablePredictionIntegration && config_.enableMemoryIntegration && config_.enableCognitionIntegration) {
        return decodeMotorCommandWithPrediction();
    } else if (config_.enablePredictionIntegration && config_.enableMemoryIntegration) {
        return decodeMotorCommandWithMemory();
    } else if (config_.enablePredictionIntegration && config_.enableCognitionIntegration) {
        return decodeWithCognition({});
    } else {
        return decodeFromMotorNeurons();
    }
}

MotorCommand AgentBrain::decodeMotorCommandWithPrediction() const {
    // For now, use standard decoding with prediction-weighted considerations
    MotorCommand cmd = decodeFromMotorNeurons();
    
    // Apply prediction-based modulation if prediction error is high
    if (std::abs(predictionError_) > config_.curiosityThreshold) {
        cmd = selectWithCuriosity(cmd);
    }
    
    return cmd;
}

MotorCommand AgentBrain::decodeMotorCommandWithMemory() const {
    // Memory-based decoding could use stored episodes to influence decisions
    // For now, use standard decoding
    return decodeFromMotorNeurons();
}

MotorCommand AgentBrain::decodeMotorCommandWithCognition() const {
    // Cognition-based decoding could use concepts and planning
    // For now, use standard decoding with enhanced curiosity
    MotorCommand cmd = decodeFromMotorNeurons();
    
    if (config_.enableCuriosity && curiosityLevel_ > 0.5f) {
        cmd = selectWithCuriosity(cmd);
    }
    
    return cmd;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() const {
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            if (n) {
                // Use membrane potential deviation from rest as activity measure
                sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
            }
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
    float bestActivity = waitAct; // Default to wait if nothing stronger
    
    for (const auto& c : commands) {
        if (c.activity > bestActivity) {
            bestActivity = c.activity;
            best = c.cmd;
        }
    }
    
    // Only act if there's meaningful activity
    if (bestActivity < config_.minActivityThreshold) {
        return MotorCommand::Wait;
    }
    
    // Validate the command
    if (!validateMotorCommand(best)) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) const {
    // Enhanced exploration with configurable parameters
    if (config_.enableCuriosity && curiosityLevel_ > config_.curiosityThreshold) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * config_.explorationRate;
        
        if (brain_) {
            float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
            if (r < exploreChance) {
                // Random motor command with consideration of enabled groups
                std::vector<MotorCommand> availableCommands;
                
                if (config_.enableMotorForward) availableCommands.push_back(MotorCommand::MoveForward);
                if (config_.enableMotorBackward) availableCommands.push_back(MotorCommand::MoveBackward);
                if (config_.enableMotorTurnLeft) availableCommands.push_back(MotorCommand::TurnLeft);
                if (config_.enableMotorTurnRight) availableCommands.push_back(MotorCommand::TurnRight);
                if (config_.enableMotorInteract) availableCommands.push_back(MotorCommand::Interact);
                if (config_.enableMotorWait) availableCommands.push_back(MotorCommand::Wait);
                
                if (!availableCommands.empty()) {
                    int choice = brain_->getRandomGenerator()->uniformInt(0, availableCommands.size());
                    return availableCommands[choice];
                }
            }
        }
    }
    
    return defaultCmd;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !config_.enableRewardModulation) return;
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = config_.rewardDecay * expectedReward_ + (1.0f - config_.rewardDecay) * reward;
    
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
                syn->decayEligibilityTrace(config_.eligibilityDecay);
            }
        }
    }
    
    // Modulate plasticity based on dopamine
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = config_.plasticityBase + config_.plasticityBoost * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
    }
    
    if (config_.enableDebugLogging) {
        logDebugInfo(std::string("Reward modulation: reward=") + std::to_string(reward) +
                    ", predicted=") + std::to_string(predictedReward) +
                    ", error=" + std::to_string(predictionError_) +
                    ", dopamine=" + std::to_string(dopamineLevel_) +
                    ", plasticity_factor=" + std::to_string(plasticityFactor));
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !config_.enableDevelopment) return;
    
    developmentalAge_ += timestep;
    
    // Simple developmental stages based on age
    // This is a biologically inspired approximation
    if (developmentalAge_ < 60.0) {  // ~1 minute
        plasticityModifier_ = 1.0f; // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < 300.0) {  // ~5 minutes
        plasticityModifier_ = config_.plasticityBoost;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < 900.0) {  // ~15 minutes
        plasticityModifier_ = 0.5f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = config_.plasticityBase; // Adult - more stable
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Structural plasticity changes with development
    if (config_.enableStructuralPlasticity) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // Higher synaptogenesis in early development
            float synRate = 0.0001f * plasticityModifier_;
            float pruneRate = 0.00001f * (2.0f - plasticityModifier_);
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
    }
    
    if (config_.enableDebugLogging && developmentalAge_ < 60.0) {
        logDebugInfo(std::string("Development stage: ") + getDevelopmentalStageString() +
                    ", age=") + std::to_string(developmentalAge_) +
                    ", plasticity=") + std::to_string(plasticityModifier_);
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
    
    // Reset neuron states
    for (auto* neuron : motorForward_) {
        if (neuron) neuron->reset();
    }
    for (auto* neuron : motorBackward_) {
        if (neuron) neuron->reset();
    }
    for (auto* neuron : motorTurnLeft_) {
        if (neuron) neuron->reset();
    }
    for (auto* neuron : motorTurnRight_) {
        if (neuron) neuron->reset();
    }
    for (auto* neuron : motorInteract_) {
        if (neuron) neuron->reset();
    }
    for (auto* neuron : motorWait_) {
        if (neuron) neuron->reset();
    }
    
    for (auto* neuron : sensoryVision_) {
        if (neuron) neuron->reset();
    }
    for (auto* neuron : sensoryTouch_) {
        if (neuron) neuron->reset();
    }
    for (auto* neuron : sensoryInternal_) {
        if (neuron) neuron->reset();
    }
    for (auto* neuron : sensoryProprioception_) {
        if (neuron) neuron->reset();
    }
    
    if (config_.enableDebugLogging) {
        logDebugInfo("AgentBrain reset completed");
    }
}

void AgentBrain::updateConfig(const AgentBrainConfig& newConfig) {
    if (newConfig.validate()) {
        config_ = newConfig;
        // Re-initialize neuron groups with new configuration
        initializeNeuronGroups();
        
        if (config_.enableDebugLogging) {
            logDebugInfo("AgentBrain configuration updated successfully");
        }
    } else {
        std::cerr << "AgentBrain: Cannot update configuration - validation failed!" << std::endl;
    }
}

void AgentBrain::setConfigValue(const std::string& key, const std::string& value) {
    // This is a simplified setter - in practice, you'd want proper key-value parsing
    // For now, just log the attempt
    if (config_.enableDebugLogging) {
        logDebugInfo(std::string("Config value requested: key=") + key + ", value=" + value);
    }
}

float AgentBrain::getAverageFiringRate() const {
    if (!brain_) return 0.0f;
    
    size_t totalFiringNeurons = 0;
    size_t totalNeurons = 0;
    
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            totalFiringNeurons += pop->getFiringCount();
            totalNeurons += pop->getSize();
        }
    }
    
    return totalNeurons > 0 ? static_cast<float>(totalFiringNeurons) / totalNeurons : 0.0f;
}

std::string AgentBrain::getDevelopmentalStageString() const {
    DevelopmentalStage stage = getDevelopmentalStage();
    switch (stage) {
        case DevelopmentalStage::Initial: return "Initial";
        case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
        case DevelopmentalStage::Maturation: return "Maturation";
        case DevelopmentalStage::Adult: return "Adult";
        case DevelopmentalStage::Aging: return "Aging";
        default: return "Unknown";
    }
}

void AgentBrain::logDebugInfo(const std::string& message) const {
    if (config_.enableDebugLogging) {
        std::ofstream logFile(config_.logFile, std::ios::app);
        if (logFile.is_open()) {
            auto now = std::chrono::system_clock::now();
            auto time = std::chrono::system_clock::to_time_t(now);
            logFile << "[" << std::ctime(&time) << "] " << message << std::endl;
        }
        
        // Also output to stderr for immediate feedback
        std::cerr << "[AgentBrain] " << message << std::endl;
    }
}

void AgentBrain::logPerformanceMetrics() const {
    if (config_.enablePerformanceMetrics && totalStepsProcessed_ > 0) {
        double avgTimePerStep = totalProcessingTime_ / totalStepsProcessed_;
        std::cout << "=== Performance Metrics ===\n";
        std::cout << "Total steps processed: " << totalStepsProcessed_ << "\n";
        std::cout << "Total neurons processed: " << totalNeuronsProcessed_ << "\n";
        std::cout << "Average processing time per step: " << avgTimePerStep << " ms\n";
        std::cout << "Average firing rate: " << getAverageFiringRate() << "\n";
        std::cout << "Final curiosity level: " << curiosityLevel_ << "\n";
        std::cout << "Final novelty level: " << noveltyLevel_ << "\n";
        std::cout << "Developmental stage: " << getDevelopmentalStageString() << "\n";
        std::cout << "===================================\n";
    }
}

float AgentBrain::calculateEnergyCost(const MotorCommand& cmd) const {
    // Simple energy cost calculation based on motor command
    switch (cmd) {
        case MotorCommand::MoveForward:
        case MotorCommand::MoveBackward:
            return config_.maxEnergyConsumption * 0.7f;
        case MotorCommand::TurnLeft:
        case MotorCommand::TurnRight:
            return config_.maxEnergyConsumption * 0.5f;
        case MotorCommand::Interact:
            return config_.maxEnergyConsumption * 0.8f;
        case MotorCommand::Wait:
            return config_.maxEnergyConsumption * 0.1f;
        default:
            return config_.maxEnergyConsumption * 0.5f;
    }
}

float AgentBrain::calculateHealthDecay(float energy) const {
    return config_.maxHealthDecay * (1.0f - energy);
}

bool AgentBrain::validateAction(const MotorCommand& cmd) const {
    // Check if the motor command is enabled
    switch (cmd) {
        case MotorCommand::MoveForward: return config_.enableMotorForward;
        case MotorCommand::MoveBackward: return config_.enableMotorBackward;
        case MotorCommand::TurnLeft: return config_.enableMotorTurnLeft;
        case MotorCommand::TurnRight: return config_.enableMotorTurnRight;
        case MotorCommand::Interact: return config_.enableMotorInteract;
        case MotorCommand::Wait: return config_.enableMotorWait;
        default: return false;
    }
}

bool AgentBrain::validateNeuralActivity(float activity) const {
    return activity >= config_.minActivityThreshold && activity <= config_.maxActivityThreshold;
}

bool AgentBrain::validateMotorCommand(const MotorCommand& cmd) const {
    if (!validateAction(cmd)) {
        return false;
    }
    
    // Additional validation logic can be added here
    return true;
}

} // namespace nlm