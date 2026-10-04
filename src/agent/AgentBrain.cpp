#include "AgentBrain.hpp"
#include "AgentBody.hpp"
#include "MotorNeuronManager.hpp"
#include "SensoryNeuronManager.hpp"
#include "SensoryProcessor.hpp"
#include "MotorCommandDecoder.hpp"
#include "NeuromodulationManager.hpp"
#include "DevelopmentManager.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(brain)
    , previousVision_()
{
    // Initialize managers
    motorNeuronManager_ = std::make_unique<MotorNeuronManager>(brain);
    sensoryNeuronManager_ = std::make_unique<SensoryNeuronManager>(brain);
    sensoryProcessor_ = std::make_unique<SensoryProcessor>(0.99f);
    motorCommandDecoder_ = std::make_unique<MotorCommandDecoder>(brain);
    neuromodulationManager_ = std::make_unique<NeuromodulationManager>(brain);
    developmentManager_ = std::make_unique<DevelopmentManager>(brain);
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    
    // Initialize all managers
    if (motorNeuronManager_) {
        motorNeuronManager_->distributeAndGroupMotorNeurons();
    }
    if (sensoryNeuronManager_) {
        sensoryNeuronManager_->distributeAndGroupSensoryNeurons();
    }
    if (developmentManager_) {
        developmentManager_->update(0.0, true, true);
    }
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryNeuronManager_ ? sensoryNeuronManager_->getSensoryVision().size() : 0) + " vision sensory neurons, " +
                 std::to_string(sensoryNeuronManager_ ? sensoryNeuronManager_->getSensoryTouch().size() : 0) + " touch sensory neurons, " +
                 std::to_string(sensoryNeuronManager_ ? sensoryNeuronManager_->getSensoryInternal().size() : 0) + " internal sensory neurons");
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
    
    // Process sensory input using the sensory processor
    if (sensoryProcessor_ && sensoryNeuronManager_) {
        sensoryProcessor_->processInput(
            percept,
            sensoryNeuronManager_->getSensoryVision(),
            sensoryNeuronManager_->getSensoryTouch(),
            sensoryNeuronManager_->getSensoryInternal(),
            sensoryNeuronManager_->getSensoryProprioception(),
            previousVision_,
            neuromodulationManager_ ? neuromodulationManager_->getPredictionError() : 0.0f
        );
    }
    
    // Log statistics
    NLM_LOG_INFO("Processing sensory input - Novelty: " + std::to_string(sensoryProcessor_ ? sensoryProcessor_->getNoveltyLevel() : 0.0f) +
                 ", Curiosity: " + std::to_string(sensoryProcessor_ ? sensoryProcessor_->getCuriosityLevel() : 0.0f));
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    // Get neuromodulation level for curiosity
    float neuromodulationLevel = neuromodulationManager_ ? neuromodulationManager_->getDopamineLevel() : 0.0f;
    
    // Decode motor command using the motor command decoder
    MotorCommand decoded = motorCommandDecoder_->decode(
        motorNeuronManager_ ? motorNeuronManager_->getMotorForward() : std::vector<Neuron*>() ,
        motorNeuronManager_ ? motorNeuronManager_->getMotorBackward() : std::vector<Neuron*>() ,
        motorNeuronManager_ ? motorNeuronManager_->getMotorTurnLeft() : std::vector<Neuron*>() ,
        motorNeuronManager_ ? motorNeuronManager_->getMotorTurnRight() : std::vector<Neuron*>() ,
        motorNeuronManager_ ? motorNeuronManager_->getMotorInteract() : std::vector<Neuron*>() ,
        motorNeuronManager_ ? motorNeuronManager_->getMotorWait() : std::vector<Neuron*>() ,
        sensoryProcessor_ && sensoryProcessor_->isCuriosityEnabled(),
        sensoryProcessor_ ? sensoryProcessor_->getCuriosityLevel() : 0.0f
    );
    
    // Apply curiosity-based exploration using neuromodulation manager
    if (sensoryProcessor_ && sensoryProcessor_->isCuriosityEnabled() && 
        sensoryProcessor_->getCuriosityLevel() > 0.3f) {
        decoded = motorCommandDecoder_->selectWithCuriosity(
            decoded,
            sensoryProcessor_->isCuriosityEnabled(),
            sensoryProcessor_->getCuriosityLevel(),
            brain_
        );
    }
    
    return decoded;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (neuromodulationManager_) {
        neuromodulationManager_->processReward(reward, predictedReward, 
                                             neuromodulationManager_->isRewardModulationEnabled());
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (developmentManager_) {
        developmentManager_->update(timestep, developmentManager_->isDevelopmentEnabled(),
                                  developmentManager_->isStructuralPlasticityEnabled());
    }
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    return developmentManager_ ? developmentManager_->getStage() : DevelopmentalStage::Initial;
}

float AgentBrain::getNeuromodulationLevel() const {
    return neuromodulationManager_ ? neuromodulationManager_->getDopamineLevel() : 0.0f;
}

float AgentBrain::getCuriosityLevel() const {
    return sensoryProcessor_ ? sensoryProcessor_->getCuriosityLevel() : 0.0f;
}

float AgentBrain::getNoveltyLevel() const {
    return sensoryProcessor_ ? sensoryProcessor_->getNoveltyLevel() : 0.0f;
}

float AgentBrain::getPredictionError() const {
    return neuromodulationManager_ ? neuromodulationManager_->getPredictionError() : 0.0f;
}

void AgentBrain::reset() {
    // Reset all managers
    if (neuromodulationManager_) {
        neuromodulationManager_->processReward(0.0f, 0.0f, true);
    }
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

void AgentBrain::enableRewardModulation(bool enable) {
    if (neuromodulationManager_) {
        neuromodulationManager_->enableRewardModulation(enable);
    }
}

void AgentBrain::enableStructuralPlasticity(bool enable) {
    if (developmentManager_) {
        developmentManager_->enableStructuralPlasticity(enable);
    }
}

void AgentBrain::enableDevelopment(bool enable) {
    if (developmentManager_) {
        developmentManager_->enableDevelopment(enable);
    }
}

void AgentBrain::enableCuriosity(bool enable) {
    if (sensoryProcessor_) {
        sensoryProcessor_->setCuriosityEnabled(enable);
    }
}

} // namespace nlm
