// PluginManager.h
// Main plugin loading and management system

#pragma once

#include "PluginInterface.h"
#include "PluginRegistry.h"
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>

namespace nlm {

    // Plugin manager configuration
    struct PluginManagerConfig {
        std::vector<std::filesystem::path> pluginSearchPaths;
        std::vector<std::string> enabledCategories;
        std::vector<std::string> disabledPlugins;
        std::map<std::string, std::map<std::string, std::any>> pluginConfigs;
        std::vector<std::string> requiredPlugins;
        bool autoLoadPlugins = true;
        bool enableSecurityValidation = true;
        bool enableVersionChecking = true;
        size_t maxConcurrentLoads = 4;
        std::chrono::milliseconds loadTimeout{30000};
        std::chrono::milliseconds operationTimeout{10000};
        std::string pluginStateFile;
    };

    // Plugin manager event types
    namespace ManagerEvents {
        constexpr const char* PLUGIN_LOAD_REQUESTED = "plugin_load_requested";
        constexpr const char* PLUGIN_LOADED = "plugin_loaded";
        constexpr const char* PLUGIN_LOAD_FAILED = "plugin_load_failed";
        constexpr const char* PLUGIN_UNLOAD_REQUESTED = "plugin_unload_requested";
        constexpr const char* PLUGIN_UNLOADED = "plugin_unloaded";
        constexpr const char* PLUGIN_CONFIGURED = "plugin_configured";
        constexpr const char* PLUGIN_STARTED = "plugin_started";
        constexpr const char* PLUGIN_STOPPED = "plugin_stopped";
        constexpr const char* PLUGIN_ERROR = "plugin_error";
        constexpr const char* MANAGER_SHUTDOWN = "manager_shutdown";
        constexpr const char* SYSTEM_READY = "system_ready";
    }

    // Plugin loading task
    struct PluginTask {
        std::string pluginName;
        PluginState state;
        std::map<std::string, std::any> data;
        std::chrono::steady_clock::time_point startTime;
        std::string error;
        std::promise<bool> completionPromise;
    };

    // Plugin manager implementation
    class PluginManager {
    public:
        PluginManager();
        explicit PluginManager(const PluginManagerConfig& config);
        ~PluginManager();

        // Plugin lifecycle management
        bool loadPlugin(const std::string& name);
        bool unloadPlugin(const std::string& name);
        bool configurePlugin(const std::string& name, const std::map<std::string, std::any>& config);
        bool initializePlugin(const std::string& name);
        bool startPlugin(const std::string& name);
        bool stopPlugin(const std::string& name);
        bool unloadPluginCompletely(const std::string& name);
        
        // Plugin information and status
        std::shared_ptr<PluginInterface> getPlugin(const std::string& name) const;
        PluginState getPluginState(const std::string& name) const;
        const PluginMetadata& getPluginMetadata(const std::string& name) const;
        std::vector<std::string> getPluginNames() const;
        std::map<std::string, PluginState> getAllPluginStates() const;
        std::map<std::string, std::any> getPluginStats() const;
        
        // Plugin discovery and registration
        bool discoverPlugins();
        bool discoverPlugins(const std::vector<std::filesystem::path>& directories);
        bool registerPlugin(const std::filesystem::path& pluginPath);
        bool unregisterPlugin(const std::string& name);
        std::vector<RegistrationInfo> getAvailablePlugins() const;
        
        // Plugin configuration and setup
        void updateConfig(const PluginManagerConfig& config);
        PluginManagerConfig getConfig() const;
        bool setPluginEnabled(const std::string& name, bool enabled);
        bool isPluginEnabled(const std::string& name) const;
        
        // Plugin communication and events
        void addEventListener(const std::string& eventType, std::function<void(const std::string&, const std::any&)> listener);
        void removeEventListener(const std::string& eventType, const std::function<void(const std::string&, const std::any&)>& listener);
        void emitEvent(const std::string& eventType, const std::string& pluginName, const std::any& data = nullptr);
        
        // Plugin dependency management
        bool checkDependencies(const std::string& name) const;
        std::vector<std::string> getPluginDependencies(const std::string& name) const;
        bool waitForDependency(const std::string& pluginName, const std::string& dependencyName, std::chrono::milliseconds timeout) const;
        
        // Plugin factory and instantiation
        bool registerPluginFactory(const std::string& name, const PluginFactory& factory);
        std::shared_ptr<PluginInterface> createPlugin(const std::string& name);
        std::shared_ptr<PluginInterface> createPlugin(const RegistrationInfo& info);
        
        // Plugin interface access
        std::shared_ptr<NeuralComponentPlugin> getNeuralComponentPlugin(const std::string& name) const;
        std::shared_ptr<MemoryPlugin> getMemoryPlugin(const std::string& name) const;
        std::shared_ptr<ProcessingPlugin> getProcessingPlugin(const std::string& name) const;
        
        // Plugin utility methods
        bool validateAllPlugins() const;
        bool cleanupAllPlugins();
        bool saveState();
        bool loadState();
        void shutdown();
        
        // Plugin system management
        bool initialize();
        bool isInitialized() const;
        size_t getLoadedPluginCount() const;
        bool isPluginLoaded(const std::string& name) const;
        
        // Plugin-specific operations
        bool processPluginMessage(const std::string& pluginName, const std::string& messageType, const std::any& message);
        bool broadcastPluginMessage(const std::string& messageType, const std::any& message, const std::vector<std::string>& excludePlugins = {});
        std::vector<std::any> queryPlugins(const std::string& queryType, const std::map<std::string, std::any>& params);
        
        // Thread safety and task management
        void lock() const;
        void unlock() const;
        void processTaskQueue();
        bool waitForPluginState(const std::string& name, PluginState targetState, std::chrono::milliseconds timeout) const;
        bool cancelPluginOperation(const std::string& name, PluginState state);
        
        // Plugin category management
        bool registerPluginCategory(const std::string& name, const std::vector<PluginCapability>& capabilities);
        std::vector<std::string> getPluginCategories() const;
        std::vector<PluginCapability> getCategoryCapabilities(const std::string& category) const;
        
        // Plugin version management
        bool checkVersionCompatibility(const std::string& name) const;
        std::string getSystemVersion() const;
        void setSystemVersion(const std::string& version);
        
        // Plugin security and validation
        bool validatePluginSecurity(const std::string& name) const;
        bool validatePluginPath(const std::filesystem::path& path) const;
        void setSecurityMode(bool enabled);
        
        // Plugin statistics and monitoring
        std::map<std::string, std::any> getManagerStats() const;
        void resetStats();
        
        // Plugin resource management
        bool reserveResources(const std::string& name, size_t memoryBytes, size_t threads = 1);
        bool releaseResources(const std::string& name);
        std::map<std::string, size_t> getResourceUsage() const;
        
        // Plugin callback system
        void addPluginCallback(const std::string& pluginName, const std::string& callbackType, std::function<void(const std::any&)> callback);
        void removePluginCallback(const std::string& pluginName, const std::string& callbackType);
        void invokePluginCallback(const std::string& pluginName, const std::string& callbackType, const std::any& data);

    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
        
        // Private helper methods
        bool executePluginTask(const std::string& name, PluginState targetState, const std::map<std::string, std::any>& data = {});
        std::shared_ptr<PluginTask> createTask(const std::string& name, PluginState state);
        void completeTask(const std::shared_ptr<PluginTask>& task, bool success, const std::string& error = "");
        void updatePluginStateInternal(const std::string& name, PluginState state);
        bool resolveDependencies(const std::string& name, std::set<std::string>& visited) const;
        void cleanupPluginResources(const std::string& name);
        void scheduleTask(std::shared_ptr<PluginTask> task);
        std::shared_ptr<PluginTask> getCurrentTask(const std::string& name) const;
        void removeCompletedTasks();
        std::chrono::milliseconds getTaskTimeout(PluginState state) const;
        bool isTimeoutExceeded(const std::chrono::steady_clock::time_point& startTime) const;
        void validatePluginForLoading(const std::string& name) const;
        void validatePluginForUnloading(const std::string& name) const;
        void handlePluginError(const std::string& name, const std::exception& e);
        void logPluginEvent(const std::string& eventType, const std::string& pluginName, const std::any& data = nullptr);
        bool setupPluginEnvironment(const std::string& name);
        bool cleanupPluginEnvironment(const std::string& name);
        void updateSystemStatus();
        bool checkSystemHealth() const;
        std::map<std::string, std::any> collectPluginDiagnostics() const;
        void broadcastSystemEvent(const std::string& eventType, const std::any& data = nullptr);
    };

    // Plugin factory function type
    using PluginFactory = std::function<std::shared_ptr<PluginInterface>()>;
    
    // Plugin registration macro
    #define REGISTER_PLUGIN_MANAGER_PLUGIN(PluginClass) \
        namespace { \
            struct PluginManagerRegistrar { \
                PluginManagerRegistrar() { \
                    /* Registration logic would go here */ \
                } \
            }; \
            PluginManagerRegistrar registrar; \
        }

    // Plugin system utility functions
    namespace PluginSystem {
        std::string generatePluginId(const std::string& name, const std::string& version);
        bool validatePluginName(const std::string& name);
        std::string sanitizePluginPath(const std::filesystem::path& path);
        std::vector<std::string> splitString(const std::string& str, char delimiter);
        std::string joinString(const std::vector<std::string>& strings, char delimiter);
        std::string generateUUID();
        std::string getCurrentTimestamp();
    }

} // namespace nlm