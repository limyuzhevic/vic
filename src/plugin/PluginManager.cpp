#include "PluginManager.hpp"
#include "PluginLoader.hpp"
#include <fstream>
#include <nlohmann/json.hpp>
#include <algorithm>

namespace nlm {

bool PluginManager::registerBuiltInPlugin(std::unique_ptr<PluginBase> plugin) {
    if (!plugin) return false;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    const std::string& name = plugin->getName();
    if (hasPlugin(name)) {
        return false; // Plugin already registered
    }
    
    plugins_[name] = std::move(plugin);
    metadata_[name] = plugins_[name]->getMetadata();
    
    return true;
}

bool PluginManager::registerExternalPlugin(const std::string& name,
                                          const std::string& libraryPath,
                                          const PluginMetadata& metadata) {
    try {
        auto plugin = PluginLoader::loadPlugin(libraryPath);
        if (!plugin) return false;
        
        // Override metadata with provided metadata
        plugin->setName(metadata.name);
        plugin->setVersion(metadata.version);
        plugin->setDescription(metadata.description);
        plugin->setAuthor(metadata.author);
        
        std::lock_guard<std::mutex> lock(mutex_);
        plugins_[name] = std::move(plugin);
        metadata_[name] = metadata;
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool PluginManager::unregisterPlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = plugins_.find(name);
    if (it == plugins_.end()) return false;
    
    // If plugin is initialized, shutdown first
    if (it->second->isInitialized()) {
        it->second->shutdown();
    }
    
    plugins_.erase(it);
    metadata_.erase(name);
    dependencies_.erase(name);
    pluginConfigs_.erase(name);
    
    return true;
}

bool PluginManager::initializePlugin(const std::string& name,
                                    const std::unordered_map<std::string, std::string>& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = plugins_.find(name);
    if (it == plugins_.end()) return false;
    
    // Check dependencies
    if (!validatePluginDependencies(name)) return false;
    
    // Merge configs (global + plugin-specific)
    std::unordered_map<std::string, std::string> mergedConfig = globalConfig_;
    auto pluginConfigIt = pluginConfigs_.find(name);
    if (pluginConfigIt != pluginConfigs_.end()) {
        mergedConfig = PluginConfig::mergeConfigs(mergedConfig, pluginConfigIt->second);
    }
    mergedConfig = PluginConfig::mergeConfigs(mergedConfig, config);
    
    // Initialize plugin
    bool success = it->second->initialize(mergedConfig);
    if (success) {
        // Notify dependencies that this plugin is now available
        notifyDependencies(name, true);
    }
    
    return success;
}

bool PluginManager::shutdownPlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = plugins_.find(name);
    if (it == plugins_.end()) return false;
    
    // Shutdown plugin
    it->second->shutdown();
    
    // Notify dependents that this plugin is no longer available
    notifyDependencies(name, false);
    
    return true;
}

bool PluginManager::activatePlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = plugins_.find(name);
    if (it == plugins_.end()) return false;
    
    it->second->setActive(true);
    return true;
}

bool PluginManager::deactivatePlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = plugins_.find(name);
    if (it == plugins_.end()) return false;
    
    it->second->setActive(false);
    return true;
}

void PluginManager::setGlobalConfig(const std::unordered_map<std::string, std::string>& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    globalConfig_ = config;
}

std::unordered_map<std::string, std::string> PluginManager::getGlobalConfig() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return globalConfig_;
}

void PluginManager::mergeGlobalConfig(const std::unordered_map<std::string, std::string>& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    globalConfig_ = PluginConfig::mergeConfigs(globalConfig_, config);
}

bool PluginManager::hasPlugin(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return plugins_.find(name) != plugins_.end();
}

std::vector<std::string> PluginManager::getPluginNames() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> names;
    names.reserve(plugins_.size());
    
    for (const auto& pair : plugins_) {
        names.push_back(pair.first);
    }
    
    return names;
}

PluginMetadata PluginManager::getPluginMetadata(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = metadata_.find(name);
    return it != metadata_.end() ? it->second : PluginMetadata{};
}

std::unique_ptr<PluginBase> PluginManager::getPlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = plugins_.find(name);
    if (it == plugins_.end()) return nullptr;
    
    return it->second->clone();
}

std::vector<PluginManager::PluginStats> PluginManager::getAllStats() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<PluginStats> stats;
    
    for (const auto& pair : plugins_) {
        PluginStats pluginStats;
        pluginStats.name = pair.first;
        pluginStats.version = pair.second->getVersion();
        pluginStats.neuronCount = pair.second->getNeuronCount();
        pluginStats.synapseCount = pair.second->getSynapseCount();
        pluginStats.memoryUsage = pair.second->getMemoryUsage();
        pluginStats.isActive = pair.second->isActive();
        pluginStats.isInitialized = pair.second->isInitialized();
        pluginStats.lastActivity = std::chrono::system_clock::now();
        
        stats.push_back(pluginStats);
    }
    
    return stats;
}

PluginManager::PluginStats PluginManager::getPluginStats(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    PluginStats stats;
    
    auto it = plugins_.find(name);
    if (it != plugins_.end()) {
        stats.name = name;
        stats.version = it->second->getVersion();
        stats.neuronCount = it->second->getNeuronCount();
        stats.synapseCount = it->second->getSynapseCount();
        stats.memoryUsage = it->second->getMemoryUsage();
        stats.isActive = it->second->isActive();
        stats.isInitialized = it->second->isInitialized();
        stats.lastActivity = std::chrono::system_clock::now();
    }
    
    return stats;
}

void PluginManager::initializeAll(const std::unordered_map<std::string, std::string>& globalConfig) {
    setGlobalConfig(globalConfig);
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (auto& pair : plugins_) {
        if (validatePluginDependencies(pair.first)) {
            initializePlugin(pair.first, {});
        }
    }
}

void PluginManager::shutdownAll() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (auto& pair : plugins_) {
        if (pair.second->isInitialized()) {
            shutdownPlugin(pair.first);
        }
    }
}

void PluginManager::updateAll(double timestep) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto currentTime = std::chrono::system_clock::now();
    
    for (auto& pair : plugins_) {
        if (pair.second->isActive() && pair.second->isInitialized()) {
            auto updateStart = std::chrono::system_clock::now();
            pair.second->update(timestep);
            auto updateEnd = std::chrono::system_clock::now();
            
            auto updateDuration = std::chrono::duration_cast<std::chrono::duration<double>>(updateEnd - updateStart);
            
            // Update average update time
            if (updateCount_ > 0) {
                averageUpdateTime_ = (averageUpdateTime_ * (updateCount_ - 1) + updateDuration.count()) / updateCount_;
            } else {
                averageUpdateTime_ = updateDuration.count();
            }
            
            updateCount_++;
            pair.second->setLastActivity(currentTime);
        }
    }
    
    lastUpdateTime_ = currentTime;
    
    if (monitoringEnabled_) {
        auto totalTime = std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - monitoringStartTime_);
        if (totalTime.count() > 0.0) {
            fps_ = updateCount_ / totalTime.count();
        }
    }
}

void PluginManager::resetAll() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (auto& pair : plugins_) {
        if (pair.second->isInitialized()) {
            pair.second->reset();
        }
    }
}

void PluginManager::startMonitoring() {
    monitoringEnabled_ = true;
    monitoringStartTime_ = std::chrono::system_clock::now();
    lastUpdateTime_ = monitoringStartTime_;
    updateCount_ = 0;
}

void PluginManager::stopMonitoring() {
    monitoringEnabled_ = false;
}

bool PluginManager::isMonitoring() const {
    return monitoringEnabled_;
}

double PluginManager::getFPS() const {
    return fps_;
}

double PluginManager::getAverageUpdateTime() const {
    return averageUpdateTime_;
}

bool PluginManager::loadConfigFromFile(const std::string& filepath) {
    std::unordered_map<std::string, std::string> config;
    if (!PluginConfig::loadFromFile(filepath, config)) {
        return false;
    }
    
    setGlobalConfig(config);
    return true;
}

bool PluginManager::saveConfigToFile(const std::string& filepath) {
    return PluginConfig::saveToFile(filepath, getGlobalConfig());
}

bool PluginManager::loadPluginConfigFromFile(const std::string& pluginName, const std::string& filepath) {
    std::unordered_map<std::string, std::string> config;
    if (!PluginConfig::loadFromFile(filepath, config)) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    pluginConfigs_[pluginName] = config;
    
    // If plugin is initialized, apply config
    if (hasPlugin(pluginName) && plugins_[pluginName]->isInitialized()) {
        initializePlugin(pluginName, config);
    }
    
    return true;
}

bool PluginManager::savePluginConfigToFile(const std::string& pluginName, const std::string& filepath) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = pluginConfigs_.find(pluginName);
    if (it == pluginConfigs_.end()) return false;
    
    return PluginConfig::saveToFile(filepath, it->second);
}

bool PluginManager::addPluginDependency(const std::string& pluginName, const std::string& dependencyName) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!hasPlugin(pluginName) || !hasPlugin(dependencyName)) {
        return false;
    }
    
    dependencies_[pluginName].push_back(dependencyName);
    return true;
}

bool PluginManager::removePluginDependency(const std::string& pluginName, const std::string& dependencyName) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = dependencies_.find(pluginName);
    if (it == dependencies_.end()) return false;
    
    auto depIt = std::find(it->second.begin(), it->second.end(), dependencyName);
    if (depIt == it->second.end()) return false;
    
    it->second.erase(depIt);
    return true;
}

std::vector<std::string> PluginManager::getPluginDependencies(const std::string& pluginName) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = dependencies_.find(pluginName);
    if (it == dependencies_.end()) return {};
    
    return it->second;
}

bool PluginManager::canLoadPlugin(const std::string& pluginName) const {
    return validatePluginDependencies(pluginName);
}

void PluginManager::registerEventCallback(const std::string& eventType, const PluginEventCallback& callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    eventCallbacks_[eventType].push_back(callback);
}

void PluginManager::unregisterEventCallback(const std::string& eventType, const PluginEventCallback& callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = eventCallbacks_.find(eventType);
    if (it != eventCallbacks_.end()) {
        auto callbackIt = std::find(it->second.begin(), it->second.end(), callback);
        if (callbackIt != it->second.end()) {
            it->second.erase(callbackIt);
        }
    }
}

void PluginManager::triggerEvent(const std::string& pluginName, const std::string& eventType,
                                 const std::unordered_map<std::string, std::string>& data) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = eventCallbacks_.find(eventType);
    if (it != eventCallbacks_.end()) {
        for (const auto& callback : it->second) {
            callback(pluginName, eventType, data);
        }
    }
}

PluginManager::SystemStats PluginManager::getSystemStats() const {
    SystemStats stats;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    stats.totalPlugins = plugins_.size();
    stats.activePlugins = 0;
    stats.initializedPlugins = 0;
    stats.totalMemoryUsage = 0.0f;
    
    for (const auto& pair : plugins_) {
        if (pair.second->isActive()) stats.activePlugins++;
        if (pair.second->isInitialized()) stats.initializedPlugins++;
        stats.totalMemoryUsage += pair.second->getMemoryUsage();
    }
    
    if (monitoringEnabled_) {
        auto currentTime = std::chrono::system_clock::now();
        auto totalTime = std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - monitoringStartTime_);
        
        stats.totalSimulationTime = totalTime.count();
        stats.averageUpdateTime = averageUpdateTime_;
        stats.fps = fps_;
        stats.startTime = monitoringStartTime_;
        stats.lastUpdateTime = lastUpdateTime_;
        stats.totalUpdates = updateCount_;
    }
    
    return stats;
}

bool PluginManager::validatePluginDependencies(const std::string& pluginName) const {
    auto it = dependencies_.find(pluginName);
    if (it == dependencies_.end()) return true;
    
    for (const auto& dependency : it->second) {
        if (!hasPlugin(dependency) || !plugins_.at(dependency)->isInitialized()) {
            return false;
        }
    }
    
    return true;
}

void PluginManager::notifyDependencies(const std::string& pluginName, bool isAvailable) {
    for (auto& pair : plugins_) {
        auto it = dependencies_.find(pair.first);
        if (it != dependencies_.end()) {
            auto depIt = std::find(it->second.begin(), it->second.end(), pluginName);
            if (depIt != it->second.end()) {
                // Trigger dependency event
                std::unordered_map<std::string, std::string> data;
                data["dependency_name"] = pluginName;
                data["is_available"] = isAvailable ? "true" : "false";
                
                triggerEvent(pair.first, "dependency_changed", data);
            }
        }
    }
}

void PluginManager::cleanupUnreachablePlugins() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<std::string> toRemove;
    
    for (const auto& pair : plugins_) {
        if (!pair.second->isActive() && pair.second->isInitialized()) {
            toRemove.push_back(pair.first);
        }
    }
    
    for (const auto& name : toRemove) {
        shutdownPlugin(name);
        plugins_.erase(name);
        metadata_.erase(name);
        dependencies_.erase(name);
        pluginConfigs_.erase(name);
    }
}

} // namespace nlm