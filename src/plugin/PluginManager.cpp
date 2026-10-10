// PluginManager.cpp - Implementation of plugin lifecycle management
#include "PluginManager.hpp"
#include <algorithm>
#include <chrono>
#include <sstream>

namespace nlm {

PluginManager::PluginManager(std::shared_ptr<IPluginLoader> loader) {
    this->loader = loader ? loader : std::make_shared<DynamicPluginLoader>();
    registry = std::make_shared<PluginRegistry>(this->loader);
}

bool PluginManager::resolveDependencies(const PluginManifest& manifest) const {
    for (const auto& dep : manifest.dependencies) {
        if (!registry->isPluginRegistered(dep)) {
            return false;
        }
    }
    return true;
}

std::vector<std::string> PluginManager::getSortedPluginNames(const std::vector<PluginManifest>& manifests) const {
    std::vector<std::string> names;
    for (const auto& manifest : manifests) {
        names.push_back(manifest.name);
    }
    
    // Simple topological sort based on dependencies
    std::vector<std::string> sorted;
    std::vector<std::string> visited;
    std::vector<std::string> temp;
    
    std::function<bool(const std::string&, std::vector<std::string>&)> visit = [&](const std::string& name, std::vector<std::string>& result) {
        auto it = std::find(visited.begin(), visited.end(), name);
        if (it != visited.end()) return true;
        
        auto tempIt = std::find(temp.begin(), temp.end(), name);
        if (tempIt != temp.end()) return false;
        
        temp.push_back(name);
        
        const auto& manifest = registry->getManifest(name);
        for (const auto& dep : manifest.dependencies) {
            if (!visit(dep, temp)) {
                return false;
            }
        }
        
        temp.pop_back();
        visited.push_back(name);
        result.push_back(name);
        return true;
    };
    
    for (const auto& name : names) {
        if (!visit(name, sorted)) {
            return {};
        }
    }
    
    return sorted;
}

void PluginManager::onPluginLoaded(const std::string& pluginName) {
    pluginLoadTimes[pluginName] = std::chrono::system_clock::now();
}

void PluginManager::onPluginShutdown(const std::string& pluginName) {
    pluginShutdownTimes[pluginName] = std::chrono::system_clock::now();
}

void PluginManager::onPluginError(const std::string& pluginName, const std::string& error) {
    // Store error for retrieval
    lastError = "Plugin " + pluginName + ": " + error;
}

void PluginManager::updatePluginConfiguration(const std::string& pluginName, const PluginConfig& config) {
    auto plugin = registry->getPlugin(pluginName);
    if (plugin) {
        plugin->updateConfiguration(config);
    }
}

bool PluginManager::loadPlugin(const std::string& pluginPath) {
    return registry->loadPluginFromFile(pluginPath);
}

bool PluginManager::loadPlugin(const PluginManifest& manifest) {
    if (!validatePlugin(manifest)) {
        return false;
    }
    
    if (!resolveDependencies(manifest)) {
        lastError = "Plugin dependencies not satisfied for: " + manifest.name;
        return false;
    }
    
    bool success = registry->loadPluginFromManifest(manifest);
    if (success) {
        onPluginLoaded(manifest.name);
    }
    
    return success;
}

bool PluginManager::unloadPlugin(const std::string& pluginName) {
    if (!isPluginLoaded(pluginName)) {
        return false;
    }
    
    auto plugin = registry->getPlugin(pluginName);
    if (plugin && plugin->isInitialized()) {
        plugin->shutdown();
    }
    
    bool success = registry->unregisterPlugin(pluginName);
    if (success) {
        onPluginShutdown(pluginName);
    }
    
    return success;
}

bool PluginManager::reloadPlugin(const std::string& pluginName) {
    if (!isPluginLoaded(pluginName)) {
        return false;
    }
    
    PluginManifest manifest = getPluginManifest(pluginName);
    unloadPlugin(pluginName);
    return loadPlugin(manifest);
}

bool PluginManager::initializePlugin(const std::string& pluginName, const PluginConfig& config) {
    auto plugin = registry->getPlugin(pluginName);
    if (!plugin || isPluginInitialized(pluginName)) {
        return false;
    }
    
    bool success = plugin->initialize(config);
    if (success) {
        updatePluginConfiguration(pluginName, config);
    } else {
        onPluginError(pluginName, "Failed to initialize plugin");
    }
    
    return success;
}

void PluginManager::shutdownPlugin(const std::string& pluginName) {
    auto plugin = registry->getPlugin(pluginName);
    if (plugin && isPluginInitialized(pluginName)) {
        plugin->shutdown();
    }
}

bool PluginManager::isPluginLoaded(const std::string& pluginName) const {
    return registry->isPluginRegistered(pluginName);
}

bool PluginManager::isPluginInitialized(const std::string& pluginName) const {
    auto plugin = registry->getPlugin(pluginName);
    return plugin && plugin->isInitialized();
}

std::vector<std::string> PluginManager::discoverPlugins(const std::string& directory) const {
    return registry->discoverPlugins(directory);
}

std::vector<PluginManifest> PluginManager::loadAllPlugins(const std::string& directory) {
    std::vector<PluginManifest> manifests;
    std::vector<std::string> pluginFiles = discoverPlugins(directory);
    
    for (const auto& pluginFile : pluginFiles) {
        PluginManifest manifest;
        std::string manifestPath = PluginManifestLoader::generateManifestPath(pluginFile);
        
        if (PluginManifestLoader::loadManifest(manifestPath, manifest) && validatePlugin(manifest)) {
            manifests.push_back(manifest);
        }
    }
    
    // Sort by dependencies
    std::vector<std::string> sortedNames = getSortedPluginNames(manifests);
    if (sortedNames.empty()) return manifests; // Return as-is if sorting failed
    
    // Reorder manifests
    std::vector<PluginManifest> sortedManifests;
    for (const auto& name : sortedNames) {
        for (const auto& manifest : manifests) {
            if (manifest.name == name) {
                sortedManifests.push_back(manifest);
                break;
            }
        }
    }
    
    return sortedManifests;
}

std::shared_ptr<IPlugin> PluginManager::getPlugin(const std::string& pluginName) const {
    return registry->getPlugin(pluginName);
}

PluginManifest PluginManager::getPluginManifest(const std::string& pluginName) const {
    return registry->getManifest(pluginName);
}

std::vector<std::string> PluginManager::getLoadedPlugins() const {
    return registry->getLoadedPluginNames();
}

std::vector<std::string> PluginManager::getInitializedPlugins() const {
    std::vector<std::string> initialized;
    for (const auto& name : getLoadedPlugins()) {
        if (isPluginInitialized(name)) {
            initialized.push_back(name);
        }
    }
    return initialized;
}

PluginCategory PluginManager::getPluginCategory(const std::string& pluginName) const {
    return registry->getPluginCategory(pluginName);
}

std::vector<std::string> PluginManager::getPluginDependencies(const std::string& pluginName) const {
    return registry->getPluginDependencies(pluginName);
}

std::string PluginManager::getPluginVersion(const std::string& pluginName) const {
    PluginManifest manifest = getPluginManifest(pluginName);
    return manifest.version;
}

std::string PluginManager::getPluginAuthor(const std::string& pluginName) const {
    PluginManifest manifest = getPluginManifest(pluginName);
    return manifest.author;
}

std::string PluginManager::getPluginDescription(const std::string& pluginName) const {
    PluginManifest manifest = getPluginManifest(pluginName);
    return manifest.description;
}

bool PluginManager::updatePluginConfig(const std::string& pluginName, const PluginConfig& config) {
    updatePluginConfiguration(pluginName, config);
    return true;
}

PluginConfig PluginManager::getPluginConfig(const std::string& pluginName) const {
    auto plugin = registry->getPlugin(pluginName);
    if (plugin) {
        return plugin->getCurrentConfiguration();
    }
    return PluginConfig{};
}

std::vector<bool> PluginManager::loadPlugins(const std::vector<std::string>& pluginPaths) {
    std::vector<bool> results;
    results.reserve(pluginPaths.size());
    
    for (const auto& path : pluginPaths) {
        results.push_back(loadPlugin(path));
    }
    
    return results;
}

std::vector<bool> PluginManager::unloadPlugins(const std::vector<std::string>& pluginNames) {
    std::vector<bool> results;
    results.reserve(pluginNames.size());
    
    for (const auto& name : pluginNames) {
        results.push_back(unloadPlugin(name));
    }
    
    return results;
}

std::vector<bool> PluginManager::initializePlugins(const std::vector<std::string>& pluginNames) {
    std::vector<bool> results;
    results.reserve(pluginNames.size());
    
    for (const auto& name : pluginNames) {
        results.push_back(initializePlugin(name));
    }
    
    return results;
}

std::vector<bool> PluginManager::shutdownPlugins(const std::vector<std::string>& pluginNames) {
    std::vector<bool> results;
    results.reserve(pluginNames.size());
    
    for (const auto& name : pluginNames) {
        shutdownPlugin(name);
        results.push_back(true);
    }
    
    return results;
}

void PluginManager::publishEvent(const PluginEvent& event) {
    // This would normally notify all subscribed plugins
    // For now, just store the event
}

void PluginManager::subscribeToPluginEvents(const std::string& pluginName, IEventSubscriber* subscriber) {
    // This would normally register the subscriber for events from this plugin
}

std::string PluginManager::getLastError() const {
    return lastError;
}

void PluginManager::clearErrorCache() {
    lastError.clear();
}

double PluginManager::getPluginUptime(const std::string& pluginName) const {
    auto it = pluginLoadTimes.find(pluginName);
    if (it == pluginLoadTimes.end()) return 0.0;
    
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - it->second).count();
    return static_cast<double>(duration);
}

size_t PluginManager::getPluginMemoryUsage(const std::string& pluginName) const {
    // This is a placeholder - actual implementation would depend on plugin
    return 0;
}

bool PluginManager::validatePlugin(const PluginManifest& manifest) const {
    if (manifest.name.empty() || manifest.version.empty()) {
        return false;
    }
    
    if (manifest.category == PluginCategory::UNKNOWN) {
        return false;
    }
    
    // Check version compatibility
    if (manifest.minCoreVersion.empty() || manifest.maxCoreVersion.empty()) {
        return false;
    }
    
    return true;
}

bool PluginManager::validatePluginCompatibility(const PluginManifest& manifest) const {
    // Check if all dependencies are available
    for (const auto& dep : manifest.dependencies) {
        if (!registry->isPluginRegistered(dep)) {
            return false;
        }
    }
    return true;
}

void PluginManager::unloadAllPlugins() {
    std::vector<std::string> plugins = getLoadedPlugins();
    for (const auto& name : plugins) {
        unloadPlugin(name);
    }
}

void PluginManager::shutdownAllPlugins() {
    std::vector<std::string> plugins = getInitializedPlugins();
    for (const auto& name : plugins) {
        shutdownPlugin(name);
    }
}

} // namespace nlm