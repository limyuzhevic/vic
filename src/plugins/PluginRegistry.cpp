#include "PluginRegistry.hpp"
#include "PluginCore.hpp"
#include "../core/Logger/Logger.hpp"
#include <thread>
#include <future>
#include <algorithm>
#include <unordered_set>
#include <sstream>
#include <filesystem>

namespace nlm {

struct PluginRegistry::Impl {
    bool active{false};
    PluginRegistryConfig config;
    
    std::mutex registryMutex;
    
    std::map<std::string, std::unique_ptr<Plugin>> plugins;
    std::map<std::string, PluginMetadata> pluginMetadata;
    std::map<std::string, std::vector<std::string>> pluginTags;
    std::map<std::string, std::vector<std::string>> pluginDependencies;
    std::map<std::string, std::vector<std::string>> pluginConflicts;
    std::map<std::string, PluginSecurityLevel> pluginSecurityLevels;
    std::map<std::string, PluginCapabilities> pluginCapabilities;
    std::map<std::string, std::map<std::string, PluginConfigValue>> pluginConfigs;
    
    std::map<std::string, PluginLoadStatus> pluginStatuses;
    std::map<std::string, std::chrono::system_clock::time_point> pluginStartTimes;
    std::map<std::string, std::chrono::system_clock::time_point> pluginStopTimes;
    std::map<std::string, std::vector<std::string>> pluginEvents;
    
    std::map<std::string, PluginStatistics> pluginStatistics;
    
    std::vector<std::string> pluginPaths;
    
    std::vector<std::string> systemMessages;
    std::vector<std::string> systemWarnings;
    std::vector<std::string> systemErrors;
    
    std::vector<PluginEventCallback> eventCallbacks;
    std::mutex eventCallbackMutex;
    
    std::atomic<size_t> totalPlugins{0};
    std::atomic<size_t> activePlugins{0};
    std::atomic<size_t> loadedPlugins{0};
    std::atomic<size_t> failedPlugins{0};
    std::atomic<double> systemMemoryUsage{0.0};
    std::atomic<double> systemCPUUsage{0.0};
    
    Impl() {
        // Initialize default paths
        pluginPaths.push_back("plugins");
        pluginPaths.push_back("user/plugins");
        pluginPaths.push_back("system/plugins");
    }
    
    void logMessage(const std::string& message, const std::string& level = "info") {
        if (level == "error") {
            systemErrors.push_back(message);
            NLM_LOG_ERROR(message);
        } else if (level == "warning") {
            systemWarnings.push_back(message);
            NLM_LOG_WARNING(message);
        } else {
            systemMessages.push_back(message);
            NLM_LOG_INFO(message);
        }
        
        // Keep only recent messages
        if (systemMessages.size() > 1000) {
            systemMessages.erase(systemMessages.begin());
        }
        if (systemWarnings.size() > 500) {
            systemWarnings.erase(systemWarnings.begin());
        }
        if (systemErrors.size() > 500) {
            systemErrors.erase(systemErrors.begin());
        }
    }
    
    std::vector<std::string> getPluginNamesByCategory(const std::string& category) const {
        std::vector<std::string> names;
        for (const auto& pair : pluginMetadata) {
            if (pair.second.getCategory() == category) {
                names.push_back(pair.first);
            }
        }
        return names;
    }
    
    std::vector<std::string> getPluginNamesByCapability(const std::string& capability) const {
        std::vector<std::string> names;
        for (const auto& pair : pluginMetadata) {
            if (pair.second.capabilities.canHandle(capability)) {
                names.push_back(pair.first);
            }
        }
        return names;
    }
    
    std::vector<std::string> getPluginNamesByTag(const std::string& tag) const {
        std::vector<std::string> names;
        for (const auto& pair : pluginTags) {
            if (std::find(pair.second.begin(), pair.second.end(), tag) != pair.second.end()) {
                names.push_back(pair.first);
            }
        }
        return names;
    }
    
    std::vector<std::string> getPluginNamesBySecurityLevel(PluginSecurityLevel level) const {
        std::vector<std::string> names;
        for (const auto& pair : pluginSecurityLevels) {
            if (pair.second == level) {
                names.push_back(pair.first);
            }
        }
        return names;
    }
};

PluginRegistry::PluginRegistry() : pImpl(std::make_unique<Impl>()) {}

PluginRegistry::~PluginRegistry() {
    shutdown();
}

PluginRegistry::PluginRegistry(PluginRegistry&& other) noexcept : pImpl(std::move(other.pImpl)) {}

PluginRegistry& PluginRegistry::operator=(PluginRegistry&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool PluginRegistry::initialize(const PluginRegistryConfig& config) {
    if (pImpl->active) {
        logMessage("PluginRegistry: Already initialized", "warning");
        return true;
    }
    
    pImpl->config = config;
    pImpl->active = true;
    
    logMessage("PluginRegistry: Initialized successfully");
    
    onRegistryInitialized();
    
    return true;
}

void PluginRegistry::shutdown() {
    if (!pImpl->active) {
        return;
    }
    
    logMessage("PluginRegistry: Shutting down...");
    
    // Clear all plugins
    clear();
    
    pImpl->active = false;
    
    logMessage("PluginRegistry: Shutdown successfully");
    
    onRegistryShutdown();
}

bool PluginRegistry::isActive() const {
    return pImpl->active;
}

bool PluginRegistry::registerPlugin(std::unique_ptr<Plugin> plugin) {
    if (!plugin) {
        logMessage("PluginRegistry: Cannot register null plugin", "error");
        return false;
    }
    
    const std::string& name = plugin->getName();
    
    std::lock_guard<std::mutex> lock(pImpl->registryMutex);
    
    // Check if plugin already exists
    if (pImpl->plugins.find(name) != pImpl->plugins.end()) {
        logMessage("PluginRegistry: Plugin already registered: " + name, "warning");
        return false;
    }
    
    // Validate plugin
    if (!validatePluginInternal(*plugin)) {
        logMessage("PluginRegistry: Plugin validation failed for " + name, "error");
        return false;
    }
    
    // Check for conflicts
    if (!checkPluginConflicts(*plugin)) {
        logMessage("PluginRegistry: Plugin conflicts detected for " + name, "error");
        return false;
    }
    
    // Register plugin
    pImpl->plugins[name] = std::move(plugin);
    
    // Store metadata
    Plugin* pluginPtr = pImpl->plugins[name].get();
    pImpl->pluginMetadata[name] = pluginPtr->getMetadata();
    pImpl->pluginTags[name] = pluginPtr->getTags();
    pImpl->pluginDependencies[name] = pluginPtr->getDependencies();
    pImpl->pluginConflicts[name] = pluginPtr->getTags(); // Simplified for now
    pImpl->pluginSecurityLevels[name] = pluginPtr->getSecurityLevel();
    pImpl->pluginCapabilities[name] = pluginPtr->getCapabilities();
    
    // Initialize status
    pImpl->pluginStatuses[name] = PluginLoadStatus::NotLoaded;
    pImpl->pluginStatistics[name] = pluginPtr->getStatistics();
    
    pImpl->totalPlugins++;
    
    logMessage("PluginRegistry: Registered plugin: " + name);
    
    onPluginRegistered(name);
    
    return true;
}

bool PluginRegistry::unregisterPlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->registryMutex);
    
    auto it = pImpl->plugins.find(name);
    if (it == pImpl->plugins.end()) {
        logMessage("PluginRegistry: Plugin not found: " + name, "error");
        return false;
    }
    
    // Check if plugin is active
    if (getPluginStatus(name) == PluginLoadStatus::Loaded) {
        logMessage("PluginRegistry: Plugin is still loaded: " + name, "warning");
        return false;
    }
    
    // Remove plugin
    pImpl->plugins.erase(it);
    pImpl->pluginMetadata.erase(name);
    pImpl->pluginTags.erase(name);
    pImpl->pluginDependencies.erase(name);
    pImpl->pluginConflicts.erase(name);
    pImpl->pluginSecurityLevels.erase(name);
    pImpl->pluginCapabilities.erase(name);
    pImpl->pluginConfigs.erase(name);
    pImpl->pluginStatuses.erase(name);
    pImpl->pluginStatistics.erase(name);
    pImpl->pluginStartTimes.erase(name);
    pImpl->pluginStopTimes.erase(name);
    pImpl->pluginEvents.erase(name);
    
    pImpl->totalPlugins--;
    
    logMessage("PluginRegistry: Unregistered plugin: " + name);
    
    onPluginUnregistered(name);
    
    return true;
}

bool PluginRegistry::updatePlugin(std::unique_ptr<Plugin> plugin) {
    if (!plugin) {
        logMessage("PluginRegistry: Cannot update null plugin", "error");
        return false;
    }
    
    const std::string& name = plugin->getName();
    
    std::lock_guard<std::mutex> lock(pImpl->registryMutex);
    
    auto it = pImpl->plugins.find(name);
    if (it == pImpl->plugins.end()) {
        logMessage("PluginRegistry: Plugin not found for update: " + name, "error");
        return false;
    }
    
    // Validate new plugin
    if (!validatePluginInternal(*plugin)) {
        logMessage("PluginRegistry: Plugin validation failed for update " + name, "error");
        return false;
    }
    
    // Check for conflicts with other plugins
    for (const auto& pair : pImpl->plugins) {
        if (pair.first != name && !checkPluginConflicts(*plugin)) {
            logMessage("PluginRegistry: Plugin conflicts detected with existing plugins during update " + name, "error");
            return false;
        }
    }
    
    // Update plugin
    pImpl->plugins[name] = std::move(plugin);
    Plugin* pluginPtr = pImpl->plugins[name].get();
    
    // Update metadata
    pImpl->pluginMetadata[name] = pluginPtr->getMetadata();
    pImpl->pluginTags[name] = pluginPtr->getTags();
    pImpl->pluginDependencies[name] = pluginPtr->getDependencies();
    pImpl->pluginConflicts[name] = pluginPtr->getTags(); // Simplified for now
    pImpl->pluginSecurityLevels[name] = pluginPtr->getSecurityLevel();
    pImpl->pluginCapabilities[name] = pluginPtr->getCapabilities();
    
    logMessage("PluginRegistry: Updated plugin: " + name);
    
    onPluginUpdated(name);
    
    return true;
}

bool PluginRegistry::forceUnregisterPlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->registryMutex);
    
    auto it = pImpl->plugins.find(name);
    if (it == pImpl->plugins.end()) {
        return false;
    }
    
    // Force unregister without validation
    pImpl->plugins.erase(it);
    pImpl->pluginMetadata.erase(name);
    pImpl->pluginTags.erase(name);
    pImpl->pluginDependencies.erase(name);
    pImpl->pluginConflicts.erase(name);
    pImpl->pluginSecurityLevels.erase(name);
    pImpl->pluginCapabilities.erase(name);
    pImpl->pluginConfigs.erase(name);
    pImpl->pluginStatuses.erase(name);
    pImpl->pluginStatistics.erase(name);
    pImpl->pluginStartTimes.erase(name);
    pImpl->pluginStopTimes.erase(name);
    pImpl->pluginEvents.erase(name);
    
    pImpl->totalPlugins--;
    
    return true;
}

Plugin* PluginRegistry::getPlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->registryMutex);
    
    auto it = pImpl->plugins.find(name);
    if (it != pImpl->plugins.end()) {
        return it->second.get();
    }
    
    return nullptr;
}

const Plugin* PluginRegistry::getPlugin(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->registryMutex);
    
    auto it = pImpl->plugins.find(name);
    if (it != pImpl->plugins.end()) {
        return it->second.get();
    }
    
    return nullptr;
}

Plugin* PluginRegistry::getPluginByCategory(const std::string& category) {
    std::lock_guard<std::mutex> lock(pImpl->registryMutex);
    
    for (const auto& pair : pImpl->plugins) {
        if (pair.second->getMetadata().getCategory() == category) {
            return pair.second.get();
        }
    }
    
    return nullptr;
}

Plugin* PluginRegistry::getPluginByCapability(const std::string& capability) {
    std::lock_guard<std::mutex> lock(pImpl->registryMutex);
    
    for (const auto& pair : pImpl->plugins) {
        if (pair.second->getCapabilities().canHandle(capability)) {
            return pair.second.get();
        }
    }
    
    return nullptr;
}

bool PluginRegistry::isPluginRegistered(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->registryMutex);
    return pImpl->plugins.find(name) != pImpl->plugins.end();
}

bool PluginRegistry::isPluginActive(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->registryMutex);
    
    auto it = pImpl->pluginStatuses.find(name);
    return (it != pImpl->pluginStatuses.end()) && (it->second == PluginLoadStatus::Loaded);
}

PluginMetadata PluginRegistry::getPluginMetadata(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->registryMutex);
    
    auto it = pImpl->pluginMetadata.find(name);
    if (it != pImpl->pluginMetadata.end()) {
        return it->second;
    }
    
    return PluginMetadata();
}

PluginLoadStatus PluginRegistry::getPluginStatus(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->registryMutex);
    
    auto it = pImpl->pluginStatuses.find(name);
    if (it != pImpl->pluginStatuses.end()) {
        return it->second;
    }
    
    return PluginLoadStatus::NotLoaded;
}

PluginCapabilities PluginRegistry::getPluginCapabilities(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->registryMutex);
    
    auto it = pImpl->pluginCapabilities.find(name);
    if (it != pImpl->pluginCapabilities.end()) {
        return it->second;
    }
    
    return PluginCapabilities();
}

std::vector<Plugin*> PluginRegistry::getPluginsByCategory(const std::string& category) {
    std::lock_guard<std::mutex> lock(pImpl->registryMutex);
    
    std::vector<Plugin*> plugins;
    for (const auto& pair : pImpl->plugins) {
        if (pair.second->getMetadata().getCategory() == category) {
            plugins.push_back(pair.second.get());
        }
    }
    
    return plugins;
}

std::vector<Plugin*> PluginRegistry::getPluginsByCapability(const std::string& capability) {
    std::lock_guard<std::mutex> lock(pImpl->registryMutex);
    
    std::vector<Plugin*> plugins;
    for (const auto& pair : pImpl->plugins) {
        if (pair.second->getCapabilities().canHandle(capability)) {
            plugins.push_back(pair.second.get());
        }
    }
    
    return plugins;
}

std::vector<Plugin*> PluginRegistry::getPluginsByTag(const std::string& tag) {
    std::lock_guard<std::mutex> lock(pImpl->registryMutex);
    
    std::vector<Plugin*> plugins;
    for (const auto& pair : pImpl->pluginTags) {
        if (std::find(pair.second.begin(), pair.second.end(), tag) != pair.second.end()) {
            auto pluginIt = pImpl->plugins.find(pair.first);
            if (pluginIt != pImpl->plugins.end()) {
                plugins.push_back(pluginIt->second.get());
            }
        }
    }
    
    return plugins;
}

std::vector<Plugin*> PluginRegistry::getPluginsBySecurityLevel(PluginSecurityLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->registryMutex);
    
    std::vector<Plugin*> plugins;
    for (const auto& pair : pImpl->pluginSecurityLevels) {
        if (pair.second == level) {
            auto pluginIt = pImpl->plugins.find(pair.first);
            if (pluginIt != pImpl->plugins.end()) {
                plugins.push_back(pluginIt->second.get());
            }
        }
    }
    
    return plugins;
}

std::vector<Plugin*> PluginRegistry::getPluginsByCategoryAndCapability(const std::string& category, 
                                                                  const std::string& capability) {
    std::lock_guard<std::mutex> lock(pImpl->registryMutex);
    
    std::vector<Plugin*> plugins;
    for (const auto& pair : pImpl->plugins) {
        const PluginMetadata& metadata = pair.second->getMetadata();
        const PluginCapabilities& caps = pair.second->getCapabilities();
        
        if (metadata.getCategory() == category && caps.canHandle(capability)) {
            plugins.push_back(pair.second.get());
        }
    }
    
    return plugins;
}

std::vector<Plugin*> PluginRegistry::getAllPlugins() {
    std::lock_guard<std::mutex> lock(pImpl->registryMutex);
    
    std::vector<Plugin*> plugins;
    for (const auto& pair : pImpl->plugins) {
        plugins.push_back(pair.second.get());
    }
    
    return plugins;
}

std::vector<std::string> PluginRegistry::getAllPluginNames() const {
    std::lock_guard<std::mutex> lock(pImpl->registryMutex);
    
    std::vector<std::string> names;
    for (const auto& pair : pImpl->plugins) {
        names.push_back(pair.first);
    }
    
    return names;
}

std::vector<std::string> PluginRegistry::getPluginsByCategoryNames(const std::string& category) const {
    return pImpl->getPluginNamesByCategory(category);
}

std::vector<std::string> PluginRegistry::getPluginsByCapabilityNames(const std::string& capability) const {
    return pImpl->getPluginNamesByCapability(capability);
}

std::vector<std::string> PluginRegistry::getPluginsByTagNames(const std::string& tag) const {
    return pImpl->getPluginNamesByTag(tag);
}

bool PluginRegistry::validatePlugin(const Plugin& plugin) const {
    return validatePluginInternal(plugin);
}

bool PluginRegistry::validatePlugin(const std::string& name) const {
    const Plugin* plugin = getPlugin(name);
    if (!plugin) {
        return false;
    }
    
    return validatePluginInternal(*plugin);
}

std::vector<std::string> PluginRegistry::getPluginValidationErrors(const Plugin& plugin) const {
    std::vector<std::string> errors;
    
    if (plugin.getName().empty()) {
        errors.push_back("Plugin name cannot be empty");
    }
    
    if (plugin.getVersion().empty()) {
        errors.push_back("Plugin version cannot be empty");
    }
    
    if (plugin.getDescription().empty()) {
        errors.push_back("Plugin description cannot be empty");
    }
    
    if (plugin.getAuthor().empty()) {
        errors.push_back("Plugin author cannot be empty");
    }
    
    return errors;
}

std::vector<std::string> PluginRegistry::getPluginValidationErrors(const std::string& name) const {
    const Plugin* plugin = getPlugin(name);
    if (!plugin) {
        return {"Plugin not found"};
    }
    
    return getPluginValidationErrors(*plugin);
}

bool PluginRegistry::checkPluginDependencies(const Plugin& plugin) const {
    const std::string& name = plugin.getName();
    
    for (const std::string& dependency : plugin.getDependencies()) {
        if (!isPluginRegistered(dependency) || !isPluginActive(dependency)) {
            logMessage("PluginRegistry: Plugin " + name + " depends on " + dependency + 
                      " which is not active", "warning");
            return false;
        }
    }
    
    return true;
}

bool PluginRegistry::checkPluginDependencies(const std::string& name) const {
    const Plugin* plugin = getPlugin(name);
    if (!plugin) {
        return false;
    }
    
    return checkPluginDependencies(*plugin);
}

bool PluginRegistry::resolvePluginDependencies(const std::string& name) const {
    // TODO: Implement dependency resolution
    return true;
}

bool PluginRegistry::resolveAllPluginDependencies() const {
    // TODO: Implement dependency resolution for all plugins
    return true;
}

std::vector<std::string> PluginRegistry::getDependencyOrder() const {
    // TODO: Implement dependency order determination
    return std::vector<std::string>();
}

std::vector<std::string> PluginRegistry::getDependentPlugins(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->registryMutex);
    
    std::vector<std::string> dependents;
    for (const auto& pair : pImpl->plugins) {
        if (pair.first != name) {
            const std::vector<std::string>& deps = pImpl->pluginDependencies.at(pair.first);
            if (std::find(deps.begin(), deps.end(), name) != deps.end()) {
                dependents.push_back(pair.first);
            }
        }
    }
    
    return dependents;
}

std::vector<std::string> PluginRegistry::getConflictingPlugins(const std::string& name) const {
    return pImpl->pluginConflicts.find(name) != pImpl->pluginConflicts.end() ? 
           pImpl->pluginConflicts.at(name) : std::vector<std::string>();
}

bool PluginRegistry::arePluginsCompatible(const std::string& name1, const std::string& name2) const {
    if (name1 == name2) {
        return true;
    }
    
    const std::vector<std::string>& conflicts1 = getConflictingPlugins(name1);
    const std::vector<std::string>& conflicts2 = getConflictingPlugins(name2);
    
    if (std::find(conflicts1.begin(), conflicts1.end(), name2) != conflicts1.end()) {
        return false;
    }
    
    if (std::find(conflicts2.begin(), conflicts2.end(), name1) != conflicts2.end()) {
        return false;
    }
    
    return true;
}

std::vector<std::pair<std::string, std::string>> PluginRegistry::getIncompatiblePluginPairs() const {
    std::vector<std::pair<std::string, std::string>> pairs;
    
    std::vector<std::string> pluginNames = getAllPluginNames();
    for (size_t i = 0; i < pluginNames.size(); ++i) {
        for (size_t j = i + 1; j < pluginNames.size(); ++j) {
            if (!arePluginsCompatible(pluginNames[i], pluginNames[j])) {
                pairs.push_back({pluginNames[i], pluginNames[j]});
            }
        }
    }
    
    return pairs;
}

std::vector<std::string> PluginRegistry::getCompatiblePlugins(const std::string& name) const {
    std::vector<std::string> compatible;
    
    std::vector<std::string> pluginNames = getAllPluginNames();
    for (const std::string& otherName : pluginNames) {
        if (name == otherName || arePluginsCompatible(name, otherName)) {
            compatible.push_back(otherName);
        }
    }
    
    return compatible;
}

bool PluginRegistry::updatePluginMetadata(const std::string& name, const PluginMetadata& metadata) {
    std::lock_guard<std::mutex> lock(pImpl->registryMutex);
    
    auto it = pImpl->pluginMetadata.find(name);
    if (it == pImpl->pluginMetadata.end()) {
        return false;
    }
    
    it->second = metadata;
    return true;
}

bool PluginRegistry::addPluginTags(const std::string& name, const std::vector<std::string>& tags) {
    std::lock_guard<std::mutex> lock(pImpl->registryMutex);
    
    auto it = pImpl->pluginTags.find(name);
    if (it == pImpl->pluginTags.end()) {
        return false;
    }
    
    for (const std::string& tag : tags) {
        if (std::find(it->second.begin(), it->second.end(), tag) == it->second.end()) {
            it->second.push_back(tag);
        }
    }
    
    return true;
}

bool PluginRegistry::removePluginTags(const std::string& name, const std::vector<std::string>& tags) {
    std::lock_guard<std::mutex> lock(pImpl->registryMutex);
    
    auto it = pImpl->pluginTags.find(name);
    if (it == pImpl->pluginTags.end()) {
        return false;
    }
    
    for (const std::string& tag : tags) {
        auto tagIt = std::find(it->second.begin(), it->second.end(), tag);
        if (tagIt != it->second.end()) {
            it->second.erase(tagIt);
        }
    }
    
    return true;
}

bool PluginRegistry::setPluginSecurityLevel(const std::string& name, PluginSecurityLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->registryMutex);
    
    auto it = pImpl->pluginSecurityLevels.find(name);
    if (it == pImpl->pluginSecurityLevels.end()) {
        return false;
    }
    
    it->second = level;
    return true;
}

bool PluginRegistry::addPluginCapability(const std::string& name, const PluginCapabilities& capabilities) {
    std::lock_guard<std::mutex> lock(pImpl->registryMutex);
    
    auto it = pImpl->pluginCapabilities.find(name);
    if (it == pImpl->pluginCapabilities.end()) {
        return false;
    }
    
    // Merge capabilities
    for (const std::string& ability : capabilities.abilities) {
        if (std::find(it->second.abilities.begin(), it->second.abilities.end(), ability) == it->second.abilities.end()) {
            it->second.abilities.push_back(ability);
        }
    }
    
    for (const std::string& integration : capabilities.integrations) {
        if (std::find(it->second.integrations.begin(), it->second.integrations.end(), integration) == it->second.integrations.end()) {
            it->second.integrations.push_back(integration);
        }
    }
    
    return true;
}

std::string PluginRegistry::getPluginVersion(const std::string& name) const {
    PluginMetadata metadata = getPluginMetadata(name);
    return metadata.version;
}

bool PluginRegistry::checkPluginVersion(const std::string& name, const std::string& requiredVersion) const {
    std::string version = getPluginVersion(name);
    // Simple version comparison - TODO: Implement proper version comparison
    return version == requiredVersion;
}

bool PluginRegistry::updatePluginVersion(const std::string& name, const std::string& newVersion) {
    PluginMetadata metadata = getPluginMetadata(name);
    metadata.version = newVersion;
    metadata.lastUpdated = std::chrono::system_clock::now();
    return updatePluginMetadata(name, metadata);
}

std::vector<std::string> PluginRegistry::getPluginVersionHistory(const std::string& name) const {
    // TODO: Implement version history
    return std::vector<std::string>();
}

bool PluginRegistry::addPluginVersion(const std::string& name, const std::string& version) {
    // TODO: Implement version history tracking
    return true;
}

bool PluginRegistry::setPluginConfig(const std::string& name, const std::map<std::string, PluginConfigValue>& config) {
    std::lock_guard<std::mutex> lock(pImpl->registryMutex);
    
    pImpl->pluginConfigs[name] = config;
    return true;
}

std::map<std::string, PluginConfigValue> PluginRegistry::getPluginConfig(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->registryMutex);
    
    auto it = pImpl->pluginConfigs.find(name);
    if (it != pImpl->pluginConfigs.end()) {
        return it->second;
    }
    
    return std::map<std::string, PluginConfigValue>();
}

std::map<std::string, std::map<std::string, PluginConfigValue>> PluginRegistry::getAllPluginConfigs() const {
    return pImpl->pluginConfigs;
}

bool PluginRegistry::savePluginConfig(const std::string& name, const std::string& filepath) const {
    // TODO: Implement plugin config saving
    return true;
}

bool PluginRegistry::loadPluginConfig(const std::string& name, const std::string& filepath) {
    // TODO: Implement plugin config loading
    return true;
}

bool PluginRegistry::resetPluginConfig(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->registryMutex);
    
    if (pImpl->pluginConfigs.find(name) != pImpl->pluginConfigs.end()) {
        pImpl->pluginConfigs.erase(name);
    }
    
    return true;
}

bool PluginRegistry::resetAllPluginConfigs() {
    std::lock_guard<std::mutex> lock(pImpl->registryMutex);
    pImpl->pluginConfigs.clear();
    return true;
}

bool PluginRegistry::savePluginState(const std::string& name, const std::string& filepath) const {
    Plugin* plugin = getPlugin(name);
    if (!plugin) {
        return false;
    }
    
    return plugin->saveState(filepath);
}

bool PluginRegistry::loadPluginState(const std::string& name, const std::string& filepath) {
    Plugin* plugin = getPlugin(name);
    if (!plugin) {
        return false;
    }
    
    return plugin->loadState(filepath);
}

bool PluginRegistry::saveAllPluginStates(const std::string& directory) const {
    for (const std::string& name : getAllPluginNames()) {
        std::filesystem::path filepath = std::filesystem::path(directory) / (name + ".state");
        if (!savePluginState(name, filepath.string())) {
            logMessage("PluginRegistry: Failed to save state for plugin " + name, "warning");
            return false;
        }
    }
    
    return true;
}

bool PluginRegistry::loadAllPluginStates(const std::string& directory) {
    for (const std::string& name : getAllPluginNames()) {
        std::filesystem::path filepath = std::filesystem::path(directory) / (name + ".state");
        if (!loadPluginState(name, filepath.string())) {
            logMessage("PluginRegistry: Failed to load state for plugin " + name, "warning");
            return false;
        }
    }
    
    return true;
}

bool PluginRegistry::clearPluginState(const std::string& name) {
    Plugin* plugin = getPlugin(name);
    if (!plugin) {
        return false;
    }
    
    // Clear plugin's internal state
    return true;
}

bool PluginRegistry::clearAllPluginStates() {
    for (const std::string& name : getAllPluginNames()) {
        clearPluginState(name);
    }
    return true;
}

bool PluginRegistry::startPlugin(const std::string& name) {
    Plugin* plugin = getPlugin(name);
    if (!plugin) {
        return false;
    }
    
    // Set status to Loading
    pImpl->pluginStatuses[name] = PluginLoadStatus::Loading;
    
    // Start plugin
    std::map<std::string, PluginConfigValue> config = getPluginConfig(name);
    bool success = plugin->initialize(config);
    
    if (success) {
        pImpl->pluginStatuses[name] = PluginLoadStatus::Loaded;
        pImpl->pluginStartTimes[name] = std::chrono::system_clock::now();
        pImpl->loadedPlugins++;
        
        logMessage("PluginRegistry: Started plugin: " + name);
        
        onPluginStarted(name);
    } else {
        pImpl->pluginStatuses[name] = PluginLoadStatus::Failed;
        pImpl->failedPlugins++;
        
        logMessage("PluginRegistry: Failed to start plugin: " + name, "error");
        
        onPluginError(name, "Initialization failed");
    }
    
    return success;
}

bool PluginRegistry::stopPlugin(const std::string& name) {
    Plugin* plugin = getPlugin(name);
    if (!plugin) {
        return false;
    }
    
    // Check if plugin is loaded
    if (getPluginStatus(name) != PluginLoadStatus::Loaded) {
        logMessage("PluginRegistry: Plugin not in loaded state: " + name, "warning");
        return false;
    }
    
    // Stop plugin
    bool success = plugin->shutdown();
    
    if (success) {
        pImpl->pluginStatuses[name] = PluginLoadStatus::Unloading;
        pImpl->pluginStopTimes[name] = std::chrono::system_clock::now();
        pImpl->loadedPlugins--;
        
        logMessage("PluginRegistry: Stopped plugin: " + name);
        
        onPluginStopped(name);
    } else {
        logMessage("PluginRegistry: Failed to stop plugin: " + name, "warning");
    }
    
    return success;
}

bool PluginRegistry::pausePlugin(const std::string& name) {
    Plugin* plugin = getPlugin(name);
    if (!plugin) {
        return false;
    }
    
    // TODO: Implement plugin pausing
    return true;
}

bool PluginRegistry::resumePlugin(const std::string& name) {
    Plugin* plugin = getPlugin(name);
    if (!plugin) {
        return false;
    }
    
    // TODO: Implement plugin resuming
    return true;
}

bool PluginRegistry::restartPlugin(const std::string& name) {
    if (!stopPlugin(name)) {
        return false;
    }
    
    if (!startPlugin(name)) {
        return false;
    }
    
    return true;
}

bool PluginRegistry::refreshPlugin(const std::string& name) {
    return restartPlugin(name);
}

PluginLoadStatus PluginRegistry::getPluginStatus(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->registryMutex);
    
    auto it = pImpl->pluginStatuses.find(name);
    if (it != pImpl->pluginStatuses.end()) {
        return it->second;
    }
    
    return PluginLoadStatus::NotLoaded;
}

std::chrono::system_clock::time_point PluginRegistry::getPluginStartTime(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->registryMutex);
    
    auto it = pImpl->pluginStartTimes.find(name);
    if (it != pImpl->pluginStartTimes.end()) {
        return it->second;
    }
    
    return std::chrono::system_clock::time_point();
}

std::chrono::system_clock::time_point PluginRegistry::getPluginStopTime(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->registryMutex);
    
    auto it = pImpl->pluginStopTimes.find(name);
    if (it != pImpl->pluginStopTimes.end()) {
        return it->second;
    }
    
    return std::chrono::system_clock::time_point();
}

double PluginRegistry::getPluginUptime(const std::string& name) const {
    std::chrono::system_clock::time_point startTime = getPluginStartTime(name);
    if (startTime == std::chrono::system_clock::time_point()) {
        return 0.0;
    }
    
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - startTime);
    return static_cast<double>(duration.count());
}

std::map<std::string, PluginLoadStatus> PluginRegistry::getAllPluginStatuses() const {
    return pImpl->pluginStatuses;
}

std::map<std::string, std::chrono::system_clock::time_point> PluginRegistry::getAllPluginStartTimes() const {
    return pImpl->pluginStartTimes;
}

std::map<std::string, std::chrono::system_clock::time_point> PluginRegistry::getAllPluginStopTimes() const {
    return pImpl->pluginStopTimes;
}

void PluginRegistry::addEventCallback(PluginEventCallback callback) {
    std::lock_guard<std::mutex> lock(pImpl->eventCallbackMutex);
    pImpl->eventCallbacks.push_back(callback);
}

void PluginRegistry::removeEventCallback(PluginEventCallback callback) {
    std::lock_guard<std::mutex> lock(pImpl->eventCallbackMutex);
    auto it = std::find(pImpl->eventCallbacks.begin(), pImpl->eventCallbacks.end(), callback);
    if (it != pImpl->eventCallbacks.end()) {
        pImpl->eventCallbacks.erase(it);
    }
}

void PluginRegistry::broadcastPluginEvent(const PluginEvent& event) {
    std::lock_guard<std::mutex> lock(pImpl->eventCallbackMutex);
    for (const auto& callback : pImpl->eventCallbacks) {
        try {
            callback(event);
        } catch (const std::exception& e) {
            logMessage("PluginRegistry: Exception in event callback: " + std::string(e.what()), "error");
        }
    }
}

bool PluginRegistry::executePluginCommand(const std::string& name, const std::string& command,
                                         const std::map<std::string, std::string>& args) {
    Plugin* plugin = getPlugin(name);
    if (!plugin) {
        return false;
    }
    
    return plugin->executeCommand(command, args);
}

bool PluginRegistry::executeSystemCommand(const std::string& command,
                                         const std::map<std::string, std::string>& args) {
    // TODO: Implement system command execution
    return true;
}

bool PluginRegistry::cleanRegistry() {
    // Remove plugins with invalid states
    std::vector<std::string> namesToRemove;
    
    for (const auto& pair : pImpl->pluginStatuses) {
        if (pair.second == PluginLoadStatus::Failed || pair.second == PluginLoadStatus::Errored) {
            namesToRemove.push_back(pair.first);
        }
    }
    
    for (const std::string& name : namesToRemove) {
        forceUnregisterPlugin(name);
    }
    
    return true;
}

bool PluginRegistry::validateRegistry() {
    bool success = true;
    
    for (const auto& pair : pImpl->plugins) {
        if (!validatePluginInternal(*pair.second)) {
            logMessage("PluginRegistry: Plugin validation failed: " + pair.first, "warning");
            success = false;
        }
    }
    
    return success;
}

bool PluginRegistry::repairRegistry() {
    bool success = true;
    
    for (const auto& pair : pImpl->plugins) {
        if (pair.second->getLoadStatus() == PluginLoadStatus::Failed) {
            logMessage("PluginRegistry: Attempting to repair plugin: " + pair.first, "warning");
            
            // TODO: Implement plugin repair
            success = false;
        }
    }
    
    return success;
}

bool PluginRegistry::optimizeRegistry() {
    // TODO: Implement registry optimization
    return true;
}

std::string PluginRegistry::getRegistryStatus() const {
    std::stringstream ss;
    ss << "PluginRegistry Status:" << std::endl;
    ss << "  Active: " << (isActive() ? "true" : "false") << std::endl;
    ss << "  Total Plugins: " << pImpl->totalPlugins << std::endl;
    ss << "  Active Plugins: " << pImpl->activePlugins << std::endl;
    ss << "  Loaded Plugins: " << pImpl->loadedPlugins << std::endl;
    ss << "  Failed Plugins: " << pImpl->failedPlugins << std::endl;
    ss << "  System Memory Usage: " << pImpl->systemMemoryUsage << " MB" << std::endl;
    ss << "  System CPU Usage: " << pImpl->systemCPUUsage << "%" << std::endl;
    
    return ss.str();
}

std::string PluginRegistry::getRegistryInfo() const {
    std::stringstream ss;
    ss << "PluginRegistry Information:" << std::endl;
    ss << "  Plugin Count: " << pImpl->plugins.size() << std::endl;
    ss << "  Plugin Paths: " << pImpl->pluginPaths.size() << std::endl;
    for (const auto& path : pImpl->pluginPaths) {
        ss << "    - " << path << std::endl;
    }
    
    return ss.str();
}

std::vector<std::string> PluginRegistry::getRegistryMessages() const {
    return pImpl->systemMessages;
}

std::vector<std::string> PluginRegistry::getRegistryWarnings() const {
    return pImpl->systemWarnings;
}

std::vector<std::string> PluginRegistry::getRegistryErrors() const {
    return pImpl->systemErrors;
}

bool PluginRegistry::exportRegistry(const std::string& filename, const std::string& format) const {
    // TODO: Implement registry export
    return true;
}

bool PluginRegistry::importRegistry(const std::string& filename, const std::string& format) {
    // TODO: Implement registry import
    return true;
}

std::string PluginRegistry::getAbout() const {
    return "PluginRegistry - Plugin Registration System for NLM";
}

std::string PluginRegistry::getVersion() const {
    return "1.0.0";
}

std::string PluginRegistry::getBuildInfo() const {
    return "PluginRegistry Build Information";
}

bool PluginRegistry::integratePlugins() {
    // TODO: Implement plugin integration
    return true;
}

bool PluginRegistry::removeIntegration(const std::string& name1, const std::string& name2) {
    // TODO: Implement integration removal
    return true;
}

std::vector<std::pair<std::string, std::string>> PluginRegistry::getActiveIntegrations() const {
    // TODO: Implement integration tracking
    return {};
}

bool PluginRegistry::optimizeMemoryUsage() {
    // TODO: Implement memory optimization
    return true;
}

bool PluginRegistry::cleanupUnusedPlugins() {
    // TODO: Implement cleanup
    return true;
}

size_t PluginRegistry::getPluginMemoryUsage(const std::string& name) const {
    // TODO: Implement memory usage calculation
    return 0;
}

double PluginRegistry::getPluginCPUUsage(const std::string& name) const {
    // TODO: Implement CPU usage calculation
    return 0.0;
}

void PluginRegistry::onPluginRegistered(const std::string& name) {
    // Default implementation - can be overridden
    logMessage("PluginRegistry: Plugin registered: " + name);
}

void PluginRegistry::onPluginUnregistered(const std::string& name) {
    // Default implementation - can be overridden
    logMessage("PluginRegistry: Plugin unregistered: " + name);
}

void PluginRegistry::onPluginUpdated(const std::string& name) {
    // Default implementation - can be overridden
    logMessage("PluginRegistry: Plugin updated: " + name);
}

void PluginRegistry::onPluginStarted(const std::string& name) {
    // Default implementation - can be overridden
    logMessage("PluginRegistry: Plugin started: " + name);
}

void PluginRegistry::onPluginStopped(const std::string& name) {
    // Default implementation - can be overridden
    logMessage("PluginRegistry: Plugin stopped: " + name);
}

void PluginRegistry::onPluginError(const std::string& name, const std::string& error) {
    // Default implementation - can be overridden
    logMessage("PluginRegistry: Plugin error " + name + ": " + error, "error");
}

void PluginRegistry::onPluginWarning(const std::string& name, const std::string& warning) {
    // Default implementation - can be overridden
    logMessage("PluginRegistry: Plugin warning " + name + ": " + warning, "warning");
}

void PluginRegistry::onRegistryInitialized() {
    // Default implementation - can be overridden
    logMessage("PluginRegistry: Registry initialized");
}

void PluginRegistry::onRegistryShutdown() {
    // Default implementation - can be overridden
    logMessage("PluginRegistry: Registry shutting down");
}

void PluginRegistry::onRegistryError(const std::string& error) {
    // Default implementation - can be overridden
    logMessage("PluginRegistry: Registry error: " + error, "error");
}

void PluginRegistry::onRegistryWarning(const std::string& warning) {
    // Default implementation - can be overridden
    logMessage("PluginRegistry: Registry warning: " + warning, "warning");
}

// Private helper methods
bool PluginRegistry::registerPluginInternal(std::unique_ptr<Plugin> plugin) {
    return registerPlugin(std::move(plugin));
}

bool PluginRegistry::unregisterPluginInternal(const std::string& name) {
    return unregisterPlugin(name);
}

bool PluginRegistry::updatePluginInternal(const std::string& name, std::unique_ptr<Plugin> plugin) {
    return updatePlugin(std::move(plugin));
}

bool PluginRegistry::validatePluginInternal(const Plugin& plugin) const {
    // Basic validation
    if (plugin.getName().empty() || plugin.getVersion().empty() || 
        plugin.getDescription().empty() || plugin.getAuthor().empty()) {
        return false;
    }
    
    return validatePlugin(plugin);
}

bool PluginRegistry::checkPluginConflicts(const Plugin& plugin) const {
    const std::string& name = plugin.getName();
    
    // Check conflicts with other plugins
    for (const auto& pair : pImpl->pluginConflicts) {
        if (pair.first != name && std::find(pair.second.begin(), pair.second.end(), name) != pair.second.end()) {
            return false;
        }
    }
    
    return true;
}

bool PluginRegistry::resolveDependencyGraph() const {
    // TODO: Implement dependency graph resolution
    return true;
}

bool PluginRegistry::validateAllPlugins() const {
    bool success = true;
    
    for (const auto& pair : pImpl->plugins) {
        if (!validatePluginInternal(*pair.second)) {
            logMessage("PluginRegistry: Plugin validation failed: " + pair.first, "warning");
            success = false;
        }
    }
    
    return success;
}

bool PluginRegistry::cleanupFailedPlugins() const {
    bool success = true;
    
    for (const auto& pair : pImpl->pluginStatuses) {
        if (pair.second == PluginLoadStatus::Failed) {
            forceUnregisterPlugin(pair.first);
        }
    }
    
    return success;
}

std::vector<std::string> PluginRegistry::findPluginsInPath(const std::filesystem::path& path) const {
    std::vector<std::string> pluginNames;
    
    if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path)) {
        return pluginNames;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().filename().string();
            // Check if it's a plugin file
            if (filename.find(".plugin") != std::string::npos || 
                filename.find(".so") != std::string::npos || 
                filename.find(".dll") != std::string::npos) {
                // Extract plugin name from filename
                std::string pluginName = filename.substr(0, filename.find_last_of("."));
                pluginNames.push_back(pluginName);
            }
        }
    }
    
    return pluginNames;
}

bool PluginRegistry::loadPluginsFromPath(const std::filesystem::path& path) {
    bool success = true;
    
    std::vector<std::string> pluginNames = findPluginsInPath(path);
    for (const std::string& pluginName : pluginNames) {
        if (!loadPluginFromPath(std::filesystem::path(path) / (pluginName + ".plugin"))) {
            logMessage("PluginRegistry: Failed to load plugin from path " + path.string() + ": " + pluginName, "warning");
            success = false;
        }
    }
    
    return success;
}

bool PluginRegistry::unloadPluginsFromPath(const std::filesystem::path& path) {
    bool success = true;
    
    std::vector<std::string> pluginNames = findPluginsInPath(path);
    for (const std::string& pluginName : pluginNames) {
        if (!unloadPluginFromPath(std::filesystem::path(path) / (pluginName + ".plugin"))) {
            logMessage("PluginRegistry: Failed to unload plugin from path " + path.string() + ": " + pluginName, "warning");
            success = false;
        }
    }
    
    return success;
}

bool PluginRegistry::savePluginRegistry() const {
    // TODO: Implement registry saving
    return true;
}

bool PluginRegistry::loadPluginRegistry() {
    // TODO: Implement registry loading
    return true;
}

bool PluginRegistry::backupPluginRegistry() const {
    // TODO: Implement registry backup
    return true;
}

bool PluginRegistry::restorePluginRegistry() const {
    // TODO: Implement registry restoration
    return true;
}

void PluginRegistry::logPluginEvent(const PluginEvent& event) {
    std::stringstream ss;
    ss << "Plugin " << event.pluginName << " " << static_cast<int>(event.type);
    if (!event.message.empty()) {
        ss << ": " << event.message;
    }
    
    logMessage(ss.str());
}

void PluginRegistry::updateRegistryStatistics() {
    // TODO: Implement registry statistics update
}

bool PluginRegistry::cleanupPluginResources(const std::string& name) {
    // TODO: Implement resource cleanup
    return true;
}

bool PluginRegistry::validatePluginDependenciesInternal(const std::string& name) const {
    return checkPluginDependencies(name);
}

void PluginRegistry::resolveDependencyOrderInternal() {
    // TODO: Implement dependency order resolution
}

std::vector<std::string> PluginRegistry::getLoadOrderInternal(const std::vector<std::string>& pluginNames) const {
    // TODO: Implement load order determination
    return pluginNames;
}

bool PluginRegistry::checkVersionCompatibility(const std::string& pluginName, const PluginMetadata& metadata) const {
    // TODO: Implement version compatibility checking
    return true;
}

bool PluginRegistry::updatePluginCache(const std::string& name, const PluginMetadata& metadata) {
    // TODO: Implement plugin caching
    return true;
}

std::optional<PluginMetadata> PluginRegistry::getPluginFromCache(const std::string& name) const {
    // TODO: Implement plugin cache retrieval
    return std::nullopt;
}

bool PluginRegistry::removePluginFromCache(const std::string& name) {
    // TODO: Implement plugin cache removal
    return true;
}

std::vector<std::string> PluginRegistry::getPluginDependenciesFromCache(const std::string& name) const {
    // TODO: Implement dependency cache retrieval
    return std::vector<std::string>();
}

bool PluginRegistry::updateDependencyCache(const std::string& name, const std::vector<std::string>& dependencies) {
    // TODO: Implement dependency cache update
    return true;
}

std::vector<std::string> PluginRegistry::getConflictingPluginsFromCache(const std::string& name) const {
    // TODO: Implement conflict cache retrieval
    return std::vector<std::string>();
}

bool PluginRegistry::updateConflictCache(const std::string& name, const std::vector<std::string>& conflicts) {
    // TODO: Implement conflict cache update
    return true;
}

} // namespace nlm