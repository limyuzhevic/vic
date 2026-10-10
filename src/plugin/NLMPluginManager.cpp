// NLM Plugin Manager Implementation
#include "NLMPluginManager.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

namespace nlm {

NLMPluginManager::NLMPluginManager() {
    NLM_LOG_INFO("Initializing NLM Plugin Manager");
}

NLMPluginManager::~NLMPluginManager() {
    NLM_LOG_INFO("Shutting down NLM Plugin Manager");
}

bool NLMPluginManager::loadPlugin(const std::string& filePath) {
    NLM_LOG_INFO("Loading plugin: " + filePath);
    
    // In a real implementation, this would load the plugin from a shared library
    // For simplicity, we'll create a dummy plugin
    std::string pluginName = "Plugin_" + filePath;
    
    auto plugin = std::make_unique<NLMPlugin>(pluginName, "1.0.0", "Dummy plugin");
    
    if (plugin->initialize(PluginConfig())) {
        _plugins[pluginName] = std::move(plugin);
        NLM_LOG_INFO("Plugin loaded successfully: " + pluginName);
        return true;
    }
    
    NLM_LOG_ERROR("Failed to load plugin: " + filePath);
    return false;
}

bool NLMPluginManager::unloadPlugin(const std::string& name) {
    NLM_LOG_INFO("Unloading plugin: " + name);
    
    auto it = _plugins.find(name);
    if (it == _plugins.end()) {
        NLM_LOG_ERROR("Plugin not found: " + name);
        return false;
    }
    
    it->second->shutdown();
    _plugins.erase(it);
    
    NLM_LOG_INFO("Plugin unloaded successfully: " + name);
    return true;
}

bool NLMPluginManager::enablePlugin(const std::string& name) {
    NLM_LOG_INFO("Enabling plugin: " + name);
    
    auto it = _plugins.find(name);
    if (it == _plugins.end()) {
        NLM_LOG_ERROR("Plugin not found: " + name);
        return false;
    }
    
    if (it->second->isInitialized()) {
        NLM_LOG_INFO("Plugin already initialized: " + name);
        return true;
    }
    
    PluginConfig config;
    if (it->second->initialize(config)) {
        NLM_LOG_INFO("Plugin enabled successfully: " + name);
        return true;
    }
    
    NLM_LOG_ERROR("Failed to enable plugin: " + name);
    return false;
}

bool NLMPluginManager::disablePlugin(const std::string& name) {
    NLM_LOG_INFO("Disabling plugin: " + name);
    
    auto it = _plugins.find(name);
    if (it == _plugins.end()) {
        NLM_LOG_ERROR("Plugin not found: " + name);
        return false;
    }
    
    if (!it->second->isInitialized()) {
        NLM_LOG_INFO("Plugin not initialized: " + name);
        return true;
    }
    
    it->second->shutdown();
    
    NLM_LOG_INFO("Plugin disabled successfully: " + name);
    return true;
}

NLMPlugin* NLMPluginManager::getPlugin(const std::string& name) {
    auto it = _plugins.find(name);
    if (it != _plugins.end()) {
        return it->second.get();
    }
    return nullptr;
}

const std::map<std::string, std::unique_ptr<NLMPlugin>>& NLMPluginManager::getAllPlugins() const {
    return _plugins;
}

void NLMPluginManager::executePluginExtension(const std::string& name, const std::string& extension) {
    auto it = _plugins.find(name);
    if (it != _plugins.end()) {
        it->second->executeExtension(extension);
    } else {
        NLM_LOG_ERROR("Plugin not found: " + name);
    }
}

void NLMPluginManager::savePluginConfig(const std::string& name, const PluginConfig& config) {
    auto it = _plugins.find(name);
    if (it != _plugins.end()) {
        it->second->setConfig(config);
        NLM_LOG_INFO("Plugin config saved for: " + name);
    } else {
        NLM_LOG_ERROR("Plugin not found: " + name);
    }
}

// ============================================================================
// NLMModule Manager Implementation
// ============================================================================

NLMModuleManager::NLMModuleManager() {
    NLM_LOG_INFO("Initializing NLM Module Manager");
}

NLMModuleManager::~NLMModuleManager() {
    NLM_LOG_INFO("Shutting down NLM Module Manager");
}

bool NLMModuleManager::loadModule(const std::string& filePath) {
    NLM_LOG_INFO("Loading module: " + filePath);
    
    // In a real implementation, this would load the module from a shared library
    // For simplicity, we'll just create a dummy module
    std::string moduleName = "Module_" + filePath;
    
    auto module = std::make_unique<NLMModule>(moduleName, "core", "1.0.0");
    
    if (module->initialize()) {
        _modules[moduleName] = std::move(module);
        NLM_LOG_INFO("Module loaded successfully: " + moduleName);
        return true;
    }
    
    NLM_LOG_ERROR("Failed to load module: " + filePath);
    return false;
}

bool NLMModuleManager::unloadModule(const std::string& name) {
    NLM_LOG_INFO("Unloading module: " + name);
    
    auto it = _modules.find(name);
    if (it == _modules.end()) {
        NLM_LOG_ERROR("Module not found: " + name);
        return false;
    }
    
    it->second->shutdown();
    _modules.erase(it);
    
    NLM_LOG_INFO("Module unloaded successfully: " + name);
    return true;
}

bool NLMModuleManager::enableModule(const std::string& name) {
    NLM_LOG_INFO("Enabling module: " + name);
    
    auto it = _modules.find(name);
    if (it == _modules.end()) {
        NLM_LOG_ERROR("Module not found: " + name);
        return false;
    }
    
    if (it->second->isEnabled()) {
        NLM_LOG_INFO("Module already enabled: " + name);
        return true;
    }
    
    if (it->second->enable()) {
        NLM_LOG_INFO("Module enabled successfully: " + name);
        return true;
    }
    
    NLM_LOG_ERROR("Failed to enable module: " + name);
    return false;
}

bool NLMModuleManager::disableModule(const std::string& name) {
    NLM_LOG_INFO("Disabling module: " + name);
    
    auto it = _modules.find(name);
    if (it == _modules.end()) {
        NLM_LOG_ERROR("Module not found: " + name);
        return false;
    }
    
    if (!it->second->isEnabled()) {
        NLM_LOG_INFO("Module not enabled: " + name);
        return true;
    }
    
    it->second->disable();
    
    NLM_LOG_INFO("Module disabled successfully: " + name);
    return true;
}

NLMModule* NLMModuleManager::getModule(const std::string& name) {
    auto it = _modules.find(name);
    if (it != _modules.end()) {
        return it->second.get();
    }
    return nullptr;
}

const std::map<std::string, std::unique_ptr<NLMModule>>& NLMModuleManager::getAllModules() const {
    return _modules;
}

void NLMModuleManager::saveModuleConfig(const std::string& name, const ModuleConfig& config) {
    // In a real implementation, this would save the module configuration
    NLM_LOG_INFO("Module config saved for: " + name);
}

// ============================================================================
// NLMImplementation
// ============================================================================

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