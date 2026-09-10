#include "BrainPluginRegistry.hpp"
#include <iostream>

namespace nlm {

BrainPluginRegistry& BrainPluginRegistry::getInstance() {
    static BrainPluginRegistry instance;
    return instance;
}

BrainPluginRegistry::BrainPluginRegistry() {
    initializeDefaultUpdaters();
}

void BrainPluginRegistry::registerUpdater(const std::string& name, UpdaterPluginFunc factory) {
    if (factory) {
        updaters_[name] = std::move(factory);
        std::cout << "[BrainPluginRegistry] Registered updater: " << name << std::endl;
    } else {
        std::cerr << "[BrainPluginRegistry] ERROR: Failed to register updater: " << name 
                 << " - factory is null" << std::endl;
    }
}

BrainPluginRegistry::UpdaterFactory BrainPluginRegistry::getUpdaterFactory(const std::string& name) const {
    auto it = updaters_.find(name);
    if (it != updaters_.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<std::string> BrainPluginRegistry::getAvailableUpdaters() const {
    std::vector<std::string> names;
    names.reserve(updaters_.size());
    for (const auto& pair : updaters_) {
        names.push_back(pair.first);
    }
    return names;
}

std::unique_ptr<BrainSystemUpdater> BrainPluginRegistry::createUpdater(const std::string& name) {
    auto factory = getUpdaterFactory(name);
    if (factory) {
        return factory();
    }
    std::cerr << "[BrainPluginRegistry] ERROR: No factory found for updater: " << name << std::endl;
    return nullptr;
}

void BrainPluginRegistry::initializeDefaultUpdaters() {
    // Register core updaters
    registerUpdater("NeuronSystemUpdater", []() { return std::make_unique<NeuronSystemUpdater>(); });
    registerUpdater("MemorySystemUpdater", []() { return std::make_unique<MemorySystemUpdater>(); });
    registerUpdater("NeuromodulationUpdater", []() { return std::make_unique<NeuromodulationUpdater>(); });
    
    // Register user-extensible plugins
    registerUpdater("AnalysisUpdater", []() { return std::make_unique<AnalysisUpdater>(); });
    registerUpdater("LearningUpdater", []() { return std::make_unique<LearningUpdater>(); });
    registerUpdater("DebugUpdater", []() { return std::make_unique<DebugUpdater>(); });
    
    std::cout << "[BrainPluginRegistry] Initialized with " << updaters_.size() << " default updaters" << std::endl;
}

void BrainPluginManager::loadPlugins(const std::string& configPath) {
    std::cout << "[BrainPluginManager] Loading plugins from: " << configPath << std::endl;
    
    // For now, just log the attempt
    // A real implementation would parse the config file and load plugins
    std::cout << "[BrainPluginManager] Note: Config file parsing not yet implemented" << std::endl;
}

void BrainPluginManager::configureFromFile(const std::string& configFile) {
    std::cout << "[BrainPluginManager] Configuring from file: " << configFile << std::endl;
    
    // For now, just create default orchestrator
    // A real implementation would parse configuration and enable/disable updaters
    std::cout << "[BrainPluginManager] Using default plugin configuration" << std::endl;
}

std::unique_ptr<BrainUpdaterOrchestrator> BrainPluginManager::createOrchestrator() {
    auto orchestrator = std::make_unique<BrainUpdaterOrchestrator>();
    
    // Add core updaters
    orchestrator->addUpdater(std::make_unique<NeuronSystemUpdater>());
    orchestrator->addUpdater(std::make_unique<MemorySystemUpdater>());
    orchestrator->addUpdater(std::make_unique<NeuromodulationUpdater>());
    
    // Add plugins based on configuration
    auto& registry = BrainPluginRegistry::getInstance();
    
    // Add analysis updater if available
    if (registry.getUpdaterFactory("AnalysisUpdater")) {
        orchestrator->addUpdater(registry.createUpdater("AnalysisUpdater"));
    }
    
    // Add learning updater if available
    if (registry.getUpdaterFactory("LearningUpdater")) {
        orchestrator->addUpdater(registry.createUpdater("LearningUpdater"));
    }
    
    // Add debug updater if available
    if (registry.getUpdaterFactory("DebugUpdater")) {
        orchestrator->addUpdater(registry.createUpdater("DebugUpdater"));
    }
    
    std::cout << "[BrainPluginManager] Created orchestrator with " 
              << orchestrator->getUpdaterCount() << " updaters" << std::endl;
    
    return orchestrator;
}

} // namespace nlm
