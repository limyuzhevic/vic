#include "PluginSystem.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../core/Config/Config.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <dlfcn.h>
#include <memory>
#include <stdexcept>

namespace nlm {

struct PluginSystem::Impl {
    std::map<std::string, PluginPtr> plugins;
    std::map<std::string, PluginInfo> pluginInfo;
    std::map<std::string, std::function<bool(const std::vector<std::string>&)>> commandHandlers;
    std::vector<std::string> configKeys;
    bool initialized = false;
    std::string configFilepath;
};

PluginSystem::PluginSystem() : pImpl(std::make_unique<Impl>()) {
}

PluginSystem::~PluginSystem() {
    shutdownAllPlugins();
}

bool PluginSystem::loadPlugin(const std::string& filePath) {
    // Check if plugin already loaded
    if (findPluginByName(filePath) != nullptr) {
        return true;
    }
    
    // Load plugin library
    void* handle = dlopen(filePath.c_str(), RTLD_LAZY);
    if (!handle) {
        return false;
    }
    
    // Try to get createPlugin function
    using CreatePluginFunc = PluginPtr (*)();
    auto createPlugin = reinterpret_cast<CreatePluginFunc>(dlsym(handle, "createPlugin"));
    if (!createPlugin) {
        dlclose(handle);
        return false;
    }
    
    // Create plugin
    PluginPtr plugin = createPlugin();
    if (!plugin) {
        dlclose(handle);
        return false;
    }
    
    // Register plugin
    loadPlugin(plugin);
    
    return true;
}

bool PluginSystem::loadPlugin(const PluginPtr& plugin) {
    if (!plugin) return false;
    
    std::string name = plugin->getName();
    
    // Check if already loaded
    if (pImpl->plugins.find(name) != pImpl->plugins.end()) {
        return true;
    }
    
    // Store plugin info
    PluginInfo info;
    info.name = plugin->getName();
    info.version = plugin->getVersion();
    info.description = plugin->getDescription();
    info.author = plugin->getAuthor();
    info.enabled = true;
    info.error = "";
    
    pImpl->plugins[name] = plugin;
    pImpl->pluginInfo[name] = info;
    
    return true;
}

bool PluginSystem::unloadPlugin(const std::string& name) {
    auto it = pImpl->plugins.find(name);
    if (it == pImpl->plugins.end()) return false;
    
    // Disable first
    disablePlugin(name);
    
    // Remove from lists
    pImpl->plugins.erase(it);
    pImpl->pluginInfo.erase(name);
    
    // Remove command handlers
    pImpl->commandHandlers.erase(name);
    
    // Remove config keys
    auto configKeyIt = std::remove(pImpl->configKeys.begin(), pImpl->configKeys.end(), name + "_" + name);
    pImpl->configKeys.erase(configKeyIt, pImpl->configKeys.end());
    
    return true;
}

bool PluginSystem::unloadPlugin(const PluginPtr& plugin) {
    if (!plugin) return false;
    return unloadPlugin(plugin->getName());
}

bool PluginSystem::enablePlugin(const std::string& name) {
    auto it = pImpl->plugins.find(name);
    if (it == pImpl->plugins.end()) return false;
    
    if (!it->second->isInitialized()) {
        // Try to initialize
        it->second->initialize({});
        if (!it->second->isInitialized()) {
            return false;
        }
    }
    
    pImpl->pluginInfo[name].enabled = true;
    return true;
}

bool PluginSystem::disablePlugin(const std::string& name) {
    auto it = pImpl->plugins.find(name);
    if (it == pImpl->plugins.end()) return false;
    
    if (it->second->isInitialized()) {
        it->second->shutdown();
    }
    
    pImpl->pluginInfo[name].enabled = false;
    return true;
}

bool PluginSystem::isPluginEnabled(const std::string& name) const {
    auto it = pImpl->pluginInfo.find(name);
    if (it != pImpl->pluginInfo.end()) {
        return it->second.enabled;
    }
    return false;
}

PluginPtr PluginSystem::getPlugin(const std::string& name) const {
    auto it = pImpl->plugins.find(name);
    if (it != pImpl->plugins.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<PluginPtr> PluginSystem::getPlugins() const {
    std::vector<PluginPtr> result;
    for (const auto& pair : pImpl->plugins) {
        result.push_back(pair.second);
    }
    return result;
}

std::vector<std::string> PluginSystem::getPluginNames() const {
    std::vector<std::string> names;
    for (const auto& pair : pImpl->plugins) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<PluginSystem::PluginInfo> PluginSystem::getPluginInfo() const {
    std::vector<PluginInfo> result;
    for (const auto& pair : pImpl->pluginInfo) {
        result.push_back(pair.second);
    }
    return result;
}

PluginPtr PluginSystem::findPluginByName(const std::string& name) const {
    auto it = pImpl->plugins.find(name);
    if (it != pImpl->plugins.end()) {
        return it->second;
    }
    return nullptr;
}

bool PluginSystem::initializeAllPlugins(const std::map<std::string, std::string>& globalConfig) {
    pImpl->initialized = false;
    
    for (auto& pair : pImpl->plugins) {
        if (pair.second->isInitialized()) {
            continue;
        }
        
        // Convert global config to plugin-specific config
        std::map<std::string, std::string> config;
        for (const auto& globalPair : globalConfig) {
            if (globalPair.first.find(pair.first + "_") == 0) {
                std::string key = globalPair.first.substr(pair.first.size() + 1);
                config[key] = globalPair.second;
            }
        }
        
        if (!pair.second->initialize(config)) {
            pImpl->pluginInfo[pair.first].error = "Failed to initialize plugin";
            return false;
        }
    }
    
    pImpl->initialized = true;
    return true;
}

void PluginSystem::shutdownAllPlugins() {
    for (auto& pair : pImpl->plugins) {
        if (pair.second->isInitialized()) {
            pair.second->shutdown();
        }
    }
    
    pImpl->initialized = false;
}

void PluginSystem::notifyBrainInitialize(std::shared_ptr<Brain> brain) {
    for (auto& pair : pImpl->plugins) {
        if (pair.second->isInitialized()) {
            pair.second->onBrainInitialize(brain);
        }
    }
}

void PluginSystem::notifyBrainReset(std::shared_ptr<Brain> brain) {
    for (auto& pair : pImpl->plugins) {
        if (pair.second->isInitialized()) {
            pair.second->onBrainReset(brain);
        }
    }
}

void PluginSystem::notifyBrainStep(std::shared_ptr<Brain> brain, SimulationStep step, Timestamp time) {
    for (auto& pair : pImpl->plugins) {
        if (pair.second->isInitialized()) {
            pair.second->onBrainStep(brain, step, time);
        }
    }
}

void PluginSystem::notifyNeuronSpike(std::shared_ptr<Brain> brain, std::shared_ptr<Neuron> neuron) {
    for (auto& pair : pImpl->plugins) {
        if (pair.second->isInitialized()) {
            pair.second->onNeuronSpike(brain, neuron);
        }
    }
}

void PluginSystem::notifySynapseChange(std::shared_ptr<Brain> brain, std::shared_ptr<Synapse> synapse) {
    for (auto& pair : pImpl->plugins) {
        if (pair.second->isInitialized()) {
            pair.second->onSynapseChange(brain, synapse);
        }
    }
}

void PluginSystem::notifyExperimentStart(std::shared_ptr<Brain> brain) {
    for (auto& pair : pImpl->plugins) {
        if (pair.second->isInitialized()) {
            pair.second->onExperimentStart(brain);
        }
    }
}

void PluginSystem::notifyExperimentEnd(std::shared_ptr<Brain> brain) {
    for (auto& pair : pImpl->plugins) {
        if (pair.second->isInitialized()) {
            pair.second->onExperimentEnd(brain);
        }
    }
}

std::vector<std::string> PluginSystem::getAllCommandCompletions(const std::string& partialCommand) const {
    std::vector<std::string> completions;
    
    for (const auto& pair : pImpl->plugins) {
        if (pair.second->isInitialized()) {
            auto pluginCompletions = pair.second->getCommandCompletions(partialCommand);
            completions.insert(completions.end(), pluginCompletions.begin(), pluginCompletions.end());
        }
    }
    
    return completions;
}

std::function<bool(const std::vector<std::string>&)> PluginSystem::getCommandHandler(const std::string& command) const {
    for (const auto& pair : pImpl->plugins) {
        if (pair.second->isInitialized()) {
            auto handler = pair.second->getCommandHandler(command);
            if (handler) {
                return handler;
            }
        }
    }
    return nullptr;
}

std::vector<std::string> PluginSystem::getAllConfigKeys() const {
    std::vector<std::string> keys;
    
    for (const auto& pair : pImpl->plugins) {
        if (pair.second->isInitialized()) {
            auto pluginKeys = pair.second->getConfigKeys();
            keys.insert(keys.end(), pluginKeys.begin(), pluginKeys.end());
        }
    }
    
    return keys;
}

std::string PluginSystem::getDefaultConfigValue(const std::string& key) const {
    // Extract plugin name from key
    size_t underscorePos = key.find('_');
    if (underscorePos == std::string::npos) {
        return "";
    }
    
    std::string pluginName = key.substr(0, underscorePos);
    std::string pluginKey = key.substr(underscorePos + 1);
    
    auto pluginIt = pImpl->plugins.find(pluginName);
    if (pluginIt != pImpl->plugins.end()) {
        return pluginIt->second->getDefaultConfig(pluginKey);
    }
    
    return "";
}

bool PluginSystem::savePluginConfig(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) return false;
    
    file << "# NLM Plugin Configuration\n";
    file << "# Generated by PluginSystem\n\n";
    
    for (const auto& pair : pImpl->pluginInfo) {
        file << "[" << pair.first << "]\n";
        file << "enabled = " << (pair.second.enabled ? "true" : "false") << "\n";
        file << "description = '" << pair.second.description << "'\n";
        
        file << "\n";
    }
    
    file.close();
    return true;
}

bool PluginSystem::loadPluginConfig(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;
    
    std::string currentSection;
    std::string line;
    
    while (std::getline(file, line)) {
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        if (line.empty() || line[0] == '#') continue;
        
        if (line[0] == '[' && line.back() == ']') {
            currentSection = line.substr(1, line.size() - 2);
        } else if (currentSection.empty()) {
            continue;
        } else if (line.find("enabled = ") == 0) {
            std::string value = line.substr(9);
            bool enabled = (value == "true" || value == "1" || value == "yes");
            
            auto pluginIt = pImpl->plugins.find(currentSection);
            if (pluginIt != pImpl->plugins.end()) {
                pImpl->pluginInfo[currentSection].enabled = enabled;
            }
        }
    }
    
    file.close();
    return true;
}

std::vector<std::string> PluginSystem::discoverPlugins(const std::string& directory) const {
    std::vector<std::string> plugins;
    
    if (!std::filesystem::exists(directory)) {
        return plugins;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().filename().string();
            
            // Check if it's a shared library
            if (filename.find(".so") != std::string::npos || 
                filename.find(".dylib") != std::string::npos || 
                filename.find(".dll") != std::string::npos) {
                plugins.push_back(entry.path().string());
            }
        }
    }
    
    return plugins;
}

std::string PluginSystem::getPluginManifest(const std::string& name) const {
    auto it = pImpl->pluginInfo.find(name);
    if (it == pImpl->pluginInfo.end()) return "";
    
    return it->second.name + "\n" +
           "Version: " + it->second.version + "\n" +
           "Description: " + it->second.description + "\n" +
           "Author: " + it->second.author + "\n" +
           "Enabled: " + (it->second.enabled ? "true" : "false") + "\n";
}

} // namespace nlm