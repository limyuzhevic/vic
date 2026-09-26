AgentBrain.cpp

#include "AgentBrain.hpp"
#include "MotorDecoder.hpp"
#include "SensoryProcessor.hpp"
#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world)
    : brain_(std::move(brain))
    , world_(std::move(world))
{
    if (!brain_) {
        throw std::invalid_argument("AgentBrain requires a valid brain pointer");
    }
    
    // Initialize components with error handling
    try {
        motorDecoder_ = std::make_unique<MotorDecoder>(brain_);
        sensoryProcessor_ = std::make_unique<SensoryProcessor>(brain_, world_);
        neuromodulator_ = std::make_unique<Neuromodulator>(brain_, world_);
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to initialize AgentBrain components: " + std::string(e.what()));
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    if (!brain_ || !sensoryProcessor_ || !world_) return;
    
    // Initialize all components
    sensoryProcessor_->initialize(world);
    neuromodulator_->reset();
    motorDecoder_->reset();
    
    // Initialize brain state
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    NLM_LOG_INFO("AgentBrain initialized with refactored components");
}

size_t AgentBrain::getSensoryInputSize() const {
    return motorDecoder_->getSensoryInputSize();
}

size_t AgentBrain::getMotorOutputSize() const {
    return motorDecoder_->getMotorOutputSize();
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_ || !sensoryProcessor_) return;
    
    // Process sensory input through dedicated component
    sensoryProcessor_->processSensoryInput(percept);
    
    // Extract neuromodulation signals
    if (sensoryProcessor_) {
        float novelty = sensoryProcessor_->getNoveltyLevel();
        float predictionError = neuromodulator_->getPredictionError();
        if (motorDecoder_) {
            motorDecoder_->updateCuriosityLevel(novelty, predictionError);
        }
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_ || !motorDecoder_) return MotorCommand::Wait;
    return motorDecoder_->decodeMotorCommand();
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !neuromodulator_) return;
    neuromodulator_->applyRewardModulation(reward, predictedReward);
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !neuromodulator_) return;
    neuromodulator_->updateDevelopment(timestep);
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!neuromodulator_) return DevelopmentalStage::Initial;
    return neuromodulator_->getDevelopmentalStage();
}

float AgentBrain::getNeuromodulationLevel() const {
    if (!neuromodulator_) return 0.0f;
    return neuromodulator_->getNeuromodulationLevel();
}

float AgentBrain::getCuriosityLevel() const {
    if (!motorDecoder_) return 0.0f;
    return motorDecoder_->getCuriosityLevel();
}

float AgentBrain::getNoveltyLevel() const {
    if (!sensoryProcessor_) return 0.0f;
    return sensoryProcessor_->getNoveltyLevel();
}

float AgentBrain::getPredictionError() const {
    if (!neuromodulator_) return 0.0f;
    return neuromodulator_->getPredictionError();
}

void AgentBrain::reset() {
    if (motorDecoder_) motorDecoder_->reset();
    if (sensoryProcessor_) sensoryProcessor_->reset();
    if (neuromodulator_) neuromodulator_->reset();
}

} // namespace nlm
