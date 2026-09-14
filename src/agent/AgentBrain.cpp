#include "AgentBrainSetup.hpp"
#include "AgentBrainConfig.hpp"
#include "AgentBrainSensory.hpp"
#include "AgentBrainMotor.hpp"
#include "AgentBrainModulation.hpp"
#include "AgentBrainDevelopment.hpp"
#include "AgentBrainState.hpp"
#include "AgentBrainReset.hpp"

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain) {
    setup::AgentBrainSetup::initializeConstructor(*this, brain);
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    config::AgentBrainConfig::initializeConfig(*this, world);
}

size_t AgentBrain::getSensoryInputSize() const {
    return state::AgentBrainState::getSensoryInputSize();
}

size_t AgentBrain::getMotorOutputSize() const {
    return state::AgentBrainState::getMotorOutputSize();
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    sensory::AgentBrainSensory::processSensoryInput(*this, percept);
}

MotorCommand AgentBrain::decodeMotorCommand() {
    return motor::AgentBrainMotor::decodeMotorCommand(*this);
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    modulation::AgentBrainModulation::applyRewardModulation(*this, reward, predictedReward);
}

void AgentBrain::updateDevelopment(double timestep) {
    development::AgentBrainDevelopment::updateDevelopment(*this, timestep);
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    return state::AgentBrainState::getDevelopmentalStage(*this);
}

float AgentBrain::getNeuromodulationLevel() const {
    return state::AgentBrainState::getNeuromodulationLevel(*this);
}

float AgentBrain::getCuriosityLevel() const {
    return state::AgentBrainState::getCuriosityLevel(*this);
}

float AgentBrain::getNoveltyLevel() const {
    return state::AgentBrainState::getNoveltyLevel(*this);
}

float AgentBrain::getPredictionError() const {
    return state::AgentBrainState::getPredictionError(*this);
}

void AgentBrain::reset() {
    reset::AgentBrainReset::resetState(*this);
}

// Configuration methods (delegated to config module)
void AgentBrain::enableRewardModulation(bool enable) {
    config::AgentBrainConfig::enableRewardModulation(*this, enable);
}

void AgentBrain::enableStructuralPlasticity(bool enable) {
    config::AgentBrainConfig::enableStructuralPlasticity(*this, enable);
}

void AgentBrain::enableDevelopment(bool enable) {
    config::AgentBrainConfig::enableDevelopment(*this, enable);
}

void AgentBrain::enableCuriosity(bool enable) {
    config::AgentBrainConfig::enableCuriosity(*this, enable);
}

bool AgentBrain::isRewardModulationEnabled() const {
    return config::AgentBrainConfig::isRewardModulationEnabled(*this);
}

bool AgentBrain::isStructuralPlasticityEnabled() const {
    return config::AgentBrainConfig::isStructuralPlasticityEnabled(*this);
}

bool AgentBrain::isDevelopmentEnabled() const {
    return config::AgentBrainConfig::isDevelopmentEnabled(*this);
}

bool AgentBrain::isCuriosityEnabled() const {
    return config::AgentBrainConfig::isCuriosityEnabled(*this);
}

// Brain pointer accessor
Brain* AgentBrain::getBrain() {
    return brain_.get();
}

} // namespace nlm
