#include "PluginManager.hpp"
#include "PluginRegistry.hpp"
#include "PluginLifecycleManager.hpp"
#include "PluginConfigurationManager.hpp"
#include "PluginSecurity.hpp"
#include "PluginDiscovery.hpp"
#include "../core/Logger/Logger.hpp"
#include <thread>
#include <future>
#include <algorithm>
#include <unordered_set>
#include <sstream>

namespace nlm {

struct PluginManager::Impl {
    bool active{false};
    PluginManagerConfig config;
    PluginRegistry* registry{nullptr};
    PluginLifecycleManager* lifecycleManager{nullptr};
    PluginConfigurationManager* configManager{nullptr};
    PluginSecurity* security{nullptr};
    PluginDiscovery* discovery{nullptr};
    
    std::mutex pluginMutex;
    std::condition_variable pluginCondition;
    std::vector<std::string> pluginsToLoad;
    std::vector<std::string> pluginsToUnload;
    std::atomic<size_t> maxConcurrentLoads{4};
    std::atomic<size_t> currentLoads{0};
    std::vector<std::future<bool>> pendingLoads;
    std::vector<std::future<bool>> pendingUnloads;
    
    std::map<std::string, PluginLoadStatus> pluginStatuses;
    std::map<std::string, std::chrono::system_clock::time_point> pluginStartTimes;
    std::map<std::string, std::chrono::system_clock::time_point> pluginStopTimes;
    std::map<std::string, size_t> pluginLoadCounts;
    std::map<std::string, double> pluginLoadTimes;
    std::map<std::string, double> pluginUnloadTimes;
    
    std::vector<PluginEventCallback> eventCallbacks;
    std::mutex eventCallbackMutex;
    
    std::vector<std::string> systemMessages;
    std::vector<std::string> systemWarnings;
    std::vector<std::string> systemErrors;
    
    std::atomic<size_t> totalPlugins{0};
    std::atomic<size_t> activePlugins{0};
    std::atomic<size_t> loadedPlugins{0};
    std::atomic<size_t> failedPlugins{0};
    std::atomic<double> systemMemoryUsage{0.0};
    std::atomic<double> systemCPUUsage{0.0};
    std::atomic<size_t> pluginLoadCount{0};
    std::atomic<size_t> pluginUnloadCount{0};
    std::atomic<double> averageLoadTime{0.0};
    std::atomic<double> averageUnloadTime{0.0};
    
    Impl() : maxConcurrentLoads(4) {
        // Initialize default components
        registry = new PluginRegistry();
        lifecycleManager = new PluginLifecycleManager();
        configManager = new PluginConfigurationManager();
        security = new PluginSecurity();
        discovery = new PluginDiscovery();
    }
    
    ~Impl() {
        delete registry;
        delete lifecycleManager;
        delete configManager;
        delete security;
        delete discovery;
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
};

PluginManager::PluginManager() : pImpl(std::make_unique<Impl>()) {}

PluginManager::~PluginManager() {
    shutdown();
}

PluginManager::PluginManager(PluginManager&& other) noexcept : pImpl(std::move(other.pImpl)) {}

PluginManager& PluginManager::operator=(PluginManager&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool PluginManager::initialize(const PluginManagerConfig& config) {
    if (pImpl->active) {
        logMessage("PluginManager: Already initialized", "warning");
        return true;
    }
    
    pImpl->config = config;
    pImpl->maxConcurrentLoads = config.maxConcurrentLoads;
    pImpl->active = true;
    
    logMessage("PluginManager: Initialized successfully");
    logMessage("PluginManager: Max concurrent loads = " + std::to_string(pImpl->maxConcurrentLoads));
    
    // Validate plugin paths
    validatePluginPaths();
    
    // Discover available plugins
    discoverPlugins();
    
    onSystemInitialized();
    
    return true;
}

void PluginManager::shutdown() {
    if (!pImpl->active) {
        return;
    }
    
    logMessage("PluginManager: Shutting down all plugins...");
    
    // Stop all plugins
    for (const auto& name : getLoadedPluginNames()) {
        stopPlugin(name);
    }
    
    // Unload all plugins
    for (const auto& name : getLoadedPluginNames()) {
        unloadPluginInternal(name);
    }
    
    // Clear plugin paths
    pImpl->pluginsToLoad.clear();
    pImpl->pluginsToUnload.clear();
    
    // Clear pending operations
    for (auto& future : pImpl->pendingLoads) {
        future.wait();
    }
    for (auto& future : pImpl->pendingUnloads) {
        future.wait();
    }
    pImpl->pendingLoads.clear();
    pImpl->pendingUnloads.clear();
    
    pImpl->active = false;
    
    logMessage("PluginManager: Shutdown successfully");
    
    onSystemShutdown();
}

bool PluginManager::isActive() const {
    return pImpl->active;
}

bool PluginManager::loadPlugin(const std::string& name) {
    if (!pImpl->active) {
        logMessage("PluginManager: Not active, cannot load plugin " + name, "error");
        return false;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->pluginMutex);
    
    if (pImpl->pluginsToLoad.end() != std::find(pImpl->pluginsToLoad.begin(), pImpl->pluginsToLoad.end(), name)) {
        logMessage("PluginManager: Plugin " + name + " is already scheduled for loading", "warning");
        return true;
    }
    
    if (getPluginStatus(name) != PluginLoadStatus::NotLoaded) {
        logMessage("PluginManager: Plugin " + name + " is not in NotLoaded state, status = " + 
                  std::to_string(static_cast<int>(getPluginStatus(name))), "warning");
        return false;
    }
    
    pImpl->pluginsToLoad.push_back(name);
    logMessage("PluginManager: Scheduled plugin " + name + " for loading");
    
    return true;
}

bool PluginManager::loadPluginFromFile(const std::filesystem::path& filepath) {
    if (!pImpl->active) {
        logMessage("PluginManager: Not active, cannot load plugin from file " + filepath.string(), "error");
        return false;
    }
    
    // Check if file exists and is accessible
    if (!std::filesystem::exists(filepath)) {
        logMessage("PluginManager: Plugin file does not exist: " + filepath.string(), "error");
        return false;
    }
    
    // Check if it's a valid plugin file
    std::string extension = filepath.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    std::vector<std::string> validExtensions{".so", ".dll", ".plugin", ".nlm"};
    if (std::find(validExtensions.begin(), validExtensions.end(), extension) == validExtensions.end()) {
        logMessage("PluginManager: Invalid plugin file extension: " + extension, "error");
        return false;
    }
    
    try {
        std::unique_ptr<Plugin> plugin = createPluginFromFile(filepath);
        if (!plugin) {
            logMessage("PluginManager: Failed to create plugin from file: " + filepath.string(), "error");
            return false;
        }
        
        const std::string& pluginName = plugin->getName();
        
        // Validate plugin
        if (!validatePlugin(pluginName)) {
            logMessage("PluginManager: Plugin validation failed for " + pluginName, "error");
            return false;
        }
        
        // Register plugin
        if (!pImpl->registry->registerPlugin(std::move(plugin))) {
            logMessage("PluginManager: Failed to register plugin " + pluginName, "error");
            return false;
        }
        
        pImpl->pluginsToLoad.push_back(pluginName);
        logMessage("PluginManager: Scheduled plugin " + pluginName + " for loading");
        
        return true;
    } catch (const std::exception& e) {
        logMessage("PluginManager: Exception loading plugin from file " + filepath.string() + ": " + e.what(), "error");
        return false;
    }
}

bool PluginManager::loadPluginFromDirectory(const std::filesystem::path& directory) {
    if (!pImpl->active) {
        logMessage("PluginManager: Not active, cannot load plugin from directory " + directory.string(), "error");
        return false;
    }
    
    if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory)) {
        logMessage("PluginManager: Directory does not exist or is not a directory: " + directory.string(), "error");
        return false;
    }
    
    std::vector<std::string> discovered = discoverPlugins(directory);
    bool success = true;
    
    for (const std::string& pluginName : discovered) {
        if (!loadPlugin(pluginName)) {
            logMessage("PluginManager: Failed to load plugin from directory " + directory.string() + ": " + pluginName, "warning");
            success = false;
        }
    }
    
    return success;
}

std::future<bool> PluginManager::loadPluginAsync(const std::string& name) {
    return std::async(std::launch::async, [this, name]() {
        return loadPlugin(name);
    });
}

bool PluginManager::unloadPlugin(const std::string& name) {
    if (!pImpl->active) {
        logMessage("PluginManager: Not active, cannot unload plugin " + name, "error");
        return false;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->pluginMutex);
    
    if (pImpl->pluginsToUnload.end() != std::find(pImpl->pluginsToUnload.begin(), pImpl->pluginsToUnload.end(), name)) {
        logMessage("PluginManager: Plugin " + name + " is already scheduled for unloading", "warning");
        return true;
    }
    
    if (getPluginStatus(name) != PluginLoadStatus::Loaded) {
        logMessage("PluginManager: Plugin " + name + " is not in Loaded state, status = " + 
                  std::to_string(static_cast<int>(getPluginStatus(name))), "warning");
        return false;
    }
    
    pImpl->pluginsToUnload.push_back(name);
    logMessage("PluginManager: Scheduled plugin " + name + " for unloading");
    
    return true;
}

std::future<bool> PluginManager::unloadPluginAsync(const std::string& name) {
    return std::async(std::launch::async, [this, name]() {
        return unloadPlugin(name);
    });
}

bool PluginManager::configurePlugin(const std::string& name, const std::map<std::string, PluginConfigValue>& config) {
    if (!pImpl->active) {
        logMessage("PluginManager: Not active, cannot configure plugin " + name, "error");
        return false;
    }
    
    Plugin* plugin = getPlugin(name);
    if (!plugin) {
        logMessage("PluginManager: Plugin not found: " + name, "error");
        return false;
    }
    
    return plugin->configure(config);
}

std::map<std::string, PluginConfigValue> PluginManager::getPluginConfig(const std::string& name) const {
    if (!pImpl->active) {
        logMessage("PluginManager: Not active, cannot get config for plugin " + name, "error");
        return {};
    }
    
    return pImpl->configManager->getPluginConfig(name);
}

bool PluginManager::savePluginConfig(const std::string& name, const std::string& filepath) const {
    if (!pImpl->active) {
        logMessage("PluginManager: Not active, cannot save config for plugin " + name, "error");
        return false;
    }
    
    return pImpl->configManager->savePluginConfig(name, filepath);
}

bool PluginManager::loadPluginConfig(const std::string& name, const std::string& filepath) {
    if (!pImpl->active) {
        logMessage("PluginManager: Not active, cannot load config for plugin " + name, "error");
        return false;
    }
    
    return pImpl->configManager->loadPluginConfig(name, filepath);
}

bool PluginManager::savePluginState(const std::string& name, const std::string& filepath) const {
    if (!pImpl->active) {
        logMessage("PluginManager: Not active, cannot save state for plugin " + name, "error");
        return false;
    }
    
    Plugin* plugin = getPlugin(name);
    if (!plugin) {
        logMessage("PluginManager: Plugin not found: " + name, "error");
        return false;
    }
    
    return plugin->saveState(filepath);
}

bool PluginManager::loadPluginState(const std::string& name, const std::string& filepath) {
    if (!pImpl->active) {
        logMessage("PluginManager: Not active, cannot load state for plugin " + name, "error");
        return false;
    }
    
    Plugin* plugin = getPlugin(name);
    if (!plugin) {
        logMessage("PluginManager: Plugin not found: " + name, "error");
        return false;
    }
    
    return plugin->loadState(filepath);
}

bool PluginManager::startPlugin(const std::string& name) {
    if (!pImpl->active) {
        logMessage("PluginManager: Not active, cannot start plugin " + name, "error");
        return false;
    }
    
    return pImpl->lifecycleManager->startPlugin(name);
}

bool PluginManager::stopPlugin(const std::string& name) {
    if (!pImpl->active) {
        logMessage("PluginManager: Not active, cannot stop plugin " + name, "error");
        return false;
    }
    
    return pImpl->lifecycleManager->stopPlugin(name);
}

bool PluginManager::pausePlugin(const std::string& name) {
    if (!pImpl->active) {
        logMessage("PluginManager: Not active, cannot pause plugin " + name, "error");
        return false;
    }
    
    return pImpl->lifecycleManager->pausePlugin(name);
}

bool PluginManager::resumePlugin(const std::string& name) {
    if (!pImpl->active) {
        logMessage("PluginManager: Not active, cannot resume plugin " + name, "error");
        return false;
    }
    
    return pImpl->lifecycleManager->resumePlugin(name);
}

PluginLoadStatus PluginManager::getPluginStatus(const std::string& name) const {
    if (!pImpl->active) {
        return PluginLoadStatus::NotLoaded;
    }
    
    return pImpl->lifecycleManager->getPluginStatus(name);
}

PluginLoadStatus PluginManager::getPluginLoadStatus(const std::string& name) const {
    return getPluginStatus(name);
}

std::chrono::system_clock::time_point PluginManager::getPluginStartTime(const std::string& name) const {
    if (pImpl->pluginStartTimes.find(name) != pImpl->pluginStartTimes.end()) {
        return pImpl->pluginStartTimes.at(name);
    }
    return std::chrono::system_clock::time_point();
}

std::chrono::system_clock::time_point PluginManager::getPluginStopTime(const std::string& name) const {
    if (pImpl->pluginStopTimes.find(name) != pImpl->pluginStopTimes.end()) {
        return pImpl->pluginStopTimes.at(name);
    }
    return std::chrono::system_clock::time_point();
}

double PluginManager::getPluginUptime(const std::string& name) const {
    std::chrono::system_clock::time_point startTime = getPluginStartTime(name);
    if (startTime == std::chrono::system_clock::time_point()) {
        return 0.0;
    }
    
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - startTime);
    return static_cast<double>(duration.count());
}

std::vector<std::string> PluginManager::discoverPlugins(const std::filesystem::path& directory) {
    return pImpl->discovery->discoverPlugins(directory);
}

std::vector<std::string> PluginManager::getAvailablePluginTypes() const {
    std::vector<std::string> types;
    for (const auto& factory : pImpl->pluginFactories) {
        types.push_back(factory.first);
    }
    return types;
}

std::vector<std::string> PluginManager::getRegisteredPluginNames() const {
    return pImpl->registry->getRegisteredPluginNames();
}

std::vector<std::string> PluginManager::getActivePluginNames() const {
    std::vector<std::string> activeNames;
    for (const auto& name : getRegisteredPluginNames()) {
        if (pImpl->lifecycleManager->getPluginStatus(name) == PluginLoadStatus::Loaded) {
            activeNames.push_back(name);
        }
    }
    return activeNames;
}

std::vector<std::string> PluginManager::getLoadedPluginNames() const {
    std::vector<std::string> loadedNames;
    for (const auto& name : getRegisteredPluginNames()) {
        if (pImpl->lifecycleManager->getPluginStatus(name) == PluginLoadStatus::Loaded) {
            loadedNames.push_back(name);
        }
    }
    return loadedNames;
}

Plugin* PluginManager::getPlugin(const std::string& name) {
    return pImpl->registry->getPlugin(name);
}

const Plugin* PluginManager::getPlugin(const std::string& name) const {
    return pImpl->registry->getPlugin(name);
}

PluginMetadata PluginManager::getPluginMetadata(const std::string& name) const {
    Plugin* plugin = getPlugin(name);
    if (plugin) {
        return plugin->getMetadata();
    }
    return PluginMetadata();
}

PluginCapabilities PluginManager::getPluginCapabilities(const std::string& name) const {
    Plugin* plugin = getPlugin(name);
    if (plugin) {
        return plugin->getCapabilities();
    }
    return PluginCapabilities();
}

std::vector<std::string> PluginManager::getPluginDependencies(const std::string& name) const {
    Plugin* plugin = getPlugin(name);
    if (plugin) {
        return plugin->getDependencies();
    }
    return {};
}

PluginSecurityLevel PluginManager::getPluginSecurityLevel(const std::string& name) const {
    Plugin* plugin = getPlugin(name);
    if (plugin) {
        return plugin->getSecurityLevel();
    }
    return PluginSecurityLevel::Medium;
}

bool PluginManager::registerPluginFactory(const std::string& pluginType, std::unique_ptr<PluginFactory> factory) {
    if (!factory) {
        logMessage("PluginManager: Cannot register null factory for type " + pluginType, "error");
        return false;
    }
    
    pImpl->pluginFactories[pluginType] = std::move(factory);
    logMessage("PluginManager: Registered plugin factory for type " + pluginType);
    return true;
}

bool PluginManager::unregisterPluginFactory(const std::string& pluginType) {
    auto it = pImpl->pluginFactories.find(pluginType);
    if (it == pImpl->pluginFactories.end()) {
        logMessage("PluginManager: No factory found for type " + pluginType, "warning");
        return false;
    }
    
    pImpl->pluginFactories.erase(it);
    logMessage("PluginManager: Unregistered plugin factory for type " + pluginType);
    return true;
}

PluginFactory* PluginManager::getPluginFactory(const std::string& pluginType) {
    auto it = pImpl->pluginFactories.find(pluginType);
    if (it != pImpl->pluginFactories.end()) {
        return it->second.get();
    }
    return nullptr;
}

std::vector<std::string> PluginManager::getRegisteredPluginTypes() const {
    std::vector<std::string> types;
    for (const auto& factory : pImpl->pluginFactories) {
        types.push_back(factory.first);
    }
    return types;
}

bool PluginManager::addPluginPath(const std::filesystem::path& path) {
    return pImpl->discovery->addPluginPath(path);
}

bool PluginManager::removePluginPath(const std::filesystem::path& path) {
    return pImpl->discovery->removePluginPath(path);
}

std::vector<std::filesystem::path> PluginManager::getPluginPaths() const {
    return pImpl->discovery->getPluginPaths();
}

std::vector<Plugin*> PluginManager::getPluginsByCategory(const std::string& category) {
    return pImpl->registry->getPluginsByCategory(category);
}

std::vector<Plugin*> PluginManager::getPluginsByCapability(const std::string& capability) {
    return pImpl->registry->getPluginsByCapability(capability);
}

std::vector<Plugin*> PluginManager::getPluginsByTag(const std::string& tag) {
    std::vector<Plugin*> plugins;
    for (const auto& name : getRegisteredPluginNames()) {
        Plugin* plugin = getPlugin(name);
        if (plugin) {
            const std::vector<std::string>& tags = plugin->getTags();
            if (std::find(tags.begin(), tags.end(), tag) != tags.end()) {
                plugins.push_back(plugin);
            }
        }
    }
    return plugins;
}

std::vector<Plugin*> PluginManager::getPluginsBySecurityLevel(PluginSecurityLevel level) {
    std::vector<Plugin*> plugins;
    for (const auto& name : getRegisteredPluginNames()) {
        PluginSecurityLevel pluginLevel = getPluginSecurityLevel(name);
        if (pluginLevel == level) {
            plugins.push_back(getPlugin(name));
        }
    }
    return plugins;
}

std::vector<Plugin*> PluginManager::getCompatiblePlugins(const PluginMetadata& pluginMetadata) const {
    return pImpl->registry->getPluginsByCategory(pluginMetadata.name);
}

bool PluginManager::validatePlugin(const std::string& name) const {
    Plugin* plugin = getPlugin(name);
    if (!plugin) {
        logMessage("PluginManager: Plugin not found: " + name, "error");
        return false;
    }
    
    return plugin->validate();
}

bool PluginManager::verifyPlugin(const std::string& name) const {
    if (!pImpl->security) {
        logMessage("PluginManager: Security system not available", "error");
        return false;
    }
    
    Plugin* plugin = getPlugin(name);
    if (!plugin) {
        logMessage("PluginManager: Plugin not found: " + name, "error");
        return false;
    }
    
    return pImpl->security->verifyPlugin(std::filesystem::path());
}

bool PluginManager::checkPluginDependencies(const std::string& name) const {
    std::vector<std::string> dependencies = getPluginDependencies(name);
    
    for (const std::string& dependency : dependencies) {
        PluginLoadStatus status = getPluginStatus(dependency);
        if (status != PluginLoadStatus::Loaded && status != PluginLoadStatus::Loading) {
            logMessage("PluginManager: Plugin " + name + " depends on " + dependency + 
                      " which is not loaded (status: " + std::to_string(static_cast<int>(status)) + ")", 
                      "warning");
            return false;
        }
    }
    
    return true;
}

bool PluginManager::resolvePluginDependencies() const {
    // TODO: Implement dependency resolution
    return true;
}

std::vector<std::string> PluginManager::getPluginValidationErrors(const std::string& name) const {
    Plugin* plugin = getPlugin(name);
    if (!plugin) {
        return {"Plugin not found"};
    }
    
    return plugin->getValidationErrors();
}

void PluginManager::addEventCallback(PluginEventCallback callback) {
    std::lock_guard<std::mutex> lock(pImpl->eventCallbackMutex);
    pImpl->eventCallbacks.push_back(callback);
}

void PluginManager::removeEventCallback(PluginEventCallback callback) {
    std::lock_guard<std::mutex> lock(pImpl->eventCallbackMutex);
    auto it = std::find(pImpl->eventCallbacks.begin(), pImpl->eventCallbacks.end(), callback);
    if (it != pImpl->eventCallbacks.end()) {
        pImpl->eventCallbacks.erase(it);
    }
}

void PluginManager::broadcastPluginEvent(const PluginEvent& event) {
    std::lock_guard<std::mutex> lock(pImpl->eventCallbackMutex);
    for (const auto& callback : pImpl->eventCallbacks) {
        try {
            callback(event);
        } catch (const std::exception& e) {
            logMessage("PluginManager: Exception in event callback: " + std::string(e.what()), "error");
        }
    }
}

PluginStatistics PluginManager::getPluginStatistics(const std::string& name) const {
    Plugin* plugin = getPlugin(name);
    if (!plugin) {
        return PluginStatistics();
    }
    
    return plugin->getStatistics();
}

PluginSystemStatistics PluginManager::getSystemStatistics() const {
    PluginSystemStatistics stats;
    
    stats.totalPlugins = pImpl->totalPlugins;
    stats.activePlugins = pImpl->activePlugins;
    stats.loadedPlugins = pImpl->loadedPlugins;
    stats.failedPlugins = pImpl->failedPlugins;
    stats.systemMemoryUsage = pImpl->systemMemoryUsage;
    stats.systemCPUUsage = pImpl->systemCPUUsage;
    stats.pluginLoadCount = pImpl->pluginLoadCount;
    stats.pluginUnloadCount = pImpl->pluginUnloadCount;
    stats.averagePluginLoadTime = pImpl->averageLoadTime;
    stats.averagePluginUnloadTime = pImpl->averageUnloadTime;
    
    return stats;
}

double PluginManager::getAverageLoadTime(const std::string& name) const {
    if (pImpl->pluginLoadTimes.find(name) != pImpl->pluginLoadTimes.end()) {
        return pImpl->pluginLoadTimes.at(name);
    }
    return 0.0;
}

double PluginManager::getAverageUnloadTime(const std::string& name) const {
    if (pImpl->pluginUnloadTimes.find(name) != pImpl->pluginUnloadTimes.end()) {
        return pImpl->pluginUnloadTimes.at(name);
    }
    return 0.0;
}

void PluginManager::setConfig(const PluginManagerConfig& config) {
    pImpl->config = config;
    pImpl->maxConcurrentLoads = config.maxConcurrentLoads;
}

PluginManagerConfig PluginManager::getConfig() const {
    return pImpl->config;
}

bool PluginManager::enablePlugin(const std::string& name) {
    Plugin* plugin = getPlugin(name);
    if (!plugin) {
        logMessage("PluginManager: Plugin not found: " + name, "error");
        return false;
    }
    
    plugin->setActive(true);
    logMessage("PluginManager: Enabled plugin " + name);
    return true;
}

bool PluginManager::disablePlugin(const std::string& name) {
    Plugin* plugin = getPlugin(name);
    if (!plugin) {
        logMessage("PluginManager: Plugin not found: " + name, "error");
        return false;
    }
    
    plugin->setActive(false);
    logMessage("PluginManager: Disabled plugin " + name);
    return true;
}

bool PluginManager::restartPlugin(const std::string& name) {
    if (!stopPlugin(name)) {
        logMessage("PluginManager: Failed to stop plugin " + name, "error");
        return false;
    }
    
    if (!unloadPlugin(name)) {
        logMessage("PluginManager: Failed to unload plugin " + name, "error");
        return false;
    }
    
    if (!loadPlugin(name)) {
        logMessage("PluginManager: Failed to load plugin " + name, "error");
        return false;
    }
    
    if (!startPlugin(name)) {
        logMessage("PluginManager: Failed to start plugin " + name, "error");
        return false;
    }
    
    logMessage("PluginManager: Restarted plugin " + name);
    return true;
}

bool PluginManager::refreshPlugin(const std::string& name) {
    return restartPlugin(name);
}

bool PluginManager::updateAllPlugins(double deltaTime) {
    bool success = true;
    
    for (const auto& pluginName : getLoadedPluginNames()) {
        Plugin* plugin = getPlugin(pluginName);
        if (plugin && plugin->isActive()) {
            if (!plugin->update(deltaTime)) {
                logMessage("PluginManager: Plugin " + pluginName + " failed to update", "warning");
                success = false;
            }
        }
    }
    
    return success;
}

bool PluginManager::cleanPluginCache() {
    // TODO: Implement cache cleaning
    return true;
}

bool PluginManager::validatePluginSystem() {
    bool success = true;
    
    for (const auto& pluginName : getRegisteredPluginNames()) {
        if (!validatePlugin(pluginName)) {
            logMessage("PluginManager: Plugin validation failed for " + pluginName, "warning");
            success = false;
        }
    }
    
    return success;
}

bool PluginManager::repairPluginSystem() {
    bool success = true;
    
    for (const auto& pluginName : getLoadedPluginNames()) {
        if (getPluginStatus(pluginName) != PluginLoadStatus::Loaded) {
            logMessage("PluginManager: Plugin " + pluginName + " is not in Loaded state, attempting to fix", 
                      "warning");
            if (!restartPlugin(pluginName)) {
                logMessage("PluginManager: Failed to repair plugin " + pluginName, "warning");
                success = false;
            }
        }
    }
    
    return success;
}

bool PluginManager::optimizePluginSystem() {
    // TODO: Implement system optimization
    return true;
}

std::string PluginManager::getSystemStatus() const {
    std::stringstream ss;
    ss << "PluginSystem Status:" << std::endl;
    ss << "  Active: " << (isActive() ? "true" : "false") << std::endl;
    ss << "  Total Plugins: " << getRegisteredPluginNames().size() << std::endl;
    ss << "  Loaded Plugins: " << getLoadedPluginNames().size() << std::endl;
    ss << "  Active Plugins: " << getActivePluginNames().size() << std::endl;
    ss << "  Failed Plugins: " << pImpl->failedPlugins << std::endl;
    ss << "  System Memory Usage: " << pImpl->systemMemoryUsage << " MB" << std::endl;
    ss << "  System CPU Usage: " << pImpl->systemCPUUsage << "%" << std::endl;
    ss << "  Plugin Load Count: " << pImpl->pluginLoadCount << std::endl;
    ss << "  Plugin Unload Count: " << pImpl->pluginUnloadCount << std::endl;
    ss << "  Average Load Time: " << pImpl->averageLoadTime << " ms" << std::endl;
    ss << "  Average Unload Time: " << pImpl->averageUnloadTime << " ms" << std::endl;
    
    return ss.str();
}

std::string PluginManager::getSystemInfo() const {
    std::stringstream ss;
    ss << "PluginSystem Information:" << std::endl;
    ss << "  Name: " << PLUGIN_SYSTEM_NAME << std::endl;
    ss << "  Version: " << PLUGIN_SYSTEM_VERSION << std::endl;
    ss << "  Max Concurrent Loads: " << pImpl->maxConcurrentLoads << std::endl;
    ss << "  Default Security Level: " << static_cast<int>(pImpl->config.defaultSecurityLevel) << std::endl;
    ss << "  Max Plugin Memory Usage: " << pImpl->config.maxPluginMemoryUsageMB << " MB" << std::endl;
    ss << "  Max CPU Usage: " << pImpl->config.maxCPUUsagePercent << "%" << std::endl;
    ss << "  Plugin Paths: " << getPluginPaths().size() << std::endl;
    for (const auto& path : getPluginPaths()) {
        ss << "    - " << path.string() << std::endl;
    }
    
    return ss.str();
}

std::vector<std::string> PluginManager::getSystemMessages() const {
    return pImpl->systemMessages;
}

std::vector<std::string> PluginManager::getSystemWarnings() const {
    return pImpl->systemWarnings;
}

std::vector<std::string> PluginManager::getSystemErrors() const {
    return pImpl->systemErrors;
}

bool PluginManager::exportPluginSystem(const std::string& filename, const std::string& format) const {
    // TODO: Implement plugin system export
    return true;
}

bool PluginManager::importPluginSystem(const std::string& filename, const std::string& format) {
    // TODO: Implement plugin system import
    return true;
}

bool PluginManager::executePluginCommand(const std::string& name, const std::string& command, 
                                       const std::map<std::string, std::string>& args) {
    Plugin* plugin = getPlugin(name);
    if (!plugin) {
        logMessage("PluginManager: Plugin not found: " + name, "error");
        return false;
    }
    
    return plugin->executeCommand(command, args);
}

bool PluginManager::executeSystemCommand(const std::string& command, 
                                       const std::map<std::string, std::string>& args) {
    // TODO: Implement system command execution
    return true;
}

std::string PluginManager::getPluginHelp(const std::string& name) const {
    Plugin* plugin = getPlugin(name);
    if (!plugin) {
        return "Plugin not found: " + name;
    }
    
    return plugin->getHelp();
}

std::string PluginManager::getSystemHelp() const {
    std::stringstream ss;
    ss << "PluginSystem Help:" << std::endl;
    ss << "  Available commands:" << std::endl;
    ss << "    load <plugin_name> - Load a plugin" << std::endl;
    ss << "    unload <plugin_name> - Unload a plugin" << std::endl;
    ss << "    start <plugin_name> - Start a plugin" << std::endl;
    ss << "    stop <plugin_name> - Stop a plugin" << std::endl;
    ss << "    enable <plugin_name> - Enable a plugin" << std::endl;
    ss << "    disable <plugin_name> - Disable a plugin" << std::endl;
    ss << "    configure <plugin_name> <key>=<value> - Configure a plugin" << std::endl;
    ss << "    status - Show system status" << std::endl;
    ss << "    info - Show system information" << std::endl;
    ss << "    help - Show this help" << std::endl;
    
    return ss.str();
}

std::string PluginManager::getPluginDocumentation(const std::string& name) const {
    Plugin* plugin = getPlugin(name);
    if (!plugin) {
        return "Plugin not found: " + name;
    }
    
    return plugin->getHelp();
}

std::string PluginManager::getAbout() const {
    std::stringstream ss;
    ss << "PluginSystem " << PLUGIN_SYSTEM_VERSION << std::endl;
    ss << "By: NLM Team" << std::endl;
    ss << "Description: Comprehensive plugin management system for NLM" << std::endl;
    ss << "License: MIT" << std::endl;
    
    return ss.str();
}

std::string PluginManager::getVersion() const {
    return PLUGIN_SYSTEM_VERSION;
}

std::string PluginManager::getBuildInfo() const {
    return "PluginSystem Build Information";
}

bool PluginManager::integratePlugins() {
    // TODO: Implement plugin integration
    return true;
}

bool PluginManager::removeIntegration(const std::string& name1, const std::string& name2) {
    // TODO: Implement integration removal
    return true;
}

std::vector<std::pair<std::string, std::string>> PluginManager::getActiveIntegrations() const {
    // TODO: Implement integration tracking
    return {};
}

bool PluginManager::optimizeMemoryUsage() {
    // TODO: Implement memory optimization
    return true;
}

bool PluginManager::cleanupUnusedPlugins() {
    // TODO: Implement cleanup
    return true;
}

size_t PluginManager::getPluginMemoryUsage(const std::string& name) const {
    // TODO: Implement memory usage calculation
    return 0;
}

double PluginManager::getPluginCPUUsage(const std::string& name) const {
    // TODO: Implement CPU usage calculation
    return 0.0;
}

void PluginManager::onPluginLoaded(const std::string& name) {
    // Default implementation - can be overridden
    logMessage("PluginManager: Plugin loaded: " + name);
}

void PluginManager::onPluginUnloaded(const std::string& name) {
    // Default implementation - can be overridden
    logMessage("PluginManager: Plugin unloaded: " + name);
}

void PluginManager::onPluginStarted(const std::string& name) {
    // Default implementation - can be overridden
    logMessage("PluginManager: Plugin started: " + name);
}

void PluginManager::onPluginStopped(const std::string& name) {
    // Default implementation - can be overridden
    logMessage("PluginManager: Plugin stopped: " + name);
}

void PluginManager::onPluginError(const std::string& name, const std::string& error) {
    // Default implementation - can be overridden
    logMessage("PluginManager: Plugin error " + name + ": " + error, "error");
}

void PluginManager::onPluginWarning(const std::string& name, const std::string& warning) {
    // Default implementation - can be overridden
    logMessage("PluginManager: Plugin warning " + name + ": " + warning, "warning");
}

void PluginManager::onSystemInitialized() {
    // Default implementation - can be overridden
    logMessage("PluginManager: System initialized");
}

void PluginManager::onSystemShutdown() {
    // Default implementation - can be overridden
    logMessage("PluginManager: System shutting down");
}

void PluginManager::onSystemError(const std::string& error) {
    // Default implementation - can be overridden
    logMessage("PluginManager: System error: " + error, "error");
}

void PluginManager::onSystemWarning(const std::string& warning) {
    // Default implementation - can be overridden
    logMessage("PluginManager: System warning: " + warning, "warning");
}

// Private helper methods
bool PluginManager::loadPluginInternal(const std::string& name) {
    // TODO: Implement internal plugin loading
    return true;
}

bool PluginManager::unloadPluginInternal(const std::string& name) {
    // TODO: Implement internal plugin unloading
    return true;
}

void PluginManager::schedulePluginLoad(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->pluginMutex);
    pImpl->pluginsToLoad.push_back(name);
}

void PluginManager::schedulePluginUnload(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->pluginMutex);
    pImpl->pluginsToUnload.push_back(name);
}

void PluginManager::processPluginQueue() {
    // TODO: Implement plugin queue processing
}

bool PluginManager::validatePluginPaths() const {
    bool success = true;
    
    for (const auto& path : getPluginPaths()) {
        if (!std::filesystem::exists(path)) {
            logMessage("PluginManager: Plugin path does not exist: " + path.string(), "warning");
            success = false;
        } else if (!std::filesystem::is_directory(path)) {
            logMessage("PluginManager: Plugin path is not a directory: " + path.string(), "warning");
            success = false;
        }
    }
    
    return success;
}

std::filesystem::path PluginManager::getPluginFilePath(const std::string& name) const {
    for (const auto& path : getPluginPaths()) {
        std::filesystem::path pluginFile = path / (name + ".plugin");
        if (std::filesystem::exists(pluginFile)) {
            return pluginFile;
        }
    }
    return std::filesystem::path();
}

std::vector<std::filesystem::path> PluginManager::findPluginFiles(const std::filesystem::path& directory) const {
    std::vector<std::filesystem::path> pluginFiles;
    
    if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory)) {
        return pluginFiles;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            std::string extension = entry.path().extension().string();
            std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
            
            std::vector<std::string> validExtensions{".so", ".dll", ".plugin", ".nlm"};
            if (std::find(validExtensions.begin(), validExtensions.end(), extension) != validExtensions.end()) {
                pluginFiles.push_back(entry.path());
            }
        }
    }
    
    return pluginFiles;
}

bool PluginManager::loadPluginFromFileInternal(const std::filesystem::path& filepath) {
    try {
        std::unique_ptr<Plugin> plugin = createPluginFromFile(filepath);
        if (!plugin) {
            logMessage("PluginManager: Failed to create plugin from file: " + filepath.string(), "error");
            return false;
        }
        
        const std::string& pluginName = plugin->getName();
        
        if (!validatePlugin(pluginName)) {
            logMessage("PluginManager: Plugin validation failed for " + pluginName, "error");
            return false;
        }
        
        if (!pImpl->registry->registerPlugin(std::move(plugin))) {
            logMessage("PluginManager: Failed to register plugin " + pluginName, "error");
            return false;
        }
        
        logMessage("PluginManager: Loaded plugin from file: " + filepath.string());
        return true;
    } catch (const std::exception& e) {
        logMessage("PluginManager: Exception loading plugin from file " + filepath.string() + ": " + e.what(), "error");
        return false;
    }
}

std::unique_ptr<Plugin> PluginManager::createPluginFromFile(const std::filesystem::path& filepath) {
    // TODO: Implement plugin creation from file
    return nullptr;
}

std::unique_ptr<Plugin> PluginManager::createPluginFromFactory(const std::string& pluginType, const std::string& name) {
    PluginFactory* factory = getPluginFactory(pluginType);
    if (!factory) {
        logMessage("PluginManager: No factory found for plugin type " + pluginType, "error");
        return nullptr;
    }
    
    if (!factory->canCreate(name)) {
        logMessage("PluginManager: Factory cannot create plugin " + name, "error");
        return nullptr;
    }
    
    return factory->createPlugin(name);
}

bool PluginManager::checkSecurityConstraints(const PluginMetadata& metadata) const {
    return metadata.securityLevel <= pImpl->config.defaultSecurityLevel;
}

void PluginManager::logPluginEvent(const PluginEvent& event) {
    std::stringstream ss;
    ss << "Plugin " << event.pluginName << " " << static_cast<int>(event.type);
    if (!event.message.empty()) {
        ss << ": " << event.message;
    }
    
    logMessage(ss.str());
}

void PluginManager::updateSystemStatistics() {
    // TODO: Implement system statistics update
}

bool PluginManager::cleanupPluginResources(const std::string& name) {
    // TODO: Implement resource cleanup
    return true;
}

bool PluginManager::validatePluginDependencies(const std::string& name) const {
    // TODO: Implement dependency validation
    return true;
}

void PluginManager::resolveDependencyOrder() {
    // TODO: Implement dependency resolution
}

std::vector<std::string> PluginManager::getLoadOrder(const std::vector<std::string>& pluginNames) const {
    // TODO: Implement load order determination
    return pluginNames;
}

} // namespace nlm