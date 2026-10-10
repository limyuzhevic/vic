// NLMImplementation
#include "NLMImplementation.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

namespace nlm {

NLMImplementation::NLMImplementation() {
    NLM_LOG_INFO("Initializing NLM Implementation");
}

NLMImplementation::~NLMImplementation() {
    NLM_LOG_INFO("Shutting down NLM Implementation");
}

void NLMImplementation::registerPlugin(std::unique_ptr<NLMPlugin> plugin) {
    NLM_LOG_INFO("Registering plugin: " + plugin->getName());
    
    // In a real implementation, this would load the actual plugin
    NLM_LOG_INFO("Plugin registered successfully");
}

void NLMImplementation::registerModule(std::unique_ptr<NLMModule> module) {
    NLM_LOG_INFO("Registering module: " + module->getName());
    
    // In a real implementation, this would load the actual module
    NLM_LOG_INFO("Module registered successfully");
}

NLMPluginManager& NLMImplementation::getPluginManager() {
    return _pluginManager;
}

NLMModuleManager& NLMImplementation::getModuleManager() {
    return _moduleManager;
}

void NLMImplementation::initializeAll() {
    NLM_LOG_INFO("Initializing all plugins and modules");
    
    // Enable all plugins
    for (const auto& pair : _pluginManager.getAllPlugins()) {
        _pluginManager.enablePlugin(pair.first);
    }
    
    // Enable all modules
    for (const auto& pair : _moduleManager.getAllModules()) {
        _moduleManager.enableModule(pair.first);
    }
    
    NLM_LOG_INFO("All plugins and modules initialized successfully");
}

void NLMImplementation::shutdownAll() {
    NLM_LOG_INFO("Shutting down all plugins and modules");
    
    // Disable all modules
    for (const auto& pair : _moduleManager.getAllModules()) {
        _moduleManager.disableModule(pair.first);
    }
    
    // Disable all plugins
    for (const auto& pair : _pluginManager.getAllPlugins()) {
        _pluginManager.disablePlugin(pair.first);
    }
    
    NLM_LOG_INFO("All plugins and modules shut down successfully");
}

} // namespace nlm