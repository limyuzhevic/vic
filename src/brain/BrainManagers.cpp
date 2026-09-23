#include "BrainManagers.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

bool BrainManagers::initialize(Brain* brain) {
    if (!brain) {
        return false;
    }
    
    memoryManager = std::make_unique<MemoryManager>();
    neuromodulationManager = std::make_unique<NeuromodulationManager>();
    developmentManager = std::make_unique<DevelopmentManager>();
    
    bool success = true;
    if (memoryManager && !memoryManager->initialize(brain)) {
        NLM_LOG_ERROR("Failed to initialize MemoryManager");
        success = false;
    }
    
    if (neuromodulationManager && !neuromodulationManager->initialize(brain)) {
        NLM_LOG_ERROR("Failed to initialize NeuromodulationManager");
        success = false;
    }
    
    if (developmentManager && !developmentManager->initialize(brain)) {
        NLM_LOG_ERROR("Failed to initialize DevelopmentManager");
        success = false;
    }
    
    return success;
}

void BrainManagers::update(float timestep) {
    if (memoryManager) {
        memoryManager->update(timestep);
    }
    
    if (neuromodulationManager) {
        neuromodulationManager->update(timestep);
    }
    
    if (developmentManager) {
        developmentManager->update(nullptr, *static_cast<RandomGenerator*>(nullptr), timestep);
    }
}

void BrainManagers::applyNeuromodulation(std::vector<std::unique_ptr<NeuralRegion>>& regions) {
    if (neuromodulationManager) {
        neuromodulationManager->applyToNeurons(regions);
    }
}

void BrainManagers::applyDevelopmentEffects(Brain* brain, float plasticityModifier) {
    if (developmentManager && brain) {
        developmentManager->applyDevelopmentEffects(brain, plasticityModifier);
    }
}

bool BrainManagers::validateConfiguration(std::shared_ptr<Config> config) {
    if (!config) {
        return false;
    }
    
    bool success = true;
    
    if (memoryManager && !memoryManager->validateConfiguration(config)) {
        NLM_LOG_ERROR("MemoryManager configuration validation failed");
        success = false;
    }
    
    if (neuromodulationManager && !neuromodulationManager->validateConfiguration(config)) {
        NLM_LOG_ERROR("NeuromodulationManager configuration validation failed");
        success = false;
    }
    
    if (developmentManager && !developmentManager->validateConfiguration(config)) {
        NLM_LOG_ERROR("DevelopmentManager configuration validation failed");
        success = false;
    }
    
    return success;
}

void BrainManagers::logStatus() const {
    if (memoryManager) {
        NLM_LOG_INFO("MemoryManager: " + memoryManager->getStatusString());
    }
    
    if (neuromodulationManager) {
        NLM_LOG_INFO("NeuromodulationManager: " + neuromodulationManager->getStatus().toString());
    }
    
    if (developmentManager) {
        NLM_LOG_INFO("DevelopmentManager: " + developmentManager->getStatus().toString());
    }
}

} // namespace nlm

namespace nlm {

MemoryManager* BrainManagers::getMemoryManager() {
    return memoryManager.get();
}

NeuromodulationManager* BrainManagers::getNeuromodulationManager() {
    return neuromodulationManager.get();
}

DevelopmentManager* BrainManagers::getDevelopmentManager() {
    return developmentManager.get();
}

} // namespace nlm
