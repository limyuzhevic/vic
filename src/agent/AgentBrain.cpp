// AgentBrain.cpp - Enhanced AgentBrain implementation

#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <chrono>

namespace nlm {

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
    , rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , curiosityEnabled_(true)
    , advancedLearningEnabled_(false)
    , groupsInitialized_(false)
    , lastActivityCheckStep_(0)
    , totalProcessCalls_(0)
    , totalDecodeCalls_(0)
    , totalProcessingTime_(0.0)
{
    // Initialize random number generator with seed
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    rng_ = std::mt19937(seed);
    uniformDist_ = std::uniform_real_distribution<float>(0.0f, 1.0f);
    intDist_ = std::uniform_int_distribution<int>(0, 7);
    
    // Validate configuration
    if (config_.validateInputs) {
        validateConfig();
    }
    
    NLM_LOG_INFO("AgentBrain created with enhanced architecture");
}

AgentBrain::~AgentBrain() = default;

AgentBrain::AgentBrain(AgentBrain&& other) noexcept
    : brain_(std::move(other.brain_))
    , config_(std::move(other.config_))
    , neuralGroups_(std::move(other.neuralGroups_))
    , motorForward_(std::move(other.motorForward_))
    , motorBackward_(std::move(other.motorBackward_))
    , motorTurnLeft_(std::move(other.motorTurnLeft_))
    , motorTurnRight_(std::move(other.motorTurnRight_))
    , motorInteract_(std::move(other.motorInteract_))
    , motorWait_(std::move(other.motorWait_))
    , sensoryVision_(std::move(other.sensoryVision_))
    , sensoryTouch_(std::move(other.sensoryTouch_))
    , sensoryInternal_(std::move(other.sensoryInternal_))
    , sensoryProprioception_(std::move(other.sensoryProprioception_))
    , dopamineLevel_(other.dopamineLevel_)
    , noveltyLevel_(other.noveltyLevel_)
    , curiosityLevel_(other.curiosityLevel_)
    , predictionError_(other.predictionError_)
    , expectedReward_(other.expectedReward_)
    , developmentalAge_(other.developmentalAge_)
    , plasticityModifier_(other.plasticityModifier_)
    , rewardModulationEnabled_(other.rewardModulationEnabled_)
    , structuralPlasticityEnabled_(other.structuralPlasticityEnabled_)
    , developmentEnabled_(other.developmentEnabled_)
    , curiosityEnabled_(other.curiosityEnabled_)
    , advancedLearningEnabled_(other.advancedLearningEnabled_)
    , groupsInitialized_(other.groupsInitialized_)
    , lastActivityCheckStep_(other.lastActivityCheckStep_)
    , totalProcessCalls_(other.totalProcessCalls_)
    , totalDecodeCalls_(other.totalDecodeCalls_)
    , totalProcessingTime_(other.totalProcessingTime_)
    , previousVision_(std::move(other.previousVision_))
    , sensoryNoveltyDecay_(other.sensoryNoveltyDecay_)
    , maxVisionSamples_(other.maxVisionSamples_)
    , maxTouchSamples_(other.maxTouchSamples_)
    , maxInternalSamples_(other.maxInternalSamples_)
    , maxProprioceptionSamples_(other.maxProprioceptionSamples_)
{
}

AgentBrain& AgentBrain::operator=(AgentBrain&& other) noexcept {
    if (this != &other) {
        brain_ = std::move(other.brain_);
        config_ = std::move(other.config_);
        neuralGroups_ = std::move(other.neuralGroups_);
        motorForward_ = std::move(other.motorForward_);
        motorBackward_ = std::move(other.motorBackward_);
        motorTurnLeft_ = std::move(other.motorTurnLeft_);
        motorTurnRight_ = std::move(other.motorTurnRight_);
        motorInteract_ = std::move(other.motorInteract_);
        motorWait_ = std::move(other.motorWait_);
        sensoryVision_ = std::move(other.sensoryVision_);
        sensoryTouch_ = std::move(other.sensoryTouch_);
        sensoryInternal_ = std::move(other.sensoryInternal_);
        sensoryProprioception_ = std::move(other.sensoryProprioception_);
        dopamineLevel_ = other.dopamineLevel_;
        noveltyLevel_ = other.noveltyLevel_;
        curiosityLevel_ = other.curiosityLevel_;
        predictionError_ = other.predictionError_;
        expectedReward_ = other.expectedReward_;
        developmentalAge_ = other.developmentalAge_;
        plasticityModifier_ = other.plasticityModifier_;
        rewardModulationEnabled_ = other.rewardModulationEnabled_;
        structuralPlasticityEnabled_ = other.structuralPlasticityEnabled_;
        developmentEnabled_ = other.developmentEnabled_;
        curiosityEnabled_ = other.curiosityEnabled_;
        advancedLearningEnabled_ = other.advancedLearningEnabled_;
        groupsInitialized_ = other.groupsInitialized_;
        lastActivityCheckStep_ = other.lastActivityCheckStep_;
        totalProcessCalls_ = other.totalProcessCalls_;
        totalDecodeCalls_ = other.totalDecodeCalls_;
        totalProcessingTime_ = other.totalProcessingTime_;
        previousVision_ = std::move(other.previousVision_);
        sensoryNoveltyDecay_ = other.sensoryNoveltyDecay_;
        maxVisionSamples_ = other.maxVisionSamples_;
        maxTouchSamples_ = other.maxTouchSamples_;
        maxInternalSamples_ = other.maxInternalSamples_;
        maxProprioceptionSamples_ = other.maxProprioceptionSamples_;
    }
    return *this;
}

bool AgentBrain::initialize(const SimpleWorld& world) {
    NLM_LOG_INFO("Initializing AgentBrain with enhanced architecture");
    
    // Initialize random generator for world-specific seed
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    rng_ = std::mt19937(seed);
    uniformDist_ = std::uniform_real_distribution<float>(0.0f, 1.0f);
    intDist_ = std::uniform_int_distribution<int>(0, 7);
    
    // Initialize vision memory with world dimensions
    visionWidth_ = world.getVisionWidth();
    visionHeight_ = world.getVisionHeight();
    maxVision_ = visionWidth_ * visionHeight_;
    maxVisionSamples_ = maxVision_;
    maxTouchSamples_ = 8;
    maxInternalSamples_ = 4;
    maxProprioceptionSamples_ = 6;
    
    previousVision_.resize(maxVisionSamples_, 0.0f);
    
    // Initialize neural groups
    initializeNeuralGroups();
    groupsInitialized_ = true;
    
    // Validate initialization
    if (!validate()) {
        NLM_LOG_ERROR("AgentBrain initialization validation failed");
        return false;
    }
    
    NLM_LOG_INFO("AgentBrain initialized successfully");
    NLM_LOG_INFO("  Vision size: " + std::to_string(visionWidth_) + " x " + std::to_string(visionHeight_));
    NLM_LOG_INFO("  Motor groups: " + std::to_string(motorForward_.size() + motorBackward_.size() + 
                 motorTurnLeft_.size() + motorTurnRight_.size() + 
                 motorInteract_.size() + motorWait_.size()));
    NLM_LOG_INFO("  Sensory groups: " + std::to_string(sensoryVision_.size() + sensoryTouch_.size() + 
                 sensoryInternal_.size() + sensoryProprioception_.size()));
    
    return true;
}

void AgentBrain::validateConfig() const {
    NLM_LOG_INFO("Validating AgentBrain configuration");
    
    // Validate vision dimensions
    if (config_.visionWidth == 0 || config_.visionHeight == 0) {
        NLM_LOG_ERROR("Vision dimensions must be greater than 0");
    }
    
    // Validate sensory sizes
    if (config_.touchNeurons == 0) {
        NLM_LOG_ERROR("Touch neuron count must be greater than 0");
    }
    
    if (config_.internalNeurons == 0) {
        NLM_LOG_ERROR("Internal neuron count must be greater than 0");
    }
    
    if (config_.proprioceptionNeurons == 0) {
        NLM_LOG_ERROR("Proprioception neuron count must be greater than 0");
    }
    
    // Validate curiosity parameters
    if (config_.curiosityMin > config_.curiosityMax) {
        NLM_LOG_ERROR("Curiosity min cannot be greater than curiosity max");
    }
    
    // Validate exploration parameters
    if (config_.minExplorationChance > config_.maxExplorationChance) {
        NLM_LOG_ERROR("Minimum exploration chance cannot be greater than maximum");
    }
    
    // Validate developmental durations
    if (config_.initialDuration < 0 || config_.criticalPeriodDuration < 0 || 
        config_.maturationDuration < 0) {
        NLM_LOG_ERROR("Developmental durations cannot be negative");
    }
    
    NLM_LOG_INFO("Configuration validation complete");
}

void AgentBrain::initializeNeuralGroups() {
    NLM_LOG_INFO("Initializing neural groups with enhanced architecture");
    
    neuralGroups_.clear();
    
    // Initialize motor groups
    neuralGroups_[NeuralGroupType::MotorForward] = motorForward_;
    neuralGroups_[NeuralGroupType::MotorBackward] = motorBackward_;
    neuralGroups_[NeuralGroupType::MotorTurnLeft] = motorTurnLeft_;
    neuralGroups_[NeuralGroupType::MotorTurnRight] = motorTurnRight_;
    neuralGroups_[NeuralGroupType::MotorInteract] = motorInteract_;
    neuralGroups_[NeuralGroupType::MotorWait] = motorWait_;
    
    // Initialize sensory groups
    neuralGroups_[NeuralGroupType::Vision] = sensoryVision_;
    neuralGroups_[NeuralGroupType::Touch] = sensoryTouch_;
    neuralGroups_[NeuralGroupType::Internal] = sensoryInternal_;
    neuralGroups_[NeuralGroupType::Proprioception] = sensoryProprioception_;
    
    NLM_LOG_INFO("Neural groups initialized: " + std::to_string(neuralGroups_.size()) + " groups");
}

size_t AgentBrain::getSensoryInputSize() const {
    return maxVision_ + config_.touchNeurons + config_.internalNeurons + config_.proprioceptionNeurons;
}

size_t AgentBrain::getMotorOutputSize() const {
    return config_.maxMotorGroups;
}

bool AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    totalProcessCalls_++;
    
    // Validate input
    if (!validateSensoryInput(percept)) {
        return false;
    }
    
    // Process each sensory modality
    processVisionInput(percept.getVision());
    processTouchInput(percept.getTouch());
    processInternalInput(percept.getInternal());
    processProprioceptionInput(percept.getProprioception());
    
    // Update statistics
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    totalProcessingTime_ += duration.count() / 1000.0;  // Convert to milliseconds
    
    return true;
}

void AgentBrain::validateSensoryInput(const SensoryPercept& percept) const {
    // Validate vision input
    const auto& vision = percept.getVision();
    if (vision.size() > maxVisionSamples_) {
        NLM_LOG_WARNING("Vision input size exceeds maximum expected size: " + 
                       std::to_string(vision.size()) + " > " + std::to_string(maxVisionSamples_));
    }
    
    // Validate touch input
    const auto& touch = percept.getTouch();
    if (touch.size() > maxTouchSamples_) {
        NLM_LOG_WARNING("Touch input size exceeds maximum expected size: " + 
                       std::to_string(touch.size()) + " > " + std::to_string(maxTouchSamples_));
    }
    
    // Validate internal input
    const auto& internal = percept.getInternal();
    if (internal.size() > maxInternalSamples_) {
        NLM_LOG_WARNING("Internal input size exceeds maximum expected size: " + 
                       std::to_string(internal.size()) + " > " + std::to_string(maxInternalSamples_));
    }
    
    // Validate proprioception input
    const auto& proprio = percept.getProprioception();
    if (proprio.size() > maxProprioceptionSamples_) {
        NLM_LOG_WARNING("Proprioception input size exceeds maximum expected size: " + 
                       std::to_string(proprio.size()) + " > " + std::to_string(maxProprioceptionSamples_));
    }
}

void AgentBrain::processVisionInput(const std::vector<float>& vision) {
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Inject current proportional to vision intensity with validation and clamping
            float current = vision[i] * config_.sensoryInputScale;
            current = clampValue(current, -100.0f, 100.0f);
            sensoryVision_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processTouchInput(const std::vector<float>& touch) {
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * config_.touchScale;
            current = clampValue(current, -100.0f, 100.0f);
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processInternalInput(const std::vector<float>& internal) {
    for (size_t i = 0; i < sensoryInternal_.size() && i < internal.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (internal[i] * config_.internalScale - 1.0f) * 5.0f;
            current = clampValue(current, -100.0f, 100.0f);
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processProprioceptionInput(const std::vector<float>& proprio) {
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * config_.proprioceptionScale - 1.0f) * 3.0f;
            current = clampValue(current, -100.0f, 100.0f);
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    totalDecodeCalls_++;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > config_.explorationThreshold) {
        decoded = selectWithCuriosity(decoded);
    }
    
    // Validate output
    validateMotorOutput(decoded);
    
    // Update performance statistics
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    totalProcessingTime_ += duration.count() / 1000.0;
    
    return decoded;
}

void AgentBrain::validateMotorOutput(const MotorCommand& command) const {
    // Basic validation - ensure command is within valid range
    if (static_cast<int>(command) < 0 || static_cast<int>(command) > 8) {
        NLM_LOG_WARNING("Motor command out of expected range: " + std::to_string(static_cast<int>(command)));
    }
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate average activity in each motor group using neural groups
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            if (!n) continue;  // Safety check
            float potential = n->getState().membranePotential;
            float resting = n->getState().restingPotential;
            sum += std::abs(potential - resting);
        }
        return sum / neurons.size();
    };
    
    // Use neural groups for activity calculation
    float forwardAct = calcActivity(neuralGroups_[NeuralGroupType::MotorForward]);
    float backwardAct = calcActivity(neuralGroups_[NeuralGroupType::MotorBackward]);
    float leftAct = calcActivity(neuralGroups_[NeuralGroupType::MotorTurnLeft]);
    float rightAct = calcActivity(neuralGroups_[NeuralGroupType::MotorTurnRight]);
    float interactAct = calcActivity(neuralGroups_[NeuralGroupType::MotorInteract]);
    float waitAct = calcActivity(neuralGroups_[NeuralGroupType::MotorWait]);
    
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
    
    // Only act if there's meaningful activity with improved threshold
    if (bestActivity < config_.activationThreshold) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > config_.explorationThreshold) {
        // Higher curiosity = more exploration
        float exploreChance = config_.explorationChanceMultiplier * curiosityLevel_;
        exploreChance = std::clamp(exploreChance, config_.minExplorationChance, config_.maxExplorationChance);
        
        float r = uniformDist_(rng_);
        if (r < exploreChance) {
            // Random motor command with bounds checking
            int choice = intDist_(rng_);
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

bool AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    // Validate input parameters
    validateRewardValues(reward, predictedReward);
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    dopamineLevel_ = clampValue(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply to all synapses with eligibility traces
    if (brain_) {
        for (const auto& regionPtr : brain_->getRegions()) {
            const auto& region = *regionPtr;
            for (auto* syn : region.getSynapses()) {
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
    }
    
    // Modulate plasticity based on dopamine
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel_;
    plasticityFactor = clampValue(plasticityFactor, 0.1f, 2.0f);
    
    // Apply to STDP
    if (brain_) {
        auto* stdp = brain_->getSTDP();
        if (stdp) {
            stdp->setLTPWeight(0.01f * plasticityFactor);
            stdp->setLTDWeight(0.012f * plasticityFactor);
        }
    }
    
    return true;
}

void AgentBrain::validateRewardValues(float reward, float predictedReward) const {
    if (std::isnan(reward) || std::isinf(reward)) {
        NLM_LOG_ERROR("Invalid reward value: " + std::to_string(reward));
    }
    
    if (std::isnan(predictedReward) || std::isinf(predictedReward)) {
        NLM_LOG_ERROR("Invalid predicted reward value: " + std::to_string(predictedReward));
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // Simple developmental stages based on age
    // This is a biologically inspired approximation
    if (developmentalAge_ < config_.initialDuration) {  // ~1 minute
        plasticityModifier_ = 1.0f;  // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < config_.initialDuration + config_.criticalPeriodDuration) {  // ~5 minutes
        plasticityModifier_ = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < config_.initialDuration + config_.criticalPeriodDuration + config_.maturationDuration) {  // ~15 minutes
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
    
    // Reset statistics
    totalProcessCalls_ = 0;
    totalDecodeCalls_ = 0;
    totalProcessingTime_ = 0.0;
}

bool AgentBrain::validate() const {
    if (!brain_) {
        NLM_LOG_ERROR("Brain is null");
        return false;
    }
    
    if (!groupsInitialized_) {
        NLM_LOG_ERROR("Neural groups not initialized");
        return false;
    }
    
    // Validate brain configuration
    if (!brain_->getConfig()) {
        NLM_LOG_ERROR("Brain configuration is null");
        return false;
    }
    
    // Validate neural groups
    for (const auto& pair : neuralGroups_) {
        const auto& group = pair.second;
        for (const auto* neuron : group) {
            if (!neuron) {
                NLM_LOG_ERROR("Invalid neuron in neural group");
                return false;
            }
        }
    }
    
    return true;
}

void AgentBrain::logStatus() const {
    NLM_LOG_INFO("=== AgentBrain Status (Enhanced) ===");
    NLM_LOG_INFO("Brain: " + std::to_string(brain_ ? 1 : 0));
    NLM_LOG_INFO("Neural groups: " + std::to_string(neuralGroups_.size()));
    NLM_LOG_INFO("Motor groups: " + std::to_string(motorForward_.size() + motorBackward_.size() + 
                 motorTurnLeft_.size() + motorTurnRight_.size() + 
                 motorInteract_.size() + motorWait_.size()));
    NLM_LOG_INFO("Sensory groups: " + std::to_string(sensoryVision_.size() + sensoryTouch_.size() + 
                 sensoryInternal_.size() + sensoryProprioception_.size()));
    NLM_LOG_INFO("Developmental stage: " + std::to_string(static_cast<int>(getDevelopmentalStage())));
    NLM_LOG_INFO("Curiosity level: " + std::to_string(curiosityLevel_));
    NLM_LOG_INFO("Novelty level: " + std::to_string(noveltyLevel_));
    NLM_LOG_INFO("Neuromodulation level: " + std::to_string(dopamineLevel_));
    NLM_LOG_INFO("Prediction error: " + std::to_string(predictionError_));
    NLM_LOG_INFO("Processing calls: " + std::to_string(totalProcessCalls_));
    NLM_LOG_INFO("Decode calls: " + std::to_string(totalDecodeCalls_));
    NLM_LOG_INFO("Average processing time: " + std::to_string(totalProcessingTime_ / std::max<size_t>(totalProcessCalls_, 1)) + " ms");
}

float AgentBrain::clampValue(float value, float min, float max) const {
    return std::max(std::min(value, max), min);
}

NeuralGroupType AgentBrain::classifyNeuron(Neuron* neuron) const {
    if (!neuron) return NeuralGroupType::Unknown;
    
    // Check motor neuron groups
    if (std::find(motorForward_.begin(), motorForward_.end(), neuron) != motorForward_.end()) {
        return NeuralGroupType::MotorForward;
    }
    if (std::find(motorBackward_.begin(), motorBackward_.end(), neuron) != motorBackward_.end()) {
        return NeuralGroupType::MotorBackward;
    }
    if (std::find(motorTurnLeft_.begin(), motorTurnLeft_.end(), neuron) != motorTurnLeft_.end()) {
        return NeuralGroupType::MotorTurnLeft;
    }
    if (std::find(motorTurnRight_.begin(), motorTurnRight_.end(), neuron) != motorTurnRight_.end()) {
        return NeuralGroupType::MotorTurnRight;
    }
    if (std::find(motorInteract_.begin(), motorInteract_.end(), neuron) != motorInteract_.end()) {
        return NeuralGroupType::MotorInteract;
    }
    if (std::find(motorWait_.begin(), motorWait_.end(), neuron) != motorWait_.end()) {
        return NeuralGroupType::MotorWait;
    }
    
    // Check sensory neuron groups
    if (std::find(sensoryVision_.begin(), sensoryVision_.end(), neuron) != sensoryVision_.end()) {
        return NeuralGroupType::Vision;
    }
    if (std::find(sensoryTouch_.begin(), sensoryTouch_.end(), neuron) != sensoryTouch_.end()) {
        return NeuralGroupType::Touch;
    }
    if (std::find(sensoryInternal_.begin(), sensoryInternal_.end(), neuron) != sensoryInternal_.end()) {
        return NeuralGroupType::Internal;
    }
    if (std::find(sensoryProprioception_.begin(), sensoryProprioception_.end(), neuron) != sensoryProprioception_.end()) {
        return NeuralGroupType::Proprioception;
    }
    
    return NeuralGroupType::Unknown;
}

void AgentBrain::optimizeNeuralGroups() {
    NLM_LOG_INFO("Optimizing neural groups");
    
    // Reorganize neural groups for better performance
    updateNeuralGroupClassifications();
    optimizeGroupAssignments();
}

void AgentBrain::updateNeuralGroupClassifications() {
    // Reclassify neurons based on current state
    std::unordered_map<NeuralGroupType, std::vector<Neuron*>> newGroups;
    
    // Find all neurons from brain
    if (brain_) {
        for (const auto& regionPtr : brain_->getRegions()) {
            const auto& region = *regionPtr;
            for (const auto& popPtr : region.getPopulations()) {
                const auto& population = *popPtr;
                const auto& neurons = population.getNeurons();
                for (Neuron* neuron : neurons) {
                    NeuralGroupType type = classifyNeuron(neuron);
                    newGroups[type].push_back(neuron);
                }
            }
        }
    }
    
    // Update neural groups
    neuralGroups_ = std::move(newGroups);
}

void AgentBrain::optimizeGroupAssignments() {
    // Balance group sizes for better performance
    size_t totalMotorNeurons = 0;
    size_t totalSensoryNeurons = 0;
    
    for (const auto& pair : neuralGroups_) {
        switch (pair.first) {
            case NeuralGroupType::MotorForward:
            case NeuralGroupType::MotorBackward:
            case NeuralGroupType::MotorTurnLeft:
            case NeuralGroupType::MotorTurnRight:
            case NeuralGroupType::MotorInteract:
            case NeuralGroupType::MotorWait:
                totalMotorNeurons += pair.second.size();
                break;
            case NeuralGroupType::Vision:
            case NeuralGroupType::Touch:
            case NeuralGroupType::Internal:
            case NeuralGroupType::Proprioception:
                totalSensoryNeurons += pair.second.size();
                break;
        }
    }
    
    NLM_LOG_INFO("Optimized neural groups: " + std::to_string(totalMotorNeurons) + 
                 " motor, " + std::to_string(totalSensoryNeurons) + " sensory neurons");
}
bool AgentBrain::batchProcessSensoryInputs(const std::vector<SensoryPercept>& percepts) {
    NLM_LOG_INFO("Batch processing " + std::to_string(percepts.size()) + " sensory inputs");
    
    bool allSuccessful = true;
    for (const auto& percept : percepts) {
        if (!processSensoryInput(percept)) {
            allSuccessful = false;
        }
    }
    
    return allSuccessful;
}

std::vector<MotorCommand> AgentBrain::batchDecodeMotorCommands(size_t count) {
    std::vector<MotorCommand> commands;
    commands.reserve(count);
    
    for (size_t i = 0; i < count; ++i) {
        commands.push_back(decodeMotorCommand());
    }
    
    return commands;
}

AgentBrain::NeuralStats AgentBrain::getStatistics() const {
    NeuralStats stats;
    
    // Count neurons
    for (const auto& pair : neuralGroups_) {
        stats.totalNeurons += pair.second.size();
        for (const auto* neuron : pair.second) {
            if (neuron->isActive()) {
                stats.activeNeurons++;
            }
            if (neuron->isFiring()) {
                stats.firingNeurons++;
            }
        }
    }
    
    // Calculate average firing rate
    if (brain_) {
        stats.averageFiringRate = brain_->getAverageFiringRate();
        stats.excitationInhibitionRatio = brain_->getExcitationInhibitionRatio();
    }
    
    // Copy state information
    stats.curiosityLevel = curiosityLevel_;
    stats.noveltyLevel = noveltyLevel_;
    stats.developmentalAge = developmentalAge_;
    stats.plasticityModifier = plasticityModifier_;
    
    return stats;
}

void AgentBrain::analyzeNeuralActivity() const {
    NLM_LOG_INFO("Analyzing neural activity patterns");
    
    NeuralStats stats = getStatistics();
    
    NLM_LOG_INFO("Neural Activity Analysis:");
    NLM_LOG_INFO("  Total neurons: " + std::to_string(stats.totalNeurons));
    NLM_LOG_INFO("  Active neurons: " + std::to_string(stats.activeNeurons));
    NLM_LOG_INFO("  Firing neurons: " + std::to_string(stats.firingNeurons));
    NLM_LOG_INFO("  Average firing rate: " + std::to_string(stats.averageFiringRate));
    NLM_LOG_INFO("  Excitation/inhibition ratio: " + std::to_string(stats.excitationInhibitionRatio));
}

void AgentBrain::profilePerformance() const {
    NLM_LOG_INFO("Profiling AgentBrain performance");
    
    if (totalProcessCalls_ == 0) {
        NLM_LOG_INFO("  No processing calls recorded");
        return;
    }
    
    double avgProcessingTime = totalProcessingTime_ / totalProcessCalls_;
    double avgDecodeTime = totalProcessingTime_ / totalDecodeCalls_;
    
    NLM_LOG_INFO("Performance Profile:");
    NLM_LOG_INFO("  Total processing calls: " + std::to_string(totalProcessCalls_));
    NLM_LOG_INFO("  Total decode calls: " + std::to_string(totalDecodeCalls_));
    NLM_LOG_INFO("  Average processing time: " + std::to_string(avgProcessingTime) + " ms");
    NLM_LOG_INFO("  Average decode time: " + std::to_string(avgDecodeTime) + " ms");
    NLM_LOG_INFO("  Processing efficiency: " + std::to_string(1000.0 / avgProcessingTime) + " calls/sec");
}

} // namespace nlm
