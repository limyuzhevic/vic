// PluginManager implementation

#include "PluginManager.hpp"
#include "core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <thread>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <filesystem>

namespace nlm {

struct PluginManager::Impl {
    struct PluginInfo {
        std::shared_ptr<IPlugin> plugin;
        PluginState state;
        std::map<std::string, std::string> config;
        std::string version;
        std::string description;
        PluginType type;
        double loadTime;
        double initTime;
        double execTime;
        size_t memoryUsage;
        std::vector<std::string> dependencies;
        std::vector<std::string> providedComponents;
        int securityLevel;
        bool loggingEnabled;
        std::vector<std::pair<std::string, double>> usageHistory;
        std::map<std::string, std::string> metadata;
        std::string lastError;
        std::chrono::system_clock::time_point lastActivity;
        std::thread::id threadId;
    };
    
    std::map<std::string, PluginInfo> plugins_;
    std::map<std::string, std::vector<PluginEventCallback>> eventListeners_;
    
    PluginInitCallback initCallback_;
    PluginShutdownCallback shutdownCallback_;
    PluginErrorCallback errorCallback_;
    
    bool loggingEnabled_;
    bool hotReloadingEnabled_;
    bool debugModeEnabled_;
    bool faultToleranceEnabled_;
    bool pluginIsolationEnabled_;
    
    mutable std::mutex mutex_;
    
    std::string pluginDirectory_;
    std::string configFilePath_;
    
    Impl() : loggingEnabled_(true), hotReloadingEnabled_(false), 
             debugModeEnabled_(false), faultToleranceEnabled_(false),
             pluginIsolationEnabled_(false) {
        pluginDirectory_ = "./plugins";
        if (!std::filesystem::exists(pluginDirectory_)) {
            std::filesystem::create_directories(pluginDirectory_);
        }
    }
};

PluginManager::PluginManager() : pImpl(std::make_unique<Impl>()) {
    // Initialize logging
    if (pImpl->loggingEnabled_) {
        Logger::getGlobal().info("PluginManager initialized");
    }
}

PluginManager::~PluginManager() {
    shutdownAllPlugins();
}

bool PluginManager::loadPlugin(const std::string& name) {
    return loadPlugin(name, {});
}

bool PluginManager::loadPlugin(const std::string& name, const std::map<std::string, std::string>& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    if (pImpl->plugins_.find(name) != pImpl->plugins_.end()) {
        if (pImpl->loggingEnabled_) {
            Logger::getGlobal().warning("Plugin already loaded: " + name);
        }
        return false;
    }
    
    if (pImpl->loggingEnabled_) {
        Logger::getGlobal().info("Loading plugin: " + name);
    }
    
    // Get plugin factory from registry
    auto factory = PluginRegistry::getInstance().getPluginFactory(name);
    if (!factory) {
        if (pImpl->loggingEnabled_) {
            Logger::getGlobal().error("Plugin factory not found: " + name);
        }
        return false;
    }
    
    // Create plugin instance
    auto plugin = factory();
    if (!plugin) {
        if (pImpl->loggingEnabled_) {
            Logger::getGlobal().error("Failed to create plugin instance: " + name);
        }
        return false;
    }
    
    // Create plugin info
    auto& pluginInfo = pImpl->plugins_[name];
    pluginInfo.plugin = std::move(plugin);
    pluginInfo.config = config;
    pluginInfo.state = PluginState::Loading;
    
    // Configure plugin
    if (!pImpl->plugins_[name].config.empty()) {
        pImpl->plugins_[name].plugin->setPluginConfig(config);
    }
    
    // Get plugin metadata
    pluginInfo.name = pluginInfo.plugin->getName();
    pluginInfo.version = pluginInfo.plugin->getVersion();
    pluginInfo.description = pluginInfo.plugin->getDescription();
    pluginInfo.type = pluginInfo.plugin->getType();
    pluginInfo.dependencies = pluginInfo.plugin->getRequiredComponents();
    pluginInfo.providedComponents = pluginInfo.plugin->getProvidedComponents();
    pluginInfo.securityLevel = 1; // Default security level
    pluginInfo.loggingEnabled = pImpl->loggingEnabled_;
    pluginInfo.lastError = "";
    pluginInfo.lastActivity = std::chrono::system_clock::now();
    
    // Check dependencies
    if (!checkPluginDependencies(name)) {
        pluginInfo.state = PluginState::Error;
        if (pImpl->loggingEnabled_) {
            Logger::getGlobal().error("Plugin dependencies not satisfied: " + name);
        }
        return false;
    }
    
    // Initialize plugin
    pluginInfo.state = PluginState::Initializing;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    bool initSuccess = pluginInfo.plugin->initialize(config);
    auto endTime = std::chrono::high_resolution_clock::now();
    pluginInfo.initTime = std::chrono::duration<double>(endTime - startTime).count();
    
    if (initSuccess) {
        pluginInfo.state = PluginState::Initialized;
        if (pImpl->loggingEnabled_) {
            Logger::getGlobal().info("Plugin loaded successfully: " + name);
        }
        
        // Call init callback if set
        if (initCallback_) {
            initCallback_(name);
        }
    } else {
        pluginInfo.state = PluginState::Error;
        if (pImpl->loggingEnabled_) {
            Logger::getGlobal().error("Plugin initialization failed: " + name);
        }
        return false;
    }
    
    return true;
}

bool PluginManager::unloadPlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    auto it = pImpl->plugins_.find(name);
    if (it == pImpl->plugins_.end()) {
        if (pImpl->loggingEnabled_) {
            Logger::getGlobal().warning("Plugin not loaded: " + name);
        }
        return false;
    }
    
    if (pImpl->loggingEnabled_) {
        Logger::getGlobal().info("Unloading plugin: " + name);
    }
    
    // Shutdown plugin
    auto& pluginInfo = it->second;
    if (pluginInfo.state == PluginState::Initialized) {
        pluginInfo.state = PluginState::ShuttingDown;
        pluginInfo.plugin->shutdown();
        pluginInfo.state = PluginState::NotLoaded;
        
        // Call shutdown callback if set
        if (shutdownCallback_) {
            shutdownCallback_(name);
        }
    }
    
    // Remove plugin
    pImpl->plugins_.erase(it);
    
    if (pImpl->loggingEnabled_) {
        Logger::getGlobal().info("Plugin unloaded: " + name);
    }
    
    return true;
}

bool PluginManager::isPluginLoaded(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    return pImpl->plugins_.find(name) != pImpl->plugins_.end();
}

std::vector<std::string> PluginManager::getLoadedPlugins() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    std::vector<std::string> names;
    for (const auto& pair : pImpl->plugins_) {
        names.push_back(pair.first);
    }
    return names;
}

std::string PluginManager::getPluginName(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    auto it = pImpl->plugins_.find(name);
    if (it != pImpl->plugins_.end()) {
        return it->second.name;
    }
    return "";
}

std::string PluginManager::getPluginVersion(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    auto it = pImpl->plugins_.find(name);
    if (it != pImpl->plugins_.end()) {
        return it->second.version;
    }
    return "";
}

std::string PluginManager::getPluginDescription(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    auto it = pImpl->plugins_.find(name);
    if (it != pImpl->plugins_.end()) {
        return it->second.description;
    }
    return "";
}

PluginType PluginManager::getPluginType(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    auto it = pImpl->plugins_.find(name);
    if (it != pImpl->plugins_.end()) {
        return it->second.type;
    }
    return PluginType::Extension;
}

bool PluginManager::configurePlugin(const std::string& name, const std::map<std::string, std::string>& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    auto it = pImpl->plugins_.find(name);
    if (it == pImpl->plugins_.end()) {
        return false;
    }
    
    it->second.config = config;
    it->second.plugin->setPluginConfig(config);
    
    return true;
}

std::map<std::string, std::string> PluginManager::getPluginConfig(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    auto it = pImpl->plugins_.find(name);
    if (it != pImpl->plugins_.end()) {
        return it->second.config;
    }
    return {};
}

bool PluginManager::initializeAllPlugins(const std::map<std::string, std::map<std::string, std::string>>& configs) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    bool allSuccess = true;
    for (const auto& pair : pImpl->plugins_) {
        const std::string& name = pair.first;
        if (pair.second.state != PluginState::Initialized) {
            auto configIt = configs.find(name);
            std::map<std::string, std::string> config = (configIt != configs.end()) ? configIt->second : pair.second.config;
            
            if (!loadPlugin(name, config)) {
                allSuccess = false;
            }
        }
    }
    
    return allSuccess;
}

bool PluginManager::shutdownAllPlugins() {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    bool allSuccess = true;
    for (const auto& pair : pImpl->plugins_) {
        if (pair.second.state == PluginState::Initialized) {
            if (!unloadPlugin(pair.first)) {
                allSuccess = false;
            }
        }
    }
    
    return allSuccess;
}

bool PluginManager::isAllPluginsInitialized() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    for (const auto& pair : pImpl->plugins_) {
        if (pair.second.state != PluginState::Initialized) {
            return false;
        }
    }
    return !pImpl->plugins_.empty();
}

bool PluginManager::checkPluginDependencies(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    auto it = pImpl->plugins_.find(name);
    if (it == pImpl->plugins_.end()) {
        return false;
    }
    
    for (const std::string& dependency : it->second.dependencies) {
        if (pImpl->plugins_.find(dependency) == pImpl->plugins_.end()) {
            return false;
        }
    }
    
    return true;
}

std::vector<std::string> PluginManager::getPluginDependencies(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    auto it = pImpl->plugins_.find(name);
    if (it != pImpl->plugins_.end()) {
        return it->second.dependencies;
    }
    return {};
}

std::vector<std::string> PluginManager::getDependentPlugins(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    std::vector<std::string> dependents;
    for (const auto& pair : pImpl->plugins_) {
        if (std::find(pair.second.dependencies.begin(), pair.second.dependencies.end(), name) != pair.second.dependencies.end()) {
            dependents.push_back(pair.first);
        }
    }
    return dependents;
}

std::shared_ptr<IPlugin> PluginManager::getPlugin(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    auto it = pImpl->plugins_.find(name);
    if (it != pImpl->plugins_.end()) {
        return it->second.plugin;
    }
    return nullptr;
}

std::vector<std::shared_ptr<IPlugin>> PluginManager::getPluginsByType(PluginType type) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    std::vector<std::shared_ptr<IPlugin>> plugins;
    for (const auto& pair : pImpl->plugins_) {
        if (pair.second.type == type) {
            plugins.push_back(pair.second.plugin);
        }
    }
    return plugins;
}

void PluginManager::onBrainInitialized(std::shared_ptr<class Brain> brain) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    for (auto& pair : pImpl->plugins_) {
        if (pair.second.state == PluginState::Initialized) {
            try {
                pair.second.plugin->onBrainInitialized(brain);
                if (pImpl->loggingEnabled_) {
                    Logger::getGlobal().info("Plugin notified about brain initialization: " + pair.first);
                }
            } catch (const std::exception& e) {
                pair.second.lastError = std::string("Error in onBrainInitialized: ") + e.what();
                if (pImpl->loggingEnabled_) {
                    Logger::getGlobal().error("Plugin error in onBrainInitialized: " + pair.first + ": " + pair.second.lastError);
                }
                if (errorCallback_) {
                    errorCallback_(pair.first, pair.second.lastError);
                }
            }
        }
    }
}

void PluginManager::onExperimentStarted(const std::string& experimentName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    for (auto& pair : pImpl->plugins_) {
        if (pair.second.state == PluginState::Initialized) {
            try {
                pair.second.plugin->onExperimentStarted(experimentName);
                if (pImpl->loggingEnabled_) {
                    Logger::getGlobal().info("Plugin notified about experiment start: " + pair.first);
                }
            } catch (const std::exception& e) {
                pair.second.lastError = std::string("Error in onExperimentStarted: ") + e.what();
                if (pImpl->loggingEnabled_) {
                    Logger::getGlobal().error("Plugin error in onExperimentStarted: " + pair.first + ": " + pair.second.lastError);
                }
                if (errorCallback_) {
                    errorCallback_(pair.first, pair.second.lastError);
                }
            }
        }
    }
}

void PluginManager::onExperimentCompleted(const std::string& experimentName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    for (auto& pair : pImpl->plugins_) {
        if (pair.second.state == PluginState::Initialized) {
            try {
                pair.second.plugin->onExperimentCompleted(experimentName);
                if (pImpl->loggingEnabled_) {
                    Logger::getGlobal().info("Plugin notified about experiment completion: " + pair.first);
                }
            } catch (const std::exception& e) {
                pair.second.lastError = std::string("Error in onExperimentCompleted: ") + e.what();
                if (pImpl->loggingEnabled_) {
                    Logger::getGlobal().error("Plugin error in onExperimentCompleted: " + pair.first + ": " + pair.second.lastError);
                }
                if (errorCallback_) {
                    errorCallback_(pair.first, pair.second.lastError);
                }
            }
        }
    }
}

void PluginManager::onStepCompleted(size_t step) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    for (auto& pair : pImpl->plugins_) {
        if (pair.second.state == PluginState::Initialized) {
            try {
                pair.second.plugin->onStepCompleted(step);
            } catch (const std::exception& e) {
                pair.second.lastError = std::string("Error in onStepCompleted: ") + e.what();
                if (pImpl->loggingEnabled_) {
                    Logger::getGlobal().error("Plugin error in onStepCompleted: " + pair.first + ": " + pair.second.lastError);
                }
                if (errorCallback_) {
                    errorCallback_(pair.first, pair.second.lastError);
                }
            }
        }
    }
}

std::vector<std::string> PluginManager::discoverAvailablePlugins() const {
    return PluginRegistry::getInstance().getPluginNames();
}

std::map<std::string, std::string> PluginManager::getPluginStatistics(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    auto it = pImpl->plugins_.find(name);
    if (it == pImpl->plugins_.end()) {
        return {};
    }
    
    std::map<std::string, std::string> stats;
    stats["name"] = it->second.name;
    stats["version"] = it->second.version;
    stats["type"] = std::to_string(static_cast<int>(it->second.type));
    stats["state"] = std::to_string(static_cast<int>(it->second.state));
    stats["load_time"] = std::to_string(it->second.loadTime);
    stats["init_time"] = std::to_string(it->second.initTime);
    stats["memory_usage"] = std::to_string(it->second.memoryUsage);
    stats["dependencies"] = std::to_string(it->second.dependencies.size());
    stats["provided_components"] = std::to_string(it->second.providedComponents.size());
    
    return stats;
}

bool PluginManager::validatePlugin(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    auto it = pImpl->plugins_.find(name);
    if (it == pImpl->plugins_.end()) {
        return false;
    }
    
    // Basic validation checks
    if (it->second.name.empty() || it->second.version.empty()) {
        return false;
    }
    
    if (it->second.state != PluginState::Initialized) {
        return false;
    }
    
    return true;
}

bool PluginManager::loadPluginConfigsFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Simple INI file parsing
    std::string line;
    std::string currentSection;
    
    while (std::getline(file, line)) {
        // Remove whitespace
        line.erase(0, line.find_first_not_of(" \t\n\r"));
        line.erase(line.find_last_not_of(" \t\n\r") + 1);
        
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        if (line[0] == '[' && line.back() == ']') {
            currentSection = line.substr(1, line.size() - 2);
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            
            // Remove quotes
            if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
                value = value.substr(1, value.size() - 2);
            }
            
            if (currentSection == "plugins") {
                configurePlugin(key, {{key, value}});
            }
        }
    }
    
    return true;
}

bool PluginManager::savePluginConfigsToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "[plugins]" << std::endl;
    
    for (const auto& pair : pImpl->plugins_) {
        file << pair.first << " = \"" << pair.second.name << "\"" << std::endl;
    }
    
    return true;
}

PluginState PluginManager::getPluginState(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    auto it = pImpl->plugins_.find(name);
    if (it != pImpl->plugins_.end()) {
        return it->second.state;
    }
    return PluginState::NotLoaded;
}

std::map<std::string, PluginState> PluginManager::getAllPluginStates() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    std::map<std::string, PluginState> states;
    for (const auto& pair : pImpl->plugins_) {
        states[pair.first] = pair.second.state;
    }
    return states;
}

void PluginManager::enablePluginLogging(bool enable) {
    pImpl->loggingEnabled_ = enable;
}

bool PluginManager::isPluginLoggingEnabled() const {
    return pImpl->loggingEnabled_;
}

void PluginManager::logPluginMessage(const std::string& name, const std::string& message, int level) {
    if (!pImpl->loggingEnabled_) {
        return;
    }
    
    std::string levelStr;
    switch (level) {
        case 0: levelStr = "DEBUG"; break;
        case 1: levelStr = "INFO"; break;
        case 2: levelStr = "WARNING"; break;
        case 3: levelStr = "ERROR"; break;
        default: levelStr = "UNKNOWN"; break;
    }
    
    Logger::getGlobal().info("Plugin " + name + " [" + levelStr + "]: " + message);
}

void PluginManager::setPluginSecurityLevel(const std::string& name, int level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    auto it = pImpl->plugins_.find(name);
    if (it != pImpl->plugins_.end()) {
        it->second.securityLevel = std::max(0, std::min(10, level));
    }
}

int PluginManager::getPluginSecurityLevel(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    auto it = pImpl->plugins_.find(name);
    if (it != pImpl->plugins_.end()) {
        return it->second.securityLevel;
    }
    return 1;
}

void PluginManager::enableHotReloading(bool enable) {
    pImpl->hotReloadingEnabled_ = enable;
}

bool PluginManager::isHotReloadingEnabled() const {
    return pImpl->hotReloadingEnabled_;
}

bool PluginManager::reloadPlugin(const std::string& name) {
    if (!pImpl->hotReloadingEnabled_) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    auto it = pImpl->plugins_.find(name);
    if (it == pImpl->plugins_.end()) {
        return false;
    }
    
    // Unload current plugin
    unloadPlugin(name);
    
    // Load plugin again
    return loadPlugin(name, it->second.config);
}

void PluginManager::setPluginInitCallback(PluginInitCallback callback) {
    pImpl->initCallback_ = callback;
}

void PluginManager::setPluginShutdownCallback(PluginShutdownCallback callback) {
    pImpl->shutdownCallback_ = callback;
}

void PluginManager::setPluginErrorCallback(PluginErrorCallback callback) {
    pImpl->errorCallback_ = callback;
}

} // namespace nlm