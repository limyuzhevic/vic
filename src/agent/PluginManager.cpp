// PluginManager.cpp - Implementation of plugin system for extending NLM functionality
// Provides plugin loading, management, and event handling

#include "agent/PluginManager.hpp"
#include "core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>

namespace nlm {

struct PluginManager::Impl {
    std::map<std::string, std::unique_ptr<PluginInterface>> plugins;
    std::map<std::string, bool> pluginEnabled;
    std::map<std::string, std::string> pluginPaths;
    std::map<std::string, std::map<std::string, std::string>> pluginConfigs;
    bool loadingPlugins;
    
    Impl() : loadingPlugins(false) {}
};

PluginManager::PluginManager() : pImpl(std::make_unique<Impl>()) {}

PluginManager::~PluginManager() = default;

bool PluginManager::loadPlugin(const std::string& path) {
    return loadPlugin(nullptr, path);
}

bool PluginManager::loadPlugin(std::unique_ptr<PluginInterface> plugin) {
    if (!plugin) {
        NLM_LOG_ERROR("Cannot load null plugin");
        return false;
    }
    
    std::string name = plugin->getName();
    
    // Check if plugin already exists
    if (pImpl->plugins.find(name) != pImpl->plugins.end()) {
        NLM_LOG_WARNING(std::string("Plugin already loaded: ") + name);
        return false;
    }
    
    // Validate plugin
    if (!validatePlugin(std::move(plugin))) {
        return false;
    }
    
    // Store plugin
    auto inserted = pImpl->plugins.insert({name, std::move(plugin)});
    if (!inserted.second) {
        return false;
    }
    
    pImpl->pluginEnabled[name] = true;
    
    NLM_LOG_INFO(std::string("Loaded plugin: ") + name + 
                std::string(" (v") + inserted.first->second->getVersion() + ")");
    
    return true;
}

bool PluginManager::unloadPlugin(const std::string& name) {
    auto it = pImpl->plugins.find(name);
    if (it == pImpl->plugins.end()) {
        NLM_LOG_ERROR(std::string("Plugin not found: ") + name);
        return false;
    }
    
    // Shutdown plugin
    it->second->shutdown();
    
    // Remove from tracking
    pImpl->plugins.erase(it);
    pImpl->pluginEnabled.erase(name);
    pImpl->pluginPaths.erase(name);
    pImpl->pluginConfigs.erase(name);
    
    NLM_LOG_INFO(std::string("Unloaded plugin: ") + name);
    return true;
}

bool PluginManager::unloadPlugin(std::unique_ptr<PluginInterface> plugin) {
    if (!plugin) return false;
    
    std::string name = plugin->getName();
    return unloadPlugin(name);
}

std::unique_ptr<PluginInterface> PluginManager::getPlugin(const std::string& name) {
    auto it = pImpl->plugins.find(name);
    if (it == pImpl->plugins.end()) {
        return nullptr;
    }
    
    return std::make_unique<PluginInterface>(*it->second);
}

std::vector<std::string> PluginManager::listPlugins() const {
    std::vector<std::string> pluginNames;
    for (const auto& entry : pImpl->plugins) {
        pluginNames.push_back(entry.first);
    }
    return pluginNames;
}

bool PluginManager::enablePlugin(const std::string& name) {
    auto it = pImpl->plugins.find(name);
    if (it == pImpl->plugins.end()) {
        NLM_LOG_ERROR(std::string("Plugin not found: ") + name);
        return false;
    }
    
    bool wasEnabled = pImpl->pluginEnabled[name];
    pImpl->pluginEnabled[name] = true;
    
    if (!wasEnabled) {
        NLM_LOG_INFO(std::string("Enabled plugin: ") + name);
    }
    
    return true;
}

bool PluginManager::disablePlugin(const std::string& name) {
    auto it = pImpl->plugins.find(name);
    if (it == pImpl->plugins.end()) {
        NLM_LOG_ERROR(std::string("Plugin not found: ") + name);
        return false;
    }
    
    bool wasEnabled = pImpl->pluginEnabled[name];
    pImpl->pluginEnabled[name] = false;
    
    if (wasEnabled) {
        NLM_LOG_INFO(std::string("Disabled plugin: ") + name);
    }
    
    return true;
}

bool PluginManager::isPluginEnabled(const std::string& name) const {
    auto it = pImpl->pluginEnabled.find(name);
    return it != pImpl->pluginEnabled.end() && it->second;
}

void PluginManager::onBrainInitialized(std::shared_ptr<class Brain> brain) {
    for (auto& entry : pImpl->plugins) {
        if (entry.second->isInitialized()) {
            entry.second->onBrainInitialized(brain);
        }
    }
}

void PluginManager::onBrainReset() {
    for (auto& entry : pImpl->plugins) {
        if (entry.second->isInitialized()) {
            entry.second->onBrainReset();
        }
    }
}

void PluginManager::onStepCompleted(SimulationStep step, Timestamp time) {
    for (auto& entry : pImpl->plugins) {
        if (entry.second->isInitialized()) {
            entry.second->onStepCompleted(step, time);
        }
    }
}

void PluginManager::onCheckpointSaved(const std::string& filepath) {
    for (auto& entry : pImpl->plugins) {
        if (entry.second->isInitialized()) {
            entry.second->onCheckpointSaved(filepath);
        }
    }
}

void PluginManager::onCheckpointLoaded(const std::string& filepath) {
    for (auto& entry : pImpl->plugins) {
        if (entry.second->isInitialized()) {
            entry.second->onCheckpointLoaded(filepath);
        }
    }
}

void PluginManager::configurePlugin(const std::string& name,
                                   const std::map<std::string, std::string>& config) {
    auto it = pImpl->plugins.find(name);
    if (it != pImpl->plugins.end()) {
        it->second->configure(config);
        pImpl->pluginConfigs[name] = config;
        NLM_LOG_INFO(std::string("Configured plugin: ") + name);
    }
}

std::map<std::string, std::string> PluginManager::getPluginConfig(const std::string& name) const {
    auto it = pImpl->pluginConfigs.find(name);
    if (it != pImpl->pluginConfigs.end()) {
        return it->second;
    }
    return std::map<std::string, std::string>();
}

std::vector<std::string> PluginManager::scanPluginDirectory(const std::string& directory) const {
    std::vector<std::string> pluginFiles;
    
    // In a real implementation, you would scan the directory for shared libraries
    // For now, we return an empty list as loading plugins is typically done manually
    
    return pluginFiles;
}

bool PluginManager::validatePlugin(std::unique_ptr<PluginInterface> plugin) const {
    if (!plugin) {
        NLM_LOG_ERROR("Cannot validate null plugin");
        return false;
    }
    
    bool valid = true;
    
    // Check required properties
    if (plugin->getName().empty()) {
        NLM_LOG_ERROR("Plugin must have a name");
        valid = false;
    }
    
    if (plugin->getVersion().empty()) {
        NLM_LOG_ERROR(std::string("Plugin ") + plugin->getName() + " must have a version");
        valid = false;
    }
    
    // Try to initialize plugin
    if (!plugin->initialize()) {
        NLM_LOG_ERROR(std::string("Plugin ") + plugin->getName() + " failed to initialize");
        valid = false;
    }
    
    // If validation passed, keep the plugin in a temporary container
    // (the caller is responsible for properly managing it)
    
    return valid;
}

} // namespace nlm