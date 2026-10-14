#pragma once

#include "AgentBrain.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace nlm {

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding through modular subsystems
// Refactored for better modularity, separation of concerns, and maintainability

// Constructor with comprehensive validation
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
    , sensoryNoveltyDecay_(0.99f) {
    
    if (!brain_) {
        throw std::runtime_error("AgentBrain: Null pointer error - Brain cannot be null");
    }
    
    // Initialize neuron groups using deterministic distribution
    initializeNeuronGroups();
    
    // Log initialization status
    NLM_LOG_INFO("AgentBrain: Initialized with neural groups - " +
                 std::to_string(sensoryVision_.size()) + " vision, " +
                 std::to_string(sensoryTouch_.size()) + " touch, " +
                 std::to_string(sensoryInternal_.size()) + " internal, " +
                 std::to_string(sensoryProprioception_.size()) + " proprioception neurons");
}

AgentBrain::~AgentBrain() = default;

// Initialize with world and reset development state
void AgentBrain::initialize(const SimpleWorld& world) {
    // Initialize vision buffer for novelty detection
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    
    // Reset developmental parameters
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Reset all subsystems
    resetSubsystems();
    
    NLM_LOG_INFO("AgentBrain: World initialization completed");
}

// Get expected sensory input size from brain configuration
size_t AgentBrain::getSensoryInputSize() const {
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
    return 256 + 8 + 4 + 6;
}

// Get expected motor output size from brain configuration  
size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action
    return 6;
}

// Process sensory percept through modular processing pipeline
void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) {
        return;
    }
    
    try {
        // Extract all sensory signals for processing
        const auto& vision = percept.getVision();
        const auto& touch = percept.getTouch();
        const auto& intern = percept.getInternal();
        const auto& proprio = percept.getProprioception();
        
        // Process vision input through modular pipeline
        processVisionInput(vision);
        
        // Process touch input through modular pipeline
        processTouchInput(touch);
        
        // Process internal signals through modular pipeline
        processInternalInput(intern);
        
        // Process proprioception through modular pipeline
        processProprioceptionInput(proprio);
        
        // Compute and update novelty metrics
        computeNoveltyMetrics(vision);
        
        // Update curiosity-driven exploration
        updateCuriosityDrivenExploration();
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("AgentBrain: Error in sensory processing - " + std::string(e.what()));
    }
}

// Decode motor command using activity-based modulation
MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) {
        return MotorCommand::Wait;
    }
    
    try {
        // Primary decoding from motor neuron activity
        MotorCommand decoded = decodeFromMotorActivity();
        
        // Apply curiosity-driven exploration if enabled
        if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
            decoded = applyCuriosityExploration(decoded);
        }
        
        return decoded;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("AgentBrain: Error in motor decoding - " + std::string(e.what()));
        return MotorCommand::Wait;
    }
}

// Apply reward-based neuromodulation to synaptic weights
void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) {
        return;
    }
    
    try {
        // Compute prediction error for learning
        predictionError_ = reward - predictedReward;
        
        // Update expected reward with exponential moving average
        expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
        
        // Generate dopamine-like signal from prediction error
        dopamineLevel_ = predictionError_;
        dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
        
        // Apply neuromodulation to synaptic weights
        applyNeuromodulationToSynapses(dopamineLevel_);
        
        // Adjust plasticity rules based on neuromodulation
        adjustPlasticityRules(dopamineLevel_);
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("AgentBrain: Error in reward modulation - " + std::string(e.what()));
    }
}

// Update developmental state and plasticity parameters
void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) {
        return;
    }
    
    try {
        // Advance developmental age
        developmentalAge_ += timestep;
        
        // Update plasticity based on developmental stage
        updatePlasticityBasedOnDevelopment();
        
        // Apply structural plasticity changes
        if (structuralPlasticityEnabled_) {
            applyStructuralPlasticity();
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("AgentBrain: Error in development update - " + std::string(e.what()));
    }
}

// Get current developmental stage from brain
DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!brain_) return DevelopmentalStage::Initial;
    return brain_->getDevelopmentalStage();
}

// Get current neuromodulation level
float AgentBrain::getNeuromodulationLevel() const { return dopamineLevel_; }

// Get current curiosity level
float AgentBrain::getCuriosityLevel() const { return curiosityLevel_; }

// Get current novelty level
float AgentBrain::getNoveltyLevel() const { return noveltyLevel_; }

// Get current prediction error
float AgentBrain::getPredictionError() const { return predictionError_; }

// Reset agent state for new episode
void AgentBrain::reset() {
    // Reset neuromodulation state
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    
    // Reset developmental state
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Clear sensory history
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    
    // Reset all subsystems
    resetSubsystems();
    
    NLM_LOG_INFO("AgentBrain: Reset completed - ready for new episode");
}

// Initialize neuron groups using deterministic distribution algorithm
void AgentBrain::initializeNeuronGroups() {
    if (!brain_) return;
    
    for (const auto& region : brain_->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            
            if (type == NeuronType::Motor) {
                // Distribute motor neurons to action-specific groups
                for (Neuron* n : pop->getNeurons()) {
                    size_t groupIndex = calculateMotorGroupIndex();
                    distributeMotorNeuron(n, groupIndex);
                }
            } else if (type == NeuronType::Sensory) {
                // Distribute sensory neurons to modality-specific groups
                for (Neuron* n : pop->getNeurons()) {
                    size_t groupIndex = calculateSensoryGroupIndex();
                    distributeSensoryNeuron(n, groupIndex);
                }
            }
        }
    }
}

// Process vision input through modular pipeline
size_t AgentBrain::calculateMotorGroupIndex() const {
    size_t total = motorForward_.size() + motorBackward_.size() +
                   motorTurnLeft_.size() + motorTurnRight_.size() +
                   motorInteract_.size() + motorWait_.size();
    return total % 6;
}

// Process touch input through modular pipeline
size_t AgentBrain::calculateSensoryGroupIndex() const {
    size_t total = sensoryVision_.size() + sensoryTouch_.size() +
                   sensoryInternal_.size() + sensoryProprioception_.size();
    return total % 4;
}

// Process vision input through dedicated pipeline
void AgentBrain::distributeMotorNeuron(Neuron* n, size_t groupIndex) {
    switch (groupIndex) {
        case 0: motorForward_.push_back(n); break;
        case 1: motorBackward_.push_back(n); break;
        case 2: motorTurnLeft_.push_back(n); break;
        case 3: motorTurnRight_.push_back(n); break;
        case 4: motorInteract_.push_back(n); break;
        case 5: motorWait_.push_back(n); break;
    }
}

// Process touch input through dedicated pipeline
void AgentBrain::distributeSensoryNeuron(Neuron* n, size_t groupIndex) {
    switch (groupIndex) {
        case 0: sensoryVision_.push_back(n); break;
        case 1: sensoryTouch_.push_back(n); break;
        case 2: sensoryInternal_.push_back(n); break;
        case 3: sensoryProprioception_.push_back(n); break;
    }
}

// Process vision input with scaling and injection
void AgentBrain::processVisionInput(const std::vector<float>& vision) {
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            float current = vision[i] * 5.0f;  // Scale factor
            sensoryVision_[i]->injectCurrent(current);
        }
    }
}

// Process touch input with scaling and injection
void AgentBrain::processTouchInput(const std::vector<float>& touch) {
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * 8.0f;  // Collision signal strength
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
}

// Process internal signals with centering and scaling
void AgentBrain::processInternalInput(const std::vector<float>& intern) {
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
}

// Process proprioception with centering and scaling
void AgentBrain::processProprioceptionInput(const std::vector<float>& proprio) {
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
}

// Compute novelty metrics and update state
void AgentBrain::computeNoveltyMetrics(const std::vector<float>& vision) {
    if (vision.empty()) {
        noveltyLevel_ = 0.0f;
        return;
    }
    
    // Calculate total difference from previous vision
    float totalDiff = 0.0f;
    for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
        float diff = std::abs(vision[i] - previousVision_[i]);
        totalDiff += diff;
    }
    
    // Normalize by vision size and apply decay
    noveltyLevel_ = (totalDiff / std::max<size_t>(vision.size(), 1)) * 0.99f;
    
    // Update vision history
    previousVision_ = vision;
}

// Update curiosity based on novelty and prediction error
void AgentBrain::updateCuriosityDrivenExploration() {
    curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
    curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
}

// Decode motor command from neural activity
MotorCommand AgentBrain::decodeFromMotorActivity() {
    // Calculate average activity in each motor group
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
    
    // Find maximum activity among all motor commands
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
    
    // Only act if there's meaningful activity
    if (bestActivity < 0.5f) {
        return MotorCommand::Wait;
    }
    
    return best;
}

// Apply curiosity-driven exploration to motor command
MotorCommand AgentBrain::applyCuriosityExploration(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > 0.5f) {
        float exploreChance = curiosityLevel_ * 0.3f;
        
        // For now, use default behavior without random generator
        // In a real implementation, this would use the brain's random generator
        return defaultCmd;
    }
    
    return defaultCmd;
}

// Apply neuromodulation to synaptic weights
void AgentBrain::applyNeuromodulationToSynapses(float dopamineLevel) {
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                float delta = eligibility * dopamineLevel * plasticityModifier_;
                syn->addToWeight(delta);
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
}

// Adjust plasticity rules based on neuromodulation level
void AgentBrain::adjustPlasticityRules(float dopamineLevel) {
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
    }
}

// Update plasticity based on developmental stage
void AgentBrain::updatePlasticityBasedOnDevelopment() {
    if (developmentalAge_ < 60.0) {
        plasticityModifier_ = 1.0f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < 300.0) {
        plasticityModifier_ = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < 900.0) {
        plasticityModifier_ = 0.5f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = 0.2f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
}

// Apply structural plasticity changes
void AgentBrain::applyStructuralPlasticity() {
    auto* sp = brain_->getStructuralPlasticity();
    if (sp) {
        float synRate = 0.0001f * plasticityModifier_;
        float pruneRate = 0.00001f * (2.0f - plasticityModifier_);
        sp->setSynaptogenesisRate(synRate);
        sp->setPruningRate(pruneRate);
    }
}

// Reset all subsystems to initial state
void AgentBrain::resetSubsystems() {
    // Reset neuron group arrays
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
    
    // Re-initialize with fresh state
    if (brain_) {
        initializeNeuronGroups();
    }
}

// Configuration methods
void AgentBrain::enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
void AgentBrain::enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
void AgentBrain::enableDevelopment(bool enable) { developmentEnabled_ = enable; }
void AgentBrain::enableCuriosity(bool enable) { curiosityEnabled_ = enable; }

bool AgentBrain::isRewardModulationEnabled() const { return rewardModulationEnabled_; }
bool AgentBrain::isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
bool AgentBrain::isDevelopmentEnabled() const { return developmentEnabled_; }
bool AgentBrain::isCuriosityEnabled() const { return curiosityEnabled_; }

} // namespace nlm
