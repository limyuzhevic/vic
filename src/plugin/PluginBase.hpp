//! Plugin Architecture for NLM Neural Simulation Project
//! 
//! This module implements a comprehensive plugin system that enables extensibility
//! for the NLM neural simulation project. The plugin system allows users to add
//! custom components without modifying the core Brain class.
//! 
//! Key Features:
//! - Dynamic plugin loading from shared libraries
//! - Abstract plugin interfaces for different component types
//! - Thread-safe plugin management
//! - Plugin lifecycle management (load, initialize, shutdown, unload)
//! - Dependency resolution between plugins
//! - Version management for compatibility
//! - Event-driven inter-plugin communication
//! - Security sandboxing for untrusted plugins
//! - Hook system for extending core functionality
//! 
//! Plugin Types:
//! - PlasticityRulePlugin: Extends plasticity mechanisms
//! - NeuromodulatorPlugin: Adds/modifies neuromodulation systems
//! - DevelopmentPlugin: Alters developmental processes
//! - MemoryPlugin: Adds new memory architectures
//! - PredictionPlugin: Extends prediction capabilities
//! - CognitionPlugin: Adds/modifies cognitive mechanisms
//! - SensoryPlugin: Modifies sensory processing
//! - MotorPlugin: Modifies motor output
//! - IntegrationPlugin: Integrates with external systems

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <mutex>
#include <atomic>
#include <thread>
#include <chrono>
#include <variant>
#include <any>
#include <type_traits>
#include <dlfcn.h>

namespace nlm {

// Forward declarations
class PluginBase;
class PluginManager;
class PluginRegistry;
class HookSystem;
class ComponentFactory;
class EventSystem;
class PluginConfiguration;
class PluginSecurityManager;
class PluginUpdateManager;

// Plugin version information
struct PluginVersion {
    int major;
    int minor;
    int patch;
    std::string preRelease;
    std::string build;
    
    PluginVersion(int major = 0, int minor = 0, int patch = 0,
                  const std::string& preRelease = "",
                  const std::string& build = "")
        : major(major), minor(minor), patch(patch),
          preRelease(preRelease), build(build) {}
    
    bool operator==(const PluginVersion& other) const {
        return major == other.major && minor == other.minor && patch == other.patch;
    }
    
    bool operator<(const PluginVersion& other) const {
        if (major != other.major) return major < other.major;
        if (minor != other.minor) return minor < other.minor;
        return patch < other.patch;
    }
};

// Plugin metadata
struct PluginMetadata {
    std::string name;
    std::string description;
    PluginVersion version;
    std::string author;
    std::string license;
    std::string pluginType; // e.g., "PlasticityRule", "Neuromodulator", etc.
    std::vector<std::string> dependencies;
    std::vector<std::string> optionalDependencies;
    std::unordered_map<std::string, std::any> configSchema;
    std::unordered_map<std::string, std::string> capabilities;
    std::unordered_map<std::string, std::string> tags;
    std::chrono::system_clock::time_point created;
    std::chrono::system_clock::time_point lastUpdated;
    bool enabled;
    std::string filePath;
    std::string entryPoint;
    
    PluginMetadata()
        : version(1, 0, 0), enabled(true), created(std::chrono::system_clock::now()) {}
};

// Plugin lifecycle states
enum class PluginState {
    Unloaded,
    Loading,
    Loaded,
    Initializing,
    Initialized,
    Running,
    ShuttingDown,
    Unloading,
    Failed,
    Disabled
};

// Plugin events
enum class PluginEvent {
    Load,
    Initialize,
    Start,
    Update,
    Stop,
    Shutdown,
    Unload,
    Error,
    ConfigurationChanged,
    DependencyResolved,
    SecurityViolation
};

// Plugin configuration
struct PluginConfig {
    std::string name;
    std::string description;
    std::unordered_map<std::string, std::any> parameters;
    bool enabled;
    std::unordered_map<std::string, std::any> metadata;
    
    PluginConfig() : enabled(true) {}
    
    template<typename T>
    T getParameter(const std::string& key, const T& defaultValue = T{}) const {
        auto it = parameters.find(key);
        if (it != parameters.end()) {
            try {
                return std::any_cast<T>(it->second);
            } catch (const std::bad_any_cast&) {
                // Return default if type mismatch
                return defaultValue;
            }
        }
        return defaultValue;
    }
    
    template<typename T>
    void setParameter(const std::string& key, const T& value) {
        parameters[key] = value;
    }
};

// Plugin event data
struct PluginEventData {
    PluginEvent event;
    std::string pluginName;
    std::unordered_map<std::string, std::any> data;
    std::string errorMessage;
    std::chrono::system_clock::time_point timestamp;
    
    PluginEventData() : event(PluginEvent::Error), timestamp(std::chrono::system_clock::now()) {}
};

// Plugin hook points
enum class PluginHook {
    // Plasticity hooks
    PlasticityRuleAdded,
    PlasticityRuleUpdated,
    PlasticityRuleRemoved,
    
    // Neuromodulation hooks
    NeuromodulatorAdded,
    NeuromodulatorUpdated,
    NeuromodulatorRemoved,
    
    // Development hooks
    DevelopmentalStageChanged,
    DevelopmentRuleAdded,
    DevelopmentRuleUpdated,
    DevelopmentRuleRemoved,
    
    // Memory hooks
    MemorySystemAdded,
    MemorySystemUpdated,
    MemorySystemRemoved,
    
    // Prediction hooks
    PredictionSystemAdded,
    PredictionSystemUpdated,
    PredictionSystemRemoved,
    
    // Cognition hooks
    CognitionSystemAdded,
    CognitionSystemUpdated,
    CognitionSystemRemoved,
    
    // Sensory/Motor hooks
    SensorySystemAdded,
    SensorySystemUpdated,
    SensorySystemRemoved,
    MotorSystemAdded,
    MotorSystemUpdated,
    MotorSystemRemoved,
    
    // Integration hooks
    IntegrationPointAdded,
    IntegrationPointUpdated,
    IntegrationPointRemoved,
    
    // System-wide hooks
    SystemInitialize,
    SystemUpdate,
    SystemShutdown,
    ConfigurationChanged,
    PluginLoaded,
    PluginUnloaded
};

// Hook event data
struct HookEventData {
    PluginHook hook;
    std::string pluginName;
    std::unordered_map<std::string, std::any> data;
    std::vector<std::any> results;
    bool preventDefault;
    bool stopPropagation;
    
    HookEventData() : preventDefault(false), stopPropagation(false) {}
};

// Abstract base class for all plugins
class PluginBase {
public:
    virtual ~PluginBase() = default;
    
    // Plugin identification
    virtual const PluginMetadata& getMetadata() const = 0;
    virtual const PluginConfig& getConfig() const = 0;
    virtual PluginConfig& getConfig() = 0;
    
    // Plugin lifecycle
    virtual bool load(const PluginConfig& config) = 0;
    virtual bool initialize() = 0;
    virtual void start() = 0;
    virtual void update(double deltaTime) = 0;
    virtual void stop() = 0;
    virtual bool shutdown() = 0;
    virtual bool unload() = 0;
    
    // Plugin state
    virtual PluginState getState() const = 0;
    virtual std::string getStateString() const = 0;
    
    // Plugin information
    virtual const std::string& getName() const = 0;
    virtual const std::string& getDescription() const = 0;
    virtual PluginVersion getVersion() const = 0;
    
    // Event handling
    virtual void onEvent(const PluginEventData& event) = 0;
    
    // Hook handling
    virtual void onHook(const HookEventData& hook) = 0;
    
    // Dependency management
    virtual const std::vector<std::string>& getDependencies() const = 0;
    virtual const std::vector<std::string>& getOptionalDependencies() const = 0;
    
    // Serialization support
    virtual std::string serializeConfig() const = 0;
    virtual bool deserializeConfig(const std::string& data) = 0;
    
    // Plugin-specific operations
    virtual bool validateConfiguration() = 0;
    virtual std::string getConfigurationError() const = 0;
    
    // Security and permissions
    virtual bool checkSecurityPermissions() const = 0;
    virtual std::vector<std::string> getRequiredPermissions() const = 0;
    virtual std::vector<std::string> getOptionalPermissions() const = 0;
    
protected:
    PluginState state_;
    PluginConfig config_;
    mutable std::mutex stateMutex_;
    
    PluginBase() : state_(PluginState::Unloaded) {}
};

// Template plugin base for specific plugin types
class PlasticityRulePlugin : public PluginBase {
public:
    virtual void addPlasticityRule(class PlasticityRule* rule) = 0;
    virtual bool removePlasticityRule(const std::string& name) = 0;
    virtual const std::vector<class PlasticityRule*>& getPlasticityRules() const = 0;
    virtual void updatePlasticityRules(class Brain* brain) = 0;
};

class NeuromodulatorPlugin : public PluginBase {
public:
    virtual void addNeuromodulator(class Neuromodulator* modulator) = 0;
    virtual bool removeNeuromodulator(const std::string& name) = 0;
    virtual const std::vector<class Neuromodulator*>& getNeuromodulators() const = 0;
    virtual void updateNeuromodulators(class Brain* brain, double deltaTime) = 0;
};

class DevelopmentPlugin : public PluginBase {
public:
    virtual void setDevelopmentalStage(class DevelopmentalStage stage) = 0;
    virtual class DevelopmentalStage getDevelopmentalStage() const = 0;
    virtual void addDevelopmentRule(class DevelopmentRule* rule) = 0;
    virtual bool removeDevelopmentRule(const std::string& name) = 0;
    virtual const std::vector<class DevelopmentRule*>& getDevelopmentRules() const = 0;
};

// Plugin factory interface
class IPluginFactory {
public:
    virtual ~IPluginFactory() = default;
    virtual std::unique_ptr<PluginBase> createPlugin() = 0;
    virtual const PluginMetadata& getMetadata() const = 0;
    virtual bool canCreate() const = 0;
};

// Plugin loader interface
class IPluginLoader {
public:
    virtual ~IPluginLoader() = default;
    virtual bool loadPlugin(const std::string& filePath) = 0;
    virtual bool unloadPlugin(const std::string& pluginName) = 0;
    virtual std::vector<std::string> getLoadedPlugins() const = 0;
    virtual bool isPluginLoaded(const std::string& pluginName) const = 0;
    virtual PluginState getPluginState(const std::string& pluginName) const = 0;
};

// Plugin registry interface
class IPluginRegistry {
public:
    virtual ~IPluginRegistry() = default;
    virtual bool registerPlugin(const PluginMetadata& metadata) = 0;
    virtual bool unregisterPlugin(const std::string& pluginName) = 0;
    virtual const PluginMetadata* findPlugin(const std::string& pluginName) const = 0;
    virtual std::vector<const PluginMetadata*> getAllPlugins() const = 0;
    virtual std::vector<const PluginMetadata*> getPluginsOfType(const std::string& pluginType) const = 0;
    virtual std::vector<const PluginMetadata*> getPluginsWithTag(const std::string& tag) const = 0;
    virtual bool updatePluginMetadata(const std::string& pluginName, const PluginMetadata& metadata) = 0;
};

// Event listener interface
class IEventListener {
public:
    virtual ~IEventListener() = default;
    virtual void onPluginEvent(const PluginEventData& event) = 0;
    virtual void onHookEvent(const HookEventData& event) = 0;
    virtual const std::string& getName() const = 0;
};

// Dynamic plugin loading utilities
namespace PluginUtils {
    template<typename T>
    std::unique_ptr<T> loadPluginFromLibrary(const std::string& filePath) {
        // Open shared library
        void* handle = dlopen(filePath.c_str(), RTLD_LAZY);
        if (!handle) {
            return nullptr;
        }
        
        // Find factory function
        std::string factoryName = "create_" + std::string(typeid(T).name());
        void* factory = dlsym(handle, factoryName.c_str());
        if (!factory) {
            dlclose(handle);
            return nullptr;
        }
        
        // Cast factory function
        typedef std::unique_ptr<T>(*)FactoryFunc();
        FactoryFunc createFunc = reinterpret_cast<FactoryFunc>(factory);
        
        // Create plugin instance
        std::unique_ptr<T> plugin = createFunc();
        if (!plugin) {
            dlclose(handle);
            return nullptr;
        }
        
        return plugin;
    }
    
    std::string getLibraryExtension();
    std::string getPluginSymbolPrefix();
    bool isPluginFile(const std::string& filePath);
    PluginVersion parsePluginVersion(const std::string& versionString);
    std::string getPluginTypeFromName(const std::string& pluginName);
}

} // namespace nlm