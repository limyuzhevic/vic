#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <numeric>
#include <unordered_map>
#include <stdexcept>

namespace nlm {

// AgentBrainConfig constructor implementation
AgentBrainConfig::AgentBrainConfig()
    : distributionStrategy_(DistributionStrategy::Uniform)
    , visionScaleFactor_(DEFAULT_VISION_SCALE)
    , touchScaleFactor_(DEFAULT_TOUCH_SCALE)
    , internalScaleFactor_(DEFAULT_INTERNAL_SCALE)
    , proprioceptionScaleFactor_(DEFAULT_PROPRIOCEPTION_SCALE)
    , curiosityThreshold_(DEFAULT_CURIOUSITY_THRESHOLD)
    , explorationThreshold_(DEFAULT_EXPLORATION_THRESHOLD)
    , noveltyDecayRate_(DEFAULT_NOVELTY_DECAY)
    , curiosityGain_(DEFAULT_CURIOUSITY_GAIN)
    , predictionErrorWeight_(DEFAULT_PREDICTION_ERROR_WEIGHT)
    , motorActivationThreshold_(DEFAULT_MOTOR_ACTIVATION_THRESHOLD)
    , developmentalStageStart_(60.0)
    , criticalPeriodEnd_(300.0)
    , maturationEnd_(900.0)
    , strictValidation_(true)
    , logWarnings_(true)
{
    // Initialize weight vectors with default values
    motorGroupWeights_ = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};  // [forward, backward, left, right, interact, wait]
    sensoryGroupWeights_ = {1.0f, 1.0f, 1.0f, 1.0f};              // [vision, touch, internal, proprioception]
    motorCommandWeights_ = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};  // [forward, backward, left, right, lookleft, lookright, interact, wait]
}

// AgentBrain constructor implementations
AgentBrain::AgentBrain(std::shared_ptr<Brain> brain) : AgentBrain(brain, AgentBrainConfig()) {}

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain, const AgentBrainConfig& config) : config_(config) {
    validateConfig();
    
    brain_ = std::move(brain);
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    rewardModulationEnabled_ = true;
    structuralPlasticityEnabled_ = true;
    developmentEnabled_ = true;
    curiosityEnabled_ = true;
    sensoryNoveltyDecay_ = config_.noveltyDecayRate_;
    
    // Initialize neuron groups using configurable distribution
    if (brain_) {
        redistributeNeurons();
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    validateInput(world);
    
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
}

void AgentBrain::validateConfig() const {
    if (config_.strictValidation_) {
        if (config_.visionScaleFactor_ <= 0.0f) {
            throw std::invalid_argument("Vision scale factor must be positive");
        }
        if (config_.touchScaleFactor_ <= 0.0f) {
            throw std::invalid_argument("Touch scale factor must be positive");
        }
        if (config_.internalScaleFactor_ <= 0.0f) {
            throw std::invalid_argument("Internal scale factor must be positive");
        }
        if (config_.proprioceptionScaleFactor_ <= 0.0f) {
            throw std::invalid_argument("Proprioception scale factor must be positive");
        }
        if (config_.curiosityThreshold_ < 0.0f || config_.curiosityThreshold_ > 1.0f) {
            throw std::invalid_argument("Curiosity threshold must be between 0.0 and 1.0");
        }
        if (config_.explorationThreshold_ < 0.0f || config_.explorationThreshold_ > 1.0f) {
            throw std::invalid_argument("Exploration threshold must be between 0.0 and 1.0");
        }
        if (config_.noveltyDecayRate_ <= 0.0f || config_.noveltyDecayRate_ > 1.0f) {
            throw std::invalid_argument("Novelty decay rate must be between 0.0 and 1.0");
        }
        if (config_.motorActivationThreshold_ < 0.0f || config_.motorActivationThreshold_ > 1.0f) {
            throw std::invalid_argument("Motor activation threshold must be between 0.0 and 1.0");
        }
        
        // Validate weight vectors
        if (config_.motorGroupWeights_.size() != 6) {
            throw std::invalid_argument("Motor group weights must have exactly 6 elements");
        }
        if (config_.sensoryGroupWeights_.size() != 4) {
            throw std::invalid_argument("Sensory group weights must have exactly 4 elements");
        }
        
        normalizeWeights(config_.motorGroupWeights_);
        normalizeWeights(config_.sensoryGroupWeights_);
    }
}

void AgentBrain::validateSensoryInput(const SensoryPercept& percept) const {
    if (config_.strictValidation_) {
        const auto& vision = percept.getVision();
        const auto& touch = percept.getTouch();
        const auto& intern = percept.getInternal();
        const auto& proprio = percept.getProprioception();
        
        if (!vision.empty()) {
            for (float val : vision) {
                if (std::isnan(val) || std::isinf(val)) {
                    throw std::invalid_argument("Vision input contains invalid values (NaN or infinity)");
                }
            }
        }
        
        for (float val : touch) {
            if (std::isnan(val) || std::isinf(val)) {
                throw std::invalid_argument("Touch input contains invalid values (NaN or infinity)");
            }
        }
        
        for (float val : intern) {
            if (std::isnan(val) || std::isinf(val)) {
                throw std::invalid_argument("Internal input contains invalid values (NaN or infinity)");
            }
        }
        
        for (float val : proprio) {
            if (std::isnan(val) || std::isinf(val)) {
                throw std::invalid_argument("Proprioception input contains invalid values (NaN or infinity)");
            }
        }
    }
}

void AgentBrain::validateMotorCommand(MotorCommand& command) const {
    // Basic validation - ensure command is valid (this would check against MotorCommand enum values in a real implementation)
    if (config_.strictValidation_) {
        // Additional validation could be added here
    }
}

void AgentBrain::logWarning(const std::string& message) const {
    if (config_.logWarnings_) {
        NLM_LOG_WARNING(message);
    }
}

void AgentBrain::setConfig(const AgentBrainConfig& config) {
    config_ = config;
    validateConfig();
    logWarning("AgentBrain configuration updated");
}

// Helper functions implementation
void AgentBrain::normalizeWeights(std::vector<float>& weights) const {
    float sum = std::accumulate(weights.begin(), weights.end(), 0.0f);
    if (sum > 0.0f) {
        for (float& weight : weights) {
            weight /= sum;
        }
    } else {
        // If all weights are zero or sum is zero, use uniform distribution
        float uniformWeight = 1.0f / weights.size();
        for (float& weight : weights) {
            weight = uniformWeight;
        }
    }
}

size_t AgentBrain::calculateWeightedIndex(const std::vector<float>& weights, size_t totalNeurons) const {
    if (weights.empty() || totalNeurons == 0) {
        return 0;
    }
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::discrete_distribution<size_t> dist(weights.begin(), weights.end());
    return dist(gen) % weights.size();
}

float AgentBrain::calculateSensoryCurrent(float input, float scale, float offset, float center) const {
    // Center and scale input to range [-1, 1], then apply scale
    float normalized = (input * 2.0f - 1.0f) * center;
    return normalized * scale;
}

float AgentBrain::calculateMotorActivity(const std::vector<Neuron*>& neurons) const {
    if (neurons.empty()) {
        return 0.0f;
    }
    
    float sum = 0.0f;
    for (const Neuron* n : neurons) {
        // Use membrane potential deviation from rest as activity measure
        float deviation = std::abs(n->getState().membranePotential - n->getState().restingPotential);
        sum += deviation;
    }
    
    return sum / neurons.size();
}

float AgentBrain::calculateNovelty(const std::vector<float>& current, const std::vector<float>& previous) const {
    if (current.empty()) {
        return 0.0f;
    }
    
    float totalDiff = 0.0f;
    size_t minSize = std::min(current.size(), previous.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        float diff = std::abs(current[i] - previous[i]);
        totalDiff += diff;
    }
    
    return totalDiff / std::max<size_t>(current.size(), 1);
}

void AgentBrain::validateInput(const SimpleWorld& world) const {
    if (config_.strictValidation_) {
        if (world.getVisionWidth() <= 0 || world.getVisionHeight() <= 0) {
            throw std::invalid_argument("World must have positive dimensions");
        }
    }
}

void AgentBrain::redistributeNeurons() {
    // Clear existing neuron groups
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
    
    if (!brain_) {
        logWarning("Cannot redistribute neurons - brain pointer is null");
        return;
    }
    
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            size_t totalNeurons = pop->getNeurons().size();
            
            if (type == NeuronType::Motor) {
                // Distribute motor neurons based on configuration
                distributeMotorNeurons(pop->getNeurons(), totalNeurons);
            } else if (type == NeuronType::Sensory) {
                // Distribute sensory neurons based on configuration
                distributeSensoryNeurons(pop->getNeurons(), totalNeurons);
            }
        }
    }
}

void AgentBrain::distributeMotorNeurons(const std::vector<Neuron*>& neurons, size_t totalNeurons) {
    switch (config_.distributionStrategy_) {
        case DistributionStrategy::Uniform:
            distributeMotorUniform(neurons);
            break;
        case DistributionStrategy::Weighted:
            distributeMotorWeighted(neurons, totalNeurons);
            break;
        case DistributionStrategy::Random:
            distributeMotorRandom(neurons);
            break;
        case DistributionStrategy::Custom:
            if (config_.customDistributionFunc_) {
                distributeMotorCustom(neurons, totalNeurons);
            } else {
                logWarning("Custom distribution function not set, falling back to uniform");
                distributeMotorUniform(neurons);
            }
            break;
    }
}

void AgentBrain::distributeSensoryNeurons(const std::vector<Neuron*>& neurons, size_t totalNeurons) {
    switch (config_.distributionStrategy_) {
        case DistributionStrategy::Uniform:
            distributeSensoryUniform(neurons);
            break;
        case DistributionStrategy::Weighted:
            distributeSensoryWeighted(neurons, totalNeurons);
            break;
        case DistributionStrategy::Random:
            distributeSensoryRandom(neurons);
            break;
        case DistributionStrategy::Custom:
            if (config_.customDistributionFunc_) {
                distributeSensoryCustom(neurons, totalNeurons);
            } else {
                logWarning("Custom distribution function not set, falling back to uniform");
                distributeSensoryUniform(neurons);
            }
            break;
    }
}

void AgentBrain::distributeMotorUniform(const std::vector<Neuron*>& neurons) {
    size_t groupSize = neurons.size() / 6;
    size_t remainder = neurons.size() % 6;
    
    size_t index = 0;
    for (size_t i = 0; i < 6; ++i) {
        size_t currentGroupSize = groupSize + (i < remainder ? 1 : 0);
        
        switch (static_cast<MotorGroup>(i)) {
            case MotorGroup::Forward:
                for (size_t j = 0; j < currentGroupSize; ++j) {
                    if (index < neurons.size()) {
                        motorForward_.push_back(neurons[index++]);
                    }
                }
                break;
            case MotorGroup::Backward:
                for (size_t j = 0; j < currentGroupSize; ++j) {
                    if (index < neurons.size()) {
                        motorBackward_.push_back(neurons[index++]);
                    }
                }
                break;
            case MotorGroup::Left:
                for (size_t j = 0; j < currentGroupSize; ++j) {
                    if (index < neurons.size()) {
                        motorTurnLeft_.push_back(neurons[index++]);
                    }
                }
                break;
            case MotorGroup::Right:
                for (size_t j = 0; j < currentGroupSize; ++j) {
                    if (index < neurons.size()) {
                        motorTurnRight_.push_back(neurons[index++]);
                    }
                }
                break;
            case MotorGroup::Interact:
                for (size_t j = 0; j < currentGroupSize; ++j) {
                    if (index < neurons.size()) {
                        motorInteract_.push_back(neurons[index++]);
                    }
                }
                break;
            case MotorGroup::Wait:
                for (size_t j = 0; j < currentGroupSize; ++j) {
                    if (index < neurons.size()) {
                        motorWait_.push_back(neurons[index++]);
                    }
                }
                break;
        }
    }
}

void AgentBrain::distributeMotorWeighted(const std::vector<Neuron*>& neurons, size_t totalNeurons) {
    if (config_.motorGroupWeights_.empty()) {
        distributeMotorUniform(neurons);
        return;
    }
    
    size_t neuronsPerGroup = totalNeurons / 6;
    size_t remainder = totalNeurons % 6;
    
    std::vector<std::vector<Neuron*>>& groups = {
        motorForward_, motorBackward_, motorTurnLeft_, 
        motorTurnRight_, motorInteract_, motorWait_
    };
    
    for (size_t i = 0; i < 6; ++i) {
        size_t targetCount = neuronsPerGroup + (i < remainder ? 1 : 0);
        targetCount = static_cast<size_t>(static_cast<float>(targetCount) * config_.motorGroupWeights_[i]);
        
        size_t allocated = 0;
        while (allocated < targetCount && !neurons.empty()) {
            size_t idx = calculateWeightedIndex(config_.motorGroupWeights_, neurons.size());
            if (idx == i && allocated < targetCount) {
                groups[i].push_back(neurons.back());
                neurons.pop_back();
                allocated++;
            }
        }
    }
}

void AgentBrain::distributeMotorRandom(const std::vector<Neuron*>& neurons) {
    std::vector<Neuron*> shuffledNeurons = neurons;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(shuffledNeurons.begin(), shuffledNeurons.end(), gen);
    
    size_t groupSize = shuffledNeurons.size() / 6;
    size_t remainder = shuffledNeurons.size() % 6;
    
    size_t index = 0;
    for (size_t i = 0; i < 6; ++i) {
        size_t currentGroupSize = groupSize + (i < remainder ? 1 : 0);
        
        switch (static_cast<MotorGroup>(i)) {
            case MotorGroup::Forward:
                for (size_t j = 0; j < currentGroupSize && index < shuffledNeurons.size(); ++j) {
                    motorForward_.push_back(shuffledNeurons[index++]);
                }
                break;
            case MotorGroup::Backward:
                for (size_t j = 0; j < currentGroupSize && index < shuffledNeurons.size(); ++j) {
                    motorBackward_.push_back(shuffledNeurons[index++]);
                }
                break;
            case MotorGroup::Left:
                for (size_t j = 0; j < currentGroupSize && index < shuffledNeurons.size(); ++j) {
                    motorTurnLeft_.push_back(shuffledNeurons[index++]);
                }
                break;
            case MotorGroup::Right:
                for (size_t j = 0; j < currentGroupSize && index < shuffledNeurons.size(); ++j) {
                    motorTurnRight_.push_back(shuffledNeurons[index++]);
                }
                break;
            case MotorGroup::Interact:
                for (size_t j = 0; j < currentGroupSize && index < shuffledNeurons.size(); ++j) {
                    motorInteract_.push_back(shuffledNeurons[index++]);
                }
                break;
            case MotorGroup::Wait:
                for (size_t j = 0; j < currentGroupSize && index < shuffledNeurons.size(); ++j) {
                    motorWait_.push_back(shuffledNeurons[index++]);
                }
                break;
        }
    }
}

void AgentBrain::distributeMotorCustom(const std::vector<Neuron*>& neurons, size_t totalNeurons) {
    if (!config_.customDistributionFunc_) {
        return;
    }
    
    // Call custom distribution function
    std::vector<size_t> distribution = config_.customDistributionFunc_(totalNeurons, neurons);
    
    if (distribution.size() != 6) {
        logWarning("Custom distribution function returned invalid size, using uniform distribution");
        distributeMotorUniform(neurons);
        return;
    }
    
    std::vector<std::vector<Neuron*>>& groups = {
        motorForward_, motorBackward_, motorTurnLeft_, 
        motorTurnRight_, motorInteract_, motorWait_
    };
    
    size_t neuronIndex = 0;
    for (size_t i = 0; i < 6; ++i) {
        for (size_t j = 0; j < distribution[i] && neuronIndex < neurons.size(); ++j) {
            groups[i].push_back(neurons[neuronIndex++]);
        }
    }
}

void AgentBrain::distributeSensoryUniform(const std::vector<Neuron*>& neurons) {
    size_t groupSize = neurons.size() / 4;
    size_t remainder = neurons.size() % 4;
    
    size_t index = 0;
    for (size_t i = 0; i < 4; ++i) {
        size_t currentGroupSize = groupSize + (i < remainder ? 1 : 0);
        
        switch (static_cast<SensoryGroup>(i)) {
            case SensoryGroup::Vision:
                for (size_t j = 0; j < currentGroupSize && index < neurons.size(); ++j) {
                    sensoryVision_.push_back(neurons[index++]);
                }
                break;
            case SensoryGroup::Touch:
                for (size_t j = 0; j < currentGroupSize && index < neurons.size(); ++j) {
                    sensoryTouch_.push_back(neurons[index++]);
                }
                break;
            case SensoryGroup::Internal:
                for (size_t j = 0; j < currentGroupSize && index < neurons.size(); ++j) {
                    sensoryInternal_.push_back(neurons[index++]);
                }
                break;
            case SensoryGroup::Proprioception:
                for (size_t j = 0; j < currentGroupSize && index < neurons.size(); ++j) {
                    sensoryProprioception_.push_back(neurons[index++]);
                }
                break;
        }
    }
}

void AgentBrain::distributeSensoryWeighted(const std::vector<Neuron*>& neurons, size_t totalNeurons) {
    if (config_.sensoryGroupWeights_.empty()) {
        distributeSensoryUniform(neurons);
        return;
    }
    
    size_t neuronsPerGroup = totalNeurons / 4;
    size_t remainder = totalNeurons % 4;
    
    std::vector<std::vector<Neuron*>>& groups = {
        sensoryVision_, sensoryTouch_, sensoryInternal_, sensoryProprioception_
    };
    
    for (size_t i = 0; i < 4; ++i) {
        size_t targetCount = neuronsPerGroup + (i < remainder ? 1 : 0);
        targetCount = static_cast<size_t>(static_cast<float>(targetCount) * config_.sensoryGroupWeights_[i]);
        
        size_t allocated = 0;
        while (allocated < targetCount && !neurons.empty()) {
            size_t idx = calculateWeightedIndex(config_.sensoryGroupWeights_, neurons.size());
            if (idx == i && allocated < targetCount) {
                groups[i].push_back(neurons.back());
                neurons.pop_back();
                allocated++;
            }
        }
    }
}

void AgentBrain::distributeSensoryRandom(const std::vector<Neuron*>& neurons) {
    std::vector<Neuron*> shuffledNeurons = neurons;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(shuffledNeurons.begin(), shuffledNeurons.end(), gen);
    
    size_t groupSize = shuffledNeurons.size() / 4;
    size_t remainder = shuffledNeurons.size() % 4;
    
    size_t index = 0;
    for (size_t i = 0; i < 4; ++i) {
        size_t currentGroupSize = groupSize + (i < remainder ? 1 : 0);
        
        switch (static_cast<SensoryGroup>(i)) {
            case SensoryGroup::Vision:
                for (size_t j = 0; j < currentGroupSize && index < shuffledNeurons.size(); ++j) {
                    sensoryVision_.push_back(shuffledNeurons[index++]);
                }
                break;
            case SensoryGroup::Touch:
                for (size_t j = 0; j < currentGroupSize && index < shuffledNeurons.size(); ++j) {
                    sensoryTouch_.push_back(shuffledNeurons[index++]);
                }
                break;
            case SensoryGroup::Internal:
                for (size_t j = 0; j < currentGroupSize && index < shuffledNeurons.size(); ++j) {
                    sensoryInternal_.push_back(shuffledNeurons[index++]);
                }
                break;
            case SensoryGroup::Proprioception:
                for (size_t j = 0; j < currentGroupSize && index < shuffledNeurons.size(); ++j) {
                    sensoryProprioception_.push_back(shuffledNeurons[index++]);
                }
                break;
        }
    }
}

void AgentBrain::distributeSensoryCustom(const std::vector<Neuron*>& neurons, size_t totalNeurons) {
    if (!config_.customDistributionFunc_) {
        return;
    }
    
    // Call custom distribution function
    std::vector<size_t> distribution = config_.customDistributionFunc_(totalNeurons, neurons);
    
    if (distribution.size() != 4) {
        logWarning("Custom distribution function returned invalid size, using uniform distribution");
        distributeSensoryUniform(neurons);
        return;
    }
    
    std::vector<std::vector<Neuron*>>& groups = {
        sensoryVision_, sensoryTouch_, sensoryInternal_, sensoryProprioception_
    };
    
    size_t neuronIndex = 0;
    for (size_t i = 0; i < 4; ++i) {
        for (size_t j = 0; j < distribution[i] && neuronIndex < neurons.size(); ++j) {
            groups[i].push_back(neurons[neuronIndex++]);
        }
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
    if (!brain_) return;
    
    validateSensoryInput(percept);
    
    // Process each sensory modality using helper functions
    processVisionInput(percept);
    processTouchInput(percept);
    processInternalInput(percept);
    processProprioceptionInput(percept);
    
    // Compute novelty and update curiosity
    computeNoveltyAndCuriosity(percept);
}

void AgentBrain::processVisionInput(const SensoryPercept& percept) {
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            float current = calculateSensoryCurrent(vision[i], config_.visionScaleFactor_, 1.0f, 1.0f);
            sensoryVision_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processTouchInput(const SensoryPercept& percept) {
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = calculateSensoryCurrent(touch[i], config_.touchScaleFactor_);
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processInternalInput(const SensoryPercept& percept) {
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = calculateSensoryCurrent(intern[i], config_.internalScaleFactor_, 1.0f, 2.0f);
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processProprioceptionInput(const SensoryPercept& percept) {
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = calculateSensoryCurrent(proprio[i], config_.proprioceptionScaleFactor_, 1.0f, 2.0f);
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::computeNoveltyAndCuriosity(const SensoryPercept& percept) {
    const auto& vision = percept.getVision();
    
    if (!vision.empty()) {
        noveltyLevel_ = calculateNovelty(vision, previousVision_);
        
        // Decay and update
        noveltyLevel_ *= config_.noveltyDecayRate_;
        
        // Store for next time
        previousVision_ = vision;
    }
    
    // Update curiosity based on novelty and prediction error
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * config_.curiosityGain_ + 
                         std::abs(predictionError_) * config_.predictionErrorWeight_;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration with configurable threshold
    if (curiosityEnabled_ && curiosityLevel_ > config_.curiosityThreshold_) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate average activity in each motor group using helper function
    float forwardAct = calculateMotorActivity(motorForward_);
    float backwardAct = calculateMotorActivity(motorBackward_);
    float leftAct = calculateMotorActivity(motorTurnLeft_);
    float rightAct = calculateMotorActivity(motorTurnRight_);
    float interactAct = calculateMotorActivity(motorInteract_);
    float waitAct = calculateMotorActivity(motorWait_);
    
    // Create command structure for processing
    std::vector<MotorCommandInfo> commands = createMotorCommandStructure(
        forwardAct, backwardAct, leftAct, rightAct, interactAct, waitAct
    );
    
    return selectBestMotorCommand(commands);
}

MotorCommand AgentBrain::selectBestMotorCommand(const std::vector<MotorCommandInfo>& commands) {
    if (commands.empty()) {
        return MotorCommand::Wait;
    }
    
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = 0.0f;
    
    for (const auto& cmdInfo : commands) {
        // Apply motor command weights
        float weightedActivity = cmdInfo.activity * config_.motorCommandWeights_[cmdInfo.command];
        
        if (weightedActivity > bestActivity) {
            bestActivity = weightedActivity;
            best = cmdInfo.command;
        }
    }
    
    // Only act if there's meaningful activity (configurable threshold)
    if (bestActivity < config_.motorActivationThreshold_) {
        return MotorCommand::Wait;
    }
    
    return best;
}

std::vector<MotorCommandInfo> AgentBrain::createMotorCommandStructure(
    float forwardAct, float backwardAct, float leftAct, float rightAct, 
    float interactAct, float waitAct) {
    
    std::vector<MotorCommandInfo> commands;
    commands.reserve(6);
    
    commands.push_back({MotorCommand::MoveForward, forwardAct});
    commands.push_back({MotorCommand::MoveBackward, backwardAct});
    commands.push_back({MotorCommand::TurnLeft, leftAct});
    commands.push_back({MotorCommand::TurnRight, rightAct});
    commands.push_back({MotorCommand::Interact, interactAct});
    commands.push_back({MotorCommand::Wait, waitAct});
    
    return commands;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Advanced curiosity-based exploration with configurable thresholds
    if (curiosityLevel_ > config_.explorationThreshold_) {
        // Calculate exploration probability based on curiosity
        float exploreChance = curiosityLevel_ * config_.curiosityGain_ * 0.3f;  // Up to 30% random
        
        // Ensure exploreChance is within [0, 1]
        exploreChance = std::clamp(exploreChance, 0.0f, 1.0f);
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            return selectRandomMotorCommand();
        }
    }
    
    return defaultCmd;
}

MotorCommand AgentBrain::selectRandomMotorCommand() {
    // Use weighted random selection based on motor command weights
    if (config_.motorCommandWeights_.size() < 8) {
        // Default to 8 possible commands: MoveForward, MoveBackward, TurnLeft, TurnRight, 
        // LookLeft, LookRight, Interact, Wait
        std::discrete_distribution<size_t> dist(
            config_.motorCommandWeights_.begin(), 
            config_.motorCommandWeights_.begin() + std::min<size_t>(8, config_.motorCommandWeights_.size())
        );
        
        std::random_device rd;
        std::mt19937 gen(rd());
        size_t choice = dist(gen);
        
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
    
    // If we have more weights, use all of them
    std::discrete_distribution<size_t> dist(
        config_.motorCommandWeights_.begin(), 
        config_.motorCommandWeights_.end()
    );
    
    std::random_device rd;
    std::mt19937 gen(rd());
    size_t choice = dist(gen);
    
    // Map index to motor command (simplified mapping)
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

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Validate inputs
    if (std::isnan(reward) || std::isinf(reward)) {
        logWarning("Invalid reward value: " + std::to_string(reward));
        return;
    }
    if (std::isnan(predictedReward) || std::isinf(predictedReward)) {
        logWarning("Invalid predicted reward value: " + std::to_string(predictedReward));
        return;
    }
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply to all synapses with eligibility traces
    applyRewardToSynapses(dopamineLevel_);
    
    // Modulate plasticity based on dopamine
    updatePlasticityBasedOnDopamine(dopamineLevel_);
}

void AgentBrain::applyRewardToSynapses(float dopamineLevel) {
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel * plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
}

void AgentBrain::updatePlasticityBasedOnDopamine(float dopamineLevel) {
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel;
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
    
    // Use configurable developmental stages
    updateDevelopmentalStage();
    
    // Structural plasticity changes with development
    if (structuralPlasticityEnabled_) {
        updateStructuralPlasticity();
    }
}

void AgentBrain::updateDevelopmentalStage() {
    if (developmentalAge_ < config_.developmentalStageStart_) {  // ~1 minute
        plasticityModifier_ = 1.0f;  // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < config_.criticalPeriodEnd_) {  // ~5 minutes
        plasticityModifier_ = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < config_.maturationEnd_) {  // ~15 minutes
        plasticityModifier_ = 0.5f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = 0.2f;  // Adult - more stable
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
}

void AgentBrain::updateStructuralPlasticity() {
    auto* sp = brain_->getStructuralPlasticity();
    if (sp) {
        // Higher synaptogenesis in early development
        float synRate = 0.0001f * plasticityModifier_;
        float pruneRate = 0.00001f * (2.0f - plasticityModifier_);
        sp->setSynaptogenesisRate(synRate);
        sp->setPruningRate(pruneRate);
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