#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config)
    : brain_(brain)
    , config_(config)
    , body_(std::make_shared<AgentBody>())
    , rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , curiosityEnabled_(true)
{
    // Initialize modular components with configuration
    sensoryTransduction_ = std::make_unique<SensoryTransduction>(config_);
    motorDecoder_ = std::make_unique<MotorDecoder>(config_);
    neuromodulator_ = std::make_unique<Neuromodulator>(config_);
    developer_ = std::make_unique<Developer>(config_);
    explorationAgent_ = std::make_unique<ExplorationAgent>(config_);
    
    // Configure components with brain reference
    sensoryTransduction_->initialize(brain_, *body_);
    motorDecoder_->initialize(brain_, *body_, explorationAgent_);
    neuromodulator_->initialize(brain_);
    developer_->initialize(brain_);
    explorationAgent_->initialize(brain_);
    
    NLM_LOG_INFO("AgentBrain: Initialized with modular architecture");
}

// Legacy constructor for backward compatibility
// Creates a default configuration
AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : AgentBrain(brain, std::make_shared<Config>())
{
    NLM_LOG_INFO("AgentBrain: Legacy constructor called - using default configuration");
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    // Update config dimensions from world
    config_->setVisionDimensions(world.getVisionWidth(), world.getVisionHeight());
    
    // Initialize all components
    body_->reset();
    body_->x = 10.0f;
    body_->y = 10.0f;
    body_->orientation = 0.0f;
    
    sensoryTransduction_->setPreviousVision(std::vector<float>(world.getVisionWidth() * world.getVisionHeight(), 0.0f));
    sensoryTransduction_->setPreviousVision(std::vector<float>(world.getVisionWidth() * world.getVisionHeight(), 0.0f));
    
    neuromodulator_->enableRewardModulation(rewardModulationEnabled_);
    developer_->enableDevelopment(developmentEnabled_);
    explorationAgent_->enableCuriosity(curiosityEnabled_);
    
    NLM_LOG_INFO("AgentBrain: Modular components initialized");
}

size_t AgentBrain::getSensoryInputSize() const {
    return sensoryTransduction_->getSensoryInputSize();
}

size_t AgentBrain::getMotorOutputSize() const {
    return motorDecoder_->getMotorOutputSize();
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    // Process through sensory transduction module
    sensoryTransduction_->processSensoryInput(percept);
    
    // Get novelty level for exploration agent
    float noveltyLevel = sensoryTransduction_->getNoveltyLevel();
    explorationAgent_->update(0.01, noveltyLevel);
    
    // Update neuromodulator with current state
    neuromodulator_->update(0.01);
}

MotorCommand AgentBrain::decodeMotorCommand() {
    return motorDecoder_->decodeMotorCommand();
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    neuromodulator_->applyRewardModulation(reward, predictedReward);
}

void AgentBrain::updateDevelopment(double timestep) {
    developer_->updateDevelopment(timestep);
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    return developer_->getDevelopmentalStage();
}

float AgentBrain::getNeuromodulationLevel() const {
    return neuromodulator_->getNeuromodulationLevel();
}

float AgentBrain::getCuriosityLevel() const {
    return explorationAgent_->getCuriosityLevel();
}

float AgentBrain::getNoveltyLevel() const {
    return sensoryTransduction_->getNoveltyLevel();
}

float AgentBrain::getPredictionError() const {
    return neuromodulator_->getPredictionError();
}

void AgentBrain::reset() {
    sensoryTransduction_->reset();
    motorDecoder_->reset();
    neuromodulator_->reset();
    developer_->reset();
    explorationAgent_->reset();
    
    body_->reset();
    body_->x = 10.0f;
    body_->y = 10.0f;
    
    NLM_LOG_INFO("AgentBrain: Reset all modular components");
}

void AgentBrain::enableRewardModulation(bool enable) {
    rewardModulationEnabled_ = enable;
    neuromodulator_->enableRewardModulation(enable);
}

void AgentBrain::enableStructuralPlasticity(bool enable) {
    structuralPlasticityEnabled_ = enable;
}

void AgentBrain::enableDevelopment(bool enable) {
    developmentEnabled_ = enable;
    developer_->enableDevelopment(enable);
}

void AgentBrain::enableCuriosity(bool enable) {
    curiosityEnabled_ = enable;
    explorationAgent_->enableCuriosity(enable);
}

} // namespace nlm
