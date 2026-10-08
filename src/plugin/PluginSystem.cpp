#include "PluginSystem.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>

namespace nlm {

// PluginRegistry implementation
struct PluginRegistry::Impl {
    std::shared_mutex mutex;
    
    // Plugin management
    std::map<std::string, std::unique_ptr<PluginInterface>> plugins;
    std::map<std::string, PluginMetadata> pluginMetadata;
    std::map<std::string, std::map<std::string, std::string>> pluginConfigs;
    std::set<std::string> enabledPlugins;
    std::set<std::string> loadedPlugins;
    
    // Plugin discovery
    std::vector<std::string> availablePlugins;
    std::map<std::string, std::vector<std::string>> pluginDependencies;
    
    // System integration
    std::weak_ptr<Brain> currentBrain;
    std::weak_ptr<AgentBrain> currentAgent;
    std::weak_ptr<Environment> currentWorld;
    
    // Plugin factories and loaders
    std::vector<std::unique_ptr<PluginLoader>> loaders;
    std::map<std::string, std::unique_ptr<PluginFactory>> factories;
    
    // Statistics
    std::map<std::string, std::map<std::string, std::string>> pluginStats;
    
    // Health monitoring
    std::vector<std::string> healthIssues;
    
    Impl() {
        // Initialize default loaders
        loaders.push_back(std::make_unique<DynamicLibraryPluginLoader>());
        loaders.push_back(std::make_unique<ScriptPluginLoader>());
    }
};

PluginRegistry::PluginRegistry() {
    pImpl = std::make_unique<Impl>();
}

PluginRegistry::~PluginRegistry() {
    shutdownAllPlugins();
}

bool PluginRegistry::loadPlugin(const std::string& pluginPath, const std::string& name) {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    
    try {
        // Determine plugin name
        std::string pluginName = name;
        if (pluginName.empty()) {
            // Extract name from path
            size_t lastSlash = pluginPath.find_last_of("/\\");
            if (lastSlash != std::string::npos) {
                pluginName = pluginPath.substr(lastSlash + 1);
                // Remove extension
                size_t dotPos = pluginName.find_last_of('.');
                if (dotPos != std::string::npos) {
                    pluginName = pluginName.substr(0, dotPos);
                }
            } else {
                pluginName = pluginPath;
            }
        }
        
        // Check if plugin already loaded
        if (pImpl->loadedPlugins.find(pluginName) != pImpl->loadedPlugins.end()) {
            NLM_LOG_WARNING("Plugin already loaded: " + pluginName);
            return true;
        }
        
        // Find suitable loader
        std::unique_ptr<PluginLoader> loader;
        for (auto& l : pImpl->loaders) {
            if (l->canLoad(pluginPath)) {
                loader = std::move(l);
                break;
            }
        }
        
        if (!loader) {
            NLM_LOG_ERROR("No suitable loader found for plugin: " + pluginPath);
            return false;
        }
        
        // Load plugin
        auto plugin = loader->loadPlugin(pluginPath);
        if (!plugin) {
            NLM_LOG_ERROR("Failed to load plugin from: " + pluginPath);
            return false;
        }
        
        // Get metadata
        const auto& metadata = plugin->getMetadata();
        
        // Check dependencies
        bool dependenciesSatisfied = true;
        for (const auto& dep : metadata.dependencies) {
            if (pImpl->loadedPlugins.find(dep) == pImpl->loadedPlugins.end()) {
                NLM_LOG_ERROR("Plugin dependency not satisfied: " + dep + " (required by " + pluginName + ")");
                dependenciesSatisfied = false;
                break;
            }
        }
        
        if (!dependenciesSatisfied) {
            NLM_LOG_ERROR("Cannot load plugin " + pluginName + " due to unsatisfied dependencies");
            return false;
        }
        
        // Store plugin
        pImpl->plugins[pluginName] = std::move(plugin);
        pImpl->pluginMetadata[pluginName] = metadata;
        pImpl->loadedPlugins.insert(pluginName);
        
        if (metadata.enabled) {
            pImpl->enabledPlugins.insert(pluginName);
        }
        
        // Update stats
        pImpl->pluginStats[pluginName]["load_count"] = std::to_string(pImpl->pluginMetadata[pluginName].loadCount++);
        pImpl->pluginStats[pluginName]["status"] = "loaded";
        
        NLM_LOG_INFO("Plugin loaded successfully: " + pluginName + " (version: " + metadata.version + ")");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception loading plugin: " + std::string(e.what()));
        return false;
    }
}

bool PluginRegistry::loadPluginFromString(const std::string& pluginCode, const std::string& name) {
    // For now, use a simple approach - save to temp file and load
    std::string tempFile = "temp_plugin_" + std::to_string(std::hash<std::string>{}(pluginCode)) + ".so";
    
    try {
        std::ofstream file(tempFile, std::ios::binary);
        if (!file) {
            NLM_LOG_ERROR("Failed to create temp file for plugin: " + tempFile);
            return false;
        }
        
        file.write(pluginCode.c_str(), pluginCode.size());
        file.close();
        
        bool result = loadPlugin(tempFile, name);
        
        // Clean up temp file
        std::remove(tempFile.c_str());
        
        return result;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception loading plugin from string: " + std::string(e.what()));
        return false;
    }
}

bool PluginRegistry::unloadPlugin(const std::string& name) {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    
    auto it = pImpl->plugins.find(name);
    if (it == pImpl->plugins.end()) {
        NLM_LOG_WARNING("Plugin not found for unloading: " + name);
        return false;
    }
    
    try {
        // If plugin was enabled, disable it first
        if (pImpl->enabledPlugins.find(name) != pImpl->enabledPlugins.end()) {
            pImpl->enabledPlugins.erase(name);
        }
        
        // Shutdown plugin
        it->second->shutdown();
        
        // Remove from collections
        pImpl->plugins.erase(it);
        pImpl->pluginMetadata.erase(name);
        pImpl->pluginConfigs.erase(name);
        pImpl->loadedPlugins.erase(name);
        
        // Update stats
        if (pImpl->pluginStats.find(name) != pImpl->pluginStats.end()) {
            pImpl->pluginStats[name]["unload_count"] = std::to_string(pImpl->pluginMetadata[name].unloadCount++);
            pImpl->pluginStats[name]["status"] = "unloaded";
        }
        
        NLM_LOG_INFO("Plugin unloaded successfully: " + name);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception unloading plugin " + name + ": " + std::string(e.what()));
        return false;
    }
}

bool PluginRegistry::reloadPlugin(const std::string& name) {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    
    auto it = pImpl->plugins.find(name);
    if (it == pImpl->plugins.end()) {
        NLM_LOG_ERROR("Plugin not found for reloading: " + name);
        return false;
    }
    
    try {
        // Get current config
        auto config = pImpl->pluginConfigs.find(name);
        
        // Unload plugin
        unloadPlugin(name);
        
        // We can't easily reload a dynamic library plugin, so we need to find the original path
        // For now, we'll just return false indicating reload not supported
        NLM_LOG_WARNING("Plugin reload not fully supported - plugin would need to be reloaded from source");
        
        return false;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception reloading plugin " + name + ": " + std::string(e.what()));
        return false;
    }
}

std::vector<std::string> PluginRegistry::getAvailablePlugins() const {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    return pImpl->availablePlugins;
}

std::vector<std::string> PluginRegistry::getLoadedPlugins() const {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    std::vector<std::string> result(pImpl->loadedPlugins.begin(), pImpl->loadedPlugins.end());
    return result;
}

std::vector<std::string> PluginRegistry::getEnabledPlugins() const {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    std::vector<std::string> result(pImpl->enabledPlugins.begin(), pImpl->enabledPlugins.end());
    return result;
}

bool PluginRegistry::enablePlugin(const std::string& name) {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    
    auto it = pImpl->plugins.find(name);
    if (it == pImpl->plugins.end()) {
        NLM_LOG_ERROR("Plugin not found: " + name);
        return false;
    }
    
    pImpl->enabledPlugins.insert(name);
    
    // Initialize plugin if it's being enabled
    if (!it->second->isInitialized()) {
        const auto& metadata = pImpl->pluginMetadata[name];
        auto configIt = pImpl->pluginConfigs.find(name);
        if (configIt != pImpl->pluginConfigs.end()) {
            if (!it->second->initialize(configIt->second)) {
                NLM_LOG_ERROR("Failed to initialize enabled plugin: " + name);
                pImpl->enabledPlugins.erase(name);
                return false;
            }
        } else if (!it->second->initialize({})) {
            NLM_LOG_ERROR("Failed to initialize enabled plugin (no config): " + name);
            pImpl->enabledPlugins.erase(name);
            return false;
        }
    }
    
    NLM_LOG_INFO("Plugin enabled: " + name);
    return true;
}

bool PluginRegistry::disablePlugin(const std::string& name) {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    
    auto it = pImpl->plugins.find(name);
    if (it == pImpl->plugins.end()) {
        NLM_LOG_ERROR("Plugin not found: " + name);
        return false;
    }
    
    pImpl->enabledPlugins.erase(name);
    NLM_LOG_INFO("Plugin disabled: " + name);
    return true;
}

bool PluginRegistry::setPluginConfig(const std::string& name, const std::map<std::string, std::string>& config) {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    
    auto it = pImpl->plugins.find(name);
    if (it == pImpl->plugins.end()) {
        NLM_LOG_ERROR("Plugin not found: " + name);
        return false;
    }
    
    pImpl->pluginConfigs[name] = config;
    
    // If plugin is initialized and enabled, update config
    if (it->second->isInitialized() && pImpl->enabledPlugins.find(name) != pImpl->enabledPlugins.end()) {
        // Re-initialize with new config
        if (!it->second->initialize(config)) {
            NLM_LOG_ERROR("Failed to update plugin config: " + name);
            return false;
        }
    }
    
    NLM_LOG_INFO("Plugin config updated: " + name);
    return true;
}

std::map<std::string, std::string> PluginRegistry::getPluginConfig(const std::string& name) const {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    
    auto it = pImpl->pluginConfigs.find(name);
    if (it != pImpl->pluginConfigs.end()) {
        return it->second;
    }
    
    return {};
}

bool PluginRegistry::initializeAllPlugins(const std::map<std::string, std::map<std::string, std::string>>& configs) {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    
    for (const auto& pair : pImpl->plugins) {
        const std::string& name = pair.first;
        
        // Skip already initialized plugins
        if (pair.second->isInitialized()) {
            continue;
        }
        
        // Get config
        auto configIt = configs.find(name);
        const std::map<std::string, std::string>* config = nullptr;
        if (configIt != configs.end()) {
            config = &configIt->second;
        } else {
            configIt = pImpl->pluginConfigs.find(name);
            if (configIt != pImpl->pluginConfigs.end()) {
                config = &configIt->second;
            }
        }
        
        // Initialize plugin
        if (config && !pair.second->initialize(*config)) {
            NLM_LOG_ERROR("Failed to initialize plugin: " + name);
            return false;
        } else if (!pair.second->initialize({}) && config && !config->empty()) {
            NLM_LOG_ERROR("Failed to initialize plugin (with empty config): " + name);
            return false;
        }
        
        // Enable plugin if it's in enabled list
        if (pImpl->enabledPlugins.find(name) != pImpl->enabledPlugins.end()) {
            // Already handled above
        }
    }
    
    NLM_LOG_INFO("All plugins initialized successfully");
    return true;
}

void PluginRegistry::shutdownAllPlugins() {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    
    for (auto& pair : pImpl->plugins) {
        try {
            pair.second->shutdown();
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error shutting down plugin " + pair.first + ": " + std::string(e.what()));
        }
    }
    
    // Clear collections
    pImpl->plugins.clear();
    pImpl->pluginMetadata.clear();
    pImpl->pluginConfigs.clear();
    pImpl->enabledPlugins.clear();
    pImpl->loadedPlugins.clear();
    
    NLM_LOG_INFO("All plugins shutdown complete");
}

void PluginRegistry::onBrainCreated(std::shared_ptr<Brain> brain) {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    
    pImpl->currentBrain = brain;
    
    for (auto& pair : pImpl->plugins) {
        if (pImpl->enabledPlugins.find(pair.first) != pImpl->enabledPlugins.end()) {
            try {
                pair.second->onBrainCreated(brain);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error in plugin " + pair.first + " onBrainCreated: " + std::string(e.what()));
            }
        }
    }
}

void PluginRegistry::onBrainDestroyed(std::shared_ptr<Brain> brain) {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    
    for (auto& pair : pImpl->plugins) {
        if (pImpl->enabledPlugins.find(pair.first) != pImpl->enabledPlugins.end()) {
            try {
                pair.second->onBrainDestroyed(brain);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error in plugin " + pair.first + " onBrainDestroyed: " + std::string(e.what()));
            }
        }
    }
    
    pImpl->currentBrain.reset();
}

void PluginRegistry::onAgentCreated(std::shared_ptr<AgentBrain> agent) {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    
    pImpl->currentAgent = agent;
    
    for (auto& pair : pImpl->plugins) {
        if (pImpl->enabledPlugins.find(pair.first) != pImpl->enabledPlugins.end()) {
            try {
                pair.second->onAgentCreated(agent);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error in plugin " + pair.first + " onAgentCreated: " + std::string(e.what()));
            }
        }
    }
}

void PluginRegistry::onAgentDestroyed(std::shared_ptr<AgentBrain> agent) {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    
    for (auto& pair : pImpl->plugins) {
        if (pImpl->enabledPlugins.find(pair.first) != pImpl->enabledPlugins.end()) {
            try {
                pair.second->onAgentDestroyed(agent);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error in plugin " + pair.first + " onAgentDestroyed: " + std::string(e.what()));
            }
        }
    }
    
    pImpl->currentAgent.reset();
}

void PluginRegistry::onWorldCreated(std::shared_ptr<Environment> world) {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    
    pImpl->currentWorld = world;
    
    for (auto& pair : pImpl->plugins) {
        if (pImpl->enabledPlugins.find(pair.first) != pImpl->enabledPlugins.end()) {
            try {
                pair.second->onWorldCreated(world);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error in plugin " + pair.first + " onWorldCreated: " + std::string(e.what()));
            }
        }
    }
}

void PluginRegistry::onWorldDestroyed(std::shared_ptr<Environment> world) {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    
    for (auto& pair : pImpl->plugins) {
        if (pImpl->enabledPlugins.find(pair.first) != pImpl->enabledPlugins.end()) {
            try {
                pair.second->onWorldDestroyed(world);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error in plugin " + pair.first + " onWorldDestroyed: " + std::string(e.what()));
            }
        }
    }
    
    pImpl->currentWorld.reset();
}

void PluginRegistry::beforeStep(double timestep) {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    
    for (auto& pair : pImpl->plugins) {
        if (pImpl->enabledPlugins.find(pair.first) != pImpl->enabledPlugins.end()) {
            try {
                pair.second->beforeStep(timestep);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error in plugin " + pair.first + " beforeStep: " + std::string(e.what()));
            }
        }
    }
}

void PluginRegistry::afterStep(double timestep) {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    
    for (auto& pair : pImpl->plugins) {
        if (pImpl->enabledPlugins.find(pair.first) != pImpl->enabledPlugins.end()) {
            try {
                pair.second->afterStep(timestep);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error in plugin " + pair.first + " afterStep: " + std::string(e.what()));
            }
        }
    }
}

void PluginRegistry::beforeReset() {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    
    for (auto& pair : pImpl->plugins) {
        if (pImpl->enabledPlugins.find(pair.first) != pImpl->enabledPlugins.end()) {
            try {
                pair.second->beforeReset();
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error in plugin " + pair.first + " beforeReset: " + std::string(e.what()));
            }
        }
    }
}

void PluginRegistry::afterReset() {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    
    for (auto& pair : pImpl->plugins) {
        if (pImpl->enabledPlugins.find(pair.first) != pImpl->enabledPlugins.end()) {
            try {
                pair.second->afterReset();
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error in plugin " + pair.first + " afterReset: " + std::string(e.what()));
            }
        }
    }
}

void PluginRegistry::beforeSave() {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    
    for (auto& pair : pImpl->plugins) {
        if (pImpl->enabledPlugins.find(pair.first) != pImpl->enabledPlugins.end()) {
            try {
                pair.second->beforeSave();
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error in plugin " + pair.first + " beforeSave: " + std::string(e.what()));
            }
        }
    }
}

void PluginRegistry::afterSave() {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    
    for (auto& pair : pImpl->plugins) {
        if (pImpl->enabledPlugins.find(pair.first) != pImpl->enabledPlugins.end()) {
            try {
                pair.second->afterSave();
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error in plugin " + pair.first + " afterSave: " + std::string(e.what()));
            }
        }
    }
}

void PluginRegistry::beforeLoad() {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    
    for (auto& pair : pImpl->plugins) {
        if (pImpl->enabledPlugins.find(pair.first) != pImpl->enabledPlugins.end()) {
            try {
                pair.second->beforeLoad();
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error in plugin " + pair.first + " beforeLoad: " + std::string(e.what()));
            }
        }
    }
}

void PluginRegistry::afterLoad() {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    
    for (auto& pair : pImpl->plugins) {
        if (pImpl->enabledPlugins.find(pair.first) != pImpl->enabledPlugins.end()) {
            try {
                pair.second->afterLoad();
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error in plugin " + pair.first + " afterLoad: " + std::string(e.what()));
            }
        }
    }
}

std::vector<std::string> PluginRegistry::getAvailableCommands() const {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    
    std::vector<std::string> commands;
    for (const auto& pair : pImpl->plugins) {
        if (pImpl->enabledPlugins.find(pair.first) != pImpl->enabledPlugins.end()) {
            auto pluginCommands = pair.second->getAvailableCommands();
            commands.insert(commands.end(), pluginCommands.begin(), pluginCommands.end());
        }
    }
    return commands;
}

std::map<std::string, std::string> PluginRegistry::executeCommand(const std::string& command,
                                                                const std::map<std::string, std::string>& args) {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    
    for (auto& pair : pImpl->plugins) {
        if (pImpl->enabledPlugins.find(pair.first) != pImpl->enabledPlugins.end()) {
            auto pluginCommands = pair.second->getAvailableCommands();
            if (std::find(pluginCommands.begin(), pluginCommands.end(), command) != pluginCommands.end()) {
                try {
                    return pair.second->executeCommand(command, args);
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Error executing command " + command + " in plugin " + pair.first + ": " + std::string(e.what()));
                    return {{"error", "Command execution failed: " + std::string(e.what())}};
                }
            }
        }
    }
    
    NLM_LOG_WARNING("Command not found: " + command);
    return {{"error", "Command not found: " + command}};
}

std::map<std::string, std::map<std::string, std::string>> PluginRegistry::getPluginStats() const {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    return pImpl->pluginStats;
}

std::map<std::string, std::string> PluginRegistry::getSystemStats() const {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    
    std::map<std::string, std::string> stats;
    stats["total_plugins"] = std::to_string(pImpl->plugins.size());
    stats["loaded_plugins"] = std::to_string(pImpl->loadedPlugins.size());
    stats["enabled_plugins"] = std::to_string(pImpl->enabledPlugins.size());
    stats["plugin_loaders"] = std::to_string(pImpl->loaders.size());
    
    double totalLoadTime = 0.0;
    for (const auto& pair : pImpl->pluginMetadata) {
        totalLoadTime += pair.second.totalExecutionTime;
    }
    stats["total_execution_time"] = std::to_string(totalLoadTime);
    
    return stats;
}

std::vector<std::string> PluginRegistry::discoverPlugins(const std::string& directory) {
    // For now, return empty vector - discovery would need directory scanning
    return {};
}

std::vector<std::string> PluginRegistry::getPluginInfo(const std::string& name) const {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    
    auto it = pImpl->pluginMetadata.find(name);
    if (it == pImpl->pluginMetadata.end()) {
        return {};
    }
    
    std::vector<std::string> info;
    info.push_back("Name: " + it->second.name);
    info.push_back("Version: " + it->second.version);
    info.push_back("Description: " + it->second.description);
    info.push_back("Author: " + it->second.author);
    info.push_back("Type: " + std::to_string(static_cast<int>(it->second.type)));
    info.push_back("Enabled: " + std::string(it->second.enabled ? "true" : "false"));
    info.push_back("Loaded: " + std::string(it->second.loaded ? "true" : "false"));
    
    return info;
}

bool PluginRegistry::isHealthy() const {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    
    pImpl->healthIssues.clear();
    
    for (const auto& pair : pImpl->plugins) {
        if (pImpl->enabledPlugins.find(pair.first) != pImpl->enabledPlugins.end()) {
            if (!pair.second->isHealthy()) {
                pImpl->healthIssues.push_back("Plugin unhealthy: " + pair.first);
            }
        }
    }
    
    return pImpl->healthIssues.empty();
}

std::vector<std::string> PluginRegistry::getHealthIssues() const {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    return pImpl->healthIssues;
}

bool PluginRegistry::installPlugin(const std::string& source, const std::string& target) {
    // Simple plugin installation - copy to target directory
    if (target.empty()) {
        // Use default plugin directory
        std::string pluginDir = "plugins";
        #ifdef _WIN32
            system(("mkdir " + pluginDir).c_str());
        #else
            system(("mkdir -p " + pluginDir).c_str());
        #endif
    } else {
        #ifdef _WIN32
            system(("mkdir " + target).c_str());
        #else
            system(("mkdir -p " + target).c_str());
        #endif
    }
    
    // For now, just return success
    return true;
}

bool PluginRegistry::uninstallPlugin(const std::string& name) {
    return unloadPlugin(name);
}

bool PluginRegistry::backupPlugin(const std::string& name, const std::string& backupPath) {
    // Simple backup - copy plugin directory
    if (backupPath.empty()) {
        // Create backup in backup directory
        std::string backupDir = "backups";
        #ifdef _WIN32
            system(("mkdir " + backupDir).c_str());
        #else
            system(("mkdir -p " + backupDir).c_str());
        #endif
        
        std::string backupFile = backupDir + "/" + name + "_backup.tar.gz";
        // In a real implementation, we would create a proper archive
        return true;
    }
    
    // Create backup in specified path
    // In a real implementation, we would create a proper archive
    return true;
}

bool PluginRegistry::restorePlugin(const std::string& name, const std::string& backupPath) {
    // Restore plugin from backup
    // In a real implementation, we would extract from archive
    return true;
}

std::vector<std::string> PluginRegistry::searchPlugins(const std::string& query) const {
    // Simple search - just return empty for now
    return {};
}

bool PluginRegistry::fetchPlugin(const std::string& pluginName, const std::string& version) {
    // Fetch plugin from marketplace
    // In a real implementation, this would download and install the plugin
    return true;
}

bool PluginRegistry::updatePlugin(const std::string& name, const std::string& version) {
    // Update plugin
    // In a real implementation, this would download and install the updated plugin
    return reloadPlugin(name);
}

// DynamicLibraryPluginLoader implementation
struct DynamicLibraryPluginLoader::Impl {
    // Implementation details
};

DynamicLibraryPluginLoader::DynamicLibraryPluginLoader() {
    pImpl = std::make_unique<Impl>();
}

DynamicLibraryPluginLoader::~DynamicLibraryPluginLoader() = default;

bool DynamicLibraryPluginLoader::canLoad(const std::string& path) const {
    // Check if file has a dynamic library extension
    std::string ext = path.substr(path.find_last_of('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    return ext == "so" || ext == "dll" || ext == "dylib" || ext == "plugin";
}

std::unique_ptr<PluginInterface> DynamicLibraryPluginLoader::loadPlugin(const std::string& path) {
    // For now, return nullptr - dynamic loading would require platform-specific code
    NLM_LOG_WARNING("Dynamic library loading not fully implemented");
    return nullptr;
}

std::string DynamicLibraryPluginLoader::getName() const {
    return "DynamicLibraryPluginLoader";
}

// ScriptPluginLoader implementation
struct ScriptPluginLoader::Impl {
    // Implementation details
};

ScriptPluginLoader::ScriptPluginLoader() {
    pImpl = std::make_unique<Impl>();
}

ScriptPluginLoader::~ScriptPluginLoader() = default;

bool ScriptPluginLoader::canLoad(const std::string& path) const {
    // Check if file has a script extension
    std::string ext = path.substr(path.find_last_of('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    return ext == "py" || ext == "lua" || ext == "js" || ext == "ts" || ext == "mjs";
}

std::unique_ptr<PluginInterface> ScriptPluginLoader::loadPlugin(const std::string& path) {
    // For now, return nullptr - script loading would require platform-specific code
    NLM_LOG_WARNING("Script plugin loading not fully implemented");
    return nullptr;
}

std::string ScriptPluginLoader::getName() const {
    return "ScriptPluginLoader";
}

// BuiltInPlugin implementation
BuiltInPlugin::BuiltInPlugin(const std::string& name) : initialized(false) {
    metadata.name = name;
    metadata.version = "1.0.0";
    metadata.author = "NLM Team";
    metadata.enabled = true;
    metadata.loaded = false;
    metadata.loadCount = 0;
    metadata.unloadCount = 0;
    metadata.totalExecutionTime = 0.0;
    metadata.callCount = 0;
}

BuiltInPlugin::~BuiltInPlugin() = default;

const PluginMetadata& BuiltInPlugin::getMetadata() const {
    return metadata;
}

bool BuiltInPlugin::initialize(const std::map<std::string, std::string>& config) {
    // Default initialization - check config keys
    for (const auto& pair : config) {
        setData(pair.first, pair.second);
    }
    initialized = true;
    return true;
}

void BuiltInPlugin::shutdown() {
    initialized = false;
}

bool BuiltInPlugin::isInitialized() const {
    return initialized;
}

bool BuiltInPlugin::setString(std::map<std::string, std::string>& config, const std::string& key,
                              const std::string& defaultValue, std::string& result) {
    auto it = config.find(key);
    if (it != config.end()) {
        result = it->second;
        config.erase(it);
        return true;
    }
    result = defaultValue;
    return false;
}

bool BuiltInPlugin::setInt(std::map<std::string, std::string>& config, const std::string& key,
                           int defaultValue, int& result) {
    auto it = config.find(key);
    if (it != config.end()) {
        try {
            result = std::stoi(it->second);
            config.erase(it);
            return true;
        } catch (...) {
            result = defaultValue;
            config.erase(it);
            return false;
        }
    }
    result = defaultValue;
    return false;
}

bool BuiltInPlugin::setDouble(std::map<std::string, std::string>& config, const std::string& key,
                              double defaultValue, double& result) {
    auto it = config.find(key);
    if (it != config.end()) {
        try {
            result = std::stod(it->second);
            config.erase(it);
            return true;
        } catch (...) {
            result = defaultValue;
            config.erase(it);
            return false;
        }
    }
    result = defaultValue;
    return false;
}

bool BuiltInPlugin::setBool(std::map<std::string, std::string>& config, const std::string& key,
                            bool defaultValue, bool& result) {
    auto it = config.find(key);
    if (it != config.end()) {
        std::string value = it->second;
        std::transform(value.begin(), value.end(), value.begin(), ::tolower);
        if (value == "true" || value == "1" || value == "yes") {
            result = true;
        } else if (value == "false" || value == "0" || value == "no") {
            result = false;
        } else {
            result = defaultValue;
            config.erase(it);
            return false;
        }
        config.erase(it);
        return true;
    }
    result = defaultValue;
    return false;
}

// Plugin implementations (simplified - just stubs)
MemoryPlugin::MemoryPlugin() : BuiltInPlugin("MemoryPlugin") {}
MemoryPlugin::~MemoryPlugin() = default;
void MemoryPlugin::extendMemorySystem(std::shared_ptr<Brain> brain) {}

PlasticityPlugin::PlasticityPlugin() : BuiltInPlugin("PlasticityPlugin") {}
PlasticityPlugin::~PlasticityPlugin() = default;
void PlasticityPlugin::extendPlasticitySystem(std::shared_ptr<Brain> brain) {}

NeuromodulationPlugin::NeuromodulationPlugin() : BuiltInPlugin("NeuromodulationPlugin") {}
NeuromodulationPlugin::~NeuromodulationPlugin() = default;
void NeuromodulationPlugin::extendNeuromodulationSystem(std::shared_ptr<Brain> brain) {}

CognitionPlugin::CognitionPlugin() : BuiltInPlugin("CognitionPlugin") {}
CognitionPlugin::~CognitionPlugin() = default;
void CognitionPlugin::extendCognitionSystem(std::shared_ptr<Brain> brain) {}

SensoryPlugin::SensoryPlugin() : BuiltInPlugin("SensoryPlugin") {}
SensoryPlugin::~SensoryPlugin() = default;
void SensoryPlugin::extendSensorySystem(std::shared_ptr<Brain> brain) {}

MotorPlugin::MotorPlugin() : BuiltInPlugin("MotorPlugin") {}
MotorPlugin::~MotorPlugin() = default;
void MotorPlugin::extendMotorSystem(std::shared_ptr<Brain> brain) {}

EnvironmentPlugin::EnvironmentPlugin() : BuiltInPlugin("EnvironmentPlugin") {}
EnvironmentPlugin::~EnvironmentPlugin() = default;
void EnvironmentPlugin::extendEnvironment(std::shared_ptr<Environment> world) {}

VisualizationPlugin::VisualizationPlugin() : BuiltInPlugin("VisualizationPlugin") {}
VisualizationPlugin::~VisualizationPlugin() = default;
void VisualizationPlugin::beforeStep(double timestep) {}
void VisualizationPlugin::afterStep(double timestep) {}

DataPlugin::DataPlugin() : BuiltInPlugin("DataPlugin") {}
DataPlugin::~DataPlugin() = default;

AnalysisPlugin::AnalysisPlugin() : BuiltInPlugin("AnalysisPlugin") {}
AnalysisPlugin::~AnalysisPlugin() = default;

} // namespace nlm
