#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include "AgentBrainConstants.h"
#include <algorithm>
#include <cmath>

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(brain)
    , rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , curiosityEnabled_(true)
{
    // Initialize helper classes
    neuromodulationManager_.initialize();
    developmentSystem_.initialize();
    curiositySystem_.initialize();
    
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
                        
                        switch (idx % AgentBrainConstants::MOTOR_GROUP_COUNT) {
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
                        
                        switch (idx % AgentBrainConstants::SENSORY_GROUP_COUNT) {
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
    // Initialize helper systems with world dimensions
    noveltyDetector_.initialize(world.getVisionWidth(), world.getVisionHeight());
    
    // Set up previous vision buffer
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    
    // Reset development state
    developmentSystem_.initialize();
    
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
            float current = vision[i] * AgentBrainConstants::MOTOR_VISION_SCALE;
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * AgentBrainConstants::MOTOR_TOUCH_SCALE;
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * AgentBrainConstants::MOTOR_INTERNAL_SCALE;
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * AgentBrainConstants::MOTOR_PROPRIOCEPTION_SCALE;
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
    
    // Compute novelty (difference from previous vision)
    noveltyDetector_.updateNovelty(vision, AgentBrainConstants::NOVELTY_DECAY_RATE,
                                   neuromodulationManager_.getNoveltyLevel(), previousVision_);
    
    // Update curiosity based on novelty and prediction error
    curiositySystem_.updateCuriosity(
        neuromodulationManager_.getNoveltyLevel(),
        neuromodulationManager_.getPredictionError(),
        curiosityEnabled_,
        neuromodulationManager_.getCuriosityLevel()
    );
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = motorController_.decodeMotorCommand(
        motorForward_, motorBackward_, motorTurnLeft_, motorTurnRight_, motorInteract_, motorWait_);
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && neuromodulationManager_.getCuriosityLevel() > AgentBrainConstants::CURIOSITY_THRESHOLD) {
        decoded = motorController_.applyCuriosityFilter(
            decoded,
            neuromodulationManager_.getCuriosityLevel(),
            brain_->getRandomGenerator()
        );
    }
    
    return decoded;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    neuromodulationManager_.applyRewardModulation(
        reward,
        predictedReward,
        brain_.get(),
        developmentSystem_.getPlasticityModifier(),
        neuromodulationManager_.getDopamineLevel(),
        neuromodulationManager_.getPredictionError(),
        neuromodulationManager_.getExpectedReward()
    );
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentSystem_.updateDevelopment(
        timestep,
        developmentSystem_.getDevelopmentalAge(),
        developmentSystem_.getPlasticityModifier(),
        brain_.get(),
        structuralPlasticityEnabled_
    );
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!brain_) return DevelopmentalStage::Initial;
    return brain_->getDevelopmentalStage();
}

float AgentBrain::getNeuromodulationLevel() const {
    return neuromodulationManager_.getDopamineLevel();
}

float AgentBrain::getCuriosityLevel() const {
    return curiositySystem_.getCuriosityLevel();
}

float AgentBrain::getNoveltyLevel() const {
    return neuromodulationManager_.getNoveltyLevel();
}

float AgentBrain::getPredictionError() const {
    return neuromodulationManager_.getPredictionError();
}

void AgentBrain::reset() {
    neuromodulationManager_.initialize();
    developmentSystem_.initialize();
    curiositySystem_.initialize();
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

} // namespace nlm