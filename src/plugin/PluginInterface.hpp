// Plugin Interface - Core extensibility framework for NLM
// This file provides the foundation for plugin-based extensibility in the NLM neural simulation framework

#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <functional>
#include <any>
#include <typeindex>
#include <mutex>
#include <functional>

namespace nlm {
namespace plugin {

// Plugin lifecycle states
enum class PluginState {
    Unloaded,
    Loading,
    Loaded,
    Running,
    Stopping,
    Unloading
};

// Plugin type classifications
enum class PluginType {
    Core,
    Extension,
    Experimental,
    User
};

// Plugin capability flags
enum class PluginCapability {
    None = 0,
    NeuralModel = 1 << 0,
    PlasticityRule = 1 << 1,
    MemorySystem = 1 << 2,
    Neuromodulator = 1 << 3,
    Sensor = 1 << 4,
    MotorController = 1 << 5,
    Environment = 1 << 6,
    Visualization = 1 << 7,
    LearningAlgorithm = 1 << 8,
    All = NeuralModel | PlasticityRule | MemorySystem | Neuromodulator | 
           Sensor | MotorController | Environment | Visualization | LearningAlgorithm
};

// Plugin metadata for discovery and management
struct PluginMetadata {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string license;
    PluginType type;
    PluginCapability capabilities;
    std::vector<std::string> dependencies;
    std::string website;
    std::string contact;
    std::unordered_map<std::string, std::any> configuration;
};

// Plugin event types for inter-plugin communication
enum class PluginEventType {
    PluginLoaded,
    PluginUnloaded,
    PluginError,
    ConfigurationChanged,
    NeuralEvent,
    LearningEvent,
    SimulationStep
};

// Plugin event data structure
struct PluginEvent {
    PluginEventType type;
    std::string sourcePlugin;
    double timestamp;
    std::unordered_map<std::string, std::any> data;
    bool propagate = true;
};

// Plugin event callback function type
using PluginEventCallback = std::function<void(const PluginEvent&)>;

// Base plugin interface
class IPlugin {
public:
    virtual ~IPlugin() = default;
    
    // Plugin lifecycle management
    virtual PluginMetadata getMetadata() const = 0;
    virtual bool initialize(const std::unordered_map<std::string, std::any>& config) = 0;
    virtual bool start() = 0;
    virtual bool stop() = 0;
    virtual bool shutdown() = 0;
    
    // Plugin capability queries
    virtual bool hasCapability(PluginCapability capability) const = 0;
    virtual std::vector<PluginCapability> getCapabilities() const = 0;
    
    // Event handling
    virtual void onEvent(const PluginEvent& event) {}
    virtual void registerEventCallback(const PluginEventCallback& callback) = 0;
    
    // Configuration management
    virtual void updateConfiguration(const std::unordered_map<std::string, std::any>& config) {}
    virtual std::unordered_map<std::string, std::any> getConfiguration() const { return {}; }
    
    // Resource management
    virtual void cleanup() {}
    virtual size_t getMemoryUsage() const { return 0; }
    virtual double getPerformanceImpact() const { return 0.0; }
};

// Template-based plugin factory interface
class IPluginFactory {
public:
    virtual ~IPluginFactory() = default;
    virtual std::unique_ptr<IPlugin> createPlugin() = 0;
    virtual PluginMetadata getMetadata() const = 0;
    virtual bool validateConfiguration(const std::unordered_map<std::string, std::any>& config) const = 0;
};

// Typed plugin factory for specific plugin types
template<typename T>
class PluginFactory : public IPluginFactory {
public:
    using PluginCreateFunc = std::function<std::unique_ptr<T>()>;
    
    PluginFactory(const std::string& name, const std::string& version, 
                  const std::string& description,
                  PluginCapability capabilities,
                  PluginCreateFunc createFunc)
        : metadata_({name, version, description, "Unknown", "MIT", PluginType::Extension, capabilities, {}, "", ""}),
          createFunc_(std::move(createFunc)) {}
    
    std::unique_ptr<IPlugin> createPlugin() override {
        return createFunc_();
    }
    
    PluginMetadata getMetadata() const override {
        return metadata_;
    }
    
    bool validateConfiguration(const std::unordered_map<std::string, std::any>& config) const override {
        // Default validation - can be overridden by derived classes
        return true;
    }
    
protected:
    PluginMetadata metadata_;
    PluginCreateFunc createFunc_;
};

// Plugin manager for loading, unloading, and managing plugins
class PluginManager {
public:
    PluginManager();
    ~PluginManager();
    
    // Plugin loading and unloading
    bool loadPlugin(const std::string& pluginPath);
    bool unloadPlugin(const std::string& pluginName);
    bool unloadPlugin(std::shared_ptr<IPlugin> plugin);
    
    // Plugin queries
    bool hasPlugin(const std::string& pluginName) const;
    std::shared_ptr<IPlugin> getPlugin(const std::string& pluginName) const;
    std::vector<std::shared_ptr<IPlugin>> getPlugins() const;
    std::vector<std::shared_ptr<IPlugin>> getPluginsWithCapability(PluginCapability capability) const;
    
    // Plugin lifecycle management
    bool startAllPlugins();
    bool stopAllPlugins();
    bool shutdownAllPlugins();
    
    // Configuration management
    void updatePluginConfiguration(const std::string& pluginName, 
                                   const std::unordered_map<std::string, std::any>& config);
    std::unordered_map<std::string, std::any> getPluginConfiguration(const std::string& pluginName) const;
    
    // Event system
    void subscribeToEvents(const std::string& pluginName, const PluginEventCallback& callback);
    void publishEvent(const PluginEvent& event);
    
    // Plugin discovery
    std::vector<PluginMetadata> discoverAvailablePlugins() const;
    bool registerFactory(const std::string& name, std::unique_ptr<IPluginFactory> factory);
    
    // Statistics and monitoring
    size_t getPluginCount() const;
    size_t getMemoryUsage() const;
    double getAveragePerformanceImpact() const;
    
    // Configuration management for the entire plugin system
    void setGlobalConfiguration(const std::unordered_map<std::string, std::any>& config);
    std::unordered_map<std::string, std::any> getGlobalConfiguration() const;
    
    // Error handling and reporting
    std::string getLastError() const;
    void clearError();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Prevent copying
    PluginManager(const PluginManager&) = delete;
    PluginManager& operator=(const PluginManager&) = delete;
};

// Plugin registry for built-in plugins
class PluginRegistry {
public:
    static PluginRegistry& getInstance();
    
    // Register a built-in plugin factory
    void registerPluginFactory(const std::string& name, std::unique_ptr<IPluginFactory> factory);
    
    // Get plugin factory by name
    std::unique_ptr<IPluginFactory> getPluginFactory(const std::string& name) const;
    
    // Get all registered factories
    std::vector<std::pair<std::string, std::unique_ptr<IPluginFactory>>> getAllFactories() const;
    
    // Plugin lifecycle management
    bool loadBuiltInPlugin(const std::string& name);
    bool unloadBuiltInPlugin(const std::string& name);
    
    // Clear all registered plugins
    void clear();

private:
    PluginRegistry() = default;
    ~PluginRegistry() = default;
    
    // Prevent copying
    PluginRegistry(const PluginRegistry&) = delete;
    PluginRegistry& operator=(const PluginRegistry&) = delete;
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Plugin utilities for common operations
namespace utils {
    // Plugin version parsing and comparison
    bool parseVersion(const std::string& versionStr, std::vector<int>& versionParts);
    bool isVersionCompatible(const std::string& required, const std::string& available);
    
    // Configuration validation helpers
    template<typename T>
    bool validateConfigValue(const std::string& key, const std::any& value, 
                           const std::string& typeName, T& outValue);
    
    // Plugin loading utilities
    std::string extractPluginName(const std::string& pluginPath);
    std::string extractPluginVersion(const std::string& pluginPath);
    std::string getPluginDescription(const std::string& pluginPath);
    
    // Thread-safe plugin operations
    template<typename Func>
    auto threadSafeExecute(Func&& func) -> decltype(func()) {
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);
        return func();
    }
}

} // namespace plugin
} // namespace nlm

// Plugin factory macro for easy plugin registration
#define REGISTER_PLUGIN(PluginClass, Name, Version, Description, Capabilities) \
    class PluginClass##Factory : public nlm::plugin::PluginFactory<PluginClass> { \
    public: \
        PluginClass##Factory() \
            : PluginFactory<PluginClass>(Name, Version, Description, Capabilities, \
                                        []() { return std::make_unique<PluginClass>(); }) {} \
    }; \
    \
    static bool register_##PluginClass##_plugin() { \
        static PluginClass##Factory factory; \
        return nlm::plugin::PluginRegistry::getInstance().registerPluginFactory(Name, std::make_unique<PluginClass##Factory>(factory)); \
    } \
    \
    namespace { \
        bool _##PluginClass##_plugin_reg = register_##PluginClass##_plugin(); \
    }

// Plugin capability bit operations
namespace nlm {
namespace plugin {
    inline PluginCapability operator|(PluginCapability a, PluginCapability b) {
        return static_cast<PluginCapability>(static_cast<int>(a) | static_cast<int>(b));
    }
    
    inline PluginCapability operator&(PluginCapability a, PluginCapability b) {
        return static_cast<PluginCapability>(static_cast<int>(a) & static_cast<int>(b));
    }
    
    inline bool hasCapability(PluginCapability capabilities, PluginCapability capability) {
        return (capabilities & capability) == capability;
    }
}
} // namespace nlm

// Convenience typedefs
namespace nlm::plugin {
    using PluginPtr = std::shared_ptr<IPlugin>;
    using PluginFactoryPtr = std::unique_ptr<IPluginFactory>;
}
