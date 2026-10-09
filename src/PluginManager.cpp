#include "PluginArchitecture.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <map>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <system_error>
#include <regex>

namespace nlm {

struct PluginManager::Impl {
    // Plugin registry
    PluginRegistry pluginRegistry;
    
    // Loaded plugins
    std::unordered_map<std::string, std::shared_ptr<IPlugin>> loadedPlugins;
    std::map<std::string, PluginState> pluginStates;
    std::map<std::string, std::chrono::time_point<std::chrono::system_clock>> pluginLoadTimes;
    
    // Plugin loaders
    std::map<std::string, std::function<std::shared_ptr<IPlugin>(const std::string&)>> pluginLoaders;
    
    // Plugin dependencies
    std::unordered_map<std::string, std::vector<std::string>> pluginDependencies;
    
    // Configuration
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> pluginConfigs;
    
    // Statistics
    size_t totalPluginsLoaded;
    size_t totalPluginsUnloaded;
    std::chrono::system_clock::time_point lastUpdate;
};

PluginManager::PluginManager() : pImpl(std::make_unique<Impl>()) {
    pImpl->totalPluginsLoaded = 0;
    pImpl->totalPluginsUnloaded = 0;
    pImpl->lastUpdate = std::chrono::system_clock::now();
    
    // Initialize default plugin loaders
    pImpl->pluginLoaders["shared_lib"] = [this](const std::string& path) {
        // Dynamic library loading would go here
        return nullptr;
    };
}

PluginManager::~PluginManager() {
    unloadAllPlugins();
}

PluginManager& PluginManager::getInstance() {
    static PluginManager instance;
    return instance;
}

bool PluginManager::loadPlugin(const std::string& pluginPath) {
    // Discover plugin in file system
    auto discovered = discoverPlugins(std::filesystem::path(pluginPath).parent_path());
    
    if (discovered.empty()) {
        NLM_LOG_ERROR("No plugins discovered in: " + pluginPath);
        return false;
    }
    
    bool success = true;
    for (const auto& pluginPath : discovered) {
        if (!loadPlugin(pluginPath)) {
            success = false;
        }
    }
    
    return success;
}

bool PluginManager::loadPlugin(const std::string& pluginPath) {
    std::string pluginName = std::filesystem::path(pluginPath).stem().string();
    
    NLM_LOG_INFO("Loading plugin: " + pluginName + " from " + pluginPath);
    
    // Check if already loaded
    if (pImpl->loadedPlugins.find(pluginName) != pImpl->loadedPlugins.end()) {
        NLM_LOG_WARNING("Plugin already loaded: " + pluginName);
        return true;
    }
    
    // Determine plugin loader
    std::string extension = std::filesystem::path(pluginPath).extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    auto loaderIt = pImpl->pluginLoaders.find(extension);
    if (loaderIt == pImpl->pluginLoaders.end()) {
        NLM_LOG_ERROR("No plugin loader available for extension: " + extension);
        return false;
    }
    
    // Load plugin
    auto plugin = loaderIt->second(pluginPath);
    if (!plugin) {
        NLM_LOG_ERROR("Failed to load plugin: " + pluginName);
        return false;
    }
    
    // Update state
    pImpl->loadedPlugins[pluginName] = plugin;
    pImpl->pluginStates[pluginName] = PluginState::Loading;
    pImpl->pluginLoadTimes[pluginName] = std::chrono::system_clock::now();
    
    // Check dependencies
    auto dependencies = plugin->getDependencies();
    bool dependenciesLoaded = true;
    for (const auto& dep : dependencies) {
        if (pImpl->pluginStates.find(dep) == pImpl->pluginStates.end() || 
            pImpl->pluginStates[dep] != PluginState::Initialized) {
            dependenciesLoaded = false;
            NLM_LOG_ERROR("Plugin dependency not available: " + dep + " for " + pluginName);
        }
    }
    
    if (!dependenciesLoaded) {
        pImpl->loadedPlugins.erase(pluginName);
        pImpl->pluginStates.erase(pluginName);
        pImpl->pluginLoadTimes.erase(pluginName);
        return false;
    }
    
    // Initialize plugin
    if (!plugin->initialize()) {
        NLM_LOG_ERROR("Plugin initialization failed: " + pluginName);
        pImpl->loadedPlugins.erase(pluginName);
        pImpl->pluginStates.erase(pluginName);
        pImpl->pluginLoadTimes.erase(pluginName);
        return false;
    }
    
    pImpl->pluginStates[pluginName] = PluginState::Initialized;
    pImpl->totalPluginsLoaded++;
    
    NLM_LOG_INFO("Plugin loaded and initialized: " + pluginName);
    return true;
}

bool PluginManager::unloadPlugin(const std::string& pluginName) {
    auto it = pImpl->loadedPlugins.find(pluginName);
    if (it == pImpl->loadedPlugins.end()) {
        NLM_LOG_WARNING("Plugin not loaded: " + pluginName);
        return true;
    }
    
    NLM_LOG_INFO("Unloading plugin: " + pluginName);
    
    // Stop plugin
    if (it->second->getState() == PluginState::Running) {
        if (!it->second->stop()) {
            NLM_LOG_ERROR("Plugin stop failed: " + pluginName);
            return false;
        }
    }
    
    // Unload plugin
    if (!it->second->unload()) {
        NLM_LOG_ERROR("Plugin unload failed: " + pluginName);
        return false;
    }
    
    // Remove from loaded plugins
    pImpl->loadedPlugins.erase(it);
    pImpl->pluginStates.erase(pluginName);
    pImpl->pluginLoadTimes.erase(pluginName);
    pImpl->pluginConfigs.erase(pluginName);
    
    pImpl->totalPluginsUnloaded++;
    
    NLM_LOG_INFO("Plugin unloaded: " + pluginName);
    return true;
}

bool PluginManager::reloadPlugin(const std::string& pluginName) {
    if (!unloadPlugin(pluginName)) {
        return false;
    }
    
    // Find original path (simplified - in real implementation would store paths)
    std::string pluginPath = pluginName + ".plugin"; // Placeholder
    
    return loadPlugin(pluginPath);
}

std::vector<std::string> PluginManager::discoverPlugins(const std::string& directory) {
    std::vector<std::string> plugins;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string extension = entry.path().extension().string();
                std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
                
                auto loaderIt = pImpl->pluginLoaders.find(extension);
                if (loaderIt != pImpl->pluginLoaders.end()) {
                    plugins.push_back(entry.path().string());
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        NLM_LOG_ERROR("Failed to discover plugins in " + directory + ": " + e.what());
    }
    
    return plugins;
}

std::vector<std::string> PluginManager::getLoadedPlugins() const {
    std::vector<std::string> pluginNames;
    for (const auto& pair : pImpl->loadedPlugins) {
        pluginNames.push_back(pair.first);
    }
    return pluginNames;
}

std::map<std::string, PluginState> PluginManager::getPluginStates() const {
    return pImpl->pluginStates;
}

std::shared_ptr<IPlugin> PluginManager::getPlugin(const std::string& name) {
    auto it = pImpl->loadedPlugins.find(name);
    if (it != pImpl->loadedPlugins.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<std::shared_ptr<IPlugin>> PluginManager::getPluginsByCategory(PluginCategory category) {
    std::vector<std::shared_ptr<IPlugin>> plugins;
    for (const auto& pair : pImpl->loadedPlugins) {
        if (pair.second->getCategory() == category) {
            plugins.push_back(pair.second);
        }
    }
    return plugins;
}

std::vector<std::shared_ptr<IPlugin>> PluginManager::getPluginsByInterface(const std::string& interface) {
    std::vector<std::shared_ptr<IPlugin>> plugins;
    for (const auto& pair : pImpl->loadedPlugins) {
        if (pair.second->queryInterface(interface)) {
            plugins.push_back(pair.second);
        }
    }
    return plugins;
}

bool PluginManager::configurePlugin(const std::string& name, const std::string& key, const std::string& value) {
    auto it = pImpl->loadedPlugins.find(name);
    if (it == pImpl->loadedPlugins.end()) {
        NLM_LOG_ERROR("Plugin not loaded: " + name);
        return false;
    }
    
    if (!it->second->configure(key, value)) {
        NLM_LOG_ERROR("Plugin configuration failed: " + name + ", key: " + key);
        return false;
    }
    
    pImpl->pluginConfigs[name][key] = value;
    return true;
}

std::string PluginManager::getPluginConfig(const std::string& name, const std::string& key) const {
    auto it = pImpl->loadedPlugins.find(name);
    if (it == pImpl->loadedPlugins.end()) {
        return "";
    }
    
    auto configIt = pImpl->pluginConfigs.find(name);
    if (configIt != pImpl->pluginConfigs.end()) {
        auto keyIt = configIt->second.find(key);
        if (keyIt != configIt->second.end()) {
            return keyIt->second;
        }
    }
    
    return "";
}

bool PluginManager::startAllPlugins() {
    bool success = true;
    for (auto& pair : pImpl->loadedPlugins) {
        if (!startPlugin(pair.first)) {
            success = false;
        }
    }
    return success;
}

bool PluginManager::stopAllPlugins() {
    bool success = true;
    for (auto& pair : pImpl->loadedPlugins) {
        if (!stopPlugin(pair.first)) {
            success = false;
        }
    }
    return success;
}

bool PluginManager::initializeAllPlugins() {
    bool success = true;
    for (auto& pair : pImpl->loadedPlugins) {
        if (pair.second->getState() == PluginState::Loaded) {
            if (!pair.second->initialize()) {
                success = false;
            }
        }
    }
    return success;
}

bool PluginManager::unloadAllPlugins() {
    bool success = true;
    for (auto& pair : pImpl->loadedPlugins) {
        if (!unloadPlugin(pair.first)) {
            success = false;
        }
    }
    return success;
}

bool PluginManager::integratePluginWithBrain(std::shared_ptr<IPlugin> plugin, std::shared_ptr<class Brain> brain) {
    if (!plugin || !brain) {
        return false;
    }
    
    // Try to find neural model interface
    auto neuralModelPlugin = std::dynamic_pointer_cast<INeuralModelPlugin>(plugin);
    if (neuralModelPlugin) {
        NLM_LOG_INFO("Integrating neural model plugin with brain");
        // Implementation would integrate neuron/synapse creation
        return true;
    }
    
    return false;
}

bool PluginManager::integratePluginWithEnvironment(std::shared_ptr<IPlugin> plugin, std::shared_ptr<class Environment> environment) {
    if (!plugin || !environment) {
        return false;
    }
    
    // Try to find environment plugin interface
    auto environmentPlugin = std::dynamic_pointer_cast<IEnvironmentPlugin>(plugin);
    if (environmentPlugin) {
        NLM_LOG_INFO("Integrating environment plugin with brain");
        // Implementation would integrate environmental simulation
        return true;
    }
    
    return false;
}

std::string PluginManager::getPluginInfo(const std::string& name) const {
    auto it = pImpl->loadedPlugins.find(name);
    if (it == pImpl->loadedPlugins.end()) {
        return "Plugin not found: " + name;
    }
    
    std::stringstream ss;
    ss << "Plugin: " << name << std::endl;
    ss << "State: " << getStateString(it->second->getState()) << std::endl;
    ss << "Category: " << getCategoryString(it->second->getCategory()) << std::endl;
    ss << "Version: " << it->second->getVersion() << std::endl;
    ss << "Description: " << it->second->getDescription() << std::endl;
    
    return ss.str();
}

std::string PluginManager::getSystemInfo() const {
    std::stringstream ss;
    ss << "=== NLM Plugin System Information ===" << std::endl;
    ss << "Total Plugins Loaded: " << pImpl->totalPluginsLoaded << std::endl;
    ss << "Total Plugins Unloaded: " << pImpl->totalPluginsUnloaded << std::endl;
    ss << "Registered Plugins: " << pImpl->pluginRegistry.getRegisteredPlugins().size() << std::endl;
    
    ss << "\nLoaded Plugins:" << std::endl;
    for (const auto& pair : pImpl->loadedPlugins) {
        ss << "  - " << pair.first << " (" << getStateString(pair.second->getState()) << ")" << std::endl;
    }
    
    return ss.str();
}

std::string PluginManager::getPluginDependencyInfo(const std::string& name) const {
    auto it = pImpl->loadedPlugins.find(name);
    if (it == pImpl->loadedPlugins.end()) {
        return "Plugin not found: " + name;
    }
    
    std::stringstream ss;
    ss << "Plugin Dependencies for " << name << ":" << std::endl;
    
    auto dependencies = it->second->getDependencies();
    if (dependencies.empty()) {
        ss << "  No dependencies" << std::endl;
    } else {
        for (const auto& dep : dependencies) {
            auto depIt = pImpl->pluginStates.find(dep);
            if (depIt != pImpl->pluginStates.end()) {
                ss << "  - " << dep << " (" << getStateString(depIt->second) << ")" << std::endl;
            } else {
                ss << "  - " << dep << " (Not loaded)" << std::endl;
            }
        }
    }
    
    return ss.str();
}

bool PluginManager::startPlugin(const std::string& name) {
    auto it = pImpl->loadedPlugins.find(name);
    if (it == pImpl->loadedPlugins.end()) {
        return false;
    }
    
    if (it->second->getState() == PluginState::Initialized) {
        if (!it->second->start()) {
            NLM_LOG_ERROR("Plugin start failed: " + name);
            return false;
        }
        pImpl->pluginStates[name] = PluginState::Running;
    }
    
    return true;
}

bool PluginManager::stopPlugin(const std::string& name) {
    auto it = pImpl->loadedPlugins.find(name);
    if (it == pImpl->loadedPlugins.end()) {
        return false;
    }
    
    if (it->second->getState() == PluginState::Running) {
        if (!it->second->stop()) {
            NLM_LOG_ERROR("Plugin stop failed: " + name);
            return false;
        }
        pImpl->pluginStates[name] = PluginState::Initialized;
    }
    
    return true;
}

} // namespace nlm
