#include "BrainComponents.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <memory>

namespace nlm {

// BrainStepManager implementation
struct BrainStepManager::Impl {
    Brain* brain;
    std::shared_ptr<Config> config;
    
    Impl(Brain* b, std::shared_ptr<Config> cfg) : brain(b), config(cfg) {}
};

BrainStepManager::BrainStepManager(Brain* brain, std::shared_ptr<Config> config)
    : pImpl(new Impl(brain, config)) {}

BrainStepManager::~BrainStepManager() {
    delete pImpl;
}

void BrainStepManager::executeIntegrationStep(SimulationStep step, Timestamp time) {
    // Execute the 15-step integrated brain loop
    pImpl->brain->executeStepIntegration(step, time);
}

// BrainMemorySystem implementation
struct BrainMemorySystem::Impl {
    Brain* brain;
    std::shared_ptr<Config> config;
    
    Impl(Brain* b, std::shared_ptr<Config> cfg) : brain(b), config(cfg) {}
};

BrainMemorySystem::BrainMemorySystem(Brain* brain, std::shared_ptr<Config> config)
    : pImpl(new Impl(brain, config)) {}

BrainMemorySystem::~BrainMemorySystem() {
    delete pImpl;
}

void BrainMemorySystem::initialize() {
    pImpl->brain->initializeMemorySystems();
}

void BrainMemorySystem::update(SimulationStep step) {
    pImpl->brain->updateMemorySystems(step);
}

void BrainMemorySystem::reset() {
    pImpl->brain->resetMemorySystems();
}

// BrainNeuromodulation implementation
struct BrainNeuromodulation::Impl {
    Brain* brain;
    
    Impl(Brain* b) : brain(b) {}
};

BrainNeuromodulation::BrainNeuromodulation(Brain* brain)
    : pImpl(new Impl(brain)) {}

BrainNeuromodulation::~BrainNeuromodulation() {
    delete pImpl;
}

void BrainNeuromodulation::applyAllNeuromodulation(SimulationStep step, Timestamp time) {
    pImpl->brain->applyAllNeuromodulationEffects(step, time);
}

// BrainDevelopment implementation
struct BrainDevelopment::Impl {
    Brain* brain;
    std::shared_ptr<Config> config;
    
    Impl(Brain* b, std::shared_ptr<Config> cfg) : brain(b), config(cfg) {}
};

BrainDevelopment::BrainDevelopment(Brain* brain, std::shared_ptr<Config> config)
    : pImpl(new Impl(brain, config)) {}

BrainDevelopment::~BrainDevelopment() {
    delete pImpl;
}

void BrainDevelopment::updateDevelopmentEffects(SimulationStep step) {
    pImpl->brain->updateDevelopmentStep(step);
}

void BrainDevelopment::applyDevelopmentToPlasticity() {
    pImpl->brain->applyDevelopmentToPlasticitySystem();
}

// BrainCheckpointManager implementation
struct BrainCheckpointManager::Impl {
    Brain* brain;
    
    Impl(Brain* b) : brain(b) {}
};

BrainCheckpointManager::BrainCheckpointManager(Brain* brain)
    : pImpl(new Impl(brain)) {}

BrainCheckpointManager::~BrainCheckpointManager() {
    delete pImpl;
}

bool BrainCheckpointManager::save(const std::string& filepath) const {
    return pImpl->brain->saveCheckpoint(filepath);
}

bool BrainCheckpointManager::load(const std::string& filepath) {
    return pImpl->brain->loadCheckpoint(filepath);
}

// BrainInitializer implementation
struct BrainInitializer::Impl {
    Brain* brain;
    std::shared_ptr<Config> config;
    
    Impl(Brain* b, std::shared_ptr<Config> cfg) : brain(b), config(cfg) {}
};

BrainInitializer::BrainInitializer(Brain* brain, std::shared_ptr<Config> config)
    : pImpl(new Impl(brain, config)) {}

BrainInitializer::~BrainInitializer() {
    delete pImpl;
}

bool BrainInitializer::initialize() {
    return pImpl->brain->initializeAll(pImpl->config);
}

} // namespace nlm
