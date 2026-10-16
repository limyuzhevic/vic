/*
 * Plugin Manager Implementation
 * 
 * This file contains the implementation of the PluginManager class.
 * It manages the loading, unloading, and lifecycle of plugins.
 * It also handles plugin discovery, dependency resolution, and configuration integration.
 */

#include "PluginManager.hpp"
#include "../core/Config/Config.hpp"
#include "../../brain/Neuron.hpp"
#include "../../plasticity/PlasticityRule.hpp"
#include "../../neuromodulation/Neuromodulator.hpp"
#include "../../memory/WorkingMemory.hpp"
#include <dlfcn.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <stdexcept>
#include <chrono>
#include <thread>

namespace nlm {
namespace plugin {

// DynamicLibraryLoader implementation
class DynamicLibraryLoader {
public:
    DynamicLibraryLoader() : handle_(nullptr) {}
    ~DynamicLibraryLoader() { unload(); }
    
    bool load(const std::string& path) {
        if (handle_) unload();
        
        handle_ = dlopen(path.c_str(), RTLD_LAZY | RTLD_LOCAL);
        if (!handle_) {
            std::string error = dlerror();
            return false;
        }
        return true;
    }
    
    void* getSymbol(const std::string& symbol) {
        if (!handle_) return nullptr;
        dlerror(); // Clear any existing error
        void* symbolPtr = dlsym(handle_, symbol.c_str());
        if (dlerror()) return nullptr;
        return symbolPtr;
    }
    
    void unload() {
        if (handle_) {
            dlclose(handle_);
            handle_ = nullptr;
        }
    }
    
    bool isLoaded() const { return handle_ != nullptr; }
    
    const std::string& getPath() const { return path_; }
    
private:
    void* handle_;
    std::string path_;
    
    friend class PluginManager;
};

// PluginManager implementation
PluginManager::PluginManager(const PluginManagerConfig& config)
    : config_(config)
    , activeLoadCount_(0)
    , running_(false)
    , totalLoadAttempts_(0)
    , successfulLoads_(0)
    , failedLoads_(0)
    , lastConfigCheck_(std::chrono::system_clock::now()) {
    
    // Initialize global config if not already set
    if (!globalConfig_) {
        globalConfig_ = std::make_unique<nlm::Config>();
        globalConfig_->loadFromFile("config.json"); // Default config file
    }
    
    // Start manager if configured to do so
    if (config_.enableAutoLoad) {
        start();
    }
}

PluginManager::~PluginManager() {
    stop();
    unloadAllPlugins();
}

bool PluginManager::loadPlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    
    totalLoadAttempts_++;
    
    // Check if plugin already exists
    if (plugins_.find(name) != plugins_.end()) {
        addError("PluginManager: Plugin '" + name + "' already exists");
        return false;
    }
    
    // Try to load from registry
    auto it = pluginsRegistry_.find(name);
    if (it == pluginsRegistry_.end()) {
        addError("PluginManager: Plugin '" + name + "' not found in registry");
        failedLoads_++;
        return false;
    }
    
    const PluginRegistryEntry& registryEntry = it->second;
    return loadPluginInternal(name, registryEntry);
}

bool PluginManager::loadPlugin(const std::string& name, const std::string& path) {
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    
    totalLoadAttempts_++;
    
    // Check if plugin already exists
    if (plugins_.find(name) != plugins_.end()) {
        addError("PluginManager: Plugin '" + name + "' already exists");
        return false;
    }
    
    PluginRegistryEntry registryEntry;
    registryEntry.path = path;
    registryEntry.enabled = true;
    
    return loadPluginInternal(name, registryEntry);
}

bool PluginManager::loadPlugin(const std::string& name, const PluginRegistryEntry& registryEntry) {
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    
    totalLoadAttempts_++;
    
    // Check if plugin already exists
    if (plugins_.find(name) != plugins_.end()) {
        addError("PluginManager: Plugin '" + name + "' already exists");
        return false;
    }
    
    return loadPluginInternal(name, registryEntry);
}

bool PluginManager::unloadPlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    
    auto it = plugins_.find(name);
    if (it == plugins_.end()) {
        addError("PluginManager: Plugin '" + name + "' not found");
        return false;
    }
    
    PluginInstance& pluginInstance = *it->second;
    
    // Check if plugin is running
    if (pluginInstance.status == PluginStatus::RUNNING) {
        if (!stopPluginInternal(name)) {
            addError("PluginManager: Failed to stop plugin '" + name + "' before unloading");
            return false;
        }
    }
    
    if (!unloadPluginInternal(name)) {
        addError("PluginManager: Failed to unload plugin '" + name + "'");
        return false;
    }
    
    onPluginUnloadedInternal(name);
    plugins_.erase(it);
    
    return true;
}

bool PluginManager::unloadAllPlugins() {
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    
    std::vector<std::string> pluginNames;
    for (const auto& pair : plugins_) {
        pluginNames.push_back(pair.first);
    }
    
    bool success = true;
    for (const std::string& name : pluginNames) {
        if (!unloadPlugin(name)) {
            success = false;
        }
    }
    
    return success;
}

bool PluginManager::enablePlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    
    auto it = plugins_.find(name);
    if (it == plugins_.end()) {
        addError("PluginManager: Plugin '" + name + "' not found");
        return false;
    }
    
    PluginInstance& pluginInstance = *it->second;
    
    if (pluginInstance.status == PluginStatus::LOADED || 
        pluginInstance.status == PluginStatus::FAILED) {
        pluginInstance.registryInfo.enabled = true;
    }
    
    return true;
}

bool PluginManager::disablePlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    
    auto it = plugins_.find(name);
    if (it == plugins_.end()) {
        addError("PluginManager: Plugin '" + name + "' not found");
        return false;
    }
    
    PluginInstance& pluginInstance = *it->second;
    
    if (pluginInstance.status == PluginStatus::RUNNING) {
        if (!stopPluginInternal(name)) {
            addError("PluginManager: Failed to stop plugin '" + name + "' before disabling");
            return false;
        }
    }
    
    if (pluginInstance.status == PluginStatus::LOADED) {
        // Unload the plugin if it's loaded but not running
        if (!unloadPluginInternal(name)) {
            addError("PluginManager: Failed to unload plugin '" + name + "' during disable");
            return false;
        }
    }
    
    pluginInstance.registryInfo.enabled = false;
    
    onPluginUnloadedInternal(name);
    plugins_.erase(it);
    
    return true;
}

bool PluginManager::isPluginEnabled(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    
    auto it = plugins_.find(name);
    if (it == plugins_.end()) return false;
    
    return it->second->registryInfo.enabled;
}

PluginStatus PluginManager::getPluginStatus(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    
    auto it = plugins_.find(name);
    if (it == plugins_.end()) return PluginStatus::UNLOADED;
    
    return it->second->status.load();
}

Plugin* PluginManager::getPlugin(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    
    auto it = plugins_.find(name);
    if (it == plugins_.end()) return nullptr;
    
    return it->second->plugin.get();
}

const PluginRegistryEntry& PluginManager::getPluginRegistryInfo(const std::string& name) const {
    static PluginRegistryEntry emptyEntry;
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    
    auto it = plugins_.find(name);
    if (it == plugins_.end()) return emptyEntry;
    
    return it->second->registryInfo;
}

std::vector<std::string> PluginManager::getPluginNames() const {
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    std::vector<std::string> names;
    names.reserve(plugins_.size());
    
    for (const auto& pair : plugins_) {
        names.push_back(pair.first);
    }
    
    return names;
}

std::vector<std::string> PluginManager::getLoadedPluginNames() const {
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    std::vector<std::string> names;
    
    for (const auto& pair : plugins_) {
        if (pair.second->status != PluginStatus::UNLOADED && 
            pair.second->status != PluginStatus::FAILED) {
            names.push_back(pair.first);
        }
    }
    
    return names;
}

std::vector<std::string> PluginManager::getPluginNamesByCategory(const std::string& category) const {
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    std::vector<std::string> names;
    
    for (const auto& pair : plugins_) {
        if (pair.second->registryInfo.category == category) {
            names.push_back(pair.first);
        }
    }
    
    return names;
}

bool PluginManager::scanPluginDirectory() {
    std::vector<std::string> discoveredPlugins;
    if (!scanDirectory(config_.pluginDirectory, discoveredPlugins)) {
        return false;
    }
    
    bool success = true;
    for (const std::string& pluginPath : discoveredPlugins) {
        PluginRegistryEntry registryEntry;
        if (loadPluginFromFile(pluginPath, registryEntry)) {
            std::string name = generatePluginNameFromPath(pluginPath);
            plugins_[name] = std::make_unique<PluginInstance>();
            plugins_[name]->registryInfo = registryEntry;
            plugins_[name]->status = PluginStatus::LOADED;
            
            // Auto-load if enabled
            if (config_.enableAutoLoad && registryEntry.enabled) {
                if (!resolveDependencies(name)) {
                    addError("PluginManager: Failed to resolve dependencies for plugin '" + name + "'");
                    success = false;
                    continue;
                }
                
                if (!initializePlugin(name)) {
                    addError("PluginManager: Failed to initialize plugin '" + name + "'");
                    success = false;
                    continue;
                }
                
                if (!startPluginInternal(name)) {
                    addError("PluginManager: Failed to start plugin '" + name + "'");
                    success = false;
                    continue;
                }
            }
        } else {
            success = false;
        }
    }
    
    return success;
}

bool PluginManager::loadPluginsFromRegistry(const std::string& registryFile) {
    std::ifstream file(registryFile);
    if (!file.is_open()) {
        addError("PluginManager: Could not open registry file '" + registryFile + "'");
        return false;
    }
    
    try {
        nlohmann::json registryData = nlohmann::json::parse(file);
        
        // Clear existing plugins
        plugins_.clear();
        
        // Load plugins from registry
        if (registryData.contains("plugins")) {
            for (const auto& pluginJson : registryData["plugins"]) {
                PluginRegistryEntry entry;
                entry.path = pluginJson["path"];
                entry.version = pluginJson["version"];
                entry.category = pluginJson["category"];
                entry.enabled = pluginJson["enabled"];
                entry.autoLoad = pluginJson["autoLoad"];
                
                if (pluginJson.contains("dependencies")) {
                    for (const auto& dep : pluginJson["dependencies"]) {
                        entry.dependencies.push_back(dep);
                    }
                }
                
                std::string name = pluginJson["name"];
                plugins_[name] = std::make_unique<PluginInstance>();
                plugins_[name]->registryInfo = entry;
                plugins_[name]->status = PluginStatus::LOADED;
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        addError("PluginManager: Failed to parse registry file '" + registryFile + "': " + e.what());
        return false;
    }
}

bool PluginManager::savePluginsToRegistry(const std::string& registryFile) const {
    std::ofstream file(registryFile);
    if (!file.is_open()) {
        addError("PluginManager: Could not open registry file for writing '" + registryFile + "'");
        return false;
    }
    
    nlohmann::json registryData;
    
    // Convert plugins to JSON
    nlohmann::json pluginsArray;
    for (const auto& pair : plugins_) {
        nlohmann::json pluginJson;
        pluginJson["name"] = pair.first;
        pluginJson["path"] = pair.second->registryInfo.path;
        pluginJson["version"] = pair.second->registryInfo.version;
        pluginJson["category"] = pair.second->registryInfo.category;
        pluginJson["enabled"] = pair.second->registryInfo.enabled;
        pluginJson["autoLoad"] = pair.second->registryInfo.autoLoad;
        
        // Add dependencies
        if (!pair.second->registryInfo.dependencies.empty()) {
            nlohmann::json depsArray;
            for (const auto& dep : pair.second->registryInfo.dependencies) {
                depsArray.push_back(dep);
            }
            pluginJson["dependencies"] = depsArray;
        }
        
        pluginsArray.push_back(pluginJson);
    }
    
    registryData["plugins"] = pluginsArray;
    
    try {
        file << registryData.dump(2);
        return true;
    } catch (const std::exception& e) {
        addError("PluginManager: Failed to write registry file '" + registryFile + "': " + e.what());
        return false;
    }
}

bool PluginManager::updatePluginRegistry() {
    bool success = true;
    
    // Check for modified plugins
    for (auto& pair : plugins_) {
        PluginInstance& pluginInstance = *pair.second;
        
        if (std::filesystem::exists(pluginInstance.registryInfo.path)) {
            auto lastModified = std::filesystem::last_write_time(
                pluginInstance.registryInfo.path);
            
            // Convert to system_clock time_point for comparison
            auto fsTime = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                lastModified - std::filesystem::file_time_type::clock::now() + 
                std::chrono::system_clock::now());
            
            if (fsTime > pluginInstance.registryInfo.lastModified) {
                // Plugin file has been modified
                if (pluginInstance.status == PluginStatus::RUNNING) {
                    // Stop, unload, and reload modified plugin
                    if (!stopPluginInternal(pair.first)) {
                        addError("PluginManager: Failed to stop plugin '" + pair.first + "' after modification");
                        success = false;
                        continue;
                    }
                }
                
                if (!unloadPluginInternal(pair.first)) {
                    addError("PluginManager: Failed to unload plugin '" + pair.first + "' after modification");
                    success = false;
                    continue;
                }
                
                if (!loadPluginInternal(pair.first, pluginInstance.registryInfo)) {
                    addError("PluginManager: Failed to reload plugin '" + pair.first + "' after modification");
                    success = false;
                }
            }
        }
    }
    
    return success;
}

bool PluginManager::loadPluginsFromConfig() {
    if (!globalConfig_) {
        return false;
    }
    
    std::vector<std::string> pluginsToLoad;
    
    // Get list of enabled plugins from config
    if (globalConfig_->has("plugins.enabled")) {
        auto pluginNames = globalConfig_->get<std::vector<std::string>>("plugins.enabled");
        pluginsToLoad = pluginNames;
    } else {
        addError("PluginManager: No enabled plugins found in config");
        return false;
    }
    
    bool success = true;
    for (const std::string& pluginName : pluginsToLoad) {
        if (!pluginExists(pluginName)) {
            addError("PluginManager: Plugin '" + pluginName + "' not found in registry");
            success = false;
            continue;
        }
        
        if (!isPluginLoaded(pluginName)) {
            if (!loadPlugin(pluginName)) {
                addError("PluginManager: Failed to load plugin '" + pluginName + "' from config");
                success = false;
            }
        }
    }
    
    return success;
}

bool PluginManager::updatePluginConfig(const std::string& name, const nlohmann::json& config) {
    auto plugin = getPlugin(name);
    if (!plugin) {
        addError("PluginManager: Plugin '" + name + "' not found");
        return false;
    }
    
    return plugin->validateConfig(config);
}

nlohmann::json PluginManager::getPluginConfig(const std::string& name) const {
    auto plugin = getPlugin(name);
    if (!plugin) {
        return nlohmann::json();
    }
    
    return plugin->getDefaultConfig();
}

nlohmann::json PluginManager::getAllPluginConfigs() const {
    nlohmann::json configs;
    
    for (const auto& pair : plugins_) {
        if (pair.second->status != PluginStatus::UNLOADED && 
            pair.second->status != PluginStatus::FAILED) {
            configs[pair.first] = getPluginConfig(pair.first);
        }
    }
    
    return configs;
}

bool PluginManager::resolveDependencies(const std::string& pluginName) {
    std::lock_guard<std::mutex> lock(dependencyMutex_);
    
    auto it = plugins_.find(pluginName);
    if (it == plugins_.end()) {
        addError("PluginManager: Plugin '" + pluginName + "' not found");
        return false;
    }
    
    PluginInstance& pluginInstance = *it->second;
    
    // Resolve dependencies recursively
    for (const std::string& dependency : pluginInstance.registryInfo.dependencies) {
        auto depIt = plugins_.find(dependency);
        if (depIt == plugins_.end()) {
            addError("PluginManager: Dependency plugin '" + dependency + "' not found for plugin '" + pluginName + "'");
            return false;
        }
        
        PluginInstance& depInstance = *depIt->second;
        
        if (depInstance.status != PluginStatus::LOADED && 
            depInstance.status != PluginStatus::RUNNING) {
            addError("PluginManager: Dependency plugin '" + dependency + "' for plugin '" + pluginName + "' is not loaded");
            return false;
        }
    }
    
    return true;
}

std::vector<std::string> PluginManager::getPluginDependencies(const std::string& name) const {
    std::lock_guard<std::mutex> lock(dependencyMutex_);
    
    auto it = plugins_.find(name);
    if (it == plugins_.end()) return std::vector<std::string>();
    
    return it->second->registryInfo.dependencies;
}

std::vector<std::string> PluginManager::getDependentPlugins(const std::string& name) const {
    std::lock_guard<std::mutex> lock(dependencyMutex_);
    
    std::vector<std::string> dependents;
    
    for (const auto& pair : plugins_) {
        for (const std::string& dep : pair.second->registryInfo.dependencies) {
            if (dep == name) {
                dependents.push_back(pair.first);
            }
        }
    }
    
    return dependents;
}

bool PluginManager::isDependencySatisfied(const std::string& pluginName) const {
    auto dependencies = getPluginDependencies(pluginName);
    
    for (const std::string& dependency : dependencies) {
        auto it = plugins_.find(dependency);
        if (it == plugins_.end()) return false;
        
        PluginStatus status = it->second->status.load();
        if (status != PluginStatus::LOADED && status != PluginStatus::RUNNING) {
            return false;
        }
    }
    
    return true;
}

bool PluginManager::startPlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    
    return startPluginInternal(name);
}

bool PluginManager::stopPlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    
    return stopPluginInternal(name);
}

bool PluginManager::restartPlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    
    if (!stopPluginInternal(name)) {
        addError("PluginManager: Failed to stop plugin '" + name + "' during restart");
        return false;
    }
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Brief pause
    
    if (!startPluginInternal(name)) {
        addError("PluginManager: Failed to restart plugin '" + name + "'");
        return false;
    }
    
    return true;
}

bool PluginManager::startAllPlugins() {
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    
    bool success = true;
    for (auto& pair : plugins_) {
        if (pair.second->registryInfo.enabled && 
            pair.second->status == PluginStatus::LOADED) {
            if (!startPluginInternal(pair.first)) {
                addError("PluginManager: Failed to start plugin '" + pair.first + "'");
                success = false;
            }
        }
    }
    
    return success;
}

bool PluginManager::stopAllPlugins() {
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    
    bool success = true;
    for (auto& pair : plugins_) {
        if (pair.second->status == PluginStatus::RUNNING) {
            if (!stopPluginInternal(pair.first)) {
                addError("PluginManager: Failed to stop plugin '" + pair.first + "'");
                success = false;
            }
        }
    }
    
    return success;
}

void PluginManager::lock() const {
    pluginsMutex_.lock();
}

void PluginManager::unlock() const {
    pluginsMutex_.unlock();
}

bool PluginManager::tryLock() const {
    return pluginsMutex_.try_lock();
}

void PluginManager::setPluginLoadedCallback(std::function<void(const std::string&)> callback) {
    onPluginLoaded_ = std::move(callback);
}

void PluginManager::setPluginUnloadedCallback(std::function<void(const std::string&)> callback) {
    onPluginUnloaded_ = std::move(callback);
}

void PluginManager::setPluginStatusChangedCallback(std::function<void(const std::string&, PluginStatus)> callback) {
    onPluginStatusChanged_ = std::move(callback);
}

void PluginManager::setPluginErrorCallback(std::function<void(const std::string&, const std::string&)> callback) {
    onPluginError_ = std::move(callback);
}

bool PluginManager::pluginExists(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    return plugins_.find(name) != plugins_.end();
}

bool PluginManager::isPluginLoaded(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    
    auto it = plugins_.find(name);
    if (it == plugins_.end()) return false;
    
    PluginStatus status = it->second->status.load();
    return status != PluginStatus::UNLOADED && status != PluginStatus::FAILED;
}

nlohmann::json PluginManager::getPluginInfo(const std::string& name) const {
    nlohmann::json info;
    
    auto it = plugins_.find(name);
    if (it == plugins_.end()) return info;
    
    const PluginInstance& pluginInstance = *it->second;
    
    info["name"] = name;
    info["status"] = static_cast<int>(pluginInstance.status);
    info["enabled"] = pluginInstance.registryInfo.enabled;
    info["version"] = pluginInstance.registryInfo.version;
    info["category"] = pluginInstance.registryInfo.category;
    info["path"] = pluginInstance.registryInfo.path;
    info["dependencies"] = pluginInstance.registryInfo.dependencies;
    info["loadTime"] = std::chrono::duration_cast<std::chrono::seconds>(
        pluginInstance.loadTime.time_since_epoch()).count();
    
    return info;
}

std::vector<nlohmann::json> PluginManager::getAllPluginInfos() const {
    std::vector<nlohmann::json> infos;
    
    for (const auto& pair : plugins_) {
        infos.push_back(getPluginInfo(pair.first));
    }
    
    return infos;
}

void PluginManager::setGlobalConfig(std::unique_ptr<nlm::Config> config) {
    globalConfig_ = std::move(config);
}

std::unique_ptr<nlm::Config>& PluginManager::getGlobalConfig() {
    return globalConfig_;
}

bool PluginManager::start() {
    if (running_) {
        return false;
    }
    
    running_ = true;
    startTime_ = std::chrono::system_clock::now();
    
    // Start config watcher thread
    configWatcherThread_ = std::thread(&PluginManager::configWatcherLoop, this);
    
    return true;
}

bool PluginManager::stop() {
    if (!running_) {
        return false;
    }
    
    running_ = false;
    loadCondition_.notify_all();
    
    if (configWatcherThread_.joinable()) {
        configWatcherThread_.join();
    }
    
    return true;
}

bool PluginManager::isRunning() const {
    return running_;
}

std::string PluginManager::getLastError(const std::string& pluginName) const {
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    
    if (allErrors_.empty()) return "";
    
    if (!pluginName.empty()) {
        // Find errors related to specific plugin
        for (const auto& error : allErrors_) {
            if (error.find(pluginName) != std::string::npos) {
                return error;
            }
        }
        return "";
    }
    
    return allErrors_.back();
}

std::vector<std::string> PluginManager::getAllErrors() const {
    return allErrors_;
}

size_t PluginManager::getLoadedPluginCount() const {
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    size_t count = 0;
    
    for (const auto& pair : plugins_) {
        PluginStatus status = pair.second->status.load();
        if (status != PluginStatus::UNLOADED && status != PluginStatus::FAILED) {
            count++;
        }
    }
    
    return count;
}

size_t PluginManager::getTotalPluginCount() const {
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    return plugins_.size();
}

float PluginManager::getPluginLoadProgress() const {
    if (totalLoadAttempts_ == 0) return 0.0f;
    return static_cast<float>(successfulLoads_) / static_cast<float>(totalLoadAttempts_);
}

nlohmann::json PluginManager::getManagerStats() const {
    nlohmann::json stats;
    
    stats["loadedPluginCount"] = getLoadedPluginCount();
    stats["totalPluginCount"] = getTotalPluginCount();
    stats["loadProgress"] = getPluginLoadProgress();
    stats["successfulLoads"] = successfulLoads_;
    stats["failedLoads"] = failedLoads_;
    stats["totalLoadAttempts"] = totalLoadAttempts_;
    stats["running"] = isRunning();
    stats["threadSafe"] = config_.threadSafe;
    
    return stats;
}

// Private implementation methods
bool PluginManager::loadPluginInternal(const std::string& name, const PluginRegistryEntry& registryEntry) {
    auto it = plugins_.find(name);
    if (it == plugins_.end()) return false;
    
    PluginInstance& pluginInstance = *it->second;
    
    if (pluginInstance.status != PluginStatus::UNLOADED && 
        pluginInstance.status != PluginStatus::FAILED) {
        addError("PluginManager: Plugin '" + name + "' already loaded");
        return false;
    }
    
    pluginInstance.status = PluginStatus::LOADING;
    triggerEventHandlers(name, PluginStatus::LOADING);
    
    // Create library loader
    pluginInstance.loader = std::make_shared<DynamicLibraryLoader>();
    
    // Load library
    if (!pluginInstance.loader->load(registryEntry.path)) {
        addError("PluginManager: Failed to load plugin library '" + registryEntry.path + "'");
        pluginInstance.status = PluginStatus::FAILED;
        triggerEventHandlers(name, PluginStatus::FAILED);
        failedLoads_++;
        return false;
    }
    
    pluginInstance.libraryPath = registryEntry.path;
    
    // Get plugin factory function
    void* factorySymbol = pluginInstance.loader->getSymbol("createPlugin");
    if (!factorySymbol) {
        addError("PluginManager: Plugin factory symbol not found in library '" + registryEntry.path + "'");
        pluginInstance.status = PluginStatus::FAILED;
        triggerEventHandlers(name, PluginStatus::FAILED);
        failedLoads_++;
        return false;
    }
    
    // Create plugin instance using factory
    typedef Plugin*(*CreatePluginFunc)();
    CreatePluginFunc createPlugin = reinterpret_cast<CreatePluginFunc>(factorySymbol);
    
    std::unique_ptr<Plugin> plugin = std::unique_ptr<Plugin>(createPlugin());
    if (!plugin) {
        addError("PluginManager: Failed to create plugin instance from factory");
        pluginInstance.status = PluginStatus::FAILED;
        triggerEventHandlers(name, PluginStatus::FAILED);
        failedLoads_++;
        return false;
    }
    
    // Validate plugin
    if (!validatePlugin(plugin.get())) {
        addError("PluginManager: Plugin validation failed for plugin '" + name + "'");
        pluginInstance.status = PluginStatus::FAILED;
        triggerEventHandlers(name, PluginStatus::FAILED);
        failedLoads_++;
        return false;
    }
    
    // Set metadata
    pluginInstance.plugin = std::move(plugin);
    pluginInstance.registryInfo = registryEntry;
    pluginInstance.loadTime = std::chrono::system_clock::now();
    
    pluginInstance.status = PluginStatus::LOADED;
    
    if (!initializePlugin(name)) {
        addError("PluginManager: Failed to initialize plugin '" + name + "'");
        pluginInstance.status = PluginStatus::FAILED;
        triggerEventHandlers(name, PluginStatus::FAILED);
        failedLoads_++;
        return false;
    }
    
    // Start plugin if auto-start is enabled
    if (config_.enableAutoStart) {
        if (!startPluginInternal(name)) {
            addError("PluginManager: Failed to auto-start plugin '" + name + "'");
            pluginInstance.status = PluginStatus::FAILED;
            triggerEventHandlers(name, PluginStatus::FAILED);
            failedLoads_++;
            return false;
        }
    }
    
    successfulLoads_++;
    
    onPluginLoadedInternal(name);
    triggerEventHandlers(name, PluginStatus::LOADED);
    
    return true;
}

bool PluginManager::createPluginInstance(const std::string& name, const std::string& path) {
    // This method is now handled in loadPluginInternal
    return false;
}

bool PluginManager::initializePlugin(const std::string& name) {
    auto it = plugins_.find(name);
    if (it == plugins_.end()) return false;
    
    PluginInstance& pluginInstance = *it->second;
    
    if (pluginInstance.status != PluginStatus::LOADED) {
        addError("PluginManager: Plugin '" + name + "' not in LOADED state");
        return false;
    }
    
    pluginInstance.status = PluginStatus::INITIALIZING;
    triggerEventHandlers(name, PluginStatus::INITIALIZING);
    
    // Initialize plugin
    bool success = pluginInstance.plugin->initialize();
    
    if (!success) {
        addError("PluginManager: Plugin initialization failed for plugin '" + name + "'");
        pluginInstance.status = PluginStatus::FAILED;
        triggerEventHandlers(name, PluginStatus::FAILED);
        return false;
    }
    
    pluginInstance.status = PluginStatus::RUNNING;
    triggerEventHandlers(name, PluginStatus::RUNNING);
    
    return true;
}

bool PluginManager::startPluginInternal(const std::string& name) {
    auto it = plugins_.find(name);
    if (it == plugins_.end()) return false;
    
    PluginInstance& pluginInstance = *it->second;
    
    if (pluginInstance.status != PluginStatus::RUNNING) {
        addError("PluginManager: Plugin '" + name + "' not in RUNNING state");
        return false;
    }
    
    return pluginInstance.plugin->run();
}

bool PluginManager::stopPluginInternal(const std::string& name) {
    auto it = plugins_.find(name);
    if (it == plugins_.end()) return false;
    
    PluginInstance& pluginInstance = *it->second;
    
    if (pluginInstance.status != PluginStatus::RUNNING) {
        return true; // Already stopped
    }
    
    // Stop plugin
    bool success = pluginInstance.plugin->shutdown();
    
    if (!success) {
        addError("PluginManager: Plugin shutdown failed for plugin '" + name + "'");
        return false;
    }
    
    pluginInstance.status = PluginStatus::RUNNING; // Brief pause in running state during shutdown
    return true;
}

bool PluginManager::shutdownPlugin(const std::string& name) {
    // This is handled by the plugin itself
    return false;
}

bool PluginManager::unloadPluginInternal(const std::string& name) {
    auto it = plugins_.find(name);
    if (it == plugins_.end()) return false;
    
    PluginInstance& pluginInstance = *it->second;
    
    if (pluginInstance.status != PluginStatus::LOADED && 
        pluginInstance.status != PluginStatus::RUNNING && 
        pluginInstance.status != PluginStatus::FAILED) {
        return true; // Already unloaded
    }
    
    // Unload plugin
    bool success = pluginInstance.plugin->unload();
    
    // Clean up library loader
    if (pluginInstance.loader) {
        pluginInstance.loader.reset();
    }
    
    // Reset plugin state
    pluginInstance.plugin.reset();
    pluginInstance.libraryPath.clear();
    pluginInstance.loadTime = std::chrono::system_clock::now();
    
    pluginInstance.status = PluginStatus::UNLOADED;
    
    return success;
}

bool PluginManager::scanDirectory(const std::string& directory, 
                                 std::vector<std::string>& discoveredPlugins) {
    if (!std::filesystem::exists(directory) || 
        !std::filesystem::is_directory(directory)) {
        addError("PluginManager: Plugin directory '" + directory + "' does not exist or is not a directory");
        return false;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().filename().string();
            
            // Check if file matches naming convention
            bool matchesConvention = false;
            if (config_.pluginNamingConvention == "*.so") {
                matchesConvention = (filename.size() >= 3 && 
                                    filename.substr(filename.size() - 3) == ".so");
            } else if (config_.pluginNamingConvention == "*.dylib") {
                matchesConvention = (filename.size() >= 5 && 
                                    filename.substr(filename.size() - 5) == ".dylib");
            } else if (config_.pluginNamingConvention == "*.dll") {
                matchesConvention = (filename.size() >= 4 && 
                                    filename.substr(filename.size() - 4) == ".dll");
            } else {
                // Custom pattern matching
                matchesConvention = true; // For simplicity, match all files
            }
            
            if (matchesConvention) {
                discoveredPlugins.push_back(entry.path().string());
            }
        }
    }
    
    return !discoveredPlugins.empty();
}

bool PluginManager::loadPluginFromFile(const std::string& filepath, 
                                     PluginRegistryEntry& registryEntry) {
    // Parse plugin metadata from file (simplified)
    std::string filename = std::filesystem::path(filepath).filename().string();
    
    // Generate plugin name from filename
    size_t dotPos = filename.find_last_of('.');
    if (dotPos != std::string::npos) {
        registryEntry.path = filepath;
        registryEntry.version = "1.0.0"; // Default version
        registryEntry.category = "unknown"; // Default category
        
        // Check if file is disabled
        if (std::find(config_.disabledPlugins.begin(), 
                      config_.disabledPlugins.end(), 
                      filename) != config_.disabledPlugins.end()) {
            registryEntry.enabled = false;
        }
        
        return true;
    }
    
    return false;
}

std::string PluginManager::generatePluginNameFromPath(const std::string& filepath) const {
    std::string filename = std::filesystem::path(filepath).filename().string();
    
    // Remove file extension
    size_t dotPos = filename.find_last_of('.');
    if (dotPos != std::string::npos) {
        filename = filename.substr(0, dotPos);
    }
    
    // Convert to lowercase for consistency
    std::transform(filename.begin(), filename.end(), filename.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    
    return filename;
}

bool PluginManager::validatePlugin(const Plugin* plugin) const {
    if (!plugin) {
        return false;
    }
    
    // Basic validation
    if (plugin->getName().empty() || plugin->getVersion().empty()) {
        return false;
    }
    
    return true;
}

bool PluginManager::checkDependencies(const std::string& pluginName) const {
    // This is now handled in resolveDependencies
    return false;
}

void PluginManager::triggerEventHandlers(const std::string& pluginName, 
                                        PluginStatus status, 
                                        const std::string& error) {
    // Trigger status changed callback
    if (onPluginStatusChanged_) {
        onPluginStatusChanged_(pluginName, status);
    }
    
    // Trigger error callback if there's an error
    if (!error.empty() && onPluginError_) {
        onPluginError_(pluginName, error);
    }
}

void PluginManager::configWatcherLoop() {
    while (running_) {
        std::unique_lock<std::mutex> lock(pluginsMutex_);
        
        auto now = std::chrono::system_clock::now();
        auto timeSinceLastCheck = std::chrono::duration_cast<std::chrono::seconds>(
            now - lastConfigCheck_);
        
        if (timeSinceLastCheck >= config_.configCheckInterval) {
            lastConfigCheck_ = now;
            
            // Update plugin registry
            if (!updatePluginRegistry()) {
                addError("PluginManager: Failed to update plugin registry");
            }
            
            // Clean up inactive plugins
            cleanupInactivePlugins();
        }
        
        // Wait for next check
        loadCondition_.wait_for(lock, 
                               std::chrono::seconds(1),
                               [this]() { return !running_; });
    }
}

void PluginManager::cleanupInactivePlugins() {
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    
    std::vector<std::string> pluginsToRemove;
    
    for (const auto& pair : plugins_) {
        const PluginInstance& pluginInstance = *pair.second;
        
        // Remove plugins that have been inactive for too long
        auto timeSinceLastAccess = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now() - pluginInstance.lastAccess);
        
        if (timeSinceLastAccess.count() > 300 && // 5 minutes
            pluginInstance.status != PluginStatus::RUNNING) {
            pluginsToRemove.push_back(pair.first);
        }
    }
    
    for (const std::string& name : pluginsToRemove) {
        if (!unloadPlugin(name)) {
            addError("PluginManager: Failed to clean up inactive plugin '" + name + "'");
        }
    }
}

void PluginManager::onPluginLoadedInternal(const std::string& name) {
    if (onPluginLoaded_) {
        onPluginLoaded_(name);
    }
}

void PluginManager::onPluginUnloadedInternal(const std::string& name) {
    if (onPluginUnloaded_) {
        onPluginUnloaded_(name);
    }
}

void PluginManager::onPluginStatusChangedInternal(const std::string& name, PluginStatus status) {
    if (onPluginStatusChanged_) {
        onPluginStatusChanged_(name, status);
    }
}

void PluginManager::onPluginErrorInternal(const std::string& name, const std::string& error) {
    if (onPluginError_) {
        onPluginError_(name, error);
    }
}

void PluginManager::addError(const std::string& error) {
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    allErrors_.push_back(error);
    
    if (onPluginError_) {
        onPluginError_("manager", error);
    }
}

}}} // namespace nlm::plugin
