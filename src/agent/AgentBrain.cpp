#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include "MotorDecoder.hpp"
#include "SensoryProcessor.hpp"
#include "NeuromodulationManager.hpp"
#include "DevelopmentManager.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(brain)
    , motorDecoder_(std::make_shared<MotorDecoder>(brain))
    , sensoryProcessor_(std::make_shared<SensoryProcessor>(brain))
    , neuromodulationManager_(std::make_shared<NeuromodulationManager>(brain))
    , developmentManager_(std::make_shared<DevelopmentManager>(brain))
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
    , sensoryNoveltyDecay_(0.99f) {
    // Initialize motor and sensory neuron groups for backward compatibility
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
    
    // Initialize component systems
    motorDecoder_->setSensoryNeuronGroups(
        sensoryVision_, sensoryTouch_, sensoryInternal_, sensoryProprioception_);
    
    NLM_LOG_INFO("AgentBrain initialized with component-based architecture");
}

AgentBrain::~AgentBrain() {
    NLM_LOG_INFO("AgentBrain destroyed");
}

void AgentBrain::initialize(const SimpleWorld& world) {
    NLM_LOG_INFO("AgentBrain initializing with component architecture");
    
    // Initialize the sensory processor
    sensoryProcessor_.reset();
    
    // Initialize the neuromodulation manager
    neuromodulationManager_.reset();
    
    // Initialize the development manager
    developmentManager_.reset();
    
    // Initialize legacy state variables
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Initialize components with default configurations
    // (Users can override configurations by accessing component getters)
    
    // Initialize motor and sensory neuron groups (legacy support)
    initializeMotorAndSensoryGroups();
    
    NLM_LOG_INFO("AgentBrain initialized successfully");
}

size_t AgentBrain::getSensoryInputSize() const {
    // Use sensory processor's configuration
    const auto& config = sensoryProcessor_.getConfig();
    return config.visionNeurons_ + config.touchNeurons_ + 
           config.internalNeurons_ + config.proprioceptionNeurons_;
}

size_t AgentBrain::getMotorOutputSize() const {
    // Use motor decoder's configuration
    const auto& config = motorDecoder_.getConfig();
    return config.actionSpaceSize_;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!validateBrain()) {
        NLM_LOG_WARNING("Cannot process sensory input: brain is not valid");
        return;
    }
    
    // Process sensory input through the sensory processor
    sensoryProcessor_.processSensoryInput(percept);
    
    // Legacy novelty detection (maintained for backward compatibility)
    const auto& vision = percept.getVision();
    if (!vision.empty()) {
        float totalDiff = 0.0f;
        for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Update legacy novelty level
        noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
        noveltyLevel_ *= sensoryNoveltyDecay_;
        previousVision_ = vision;
        
        // Update legacy curiosity level
        if (curiosityEnabled_) {
            curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
            curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
        }
    }
    
    // Update neuromodulation manager
    neuromodulationManager_.processNovelty(
        percept.getVision(), 
        sensoryProcessor_.getConfig().visionNeurons_ > 0 ? 
            std::vector<std::vector<float>>{visionHistory_} : 
            std::vector<std::vector<float>>() // Empty for non-vision inputs
    );
    
    // Apply neuromodulatory effects to sensory processing
    sensoryProcessor_.applyNeuromodulation(
        neuromodulationManager_.getDopamineLevel(),
        neuromodulationManager_.getNoveltyLevel()
    );
    
    // Inject current into sensory neurons (legacy implementation)
    // This is kept for backward compatibility - newer code should use the brain's
    // direct injection methods
    const auto& visionSignal = sensoryProcessor_.getVisionSignal();
    for (size_t i = 0; i < sensoryVision_.size() && i < visionSignal.size(); ++i) {
        if (sensoryVision_[i]) {
            float current = visionSignal[i] * 5.0f;  // Scale factor
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    const auto& touchSignal = sensoryProcessor_.getTouchSignal();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touchSignal.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touchSignal[i] * 8.0f;
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    const auto& internalSignal = sensoryProcessor_.getInternalSignal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < internalSignal.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (internalSignal[i] * 2.0f - 1.0f) * 5.0f;
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    const auto& proprioceptionSignal = sensoryProcessor_.getProprioceptionSignal();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprioceptionSignal.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprioceptionSignal[i] * 2.0f - 1.0f) * 3.0f;
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!validateBrain()) {
        NLM_LOG_WARNING("Cannot decode motor command: brain is not valid");
        return MotorCommand::Wait;
    }
    
    // Get activities from brain
    std::vector<float> activities = motorDecoder_.getActivityLevels();
    
    // Decode using the motor decoder
    MotorCommand decoded = motorDecoder_.decode(activities);
    
    // Apply curiosity-based exploration using neuromodulation manager
    if (curiosityEnabled_ && neuromodulationManager_.getCuriosityLevel() > 0.3f) {
        decoded = motorDecoder_.applyExploration(
            decoded,
            neuromodulationManager_.getCuriosityLevel(),
            brain_->getRandomGenerator()
        );
    }
    
    return decoded;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!validateBrain() || !rewardModulationEnabled_) {
        return;
    }
    
    // Apply reward modulation through neuromodulation manager
    neuromodulationManager_.processReward(reward, predictedReward);
    
    // Update legacy state variables
    predictionError_ = neuromodulationManager_.getPredictionError();
    expectedReward_ = neuromodulationManager_.getConfig().rewardPredictionDecay_ > 0.0f ?
        neuromodulationManager_.getExpectedReward() : expectedReward_;
    dopamineLevel_ = neuromodulationManager_.getDopamineLevel();
    
    // Apply reward-modulated weight changes (legacy implementation)
    neuromodulationManager_.processReward(reward, predictedReward);
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!validateBrain() || !developmentEnabled_) {
        return;
    }
    
    // Update development through development manager
    developmentManager_.update(timestep, developmentEnabled_);
    
    // Update legacy state variables
    developmentalAge_ += timestep;
    plasticityModifier_ = developmentManager_.calculatePlasticityModifier();
    
    // Apply developmental effects to plasticity rules
    auto* stdp = brain_->getSTDP();
    auto* hebbian = brain_->getHebbian();
    auto* structuralPlasticity = brain_->getStructuralPlasticity();
    
    developmentManager_.applyToSTDP(stdp);
    developmentManager_.applyToHebbian(hebbian);
    developmentManager_.applyToStructuralPlasticity(structuralPlasticity);
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!validateBrain()) return DevelopmentalStage::Initial;
    return brain_->getDevelopmentalStage();
}

float AgentBrain::getNeuromodulationLevel() const {
    return neuromodulationManager_.getDopamineLevel();
}

float AgentBrain::getCuriosityLevel() const {
    return neuromodulationManager_.getCuriosityLevel();
}

float AgentBrain::getNoveltyLevel() const {
    return neuromodulationManager_.getNoveltyLevel();
}

float AgentBrain::getPredictionError() const {
    return neuromodulationManager_.getPredictionError();
}

void AgentBrain::reset() {
    NLM_LOG_INFO("AgentBrain resetting");
    
    // Reset components
    motorDecoder_.reset();
    sensoryProcessor_.reset();
    neuromodulationManager_.reset();
    developmentManager_.reset();
    
    // Reset legacy state
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    
    NLM_LOG_INFO("AgentBrain reset complete");
}

bool AgentBrain::validateBrain() const {
    return brain_ != nullptr && brain_->getTotalNeuronCount() > 0;
}

void AgentBrain::initializeMotorAndSensoryGroups() {
    if (!validateBrain()) {
        NLM_LOG_WARNING("Cannot initialize neuron groups: brain is not valid");
        return;
    }
    
    // Clear existing groups
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
    
    // Populate legacy neuron groups from brain
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            
            if (type == NeuronType::Motor) {
                for (Neuron* n : pop->getNeurons()) {
                    // Use MotorDecoder to properly assign neurons
                    // This maintains backward compatibility while using new architecture
                    motorDecoder_.addNeuron(type, n, ActionType::Wait);
                    
                    // Also add to legacy groups for compatibility
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
                    // Determine sensory type based on population or use default
                    NeuronType sensoryType = NeuronType::Sensory; // Default
                    
                    // Add to legacy groups
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
    
    NLM_LOG_INFO("Initialized legacy neuron groups for backward compatibility");
}

} // namespace nlm