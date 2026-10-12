// Enhanced NLM Plugin Architecture
// Provides extensible plugin system for adding new components to the brain system
// Supports component registration, factory patterns, and lifecycle management

#pragma once

#include <memory>
#include <string>
#include <map>
#include <vector>
#include <unordered_map>
#include <functional>
#include <atomic>
#include <mutex>
#include <typeindex>
#include <set>
#include <algorithm>
#include <filesystem>
#include <sstream>

namespace nlm {

// Plugin Architecture Version
#define PLUGIN_ARCHITECTURE_VERSION "2.0.0"
#define PLUGIN_ABI_VERSION "2.0.0"

// Plugin System Configuration
struct PluginConfig {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string contact;
    std::string license;
    std::string dependencies; // Comma-separated list of required plugin names
    std::string optionalDependencies; // Comma-separated list of optional plugin names
    bool enabled = true;
    bool required = false;
    std::map<std::string, std::string> metadata; // Additional metadata
    
    PluginConfig() = default;
    
    PluginConfig(const std::string& name, const std::string& version,
                 const std::string& description = "")
        : name(name), version(version), description(description),
          author("Unknown"), contact("unknown@example.com"),
          license("MIT"), enabled(true), required(false) {}
    
    bool hasDependency(const std::string& dependencyName) const {
        if (dependencies.empty()) return false;
        
        std::istringstream iss(dependencies);
        std::string dep;
        while (std::getline(iss, dep, ',')) {
            // Trim whitespace
            dep.erase(0, dep.find_first_not_of(" \t\r\n"));
            dep.erase(dep.find_last_not_of(" \t\r\n") + 1);
            
            if (dep == dependencyName) return true;
        }
        return false;
    }
    
    std::vector<std::string> getDependencyList() const {
        std::vector<std::string> deps;
        if (dependencies.empty()) return deps;
        
        std::istringstream iss(dependencies);
        std::string dep;
        while (std::getline(iss, dep, ',')) {
            dep.erase(0, dep.find_first_not_of(" \t\r\n"));
            dep.erase(dep.find_last_not_of(" \t\r\n") + 1);
            if (!dep.empty()) deps.push_back(dep);
        }
        return deps;
    }
};

// Plugin Component Types
enum class PluginComponentType {
    NeuronType,
    SynapseType,
    PlasticityRule,
    MemorySystem,
    Neuromodulator,
    PredictionSystem,
    CognitiveSystem,
    DevelopmentalSystem,
    Sensor,
    Actuator,
    Environment,
    Custom
};

// Plugin Lifecycle States
enum class PluginState {
    UNLOADED,
    LOADING,
    LOADED,
    INITIALIZING,
    INITIALIZED,
    RUNNING,
    STOPPING,
    STOPPED,
    ERROR
};

// Plugin Event Types
enum class PluginEventType {
    LOADED,
    UNLOADED,
    INITIALIZED,
    SHUTDOWN,
    ERROR,
    CONFIGURATION_CHANGED,
    STATE_CHANGED,
    COMPONENT_CREATED,
    COMPONENT_DESTROYED
};

// Plugin Event Structure
struct PluginEvent {
    PluginEventType type;
    std::string pluginName;
    std::string message;
    std::map<std::string, std::string> data;
    double timestamp;
    
    PluginEvent(PluginEventType t, const std::string& name, const std::string& msg = "")
        : type(t), pluginName(name), message(msg), timestamp(std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1e9) {}
};

// Plugin Component Base Class
class IPluginComponent {
public:
    virtual ~IPluginComponent() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual PluginComponentType getType() const = 0;
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual bool isInitialized() const = 0;
    
    // Factory method for creating component instances
    virtual IPluginComponent* createInstance() const = 0;
    
    // Configuration management
    virtual void setConfig(const PluginConfig& config) {}
    virtual PluginConfig getConfig() const { return PluginConfig(); }
    
    // Dependency management
    virtual std::vector<std::string> getDependencies() const { return {}; }
    
    // Event handling
    virtual void onEvent(const PluginEvent& event) {}
};

// Plugin Base Class
class IPlugin {
public:
    virtual ~IPlugin() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
    virtual PluginConfig getConfig() const = 0;
    virtual void setConfig(const PluginConfig& config) = 0;
    
    // Plugin lifecycle
    virtual bool load() = 0;
    virtual bool initialize() = 0;
    virtual bool shutdown() = 0;
    virtual bool isLoaded() const = 0;
    virtual bool isInitialized() const = 0;
    
    // Component management
    virtual bool registerComponent(std::unique_ptr<IPluginComponent> component) = 0;
    virtual IPluginComponent* getComponent(const std::string& name) = 0;
    virtual std::vector<IPluginComponent*> getComponents(PluginComponentType type) = 0;
    virtual bool unregisterComponent(const std::string& name) = 0;
    
    // Event system
    virtual void addEventListener(std::function<void(const PluginEvent&)> listener) = 0;
    virtual void removeEventListener(std::function<void(const PluginEvent&)> listener) = 0;
    virtual void emitEvent(const PluginEvent& event) = 0;
    
    // Plugin management
    virtual bool activate() = 0;
    virtual bool deactivate() = 0;
    virtual bool canLoad() const = 0;
    virtual std::string getError() const = 0;
};

// Plugin Factory Base Class
class IPluginFactory {
public:
    virtual ~IPluginFactory() = default;
    
    virtual std::string getFactoryName() const = 0;
    virtual std::string getFactoryVersion() const = 0;
    
    // Create plugin instance
    virtual std::unique_ptr<IPlugin> createPlugin(const PluginConfig& config) = 0;
    
    // Check if factory can handle plugin type
    virtual bool canCreate(const PluginConfig& config) const = 0;
    
    // Plugin metadata
    virtual std::string getDescription() const = 0;
    virtual std::vector<std::string> getSupportedComponents() const = 0;
};

// Plugin Manager - Central plugin management system
class PluginManager {
public:
    PluginManager();
    ~PluginManager();
    
    // Plugin management
    bool loadPlugin(const std::string& pluginPath);
    bool unloadPlugin(const std::string& pluginName);
    bool activatePlugin(const std::string& pluginName);
    bool deactivatePlugin(const std::string& pluginName);
    bool isPluginLoaded(const std::string& pluginName) const;
    bool isPluginActive(const std::string& pluginName) const;
    
    // Plugin discovery
    std::vector<std::string> getLoadedPlugins() const;
    std::vector<std::string> getActivePlugins() const;
    PluginConfig getPluginConfig(const std::string& pluginName) const;
    
    // Component access
    IPluginComponent* getComponent(const std::string& componentName) const;
    std::vector<IPluginComponent*> getComponents(PluginComponentType type) const;
    std::vector<IPluginComponent*> getComponentsByPlugin(const std::string& pluginName) const;
    
    // Component creation
    IPluginComponent* createComponent(const std::string& componentType, const std::map<std::string, std::string>& parameters = {});
    bool registerComponent(std::unique_ptr<IPluginComponent> component);
    
    // Factory management
    bool registerFactory(std::unique_ptr<IPluginFactory> factory);
    IPluginFactory* getFactory(const std::string& factoryName) const;
    
    // Configuration management
    bool updatePluginConfig(const std::string& pluginName, const PluginConfig& config);
    PluginConfig getDefaultPluginConfig(const std::string& pluginName) const;
    
    // Event system
    void addGlobalEventListener(std::function<void(const PluginEvent&)> listener);
    void removeGlobalEventListener(std::function<void(const PluginEvent&)> listener);
    void emitEvent(const PluginEvent& event);
    
    // Plugin lifecycle management
    bool initializeAllPlugins();
    bool shutdownAllPlugins();
    bool reloadPlugin(const std::string& pluginName);
    
    // Plugin discovery from file system
    std::vector<std::string> discoverPlugins(const std::string& directoryPath);
    
    // Plugin information and diagnostics
    std::string getPluginInfo(const std::string& pluginName) const;
    std::string getSystemInfo() const;
    bool validatePluginCompatibility(const std::string& pluginName) const;
    
    // Thread safety
    void lockPlugins() const;
    void unlockPlugins() const;
    bool isLocked() const { return locked_; }
    
    // Error handling
    std::string getLastError() const { return lastError_; }
    void clearError() { lastError_.clear(); }
    
    // Performance monitoring
    size_t getComponentCount() const;
    size_t getPluginCount() const;
    double getTotalLoadTime() const;
    double getTotalInitializeTime() const;
    
    // Plugin search and filtering
    std::vector<std::string> findPluginsByComponentType(PluginComponentType type) const;
    std::vector<std::string> findPluginsByCapability(const std::string& capability) const;
    
    // Plugin configuration validation
    bool validatePluginConfig(const PluginConfig& config) const;
    
    // Backup and restore
    bool backupPlugins(const std::string& backupPath);
    bool restorePlugins(const std::string& backupPath);
    
private:
    // Internal helper methods
    bool loadPluginFromFile(const std::string& pluginPath);
    bool parsePluginInfo(const std::string& pluginPath, PluginConfig& config);
    bool initializeDependencies(const PluginConfig& config);
    void cleanupPluginResources(const std::string& pluginName);
    bool checkPluginVersionCompatibility(const PluginConfig& config) const;
    void logPluginEvent(const PluginEvent& event);
    
    // Plugin factory registry
    std::map<std::string, std::unique_ptr<IPluginFactory>> factories_;
    
    // Loaded plugins
    std::map<std::string, std::unique_ptr<IPlugin>> loadedPlugins_;
    
    // Active plugins
    std::set<std::string> activePlugins_;
    
    // Components registry
    std::map<std::string, std::unique_ptr<IPluginComponent>> components_;
    std::map<PluginComponentType, std::vector<IPluginComponent*>> componentsByType_;
    
    // Event listeners
    std::vector<std::function<void(const PluginEvent&)>> globalEventListeners_;
    std::mutex eventListenersMutex_;
    
    // Thread synchronization
    mutable std::mutex pluginsMutex_;
    bool locked_ = false;
    
    // Error handling
    std::string lastError_;
    
    // Performance metrics
    double totalLoadTime_ = 0.0;
    double totalInitializeTime_ = 0.0;
    
    // Plugin search cache
    std::map<std::string, std::vector<std::string>> pluginSearchCache_;
    
    // Dependency graph
    std::map<std::string, std::set<std::string>> dependencyGraph_;
    
    // Plugin metadata cache
    std::map<std::string, PluginConfig> pluginMetadataCache_;
};

// Plugin Macro System for easy plugin development
#define NLM_PLUGIN_DECLARE(name, version, description) \
    namespace { \
        struct PluginRegistrar { \
            PluginRegistrar() { \
                PluginManager::getInstance().registerFactory(std::make_unique<name##Factory>()); \
            } \
        } pluginRegistrar; \
    } \
    
#define NLM_PLUGIN_COMPONENT(name, type) \
    class name : public IPluginComponent { \
    public: \
        static std::string getComponentName() { return #name; } \
        static PluginComponentType getComponentType() { return type; } \
        \\\
 \
        std::string getName() const override { return getComponentName(); } \
        std::string getVersion() const override { return "1.0.0"; } \
        PluginComponentType getType() const override { return getComponentType(); } \
        bool initialize() override { return true; } \
        void shutdown() override {} \
        bool isInitialized() const override { return true; } \
        IPluginComponent* createInstance() const override { \
            return new name(*this); \
        } \
    }; \
    \\\
 \\\
class name##Factory : public IPluginFactory { \
public: \
    std::string getFactoryName() const override { return #name##Factory; } \
    std::string getFactoryVersion() const override { return "1.0.0"; } \
    std::unique_ptr<IPlugin> createPlugin(const PluginConfig& config) override { \
        auto plugin = std::make_unique<name##Plugin>(config); \
        return std::unique_ptr<IPlugin>(plugin.release()); \
    } \
    bool canCreate(const PluginConfig& config) const override { \
        return config.name == #name; \
    } \
    std::string getDescription() const override { return "Factory for " #name " plugin"; } \
    std::vector<std::string> getSupportedComponents() const override { \
        return { #name }; \
    } \
}; \
class name##Plugin : public IPlugin { \
public: \
    name##Plugin(const PluginConfig& config) : config_(config) {} \
    std::string getName() const override { return config_.name; } \
    std::string getVersion() const override { return config_.version; } \
    std::string getDescription() const override { return config_.description; } \
    PluginConfig getConfig() const override { return config_; } \
    void setConfig(const PluginConfig& config) override { config_ = config; } \
    bool load() override { return true; } \
    bool initialize() override { \
        // Create component instance \
        component_ = std::make_unique<name>(config_); \
        return component_->initialize(); \
    } \
    bool shutdown() override { \
        if (component_) { \
            component_->shutdown(); \
        } \
        return true; \
    } \
    bool isLoaded() const override { return true; } \
    bool isInitialized() const override { \
        return component_ && component_->isInitialized(); \
    } \
    bool registerComponent(std::unique_ptr<IPluginComponent> component) override { \
        // Implementation for component registration \
        return false; \
    } \
    IPluginComponent* getComponent(const std::string& name) override { \
        if (component_ && component_->getName() == name) { \
            return component_.get(); \
        } \
        return nullptr; \
    } \
    std::vector<IPluginComponent*> getComponents(PluginComponentType type) override { \
        std::vector<IPluginComponent*> result; \
        if (component_ && component_->getType() == type) { \
            result.push_back(component_.get()); \
        } \
        return result; \
    } \
    bool unregisterComponent(const std::string& name) override { \
        return false; \
    } \
    void addEventListener(std::function<void(const PluginEvent&)> listener) override { \
        // Implementation for event listener addition \
    } \
    void removeEventListener(std::function<void(const PluginEvent&)> listener) override { \
        // Implementation for event listener removal \
    } \
    void emitEvent(const PluginEvent& event) override { \
        // Implementation for event emission \
    } \
    bool activate() override { return true; } \
    bool deactivate() override { return true; } \
    bool canLoad() const override { return true; } \
    std::string getError() const override { return ""; } \
    \\\
private: \
    PluginConfig config_; \
    std::unique_ptr<name> component_; \
};

// Global plugin manager instance
inline PluginManager& getPluginManager() {
    static PluginManager instance;
    return instance;
}

// Convenience functions
inline bool loadPlugin(const std::string& pluginPath) {
    return getPluginManager().loadPlugin(pluginPath);
}

inline bool unloadPlugin(const std::string& pluginName) {
    return getPluginManager().unloadPlugin(pluginName);
}

inline bool activatePlugin(const std::string& pluginName) {
    return getPluginManager().activatePlugin(pluginName);
}

inline bool deactivatePlugin(const std::string& pluginName) {
    return getPluginManager().deactivatePlugin(pluginName);
}

inline std::vector<std::string> getLoadedPlugins() {
    return getPluginManager().getLoadedPlugins();
}

inline IPluginComponent* getComponent(const std::string& componentName) {
    return getPluginManager().getComponent(componentName);
}

inline std::vector<IPluginComponent*> getComponents(PluginComponentType type) {
    return getPluginManager().getComponents(type);
}

// Plugin system initialization and shutdown
inline bool initializePluginSystem() {
    return getPluginManager().initializeAllPlugins();
}

inline bool shutdownPluginSystem() {
    return getPluginManager().shutdownAllPlugins();
}

inline std::string getSystemInfo() {
    return getPluginManager().getSystemInfo();
}

// Plugin discovery and configuration
alignas(64) std::vector<std::string> discoverPlugins(const std::string& directoryPath = "plugins") {
    return getPluginManager().discoverPlugins(directoryPath);
}

// Plugin utilities
inline bool validatePluginConfig(const PluginConfig& config) {
    return getPluginManager().validatePluginConfig(config);
}

inline bool backupPlugins(const std::string& backupPath) {
    return getPluginManager().backupPlugins(backupPath);
}

inline bool restorePlugins(const std::string& backupPath) {
    return getPluginManager().restorePlugins(backupPath);
}

} // namespace nlm

// Global plugin manager instance declaration
inline nlm::PluginManager& getPluginManagerInstance() {
    return nlm::getPluginManager();
}

// Plugin system entry point macro
#define NLM_PLUGIN_MAIN() \
    int main() { \
        nlm::PluginManager& pluginManager = nlm::getPluginManager(); \
        \
        // Load and initialize plugins \
        std::vector<std::string> loadedPlugins = pluginManager.getLoadedPlugins(); \
        if (!loadedPlugins.empty()) { \
            if (!pluginManager.initializeAllPlugins()) { \
                std::cerr << "Failed to initialize plugins" << std::endl; \
                return 1; \
            } \
        } \
        \
        // Main application logic would go here \
        // For now, just exit successfully \
        if (!pluginManager.shutdownAllPlugins()) { \
            std::cerr << "Failed to shutdown plugins" << std::endl; \
            return 1; \
        } \
        \
        return 0; \
    }

// Plugin compilation helper
#ifdef NLM_PLUGIN_EXPORTS
    #define NLM_PLUGIN_API __declspec(dllexport)
#else
    #define NLM_PLUGIN_API __declspec(dllimport)
#endif

// Plugin build helper
#ifdef NLM_PLUGIN_BUILD
    #define NLM_PLUGIN_EXPORT __declspec(dllexport)
#else
    #define NLM_PLUGIN_EXPORT __declspec(dllimport)
#endif