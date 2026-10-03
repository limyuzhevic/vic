#pragma once

#include "PluginBase.hpp"
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <functional>
#include <atomic>
#include <thread>
#include <chrono>

namespace nlm {

class PluginManager {
public:
    static PluginManager& getInstance() {
        static PluginManager instance;
        return instance;
    }
    
    // Plugin management
    bool registerBuiltInPlugin(std::unique_ptr<PluginBase> plugin);
    bool registerExternalPlugin(const std::string& name,
                                const std::string& libraryPath,
                                const PluginMetadata& metadata);
    bool unregisterPlugin(const std::string& name);
    
    // Plugin lifecycle
    bool initializePlugin(const std::string& name,
                         const std::unordered_map<std::string, std::string>& config);
    bool shutdownPlugin(const std::string& name);
    bool activatePlugin(const std::string& name);
    bool deactivatePlugin(const std::string& name);
    
    // Global configuration
    void setGlobalConfig(const std::unordered_map<std::string, std::string>& config);
    std::unordered_map<std::string, std::string> getGlobalConfig() const;
    void mergeGlobalConfig(const std::unordered_map<std::string, std::string>& config);
    
    // Plugin query
    bool hasPlugin(const std::string& name) const;
    std::vector<std::string> getPluginNames() const;
    PluginMetadata getPluginMetadata(const std::string& name) const;
    std::unique_ptr<PluginBase> getPlugin(const std::string& name);
    
    // Plugin statistics and monitoring
    struct PluginStats {
        std::string name;
        std::string version;
        size_t neuronCount = 0;
        size_t synapseCount = 0;
        float memoryUsage = 0.0f;
        float lastUpdateTime = 0.0f;
        bool isActive = false;
        bool isInitialized = false;
        std::chrono::system_clock::time_point lastActivity;
    };
    
    std::vector<PluginStats> getAllStats() const;
    PluginStats getPluginStats(const std::string& name) const;
    
    // System management
    void initializeAll(const std::unordered_map<std::string, std::string>& globalConfig);
    void shutdownAll();
    void updateAll(double timestep);
    void resetAll();
    
    // Performance monitoring
    void startMonitoring();
    void stopMonitoring();
    bool isMonitoring() const;
    double getFPS() const;
    double getAverageUpdateTime() const;
    
    // Configuration management
    bool loadConfigFromFile(const std::string& filepath);
    bool saveConfigToFile(const std::string& filepath);
    bool loadPluginConfigFromFile(const std::string& pluginName, const std::string& filepath);
    bool savePluginConfigToFile(const std::string& pluginName, const std::string& filepath);
    
    // Dependency management
    bool addPluginDependency(const std::string& pluginName, const std::string& dependencyName);
    bool removePluginDependency(const std::string& pluginName, const std::string& dependencyName);
    std::vector<std::string> getPluginDependencies(const std::string& pluginName) const;
    bool canLoadPlugin(const std::string& pluginName) const;
    
    // Event handling
    using PluginEventCallback = std::function<void(const std::string& pluginName, const std::string& eventType, const std::unordered_map<std::string, std::string>& data)>
    void registerEventCallback(const std::string& eventType, const PluginEventCallback& callback);
    void unregisterEventCallback(const std::string& eventType, const PluginEventCallback& callback);
    void triggerEvent(const std::string& pluginName, const std::string& eventType,
                     const std::unordered_map<std::string, std::string>& data);
    
    // Thread safety
    void lock() const { std::lock_guard<std::mutex> lock(mutex_); }
    void unlock() const { std::lock_guard<std::mutex> lock(mutex_); }
    
    // Statistics and profiling
    struct SystemStats {
        size_t totalPlugins = 0;
        size_t activePlugins = 0;
        size_t initializedPlugins = 0;
        float totalMemoryUsage = 0.0f;
        double totalSimulationTime = 0.0f;
        double averageUpdateTime = 0.0f;
        double fps = 0.0f;
        std::chrono::system_clock::time_point startTime;
        std::chrono::system_clock::time_point lastUpdateTime;
        std::atomic<size_t> totalUpdates{0};
    };
    
    SystemStats getSystemStats() const;
    
private:
    PluginManager();
    ~PluginManager();
    PluginManager(const PluginManager&) = delete;
    PluginManager& operator=(const PluginManager&) = delete;
    
    // Internal helper methods
    bool validatePluginDependencies(const std::string& pluginName) const;
    void notifyDependencies(const std::string& pluginName, bool isLoading);
    void cleanupUnreachablePlugins();
    
    mutable std::mutex mutex_;
    std::unordered_map<std::string, std::unique_ptr<PluginBase>> plugins_;
    std::unordered_map<std::string, PluginMetadata> metadata_;
    std::unordered_map<std::string, std::vector<std::string>> dependencies_;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> pluginConfigs_;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> globalConfig_;
    std::unordered_map<std::string, std::vector<PluginEventCallback>> eventCallbacks_;
    
    // Performance monitoring
    bool monitoringEnabled_ = false;
    std::chrono::system_clock::time_point monitoringStartTime_;
    std::chrono::system_clock::time_point lastUpdateTime_;
    std::atomic<double> fps_{0.0};
    std::atomic<double> averageUpdateTime_{0.0};
    std::atomic<size_t> updateCount_{0};
};

} // namespace nlm