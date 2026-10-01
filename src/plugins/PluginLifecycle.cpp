#include "PluginLifecycle.hpp"
#include "PluginCore.hpp"
#include "../core/Logger/Logger.hpp"
#include <thread>
#include <future>
#include <chrono>
#include <algorithm>
#include <sstream>

namespace nlm {

struct PluginLifecycleManager::Impl {
    bool active{false};
    PluginLifecycleConfig config;
    
    std::mutex lifecycleMutex;
    std::condition_variable lifecycleCondition;
    
    std::map<std::string, PluginLoadStatus> pluginStatuses;
    std::map<std::string, std::chrono::system_clock::time_point> pluginStartTimes;
    std::map<std::string, std::chrono::system_clock::time_point> pluginStopTimes;
    std::map<std::string, std::chrono::system_clock::time_point> pluginLastUpdateTimes;
    std::map<std::string, std::chrono::system_clock::time_point> pluginLastMonitoringTimes;
    std::map<std::string, std::chrono::steady_clock::time_point> pluginOperationStartTimes;
    std::map<std::string, std::chrono::steady_clock::time_point> pluginOperationEndTimes;
    
    std::map<std::string, double> pluginLoadTimes;
    std::map<std::string, double> pluginUnloadTimes;
    std::map<std::string, double> pluginStartTimes;
    std::map<std::string, double> pluginStopTimes;
    
    std::map<std::string, std::vector<PluginLifecycleEvent>> pluginLifecycleHistories;
    std::vector<PluginLifecycleEvent> systemLifecycleEvents;
    
    std::map<std::string, PluginEventCallback> pluginMonitors;
    
    std::map<std::string, std::map<std::string, PluginConfigValue>> pluginConfigs;
    
    std::map<std::string, PluginStatistics> pluginStatistics;
    
    std::queue<std::function<void()>> lifecycleQueue;
    bool queueProcessing{false};
    
    std::vector<std::string> systemMessages;
    std::vector<std::string> systemWarnings;
    std::vector<std::string> systemErrors;
    
    std::vector<PluginLifecycleCallback> lifecycleCallbacks;
    std::mutex callbackMutex;
    
    std::atomic<size_t> totalPlugins{0};
    std::atomic<size_t> activePlugins{0};
    std::atomic<size_t> loadedPlugins{0};
    std::atomic<size_t> failedPlugins{0};
    std::atomic<double> systemMemoryUsage{0.0};
    std::atomic<double> systemCPUUsage{0.0};
    
    Impl() {
        // Initialize with default values
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
    
    void addLifecycleEvent(const std::string& name, PluginLoadStatus status, const std::string& message = "") {
        PluginLifecycleEvent event;
        event.pluginName = name;
        event.status = status;
        event.timestamp = std::chrono::system_clock::now();
        event.message = message;
        event.metadata["operation"] = "lifecycle";
        
        // Add to system events
        systemLifecycleEvents.push_back(event);
        
        // Add to plugin history
        if (pluginLifecycleHistories.find(name) == pluginLifecycleHistories.end()) {
            pluginLifecycleHistories[name] = std::vector<PluginLifecycleEvent>();
        }
        pluginLifecycleHistories[name].push_back(event);
        
        // Keep only recent events
        if (systemLifecycleEvents.size() > 10000) {
            systemLifecycleEvents.erase(systemLifecycleEvents.begin());
        }
        
        // Broadcast event
        broadcastLifecycleEvent(event);
    }
    
    void broadcastLifecycleEvent(const PluginLifecycleEvent& event) {
        std::lock_guard<std::mutex> lock(callbackMutex);
        for (const auto& callback : lifecycleCallbacks) {
            try {
                callback(event);
            } catch (const std::exception& e) {
                logMessage("PluginLifecycleManager: Exception in lifecycle callback: " + std::string(e.what()), "error");
            }
        }
    }
    
    bool checkPluginTimeout(const std::string& name, PluginLoadStatus status) const {
        auto it = pluginOperationStartTimes.find(name);
        if (it == pluginOperationStartTimes.end()) {
            return false;
        }
        
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - it->second);
        
        double timeoutSeconds = 0.0;
        switch (status) {
            case PluginLoadStatus::Loading:
                timeoutSeconds = config.loadTimeoutSeconds;
                break;
            case PluginLoadStatus::Unloading:
                timeoutSeconds = config.unloadTimeoutSeconds;
                break;
            case PluginLoadStatus::Starting:
                timeoutSeconds = config.startTimeoutSeconds;
                break;
            case PluginLoadStatus::Stopping:
                timeoutSeconds = config.stopTimeoutSeconds;
                break;
            default:
                return false;
        }
        
        return duration.count() > timeoutSeconds;
    }
    
    void handlePluginTimeout(const std::string& name, PluginLoadStatus status) {
        logMessage("PluginLifecycleManager: Plugin " + name + " timeout in state " + 
                  std::to_string(static_cast<int>(status)), "warning");
        
        // Update status to error
        pluginStatuses[name] = PluginLoadStatus::Errored;
        addLifecycleEvent(name, PluginLoadStatus::Errored, "Operation timeout");
        
        failedPlugins++;
    }
};

PluginLifecycleManager::PluginLifecycleManager() : pImpl(std::make_unique<Impl>()) {}

PluginLifecycleManager::~PluginLifecycleManager() {
    shutdown();
}

PluginLifecycleManager::PluginLifecycleManager(PluginLifecycleManager&& other) noexcept : pImpl(std::move(other.pImpl)) {}

PluginLifecycleManager& PluginLifecycleManager::operator=(PluginLifecycleManager&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool PluginLifecycleManager::initialize(const PluginLifecycleConfig& config) {
    if (pImpl->active) {
        logMessage("PluginLifecycleManager: Already initialized", "warning");
        return true;
    }
    
    pImpl->config = config;
    pImpl->active = true;
    
    logMessage("PluginLifecycleManager: Initialized successfully");
    
    onSystemInitialized();
    
    return true;
}

void PluginLifecycleManager::shutdown() {
    if (!pImpl->active) {
        return;
    }
    
    logMessage("PluginLifecycleManager: Shutting down...");
    
    // Stop all plugins
    for (const auto& pair : pImpl->pluginStatuses) {
        if (pair.second == PluginLoadStatus::Loaded || 
            pair.second == PluginLoadStatus::Starting ||
            pair.second == PluginLoadStatus::Running) {
            forceStopPlugin(pair.first);
        }
    }
    
    // Unload all plugins
    for (const auto& pair : pImpl->pluginStatuses) {
        if (pair.second == PluginLoadStatus::Loaded || 
            pair.second == PluginLoadStatus::Unloading) {
            forceUnloadPlugin(pair.first);
        }
    }
    
    // Clear queues
    std::lock_guard<std::mutex> lock(pImpl->lifecycleMutex);
    while (!pImpl->lifecycleQueue.empty()) {
        pImpl->lifecycleQueue.pop();
    }
    
    // Stop monitoring
    pImpl->pluginMonitors.clear();
    
    pImpl->active = false;
    
    logMessage("PluginLifecycleManager: Shutdown successfully");
    
    onSystemShutdown();
}

bool PluginLifecycleManager::isActive() const {
    return pImpl->active;
}

bool PluginLifecycleManager::loadPlugin(const std::string& name) {
    return loadPlugin(name, std::map<std::string, PluginConfigValue>());
}

bool PluginLifecycleManager::loadPlugin(const std::string& name, const std::map<std::string, PluginConfigValue>& config) {
    if (!pImpl->active) {
        logMessage("PluginLifecycleManager: Not active, cannot load plugin " + name, "error");
        return false;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->lifecycleMutex);
    
    // Check if plugin is already loaded
    if (pImpl->pluginStatuses.find(name) != pImpl->pluginStatuses.end() &&
        (pImpl->pluginStatuses[name] == PluginLoadStatus::Loaded ||
         pImpl->pluginStatuses[name] == PluginLoadStatus::Loading ||
         pImpl->pluginStatuses[name] == PluginLoadStatus::Starting)) {
        logMessage("PluginLifecycleManager: Plugin already loaded or loading: " + name, "warning");
        return true;
    }
    
    // Schedule load operation
    schedulePluginLoad(name, config);
    
    return true;
}

std::future<bool> PluginLifecycleManager::loadPluginAsync(const std::string& name) {
    return loadPluginAsync(name, std::map<std::string, PluginConfigValue>());
}

std::future<bool> PluginLifecycleManager::loadPluginAsync(const std::string& name, 
                                                       const std::map<std::string, PluginConfigValue>& config) {
    return std::async(std::launch::async, [this, name, config]() {
        return loadPlugin(name, config);
    });
}

bool PluginLifecycleManager::unloadPlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->lifecycleMutex);
    
    // Check if plugin is loaded
    if (pImpl->pluginStatuses.find(name) == pImpl->pluginStatuses.end() ||
        (pImpl->pluginStatuses[name] != PluginLoadStatus::Loaded &&
         pImpl->pluginStatuses[name] != PluginLoadStatus::Unloading &&
         pImpl->pluginStatuses[name] != PluginLoadStatus::Running)) {
        logMessage("PluginLifecycleManager: Plugin not in unloadable state: " + name, "warning");
        return true;
    }
    
    // Schedule unload operation
    schedulePluginUnload(name);
    
    return true;
}

std::future<bool> PluginLifecycleManager::unloadPluginAsync(const std::string& name) {
    return std::async(std::launch::async, [this, name]() {
        return unloadPlugin(name);
    });
}

bool PluginLifecycleManager::startPlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->lifecycleMutex);
    
    // Check if plugin is loaded
    if (pImpl->pluginStatuses.find(name) == pImpl->pluginStatuses.end() ||
        pImpl->pluginStatuses[name] != PluginLoadStatus::Loaded) {
        logMessage("PluginLifecycleManager: Plugin not in loaded state: " + name, "warning");
        return false;
    }
    
    // Schedule start operation
    schedulePluginStart(name);
    
    return true;
}

std::future<bool> PluginLifecycleManager::startPluginAsync(const std::string& name) {
    return std::async(std::launch::async, [this, name]() {
        return startPlugin(name);
    });
}

bool PluginLifecycleManager::stopPlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->lifecycleMutex);
    
    // Check if plugin is running
    if (pImpl->pluginStatuses.find(name) == pImpl->pluginStatuses.end() ||
        (pImpl->pluginStatuses[name] != PluginLoadStatus::Loaded &&
         pImpl->pluginStatuses[name] != PluginLoadStatus::Running &&
         pImpl->pluginStatuses[name] != PluginLoadStatus::Starting)) {
        logMessage("PluginLifecycleManager: Plugin not in running state: " + name, "warning");
        return true;
    }
    
    // Schedule stop operation
    schedulePluginStop(name);
    
    return true;
}

std::future<bool> PluginLifecycleManager::stopPluginAsync(const std::string& name) {
    return std::async(std::launch::async, [this, name]() {
        return stopPlugin(name);
    });
}

bool PluginLifecycleManager::pausePlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->lifecycleMutex);
    
    // Check if plugin is running
    if (pImpl->pluginStatuses.find(name) == pImpl->pluginStatuses.end() ||
        pImpl->pluginStatuses[name] != PluginLoadStatus::Running) {
        logMessage("PluginLifecycleManager: Plugin not in running state: " + name, "warning");
        return true;
    }
    
    // Schedule pause operation
    schedulePluginPause(name);
    
    return true;
}

std::future<bool> PluginLifecycleManager::pausePluginAsync(const std::string& name) {
    return std::async(std::launch::async, [this, name]() {
        return pausePlugin(name);
    });
}

bool PluginLifecycleManager::resumePlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->lifecycleMutex);
    
    // Check if plugin is paused
    if (pImpl->pluginStatuses.find(name) == pImpl->pluginStatuses.end() ||
        pImpl->pluginStatuses[name] != PluginLoadStatus::Paused) {
        logMessage("PluginLifecycleManager: Plugin not in paused state: " + name, "warning");
        return true;
    }
    
    // Schedule resume operation
    schedulePluginResume(name);
    
    return true;
}

std::future<bool> PluginLifecycleManager::resumePluginAsync(const std::string& name) {
    return std::async(std::launch::async, [this, name]() {
        return resumePlugin(name);
    });
}

bool PluginLifecycleManager::restartPlugin(const std::string& name) {
    if (!stopPlugin(name)) {
        logMessage("PluginLifecycleManager: Failed to stop plugin " + name, "error");
        return false;
    }
    
    if (!unloadPlugin(name)) {
        logMessage("PluginLifecycleManager: Failed to unload plugin " + name, "error");
        return false;
    }
    
    if (!loadPlugin(name)) {
        logMessage("PluginLifecycleManager: Failed to load plugin " + name, "error");
        return false;
    }
    
    if (!startPlugin(name)) {
        logMessage("PluginLifecycleManager: Failed to start plugin " + name, "error");
        return false;
    }
    
    return true;
}

std::future<bool> PluginLifecycleManager::restartPluginAsync(const std::string& name) {
    return std::async(std::launch::async, [this, name]() {
        return restartPlugin(name);
    });
}

PluginLoadStatus PluginLifecycleManager::getPluginStatus(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->lifecycleMutex);
    
    auto it = pImpl->pluginStatuses.find(name);
    if (it != pImpl->pluginStatuses.end()) {
        return it->second;
    }
    
    return PluginLoadStatus::NotLoaded;
}

std::chrono::system_clock::time_point PluginLifecycleManager::getPluginStartTime(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->lifecycleMutex);
    
    auto it = pImpl->pluginStartTimes.find(name);
    if (it != pImpl->pluginStartTimes.end()) {
        return it->second;
    }
    
    return std::chrono::system_clock::time_point();
}

std::chrono::system_clock::time_point PluginLifecycleManager::getPluginStopTime(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->lifecycleMutex);
    
    auto it = pImpl->pluginStopTimes.find(name);
    if (it != pImpl->pluginStopTimes.end()) {
        return it->second;
    }
    
    return std::chrono::system_clock::time_point();
}

std::chrono::system_clock::time_point PluginLifecycleManager::getPluginLastUpdateTime(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->lifecycleMutex);
    
    auto it = pImpl->pluginLastUpdateTimes.find(name);
    if (it != pImpl->pluginLastUpdateTimes.end()) {
        return it->second;
    }
    
    return std::chrono::system_clock::time_point();
}

double PluginLifecycleManager::getPluginUptime(const std::string& name) const {
    std::chrono::system_clock::time_point startTime = getPluginStartTime(name);
    if (startTime == std::chrono::system_clock::time_point()) {
        return 0.0;
    }
    
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - startTime);
    return static_cast<double>(duration.count());
}

double PluginLifecycleManager::getPluginRuntime(const std::string& name) const {
    // TODO: Implement runtime calculation
    return 0.0;
}

bool PluginLifecycleManager::isPluginLoaded(const std::string& name) const {
    PluginLoadStatus status = getPluginStatus(name);
    return status == PluginLoadStatus::Loaded || status == PluginLoadStatus::Loading || 
           status == PluginLoadStatus::Starting || status == PluginLoadStatus::Running;
}

bool PluginLifecycleManager::isPluginStarted(const std::string& name) const {
    PluginLoadStatus status = getPluginStatus(name);
    return status == PluginLoadStatus::Loaded || status == PluginLoadStatus::Running ||
           status == PluginLoadStatus::Paused;
}

bool PluginLifecycleManager::isPluginRunning(const std::string& name) const {
    PluginLoadStatus status = getPluginStatus(name);
    return status == PluginLoadStatus::Running;
}

bool PluginLifecycleManager::isPluginStopped(const std::string& name) const {
    PluginLoadStatus status = getPluginStatus(name);
    return status == PluginLoadStatus::Stopped || status == PluginLoadStatus::Unloaded;
}

bool PluginLifecycleManager::isPluginUnloaded(const std::string& name) const {
    PluginLoadStatus status = getPluginStatus(name);
    return status == PluginLoadStatus::Unloaded;
}

bool PluginLifecycleManager::forceLoadPlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->lifecycleMutex);
    
    // Force set status to Loaded
    pluginStatuses[name] = PluginLoadStatus::Loaded;
    pluginStartTimes[name] = std::chrono::system_clock::now();
    pluginOperationStartTimes.erase(name);
    pluginOperationEndTimes.erase(name);
    
    addLifecycleEvent(name, PluginLoadStatus::Loaded, "Force loaded");
    
    return true;
}

bool PluginLifecycleManager::forceUnloadPlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->lifecycleMutex);
    
    // Force set status to Unloaded
    pluginStatuses[name] = PluginLoadStatus::Unloaded;
    pluginStopTimes[name] = std::chrono::system_clock::now();
    pluginOperationStartTimes.erase(name);
    pluginOperationEndTimes.erase(name);
    
    addLifecycleEvent(name, PluginLoadStatus::Unloaded, "Force unloaded");
    
    return true;
}

bool PluginLifecycleManager::forceStartPlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->lifecycleMutex);
    
    // Force set status to Running
    pluginStatuses[name] = PluginLoadStatus::Running;
    pluginStartTimes[name] = std::chrono::system_clock::now();
    pluginOperationStartTimes.erase(name);
    pluginOperationEndTimes.erase(name);
    
    addLifecycleEvent(name, PluginLoadStatus::Running, "Force started");
    
    return true;
}

bool PluginLifecycleManager::forceStopPlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->lifecycleMutex);
    
    // Force set status to Stopped
    pluginStatuses[name] = PluginLoadStatus::Stopped;
    pluginStopTimes[name] = std::chrono::system_clock::now();
    pluginOperationStartTimes.erase(name);
    pluginOperationEndTimes.erase(name);
    
    addLifecycleEvent(name, PluginLoadStatus::Stopped, "Force stopped");
    
    return true;
}

bool PluginLifecycleManager::monitorPlugin(const std::string& name, double intervalSeconds) {
    std::lock_guard<std::mutex> lock(pImpl->lifecycleMutex);
    
    if (pImpl->pluginMonitors.find(name) != pImpl->pluginMonitors.end()) {
        logMessage("PluginLifecycleManager: Plugin already being monitored: " + name, "warning");
        return true;
    }
    
    // Start monitoring
    std::thread monitorThread([this, name, intervalSeconds]() {
        while (isPluginMonitored(name) && isActive()) {
            try {
                if (!checkPluginHealth(name)) {
                    logMessage("PluginLifecycleManager: Plugin health check failed for " + name, "warning");
                }
                
                pImpl->pluginLastMonitoringTimes[name] = std::chrono::system_clock::now();
                
                std::this_thread::sleep_for(std::chrono::duration<double>(intervalSeconds));
            } catch (const std::exception& e) {
                logMessage("PluginLifecycleManager: Exception in monitor thread: " + std::string(e.what()), "error");
                break;
            }
        }
    });
    
    monitorThread.detach();
    pImpl->pluginMonitors[name] = std::move(monitorThread);
    
    pImpl->pluginLastMonitoringTimes[name] = std::chrono::system_clock::now();
    
    logMessage("PluginLifecycleManager: Started monitoring plugin: " + name);
    
    return true;
}

bool PluginLifecycleManager::stopMonitoringPlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->lifecycleMutex);
    
    auto it = pImpl->pluginMonitors.find(name);
    if (it == pImpl->pluginMonitors.end()) {
        logMessage("PluginLifecycleManager: Plugin not being monitored: " + name, "warning");
        return true;
    }
    
    // TODO: Properly join monitor thread
    pImpl->pluginMonitors.erase(it);
    
    logMessage("PluginLifecycleManager: Stopped monitoring plugin: " + name);
    
    return true;
}

bool PluginLifecycleManager::isPluginMonitored(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->lifecycleMutex);
    
    auto it = pImpl->pluginMonitors.find(name);
    return it != pImpl->pluginMonitors.end();
}

std::chrono::system_clock::time_point PluginLifecycleManager::getPluginLastMonitoringTime(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->lifecycleMutex);
    
    auto it = pImpl->pluginLastMonitoringTimes.find(name);
    if (it != pImpl->pluginLastMonitoringTimes.end()) {
        return it->second;
    }
    
    return std::chrono::system_clock::time_point();n}

void PluginLifecycleManager::addLifecycleCallback(PluginLifecycleCallback callback) {
    std::lock_guard<std::mutex> lock(pImpl->callbackMutex);
    pImpl->lifecycleCallbacks.push_back(callback);
}

void PluginLifecycleManager::removeLifecycleCallback(PluginLifecycleCallback callback) {
    std::lock_guard<std::mutex> lock(pImpl->callbackMutex);
    auto it = std::find(pImpl->lifecycleCallbacks.begin(), pImpl->lifecycleCallbacks.end(), callback);
    if (it != pImpl->lifecycleCallbacks.end()) {
        pImpl->lifecycleCallbacks.erase(it);
    }
}

void PluginLifecycleManager::broadcastLifecycleEvent(const PluginLifecycleEvent& event) {
    std::lock_guard<std::mutex> lock(pImpl->callbackMutex);
    for (const auto& callback : pImpl->lifecycleCallbacks) {
        try {
            callback(event);
        } catch (const std::exception& e) {
            logMessage("PluginLifecycleManager: Exception in lifecycle callback: " + std::string(e.what()), "error");
        }
    }
}

bool PluginLifecycleManager::configurePlugin(const std::string& name, const std::map<std::string, PluginConfigValue>& config) {
    std::lock_guard<std::mutex> lock(pImpl->lifecycleMutex);
    
    pImpl->pluginConfigs[name] = config;
    
    // Update plugin if it exists
    Plugin* plugin = nullptr; // TODO: Get plugin from registry
    if (plugin) {
        plugin->configure(config);
    }
    
    logMessage("PluginLifecycleManager: Configured plugin: " + name);
    
    return true;
}

std::map<std::string, PluginConfigValue> PluginLifecycleManager::getPluginConfig(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->lifecycleMutex);
    
    auto it = pImpl->pluginConfigs.find(name);
    if (it != pImpl->pluginConfigs.end()) {
        return it->second;
    }
    
    return std::map<std::string, PluginConfigValue>();
}

PluginStatistics PluginLifecycleManager::getPluginStatistics(const std::string& name) const {
    // TODO: Implement plugin statistics
    return PluginStatistics();
}

std::vector<PluginLifecycleEvent> PluginLifecycleManager::getPluginLifecycleHistory(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->lifecycleMutex);
    
    auto it = pImpl->pluginLifecycleHistories.find(name);
    if (it != pImpl->pluginLifecycleHistories.end()) {
        return it->second;
    }
    
    return std::vector<PluginLifecycleEvent>();
}

std::vector<PluginLifecycleEvent> PluginLifecycleManager::getSystemLifecycleEvents() const {
    return pImpl->systemLifecycleEvents;
}

bool PluginLifecycleManager::updateAllPlugins(double deltaTime) {
    bool success = true;
    
    for (const auto& pair : pImpl->pluginStatuses) {
        const std::string& name = pair.first;
        if (pair.second == PluginLoadStatus::Running || pair.second == PluginLoadStatus::Paused) {
            if (!updatePlugin(name, deltaTime)) {
                logMessage("PluginLifecycleManager: Failed to update plugin " + name, "warning");
                success = false;
            }
        }
    }
    
    return success;
}

bool PluginLifecycleManager::updatePlugin(const std::string& name, double deltaTime) {
    Plugin* plugin = nullptr; // TODO: Get plugin from registry
    if (!plugin || !isPluginRunning(name)) {
        return false;
    }
    
    // Update plugin
    bool success = plugin->update(deltaTime);
    
    if (success) {
        pImpl->pluginLastUpdateTimes[name] = std::chrono::system_clock::now();
        
        PluginLifecycleEvent event;
        event.pluginName = name;
        event.status = PluginLoadStatus::Running;
        event.timestamp = pImpl->pluginLastUpdateTimes[name];
        event.message = "Updated";
        event.metadata["deltaTime"] = std::to_string(deltaTime);
        
        addLifecycleEvent(name, PluginLoadStatus::Running, "Updated");
    }
    
    return success;
}

bool PluginLifecycleManager::processLifecycleQueue() {
    std::lock_guard<std::mutex> lock(pImpl->lifecycleMutex);
    
    // Process queued operations
    while (!pImpl->lifecycleQueue.empty()) {
        std::function<void()> operation = pImpl->lifecycleQueue.front();
        pImpl->lifecycleQueue.pop();
        
        try {
            operation();
        } catch (const std::exception& e) {
            logMessage("PluginLifecycleManager: Exception processing lifecycle operation: " + std::string(e.what()), "error");
        }
    }
    
    return true;
}

bool PluginLifecycleManager::cleanPluginHistory() {
    // Clean old lifecycle events
    auto now = std::chrono::system_clock::now();
    auto cutoff = now - std::chrono::hours(24);
    
    for (auto& pair : pImpl->pluginLifecycleHistories) {
        std::vector<PluginLifecycleEvent>& history = pair.second;
        history.erase(std::remove_if(history.begin(), history.end(),
            [cutoff](const PluginLifecycleEvent& event) {
                return event.timestamp < cutoff;
            }), history.end());
    }
    
    // Clean system events
    pImpl->systemLifecycleEvents.erase(std::remove_if(pImpl->systemLifecycleEvents.begin(),
        pImpl->systemLifecycleEvents.end(),
        [cutoff](const PluginLifecycleEvent& event) {
            return event.timestamp < cutoff;
        }), pImpl->systemLifecycleEvents.end());
    
    return true;
}

bool PluginLifecycleManager::recoverPlugin(const std::string& name) {
    logMessage("PluginLifecycleManager: Recovering plugin: " + name);
    
    // Stop if running
    if (isPluginRunning(name)) {
        if (!stopPlugin(name)) {
            logMessage("PluginLifecycleManager: Failed to stop plugin during recovery: " + name, "warning");
        }
    }
    
    // Unload
    if (!unloadPlugin(name)) {
        logMessage("PluginLifecycleManager: Failed to unload plugin during recovery: " + name, "warning");
    }
    
    // Load again
    if (!loadPlugin(name)) {
        logMessage("PluginLifecycleManager: Failed to load plugin during recovery: " + name, "error");
        return false;
    }
    
    // Start
    if (!startPlugin(name)) {
        logMessage("PluginLifecycleManager: Failed to start plugin during recovery: " + name, "warning");
    }
    
    return true;
}

bool PluginLifecycleManager::recoverAllFailedPlugins() {
    bool success = true;
    
    for (const auto& pair : pImpl->pluginStatuses) {
        if (pair.second == PluginLoadStatus::Failed || pair.second == PluginLoadStatus::Errored) {
            if (!recoverPlugin(pair.first)) {
                logMessage("PluginLifecycleManager: Failed to recover plugin " + pair.first, "warning");
                success = false;
            }
        }
    }
    
    return success;
}

bool PluginLifecycleManager::handlePluginError(const std::string& name, const std::string& error) {
    logMessage("PluginLifecycleManager: Handling error for plugin " + name + ": " + error, "error");
    
    // Stop plugin if running
    if (isPluginRunning(name)) {
        stopPlugin(name);
    }
    
    // Mark as errored
    std::lock_guard<std::mutex> lock(pImpl->lifecycleMutex);
    pluginStatuses[name] = PluginLoadStatus::Errored;
    
    addLifecycleEvent(name, PluginLoadStatus::Errored, error);
    
    failedPlugins++;
    
    // Attempt recovery
    if (config.enablePluginRecovery) {
        recoverPlugin(name);
    }
    
    return true;
}

bool PluginLifecycleManager::isPluginRecovered(const std::string& name) const {
    // TODO: Implement recovery tracking
    return false;
}

bool PluginLifecycleManager::validatePluginState(const std::string& name) const {
    PluginLoadStatus status = getPluginStatus(name);
    
    // Validate state transitions
    if (status == PluginLoadStatus::Loading && 
        (pImpl->pluginStatuses.find(name) == pImpl->pluginStatuses.end() ||
         pImpl->pluginStatuses.at(name) != PluginLoadStatus::Loading)) {
        return false;
    }
    
    return true;
}

bool PluginLifecycleManager::validateAllPluginStates() const {
    for (const auto& pair : pImpl->pluginStatuses) {
        if (!validatePluginState(pair.first)) {
            return false;
        }
    }
    
    return true;
}

std::vector<std::string> PluginLifecycleManager::getPluginValidationErrors(const std::string& name) const {
    std::vector<std::string> errors;
    
    PluginLoadStatus status = getPluginStatus(name);
    if (status == PluginLoadStatus::Failed) {
        errors.push_back("Plugin failed to load");
    } else if (status == PluginLoadStatus::Errored) {
        errors.push_back("Plugin encountered an error");
    } else if (status == PluginLoadStatus::Timeout) {
        errors.push_back("Plugin operation timed out");
    }
    
    return errors;
}

PluginSystemStatistics PluginLifecycleManager::getSystemStatistics() const {
    PluginSystemStatistics stats;
    
    stats.totalPlugins = pImpl->totalPlugins;
    stats.activePlugins = pImpl->activePlugins;
    stats.loadedPlugins = pImpl->loadedPlugins;
    stats.failedPlugins = pImpl->failedPlugins;
    stats.systemMemoryUsage = pImpl->systemMemoryUsage;
    stats.systemCPUUsage = pImpl->systemCPUUsage;
    stats.pluginLoadCount = 0; // TODO: Track
    stats.pluginUnloadCount = 0; // TODO: Track
    stats.averagePluginLoadTime = 0.0;
    stats.averagePluginUnloadTime = 0.0;
    
    return stats;
}

double PluginLifecycleManager::getAverageLoadTime(const std::string& name) const {
    auto it = pImpl->pluginLoadTimes.find(name);
    if (it != pImpl->pluginLoadTimes.end()) {
        return it->second;
    }
    return 0.0;
}

double PluginLifecycleManager::getAverageUnloadTime(const std::string& name) const {
    auto it = pImpl->pluginUnloadTimes.find(name);
    if (it != pImpl->pluginUnloadTimes.end()) {
        return it->second;
    }
    return 0.0;
}

double PluginLifecycleManager::getAverageStartTime(const std::string& name) const {
    auto it = pImpl->pluginStartTimes.find(name);
    if (it != pImpl->pluginStartTimes.end()) {
        return it->second;
    }
    return 0.0;
}

double PluginLifecycleManager::getAverageStopTime(const std::string& name) const {
    auto it = pImpl->pluginStopTimes.find(name);
    if (it != pImpl->pluginStopTimes.end()) {
        return it->second;
    }
    return 0.0;
}

void PluginLifecycleManager::setConfig(const PluginLifecycleConfig& config) {
    pImpl->config = config;
}

PluginLifecycleConfig PluginLifecycleManager::getConfig() const {
    return pImpl->config;
}

bool PluginLifecycleManager::enablePlugin(const std::string& name) {
    Plugin* plugin = nullptr; // TODO: Get plugin from registry
    if (!plugin) {
        return false;
    }
    
    plugin->setActive(true);
    logMessage("PluginLifecycleManager: Enabled plugin: " + name);
    
    return true;
}

bool PluginLifecycleManager::disablePlugin(const std::string& name) {
    Plugin* plugin = nullptr; // TODO: Get plugin from registry
    if (!plugin) {
        return false;
    }
    
    plugin->setActive(false);
    logMessage("PluginLifecycleManager: Disabled plugin: " + name);
    
    return true;
}

bool PluginLifecycleManager::restartPlugin(const std::string& name) {
    return restartPlugin(name);
}

bool PluginLifecycleManager::refreshPlugin(const std::string& name) {
    return restartPlugin(name);
}

bool PluginLifecycleManager::cleanupPluginResources(const std::string& name) {
    // TODO: Implement resource cleanup
    return true;
}

bool PluginLifecycleManager::cleanupAllPluginResources() {
    for (const auto& name : getAllPluginNames()) {
        cleanupPluginResources(name);
    }
    return true;
}

bool PluginLifecycleManager::validatePluginDependencies(const std::string& name) const {
    // TODO: Implement dependency validation
    return true;
}

bool PluginLifecycleManager::resolvePluginDependencies() {
    // TODO: Implement dependency resolution
    return true;
}

bool PluginLifecycleManager::optimizePluginLifecycle() {
    // TODO: Implement lifecycle optimization
    return true;
}

std::string PluginLifecycleManager::getLifecycleStatus() const {
    std::stringstream ss;
    ss << "PluginLifecycleManager Status:" << std::endl;
    ss << "  Active: " << (isActive() ? "true" : "false") << std::endl;
    ss << "  Total Plugins: " << pImpl->totalPlugins << std::endl;
    ss << "  Loaded Plugins: " << pImpl->loadedPlugins << std::endl;
    ss << "  Failed Plugins: " << pImpl->failedPlugins << std::endl;
    ss << "  System Memory Usage: " << pImpl->systemMemoryUsage << " MB" << std::endl;
    ss << "  System CPU Usage: " << pImpl->systemCPUUsage << "%" << std::endl;
    
    return ss.str();
}

std::string PluginLifecycleManager::getLifecycleInfo() const {
    std::stringstream ss;
    ss << "PluginLifecycleManager Information:" << std::endl;
    ss << "  Update Interval: " << pImpl->config.updateIntervalSeconds << " seconds" << std::endl;
    ss << "  Load Timeout: " << pImpl->config.loadTimeoutSeconds << " seconds" << std::endl;
    ss << "  Unload Timeout: " << pImpl->config.unloadTimeoutSeconds << " seconds" << std::endl;
    ss << "  Start Timeout: " << pImpl->config.startTimeoutSeconds << " seconds" << std::endl;
    ss << "  Stop Timeout: " << pImpl->config.stopTimeoutSeconds << " seconds" << std::endl;
    ss << "  Automatic Lifecycle Management: " << (pImpl->config.enableAutomaticLifecycleManagement ? "enabled" : "disabled") << std::endl;
    ss << "  Plugin Monitoring: " << (pImpl->config.enablePluginMonitoring ? "enabled" : "disabled") << std::endl;
    ss << "  Plugin Recovery: " << (pImpl->config.enablePluginRecovery ? "enabled" : "disabled") << std::endl;
    ss << "  Recovery Timeout: " << pImpl->config.recoveryTimeoutSeconds << " seconds" << std::endl;
    
    return ss.str();
}

std::vector<std::string> PluginLifecycleManager::getLifecycleMessages() const {
    return pImpl->systemMessages;
}

std::vector<std::string> PluginLifecycleManager::getLifecycleWarnings() const {
    return pImpl->systemWarnings;
}

std::vector<std::string> PluginLifecycleManager::getLifecycleErrors() const {
    return pImpl->systemErrors;
}

bool PluginLifecycleManager::exportLifecycleData(const std::string& filename, const std::string& format) const {
    // TODO: Implement lifecycle data export
    return true;
}

bool PluginLifecycleManager::importLifecycleData(const std::string& filename, const std::string& format) {
    // TODO: Implement lifecycle data import
    return true;
}

std::string PluginLifecycleManager::getAbout() const {
    return "PluginLifecycleManager - Plugin Lifecycle Management System for NLM";
}

std::string PluginLifecycleManager::getVersion() const {
    return "1.0.0";
}

std::string PluginLifecycleManager::getBuildInfo() const {
    return "PluginLifecycleManager Build Information";
}

bool PluginLifecycleManager::integratePlugins() {
    // TODO: Implement plugin integration
    return true;
}

bool PluginLifecycleManager::removeIntegration(const std::string& name1, const std::string& name2) {
    // TODO: Implement integration removal
    return true;
}

std::vector<std::pair<std::string, std::string>> PluginLifecycleManager::getActiveIntegrations() const {
    // TODO: Implement integration tracking
    return {};
}

bool PluginLifecycleManager::optimizeMemoryUsage() {
    // TODO: Implement memory optimization
    return true;
}

bool PluginLifecycleManager::cleanupUnusedPlugins() {
    // TODO: Implement cleanup
    return true;
}

size_t PluginLifecycleManager::getPluginMemoryUsage(const std::string& name) const {
    // TODO: Implement memory usage calculation
    return 0;
}

double PluginLifecycleManager::getPluginCPUUsage(const std::string& name) const {
    // TODO: Implement CPU usage calculation
    return 0.0;
}

bool PluginLifecycleManager::checkPluginHealth(const std::string& name) const {
    // TODO: Implement health check
    return true;
}

bool PluginLifecycleManager::performHealthCheck(const std::string& name) {
    // TODO: Implement health check execution
    return true;
}

std::future<bool> PluginLifecycleManager::executeAsyncOperation(std::function<bool()> operation, double timeoutSeconds) {
    return std::async(std::launch::async, [this, operation, timeoutSeconds]() {
        std::future<bool> result = std::async(std::launch::async, operation);
        
        auto status = result.wait_for(std::chrono::duration<double>(timeoutSeconds));
        if (status == std::future_status::ready) {
            return result.get();
        } else {
            logMessage("PluginLifecycleManager: Operation timeout for plugin", "warning");
            return false;
        }
    });
}

// Private helper methods implementation
void PluginLifecycleManager::schedulePluginLoad(const std::string& name, const std::map<std::string, PluginConfigValue>& config) {
    std::lock_guard<std::mutex> lock(pImpl->lifecycleMutex);
    
    pImpl->pluginStatuses[name] = PluginLoadStatus::Loading;
    pImpl->pluginOperationStartTimes[name] = std::chrono::steady_clock::now();
    
    pImpl->lifecycleQueue.push([this, name, config]() {
        loadPluginInternal(name, config);
    });
    
    addLifecycleEvent(name, PluginLoadStatus::Loading, "Load scheduled");
}

void PluginLifecycleManager::schedulePluginUnload(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->lifecycleMutex);
    
    pImpl->pluginStatuses[name] = PluginLoadStatus::Unloading;
    pImpl->pluginOperationStartTimes[name] = std::chrono::steady_clock::now();
    
    pImpl->lifecycleQueue.push([this, name]() {
        unloadPluginInternal(name);
    });
    
    addLifecycleEvent(name, PluginLoadStatus::Unloading, "Unload scheduled");
}

void PluginLifecycleManager::schedulePluginStart(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->lifecycleMutex);
    
    pImpl->pluginStatuses[name] = PluginLoadStatus::Starting;
    pImpl->pluginOperationStartTimes[name] = std::chrono::steady_clock::now();
    
    pImpl->lifecycleQueue.push([this, name]() {
        startPluginInternal(name);
    });
    
    addLifecycleEvent(name, PluginLoadStatus::Starting, "Start scheduled");
}

void PluginLifecycleManager::schedulePluginStop(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->lifecycleMutex);
    
    pImpl->pluginStatuses[name] = PluginLoadStatus::Stopping;
    pImpl->pluginOperationStartTimes[name] = std::chrono::steady_clock::now();
    
    pImpl->lifecycleQueue.push([this, name]() {
        stopPluginInternal(name);
    });
    
    addLifecycleEvent(name, PluginLoadStatus::Stopping, "Stop scheduled");
}

void PluginLifecycleManager::schedulePluginPause(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->lifecycleMutex);
    
    pImpl->pluginStatuses[name] = PluginLoadStatus::Pausing;
    pImpl->pluginOperationStartTimes[name] = std::chrono::steady_clock::now();
    
    pImpl->lifecycleQueue.push([this, name]() {
        pausePluginInternal(name);
    });
    
    addLifecycleEvent(name, PluginLoadStatus::Pausing, "Pause scheduled");
}

void PluginLifecycleManager::schedulePluginResume(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->lifecycleMutex);
    
    pImpl->pluginStatuses[name] = PluginLoadStatus::Resuming;
    pImpl->pluginOperationStartTimes[name] = std::chrono::steady_clock::now();
    
    pImpl->lifecycleQueue.push([this, name]() {
        resumePluginInternal(name);
    });
    
    addLifecycleEvent(name, PluginLoadStatus::Resuming, "Resume scheduled");
}

void PluginLifecycleManager::processLifecycleQueue() {
    std::lock_guard<std::mutex> lock(pImpl->lifecycleMutex);
    
    while (!pImpl->lifecycleQueue.empty()) {
        std::function<void()> operation = pImpl->lifecycleQueue.front();
        pImpl->lifecycleQueue.pop();
        
        try {
            operation();
        } catch (const std::exception& e) {
            logMessage("PluginLifecycleManager: Exception processing lifecycle operation: " + std::string(e.what()), "error");
        }
    }
}

void PluginLifecycleManager::monitorActivePlugins() {
    for (const auto& pair : pImpl->pluginStatuses) {
        const std::string& name = pair.first;
        PluginLoadStatus status = pair.second;
        
        if (status == PluginLoadStatus::Running || status == PluginLoadStatus::Paused) {
            // Check for timeouts
            if (checkPluginTimeout(name, status)) {
                handlePluginTimeout(name, status);
            }
        }
    }
}

bool PluginLifecycleManager::loadPluginInternal(const std::string& name, const std::map<std::string, PluginConfigValue>& config) {
    // TODO: Implement internal plugin loading
    pImpl->pluginStatuses[name] = PluginLoadStatus::Loaded;
    pImpl->pluginStartTimes[name] = std::chrono::system_clock::now();
    pImpl->pluginLoadTimes[name] = 0.0; // TODO: Measure actual load time
    
    addLifecycleEvent(name, PluginLoadStatus::Loaded, "Load completed");
    
    onPluginLoadCompleted(name, true);
    
    return true;
}

bool PluginLifecycleManager::unloadPluginInternal(const std::string& name) {
    // TODO: Implement internal plugin unloading
    pImpl->pluginStatuses[name] = PluginLoadStatus::Unloaded;
    pImpl->pluginStopTimes[name] = std::chrono::system_clock::now();
    pImpl->pluginUnloadTimes[name] = 0.0; // TODO: Measure actual unload time
    
    addLifecycleEvent(name, PluginLoadStatus::Unloaded, "Unload completed");
    
    onPluginUnloadCompleted(name, true);
    
    return true;
}

bool PluginLifecycleManager::startPluginInternal(const std::string& name) {
    // TODO: Implement internal plugin starting
    pImpl->pluginStatuses[name] = PluginLoadStatus::Running;
    pImpl->pluginStartTimes[name] = std::chrono::system_clock::now();
    pImpl->pluginStartTimes[name] = 0.0; // TODO: Measure actual start time
    
    addLifecycleEvent(name, PluginLoadStatus::Running, "Start completed");
    
    onPluginStartCompleted(name, true);
    
    return true;
}

bool PluginLifecycleManager::stopPluginInternal(const std::string& name) {
    // TODO: Implement internal plugin stopping
    pImpl->pluginStatuses[name] = PluginLoadStatus::Stopped;
    pImpl->pluginStopTimes[name] = std::chrono::system_clock::now();
    pImpl->pluginStopTimes[name] = 0.0; // TODO: Measure actual stop time
    
    addLifecycleEvent(name, PluginLoadStatus::Stopped, "Stop completed");
    
    onPluginStopCompleted(name, true);
    
    return true;
}

bool PluginLifecycleManager::pausePluginInternal(const std::string& name) {
    // TODO: Implement internal plugin pausing
    pImpl->pluginStatuses[name] = PluginLoadStatus::Paused;
    
    addLifecycleEvent(name, PluginLoadStatus::Paused, "Pause completed");
    
    return true;
}

bool PluginLifecycleManager::resumePluginInternal(const std::string& name) {
    // TODO: Implement internal plugin resuming
    pImpl->pluginStatuses[name] = PluginLoadStatus::Running;
    
    addLifecycleEvent(name, PluginLoadStatus::Running, "Resume completed");
    
    return true;
}

// Plugin lifecycle callbacks implementation
void PluginLifecycleManager::onPluginLoadStarted(const std::string& name) {
    // Default implementation - can be overridden
    addLifecycleEvent(name, PluginLoadStatus::Loading, "Load started");
}

void PluginLifecycleManager::onPluginLoadCompleted(const std::string& name, bool success) {
    // Default implementation - can be overridden
    if (success) {
        addLifecycleEvent(name, PluginLoadStatus::Loaded, "Load completed successfully");
    } else {
        addLifecycleEvent(name, PluginLoadStatus::Failed, "Load completed with failure");
    }
}

void PluginLifecycleManager::onPluginLoadFailed(const std::string& name, const std::string& error) {
    // Default implementation - can be overridden
    addLifecycleEvent(name, PluginLoadStatus::Errored, "Load failed: " + error);
}

void PluginLifecycleManager::onPluginStartRequested(const std::string& name) {
    // Default implementation - can be overridden
    addLifecycleEvent(name, PluginLoadStatus::Starting, "Start requested");
}

void PluginLifecycleManager::onPluginStartCompleted(const std::string& name, bool success) {
    // Default implementation - can be overridden
    if (success) {
        addLifecycleEvent(name, PluginLoadStatus::Running, "Start completed successfully");
    } else {
        addLifecycleEvent(name, PluginLoadStatus::Failed, "Start completed with failure");
    }
}

void PluginLifecycleManager::onPluginStartFailed(const std::string& name, const std::string& error) {
    // Default implementation - can be overridden
    addLifecycleEvent(name, PluginLoadStatus::Errored, "Start failed: " + error);
}

void PluginLifecycleManager::onPluginStopRequested(const std::string& name) {
    // Default implementation - can be overridden
    addLifecycleEvent(name, PluginLoadStatus::Stopping, "Stop requested");
}

void PluginLifecycleManager::onPluginStopCompleted(const std::string& name, bool success) {
    // Default implementation - can be overridden
    if (success) {
        addLifecycleEvent(name, PluginLoadStatus::Stopped, "Stop completed successfully");
    } else {
        addLifecycleEvent(name, PluginLoadStatus::Errored, "Stop completed with failure");
    }
}

void PluginLifecycleManager::onPluginStopFailed(const std::string& name, const std::string& error) {
    // Default implementation - can be overridden
    addLifecycleEvent(name, PluginLoadStatus::Errored, "Stop failed: " + error);
}

void PluginLifecycleManager::onPluginUnloadRequested(const std::string& name) {
    // Default implementation - can be overridden
    addLifecycleEvent(name, PluginLoadStatus::Unloading, "Unload requested");
}

void PluginLifecycleManager::onPluginUnloadCompleted(const std::string& name, bool success) {
    // Default implementation - can be overridden
    if (success) {
        addLifecycleEvent(name, PluginLoadStatus::Unloaded, "Unload completed successfully");
    } else {
        addLifecycleEvent(name, PluginLoadStatus::Errored, "Unload completed with failure");
    }
}

void PluginLifecycleManager::onPluginUnloadFailed(const std::string& name, const std::string& error) {
    // Default implementation - can be overridden
    addLifecycleEvent(name, PluginLoadStatus::Errored, "Unload failed: " + error);
}

void PluginLifecycleManager::onPluginUpdated(const std::string& name, double deltaTime) {
    // Default implementation - can be overridden
    addLifecycleEvent(name, PluginLoadStatus::Running, "Updated: " + std::to_string(deltaTime) + "s");
}

void PluginLifecycleManager::onPluginRecovered(const std::string& name) {
    // Default implementation - can be overridden
    addLifecycleEvent(name, PluginLoadStatus::Loaded, "Recovered");
}

void PluginLifecycleManager::onSystemInitialized() {
    // Default implementation - can be overridden
    addLifecycleEvent("system", PluginLoadStatus::Loaded, "System initialized");
}

void PluginLifecycleManager::onSystemShutdown() {
    // Default implementation - can be overridden
    addLifecycleEvent("system", PluginLoadStatus::Unloaded, "System shutting down");
}

void PluginLifecycleManager::onSystemError(const std::string& error) {
    // Default implementation - can be overridden
    addLifecycleEvent("system", PluginLoadStatus::Errored, "System error: " + error);
}

void PluginLifecycleManager::onSystemWarning(const std::string& warning) {
    // Default implementation - can be overridden
    addLifecycleEvent("system", PluginLoadStatus::Warning, "System warning: " + warning);
}

} // namespace nlm