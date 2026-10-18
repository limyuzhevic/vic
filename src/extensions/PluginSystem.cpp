#include "PluginSystem.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
#include <thread>
#include <mutex>
#include <unordered_map>

namespace nlm {

// PluginLoader implementation
struct PluginLoader::Impl {
    std::string pluginDirectory = "plugins";
    bool autoLoad = true;
    std::string securityPolicy = "standard";
    
    std::unordered_map<std::string, std::unique_ptr<Plugin>> loadedPlugins;
    std::unordered_map<std::string, PluginMetadata> pluginMetadata;
    
    LoaderStats stats;
    std::mutex mutex;
};

PluginLoader::PluginLoader() : pImpl(std::make_unique<Impl>()) {}
PluginLoader::~PluginLoader() = default;

std::unique_ptr<Plugin> PluginLoader::loadPlugin(const std::string& path) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto startTime = std::chrono::steady_clock::now();
    
    try {
        if (!validatePlugin(path)) {
            pImpl->stats.failedLoads++;
            return nullptr;
        }
        
        // In a real implementation, this would dynamically load the plugin
        // For now, we'll create a placeholder
        auto plugin = std::make_unique<StaticPlugin>("placeholder", "1.0.0", "Placeholder plugin");
        
        pImpl->loadedPlugins[plugin->getName()] = plugin.get();
        pImpl->stats.totalPluginsLoaded++;
        pImpl->stats.pluginsCurrentlyLoaded++;
        pImpl->stats.lastLoad = std::chrono::steady_clock::now();
        
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        pImpl->stats.averageLoadTime = 
            (pImpl->stats.averageLoadTime * (pImpl->stats.totalPluginsLoaded - 1) + 
             duration.count()) / pImpl->stats.totalPluginsLoaded;
        
        return plugin;
        
    } catch (const std::exception& e) {
        pImpl->stats.failedLoads++;
        return nullptr;
    }
}

std::unique_ptr<Plugin> PluginLoader::loadPlugin(const std::string& name, const std::string& version) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Look for plugin by name and version
    for (const auto& [path, metadata] : pImpl->pluginMetadata) {
        if (metadata.name == name && metadata.version == version) {
            return loadPlugin(path);
        }
    }
    
    return nullptr;
}

bool PluginLoader::unloadPlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->loadedPlugins.find(name);
    if (it == pImpl->loadedPlugins.end()) {
        return false;
    }
    
    // Call unload on plugin
    it->second->unload();
    
    pImpl->loadedPlugins.erase(it);
    pImpl->stats.pluginsCurrentlyLoaded--;
    pImpl->stats.totalPluginsUnloaded++;
    
    return true;
}

bool PluginLoader::unloadPlugin(std::unique_ptr<Plugin>& plugin) {
    if (!plugin) return false;
    
    bool result = unloadPlugin(plugin->getName());
    plugin.reset();
    return result;
}

bool PluginLoader::isPluginLoaded(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->loadedPlugins.find(name) != pImpl->loadedPlugins.end();
}

std::vector<std::string> PluginLoader::getLoadedPlugins() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::string> names;
    for (const auto& pair : pImpl->loadedPlugins) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<std::string> PluginLoader::discoverPlugins(const std::string& directory) const {
    std::vector<std::string> pluginFiles;
    
    if (!std::filesystem::exists(directory)) {
        return pluginFiles;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            std::string extension = entry.path().extension().string();
            if (extension == ".plugin" || extension == ".so" || extension == ".dll" ||
                extension == ".dylib") {
                pluginFiles.push_back(entry.path().string());
            }
        }
    }
    
    return pluginFiles;
}

std::vector<PluginMetadata> PluginLoader::getPluginMetadata(const std::string& directory) const {
    std::vector<PluginMetadata> metadata;
    
    auto pluginFiles = discoverPlugins(directory);
    for (const auto& filepath : pluginFiles) {
        PluginMetadata meta = extractMetadataFromFile(filepath);
        if (meta.name.empty()) {
            continue;
        }
        metadata.push_back(meta);
    }
    
    return metadata;
}

bool PluginLoader::validatePlugin(const std::string& path) const {
    // Basic validation
    if (!std::filesystem::exists(path)) {
        return false;
    }
    
    // Check file extension
    std::string extension = std::filesystem::path(path).extension().string();
    if (extension != ".plugin" && extension != ".so" && extension != ".dll" && extension != ".dylib") {
        return false;
    }
    
    // Try to extract metadata
    PluginMetadata meta = extractMetadataFromFile(path);
    return !meta.name.empty();
}

std::vector<std::string> PluginLoader::validatePluginDependencies(const PluginMetadata& metadata) const {
    std::vector<std::string> errors;
    
    // Parse dependencies
    // In a real implementation, this would parse the dependencies string
    // and check if each dependency is available
    
    return errors;
}

void PluginLoader::setPluginDirectory(const std::string& directory) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->pluginDirectory = directory;
}

std::string PluginLoader::getPluginDirectory() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->pluginDirectory;
}

void PluginLoader::enableAutoLoad(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->autoLoad = enable;
}

bool PluginLoader::isAutoLoadEnabled() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->autoLoad;
}

void PluginLoader::setSecurityPolicy(const std::string& policy) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->securityPolicy = policy;
}

std::string PluginLoader::getSecurityPolicy() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->securityPolicy;
}

bool PluginLoader::validateSecurity(const Plugin& plugin) const {
    // Security validation logic
    return true; // Placeholder
}

PluginLoader::LoaderStats PluginLoader::getStats() const {
    return pImpl->stats;
}

void PluginLoader::resetStats() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->stats = LoaderStats();
}

// PluginRegistry implementation
struct PluginRegistry::Impl {
    std::unordered_map<std::string, std::unique_ptr<Plugin>> plugins;
    std::unordered_map<std::string, PluginMetadata> metadataMap;
    std::unordered_map<std::string, std::vector<std::function<void(const nlohmann::json&)>>> eventListeners;
    
    std::mutex mutex;
    RegistryStats stats;
};

PluginRegistry::PluginRegistry() : pImpl(std::make_unique<Impl>()) {}
PluginRegistry::~PluginRegistry() = default;

void PluginRegistry::registerPlugin(std::unique_ptr<Plugin> plugin) {
    if (!plugin) return;
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->plugins[plugin->getName()] = std::move(plugin);
    pImpl->stats.totalPlugins++;
    pImpl->stats.lastUpdate = std::chrono::steady_clock::now();
}

void PluginRegistry::unregisterPlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->plugins.find(name);
    if (it != pImpl->plugins.end()) {
        pImpl->plugins.erase(it);
        pImpl->stats.totalPlugins--;
    }
}

bool PluginRegistry::isPluginRegistered(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->plugins.find(name) != pImpl->plugins.end();
}

Plugin* PluginRegistry::getPlugin(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->plugins.find(name);
    if (it != pImpl->plugins.end()) {
        return it->second.get();
    }
    return nullptr;
}

std::vector<Plugin*> PluginRegistry::getPluginsByTag(const std::string& tag) const {
    std::vector<Plugin*> result;
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (const auto& pair : pImpl->plugins) {
        if (pair.second->hasTag(tag)) {
            result.push_back(pair.second.get());
        }
    }
    
    return result;
}

std::vector<Plugin*> PluginRegistry::getPluginsByCapability(const std::string& capability) const {
    std::vector<Plugin*> result;
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (const auto& pair : pImpl->plugins) {
        std::vector<std::string> capabilities = pair.second->getCapabilities();
        if (std::find(capabilities.begin(), capabilities.end(), capability) != capabilities.end()) {
            result.push_back(pair.second.get());
        }
    }
    
    return result;
}

std::vector<Plugin*> PluginRegistry::getAllPlugins() const {
    std::vector<Plugin*> result;
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (const auto& pair : pImpl->plugins) {
        result.push_back(pair.second.get());
    }
    
    return result;
}

void PluginRegistry::enablePlugin(const std::string& name) {
    Plugin* plugin = getPlugin(name);
    if (plugin) {
        plugin->onPluginEnable();
        pImpl->stats.enabledPlugins++;
    }
}

void PluginRegistry::disablePlugin(const std::string& name) {
    Plugin* plugin = getPlugin(name);
    if (plugin) {
        plugin->onPluginDisable();
        pImpl->stats.enabledPlugins--;
    }
}

void PluginRegistry::togglePlugin(const std::string& name) {
    if (isPluginEnabled(name)) {
        disablePlugin(name);
    } else {
        enablePlugin(name);
    }
}

bool PluginRegistry::isPluginEnabled(const std::string& name) const {
    Plugin* plugin = getPlugin(name);
    return plugin && plugin->getMetadata().isEnabled;
}

PluginMetadata PluginRegistry::getPluginMetadata(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->metadataMap.find(name);
    if (it != pImpl->metadataMap.end()) {
        return it->second;
    }
    
    Plugin* plugin = getPlugin(name);
    if (plugin) {
        return plugin->getMetadata();
    }
    
    return PluginMetadata();
}

void PluginRegistry::updatePluginMetadata(const std::string& name, const PluginMetadata& metadata) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->metadataMap[name] = metadata;
}

void PluginRegistry::setPluginConfig(const std::string& name, const nlohmann::json& config) {
    // Configuration would be stored in the plugin itself
    Plugin* plugin = getPlugin(name);
    if (plugin) {
        // In a real implementation, this would set config on the plugin
    }
}

nlohmann::json PluginRegistry::getPluginConfig(const std::string& name) const {
    // Configuration would be retrieved from the plugin itself
    nlohmann::json emptyConfig;
    return emptyConfig; // Placeholder
}

void PluginRegistry::emitEvent(const std::string& eventType, const nlohmann::json& data) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->eventListeners.find(eventType);
    if (it != pImpl->eventListeners.end()) {
        for (const auto& listener : it->second) {
            listener(data);
        }
    }
    
    // Notify all plugins
    for (const auto& pair : pImpl->plugins) {
        pair.second->onEvent(eventType, data);
    }
}

void PluginRegistry::addEventListener(const std::string& eventType,
                                     std::function<void(const nlohmann::json&)> listener) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->eventListeners[eventType].push_back(listener);
}

void PluginRegistry::removeEventListener(const std::string& eventType,
                                        std::function<void(const nlohmann::json&)> listener) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->eventListeners.find(eventType);
    if (it != pImpl->eventListeners.end()) {
        it->second.erase(
            std::remove(it->second.begin(), it->second.end(), listener),
            it->second.end()
        );
    }
}

void PluginRegistry::enableAllPlugins() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (const auto& pair : pImpl->plugins) {
        if (!pair.second->getMetadata().isEnabled) {
            pair.second->onPluginEnable();
        }
    }
    
    pImpl->stats.enabledPlugins = pImpl->stats.totalPlugins;
}

void PluginRegistry::disableAllPlugins() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (const auto& pair : pImpl->plugins) {
        if (pair.second->getMetadata().isEnabled) {
            pair.second->onPluginDisable();
        }
    }
    
    pImpl->stats.enabledPlugins = 0;
}

void PluginRegistry::shutdownAllPlugins() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (const auto& pair : pImpl->plugins) {
        pair.second->shutdown();
    }
    
    pImpl->plugins.clear();
    pImpl->stats.totalPlugins = 0;
    pImpl->stats.enabledPlugins = 0;
}

PluginRegistry::RegistryStats PluginRegistry::getStats() const {
    return pImpl->stats;
}

void PluginRegistry::resetStats() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->stats = RegistryStats();
}

void PluginRegistry::exportRegistry(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }
    
    nlohmann::json jsonRegistry;
    
    for (const auto& pair : pImpl->plugins) {
        nlohmann::json pluginJson;
        pluginJson["name"] = pair.first;
        pluginJson["metadata"] = nlohmann::json::parse(pair.second->getMetadata().metadata.at("raw"));
        jsonRegistry["plugins"].push_back(pluginJson);
    }
    
    file << jsonRegistry.dump(2);
    file.close();
}

void PluginRegistry::importRegistry(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for reading: " + filename);
    }
    
    nlohmann::json jsonRegistry;
    file >> jsonRegistry;
    file.close();
    
    // Import logic would go here
}

// PluginManager implementation
struct PluginManager::Impl {
    bool initialized = false;
    nlohmann::json globalConfig;
    std::unordered_map<std::string, std::function<void(const nlohmann::json&)>> eventListeners;
    
    std::unordered_map<std::string, std::unique_ptr<Plugin>> plugins;
    PluginRegistry registry;
    PluginLoader loader;
    
    std::mutex mutex;
    ManagerStats stats;
    bool monitoringEnabled = false;
};

PluginManager::PluginManager() : pImpl(std::make_unique<Impl>()) {}
PluginManager::~PluginManager() {
    shutdownAllPlugins();
}

bool PluginManager::initialize(const nlohmann::json& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (initialized) {
        return true;
    }
    
    pImpl->globalConfig = config;
    pImpl->initialized = true;
    return true;
}

void PluginManager::shutdown() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!initialized) {
        return;
    }
    
    pImpl->registry.shutdownAllPlugins();
    pImpl->plugins.clear();
    pImpl->initialized = false;
}

bool PluginManager::isInitialized() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->initialized;
}

bool PluginManager::loadPlugin(const std::string& path) {
    auto startTime = std::chrono::steady_clock::now();
    
    std::unique_ptr<Plugin> plugin = pImpl->loader.loadPlugin(path);
    if (!plugin) {
        pImpl->stats.failedLoads++;
        return false;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    plugin->setParent(this);
    
    if (!plugin->initialize(pImpl->globalConfig)) {
        pImpl->stats.failedLoads++;
        return false;
    }
    
    pImpl->plugins[plugin->getName()] = std::move(plugin);
    pImpl->stats.totalPluginsLoaded++;
    pImpl->stats.pluginsCurrentlyLoaded++;
    pImpl->stats.lastUpdate = std::chrono::steady_clock::now();
    
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    pImpl->stats.averageLoadTime = 
        (pImpl->stats.averageLoadTime * (pImpl->stats.totalPluginsLoaded - 1) + 
         duration.count()) / pImpl->stats.totalPluginsLoaded;
    
    return true;
}

bool PluginManager::loadPlugin(const std::string& name, const std::string& version,
                              const nlohmann::json& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto startTime = std::chrono::steady_clock::now();
    
    std::unique_ptr<Plugin> plugin = pImpl->loader.loadPlugin(name, version);
    if (!plugin) {
        pImpl->stats.failedLoads++;
        return false;
    }
    
    plugin->setParent(this);
    
    if (!plugin->initialize(config)) {
        pImpl->stats.failedLoads++;
        return false;
    }
    
    pImpl->plugins[plugin->getName()] = std::move(plugin);
    pImpl->stats.totalPluginsLoaded++;
    pImpl->stats.pluginsCurrentlyLoaded++;
    pImpl->stats.lastUpdate = std::chrono::steady_clock::now();
    
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    pImpl->stats.averageLoadTime = 
        (pImpl->stats.averageLoadTime * (pImpl->stats.totalPluginsLoaded - 1) + 
         duration.count()) / pImpl->stats.totalPluginsLoaded;
    
    return true;
}

bool PluginManager::unloadPlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->plugins.find(name);
    if (it == pImpl->plugins.end()) {
        return false;
    }
    
    it->second->shutdown();
    
    pImpl->plugins.erase(it);
    pImpl->stats.pluginsCurrentlyLoaded--;
    pImpl->stats.totalPluginsUnloaded++;
    
    return true;
}

bool PluginManager::unloadPlugin(std::unique_ptr<Plugin>& plugin) {
    if (!plugin) return false;
    
    bool result = unloadPlugin(plugin->getName());
    plugin.reset();
    return result;
}

void PluginManager::enablePlugin(const std::string& name) {
    Plugin* plugin = getPlugin(name);
    if (plugin) {
        plugin->onPluginEnable();
    }
}

void PluginManager::disablePlugin(const std::string& name) {
    Plugin* plugin = getPlugin(name);
    if (plugin) {
        plugin->onPluginDisable();
    }
}

void PluginManager::togglePlugin(const std::string& name) {
    if (isPluginEnabled(name)) {
        disablePlugin(name);
    } else {
        enablePlugin(name);
    }
}

bool PluginManager::isPluginEnabled(const std::string& name) const {
    Plugin* plugin = getPlugin(name);
    return plugin && plugin->getMetadata().isEnabled;
}

Plugin* PluginManager::getPlugin(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->plugins.find(name);
    if (it != pImpl->plugins.end()) {
        return it->second.get();
    }
    
    return pImpl->registry.getPlugin(name);
}

std::vector<Plugin*> PluginManager::getPlugins() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<Plugin*> allPlugins;
    for (const auto& pair : pImpl->plugins) {
        allPlugins.push_back(pair.second.get());
    }
    
    auto registryPlugins = pImpl->registry.getAllPlugins();
    allPlugins.insert(allPlugins.end(), registryPlugins.begin(), registryPlugins.end());
    
    return allPlugins;
}

std::vector<Plugin*> PluginManager::getPluginsByTag(const std::string& tag) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<Plugin*> allPlugins = getPlugins();
    std::vector<Plugin*> taggedPlugins;
    
    for (Plugin* plugin : allPlugins) {
        if (plugin->hasTag(tag)) {
            taggedPlugins.push_back(plugin);
        }
    }
    
    return taggedPlugins;
}

PluginMetadata PluginManager::getPluginMetadata(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check loaded plugins first
    auto it = pImpl->plugins.find(name);
    if (it != pImpl->plugins.end()) {
        return it->second->getMetadata();
    }
    
    // Then check registry
    return pImpl->registry.getPluginMetadata(name);
}

std::vector<PluginMetadata> PluginManager::getPluginMetadata() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<PluginMetadata> metadata;
    
    for (const auto& pair : pImpl->plugins) {
        metadata.push_back(pair.second->getMetadata());
    }
    
    // Also get from registry
    auto registryMetadata = pImpl->registry.getStats().pluginMetadata;
    for (const auto& pair : registryMetadata) {
        metadata.push_back(pair.second);
    }
    
    return metadata;
}

std::vector<std::string> PluginManager::getAvailablePlugins() const {
    return pImpl->loader.getLoadedPlugins();
}

void PluginManager::setGlobalConfig(const nlohmann::json& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->globalConfig = config;
}

nlohmann::json PluginManager::getGlobalConfig() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->globalConfig;
}

void PluginManager::emitEvent(const std::string& eventType, const nlohmann::json& data) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Internal listeners
    auto it = pImpl->eventListeners.find(eventType);
    if (it != pImpl->eventListeners.end()) {
        for (const auto& listener : it->second) {
            listener(data);
        }
    }
    
    // Plugin registry
    pImpl->registry.emitEvent(eventType, data);
}

void PluginManager::addEventListener(const std::string& eventType,
                                     std::function<void(const nlohmann::json&)> listener) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->eventListeners[eventType].push_back(listener);
}

void PluginManager::removeEventListener(const std::string& eventType,
                                       std::function<void(const nlohmann::json&)> listener) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->eventListeners.find(eventType);
    if (it != pImpl->eventListeners.end()) {
        it->second.erase(
            std::remove(it->second.begin(), it->second.end(), listener),
            it->second.end()
        );
    }
}

void PluginManager::enableAllPlugins() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (const auto& pair : pImpl->plugins) {
        if (!pair.second->getMetadata().isEnabled) {
            pair.second->onPluginEnable();
        }
    }
}

void PluginManager::disableAllPlugins() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (const auto& pair : pImpl->plugins) {
        if (pair.second->getMetadata().isEnabled) {
            pair.second->onPluginDisable();
        }
    }
}

void PluginManager::shutdownAllPlugins() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (const auto& pair : pImpl->plugins) {
        pair.second->shutdown();
    }
    
    pImpl->plugins.clear();
    pImpl->registry.shutdownAllPlugins();
}

void PluginManager::reloadPlugin(const std::string& name) {
    if (!isPluginLoaded(name)) {
        return;
    }
    
    // Unload first
    unloadPlugin(name);
    
    // Reload
    Plugin* plugin = getPlugin(name);
    if (plugin) {
        plugin->initialize(pImpl->globalConfig);
    }
}

PluginManager::ManagerStats PluginManager::getStats() const {
    return pImpl->stats;
}

void PluginManager::resetStats() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->stats = ManagerStats();
}

void PluginManager::enableMonitoring(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->monitoringEnabled = enable;
}

bool PluginManager::isMonitoringEnabled() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->monitoringEnabled;
}

nlohmann::json PluginManager::getPerformanceMetrics() const {
    nlohmann::json metrics;
    
    // Performance metrics would be collected from plugins
    metrics["pluginCount"] = getPlugins().size();
    metrics["enabledPluginCount"] = std::count_if(getPlugins().begin(), getPlugins().end(),
                                                  [this](Plugin* plugin) { return isPluginEnabled(plugin->getName()); });
    
    return metrics;
}

bool PluginManager::validatePlugin(const std::string& path) const {
    return pImpl->loader.validatePlugin(path);
}

std::vector<std::string> PluginManager::getValidationErrors(const std::string& path) const {
    // Validation errors would be collected during validation
    return std::vector<std::string>(); // Placeholder
}

std::vector<nlohmann::json> PluginManager::getAvailableApiEndpoints() const {
    std::vector<nlohmann::json> endpoints;
    
    for (const auto& plugin : pImpl->plugins) {
        auto pluginEndpoints = plugin.second->getApiEndpoints();
        endpoints.insert(endpoints.end(), pluginEndpoints.begin(), pluginEndpoints.end());
    }
    
    return endpoints;
}

nlohmann::json PluginManager::callPluginApi(const std::string& pluginName,
                                          const std::string& endpoint,
                                          const nlohmann::json& params) {
    Plugin* plugin = getPlugin(pluginName);
    if (!plugin) {
        throw std::runtime_error("Plugin not found: " + pluginName);
    }
    
    return plugin->callApi(endpoint, params);
}

void PluginManager::exportPluginState(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }
    
    nlohmann::json state;
    
    for (const auto& pair : pImpl->plugins) {
        nlohmann::json pluginState;
        pluginState["name"] = pair.first;
        pluginState["metadata"] = nlohmann::json::parse(pair.second->getMetadata().metadata.at("raw"));
        pluginState["status"] = pair.second->getStatus();
        state["plugins"].push_back(pluginState);
    }
    
    file << state.dump(2);
    file.close();
}

void PluginManager::importPluginState(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for reading: " + filename);
    }
    
    nlohmann::json state;
    file >> state;
    file.close();
    
    // Import logic would go here
}

// PluginUtils namespace implementations
namespace PluginUtils {
    std::string findPluginFile(const std::string& pluginName,
                              const std::string& pluginDirectory) {
        std::string searchPattern = pluginName + ".*";
        
        if (!std::filesystem::exists(pluginDirectory)) {
            return "";
        }
        
        for (const auto& entry : std::filesystem::directory_iterator(pluginDirectory)) {
            if (entry.is_regular_file() && entry.path().filename().string().find(pluginName) != std::string::npos) {
                return entry.path().string();
            }
        }
        
        return "";
    }
    
    std::string findPluginFileByName(const std::string& name,
                                    const std::string& pluginDirectory) {
        return findPluginFile(name, pluginDirectory);
    }
    
    PluginMetadata extractMetadataFromFile(const std::string& filepath) {
        PluginMetadata metadata;
        
        // Extract metadata from plugin file
        // In a real implementation, this would read metadata from the plugin file
        
        return metadata;
    }
    
    PluginMetadata extractMetadataFromSource(const std::string& sourceCode) {
        PluginMetadata metadata;
        
        // Extract metadata from source code
        // In a real implementation, this would parse source code comments
        
        return metadata;
    }
    
    bool validatePluginName(const std::string& name) {
        if (name.empty()) {
            return false;
        }
        
        // Check for invalid characters
        for (char c : name) {
            if (!std::isalnum(c) && c != '_' && c != '-') {
                return false;
            }
        }
        
        return true;
    }
    
    bool validatePluginVersion(const std::string& version) {
        if (version.empty()) {
            return false;
        }
        
        // Simple version validation (semver-like)
        size_t dotCount = std::count(version.begin(), version.end(), '.');
        return dotCount >= 1;
    }
    
    bool validatePluginPath(const std::string& path) {
        if (path.empty()) {
            return false;
        }
        
        // Check for path traversal attempts
        if (path.find("..") != std::string::npos) {
            return false;
        }
        
        return std::filesystem::exists(path);
    }
    
    bool validatePluginDependencies(const std::string& dependencies) {
        if (dependencies.empty()) {
            return true;
        }
        
        // Parse dependencies and validate each one
        // In a real implementation, this would parse JSON or other dependency format
        
        return true;
    }
    
    bool copyPluginFile(const std::string& source, const std::string& destination) {
        try {
            std::filesystem::copy_file(source, destination,
                                      std::filesystem::copy_options::overwrite_existing);
            return true;
        } catch (const std::filesystem::filesystem_error& e) {
            return false;
        }
    }
    
    bool movePluginFile(const std::string& source, const std::string& destination) {
        try {
            std::filesystem::rename(source, destination);
            return true;
        } catch (const std::filesystem::filesystem_error& e) {
            return false;
        }
    }
    
    bool deletePluginFiles(const std::string& pluginDirectory) {
        try {
            if (std::filesystem::exists(pluginDirectory)) {
                std::filesystem::remove_all(pluginDirectory);
            }
            return true;
        } catch (const std::filesystem::filesystem_error& e) {
            return false;
        }
    }
    
    bool compressPluginFiles(const std::string& sourceDirectory, const std::string& outputFile) {
        // Compression implementation would go here
        return true; // Placeholder
    }
    
    bool decompressPluginFiles(const std::string& archiveFile, const std::string& outputDirectory) {
        // Decompression implementation would go here
        return true; // Placeholder
    }
    
    bool validatePluginSourceCode(const std::string& sourceCode) {
        // Security validation of source code
        // Check for malicious patterns, etc.
        return true; // Placeholder
    }
    
    bool scanPluginForMalware(const std::string& filepath) {
        // Malware scanning would go here
        return true; // Placeholder
    }
    
    void logPluginEvent(const std::string& pluginName, const std::string& event,
                      const nlohmann::json& data) {
        // Logging implementation
        std::cout << "Plugin Event: " << pluginName << " - " << event << std::endl;
    }
    
    bool checkForPluginUpdates(const std::string& pluginName, const std::string& version) {
        // Update checking would go here
        return false; // Placeholder
    }
    
    bool updatePlugin(const std::string& pluginName, const std::string& version) {
        // Plugin update implementation would go here
        return false; // Placeholder
    }
    
    bool installPlugin(const std::string& pluginPath, const std::string& installDirectory) {
        // Plugin installation implementation would go here
        return false; // Placeholder
    }
    
    bool uninstallPlugin(const std::string& pluginName, const std::string& pluginDirectory) {
        // Plugin uninstallation implementation would go here
        return false; // Placeholder
    }
}

} // namespace nlm
