// Plugin System - Core Interfaces and Base Classes
// Provides plugin architecture for extensibility and component management

#pragma once

#include <string>
#include <memory>
#include <map>
#include <vector>
#include <functional>
#include <typeindex>
#include <unordered_map>
#include <mutex>
#include <atomic>

namespace nlm {

// Plugin system version
#define NLM_PLUGIN_VERSION_MAJOR 1
#define NLM_PLUGIN_VERSION_MINOR 0
#define NLM_PLUGIN_VERSION_PATCH 0

// Plugin type enumeration
enum class PluginType {
    COMPONENT,      // Regular component plugin
    SYSTEM,         // System-level plugin
    UTILITY,        // Utility plugin
    EXTENSER,       // Extender plugin
    BRIDGE,         // Bridge plugin
    UNKNOWN
};

// Plugin loading result
enum class PluginLoadResult {
    SUCCESS,
    FAILED,
    ALREADY_LOADED,
    DEPENDENCY_ERROR,
    VERSION_MISMATCH,
    ACCESS_DENIED
};

// Plugin lifecycle state
enum class PluginState {
    UNLOADED,
    LOADING,
    LOADED,
    UNLOADING,
    ERROR
};

// Plugin interface - base class for all plugins
class IPlugin {
public:
    virtual ~IPlugin() = default;
    
    // Get plugin metadata
    virtual const std::string& getName() const = 0;
    virtual const std::string& getVersion() const = 0;
    virtual const std::string& getDescription() const = 0;
    virtual PluginType getType() const = 0;
    virtual const std::string& getAuthor() const = 0;
    virtual const std::string& getContact() const = 0;
    
    // Get plugin dependencies
    virtual std::vector<std::string> getDependencies() const = 0;
    virtual std::vector<std::string> getOptionalDependencies() const = 0;
    
    // Get required minimum system version
    virtual std::string getMinSystemVersion() const = 0;
    
    // Plugin lifecycle methods
    virtual PluginLoadResult onLoad() = 0;
    virtual void onUnload() = 0;
    virtual void onInitialize() = 0;
    virtual void onShutdown() = 0;
    
    // State management
    virtual PluginState getState() const = 0;
    
    // Configuration interface
    virtual void configure(const std::unordered_map<std::string, std::string>& config) = 0;
    
    // Plugin-specific configuration validation
    virtual bool validateConfiguration(const std::unordered_map<std::string, std::string>& config) const = 0;
    
    // Health check
    virtual bool isHealthy() const = 0;
    virtual std::string getHealthStatus() const = 0;
    
    // Statistics and metrics
    virtual std::unordered_map<std::string, double> getMetrics() const = 0;
    
    // Event handling
    virtual void onEvent(const std::string& eventName, const std::unordered_map<std::string, std::string>& parameters) = 0;
};

// Component interface - base class for core components
class IComponent {
public:
    virtual ~IComponent() = default;
    
    // Component identification
    virtual const std::string& getName() const = 0;
    virtual const std::string& getCategory() const = 0;
    virtual std::type_index getComponentType() const = 0;
    
    // Component lifecycle
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual bool isInitialized() const = 0;
    
    // Component configuration
    virtual void configure(const std::unordered_map<std::string, std::string>& config) = 0;
    
    // Component-specific operations
    virtual bool validateConfiguration(const std::unordered_map<std::string, std::string>& config) const = 0;
    virtual std::unordered_map<std::string, std::string> getDefaultConfiguration() const = 0;
    
    // Component statistics
    virtual std::unordered_map<std::string, double> getMetrics() const = 0;
    
    // Component health
    virtual bool isHealthy() const = 0;
    virtual std::string getHealthStatus() const = 0;
};

// Plugin factory function type
using PluginFactoryFunc = std::function<std::unique_ptr<IPlugin>()>;

// Component factory function type
using ComponentFactoryFunc = std::function<std::unique_ptr<IComponent>()>;

// Plugin information structure
struct PluginInfo {
    std::string name;
    std::string version;
    std::string description;
    PluginType type;
    std::string author;
    std::string contact;
    std::vector<std::string> dependencies;
    std::vector<std::string> optionalDependencies;
    std::string minSystemVersion;
    PluginState state;
    std::unordered_map<std::string, std::string> configuration;
    double loadTime;
    std::unordered_map<std::string, double> metrics;
};

// Component registration information
struct ComponentInfo {
    std::string name;
    std::string category;
    std::type_index type;
    ComponentFactoryFunc factory;
    std::unordered_map<std::string, std::string> defaultConfig;
    bool isBuiltIn;
};

// Plugin metadata for plugin discovery
struct PluginMetadata {
    std::string name;
    std::string version;
    PluginType type;
    std::string description;
    std::string author;
    std::vector<std::string> dependencies;
    std::string filePath;
    std::string className;
    bool isBuiltIn;
};

// Plugin system statistics
struct PluginSystemStats {
    size_t totalPlugins;
    size_t loadedPlugins;
    size_t failedPlugins;
    double totalLoadTime;
    std::unordered_map<std::string, double> componentMetrics;
    size_t activeComponents;
    size_t totalComponents;
};

// Plugin loading options
struct PluginLoadOptions {
    bool autoLoadDependencies = true;
    bool validateDependencies = true;
    bool autoInitialize = true;
    bool skipAlreadyLoaded = false;
    std::unordered_map<std::string, std::string> pluginConfiguration;
    double timeoutSeconds = 30.0;
};

// Plugin event types
enum class PluginEventType {
    PLUGIN_LOADED,
    PLUGIN_UNLOADED,
    PLUGIN_ERROR,
    COMPONENT_REGISTERED,
    COMPONENT_UNREGISTERED,
    SYSTEM_INITIALIZED,
    SYSTEM_SHUTDOWN
};

// Plugin event structure
struct PluginEvent {
    PluginEventType type;
    std::string pluginName;
    std::string componentName;
    std::unordered_map<std::string, std::string> parameters;
    std::chrono::system_clock::time_point timestamp;
};

// Plugin event callback type
using PluginEventCallback = std::function<void(const PluginEvent& event)>;

// Component factory interface
class IComponentFactory {
public:
    virtual ~IComponentFactory() = default;
    
    // Create component instance
    virtual std::unique_ptr<IComponent> createComponent(const std::string& config) = 0;
    
    // Get component information
    virtual ComponentInfo getComponentInfo() const = 0;
    
    // Check if factory can create component of given type
    virtual bool canCreateComponent(const std::type_index& type) const = 0;
};

// Plugin loader interface
class IPluginLoader {
public:
    virtual ~IPluginLoader() = default;
    
    // Load plugin from file
    virtual PluginLoadResult loadPlugin(const std::string& filePath, const PluginLoadOptions& options = PluginLoadOptions()) = 0;
    
    // Unload plugin
    virtual bool unloadPlugin(const std::string& pluginName) = 0;
    
    // Get plugin information
    virtual std::optional<PluginInfo> getPluginInfo(const std::string& pluginName) const = 0;
    
    // Check if plugin is loaded
    virtual bool isPluginLoaded(const std::string& pluginName) const = 0;
    
    // Get all loaded plugins
    virtual std::vector<std::string> getLoadedPlugins() const = 0;
    
    // Get all registered components
    virtual std::vector<ComponentInfo> getAllComponents() const = 0;
    
    // Get component by name
    virtual std::optional<ComponentInfo> getComponent(const std::string& componentName) const = 0;
    
    // Create component instance
    virtual std::unique_ptr<IComponent> createComponent(const std::string& componentName, const std::string& config = "") = 0;
};

// Plugin registry interface
class IPluginRegistry {
public:
    virtual ~IPluginRegistry() = default;
    
    // Register built-in plugin
    virtual bool registerBuiltInPlugin(const PluginMetadata& metadata, PluginFactoryFunc factory) = 0;
    
    // Register built-in component
    virtual bool registerBuiltInComponent(const ComponentInfo& info) = 0;
    
    // Register plugin from file (discovery)
    virtual bool discoverAndRegisterPlugin(const std::string& directory) = 0;
    
    // Get all registered plugins
    virtual std::vector<PluginMetadata> getAllPlugins() const = 0;
    
    // Get all registered components
    virtual std::vector<ComponentInfo> getAllComponents() const = 0;
    
    // Get plugin metadata
    virtual std::optional<PluginMetadata> getPluginMetadata(const std::string& pluginName) const = 0;
    
    // Get component info
    virtual std::optional<ComponentInfo> getComponentInfo(const std::string& componentName) const = 0;
    
    // Check if plugin is registered
    virtual bool isPluginRegistered(const std::string& pluginName) const = 0;
    
    // Check if component is registered
    virtual bool isComponentRegistered(const std::string& componentName) const = 0;
    
    // Plugin event handling
    virtual void addEventCallback(PluginEventCallback callback) = 0;
    virtual void removeEventCallback(PluginEventCallback callback) = 0;
    
    // Get system statistics
    virtual PluginSystemStats getSystemStats() const = 0;
};

// Plugin manager interface
class IPluginManager {
public:
    virtual ~IPluginManager() = default;
    
    // Load plugin from file
    virtual PluginLoadResult loadPlugin(const std::string& filePath, const PluginLoadOptions& options = PluginLoadOptions()) = 0;
    
    // Unload plugin
    virtual bool unloadPlugin(const std::string& pluginName) = 0;
    
    // Initialize all loaded plugins
    virtual bool initializeAll() = 0;
    
    // Shutdown all plugins
    virtual void shutdownAll() = 0;
    
    // Get plugin status
    virtual PluginState getPluginState(const std::string& pluginName) const = 0;
    
    // Get plugin information
    virtual std::optional<PluginInfo> getPluginInfo(const std::string& pluginName) const = 0;
    
    // Get all loaded plugins
    virtual std::vector<std::string> getLoadedPlugins() const = 0;
    
    // Create component
    virtual std::unique_ptr<IComponent> createComponent(const std::string& componentName, const std::string& config = "") = 0;
    
    // Create component by type
    virtual std::unique_ptr<IComponent> createComponent(const std::type_index& type, const std::string& config = "") = 0;
    
    // Get component information
    virtual std::optional<ComponentInfo> getComponentInfo(const std::string& componentName) const = 0;
    
    // Get all available components
    virtual std::vector<ComponentInfo> getAvailableComponents() const = 0;
    
    // Get system statistics
    virtual PluginSystemStats getSystemStats() const = 0;
    
    // Save plugin configuration
    virtual bool saveConfiguration(const std::string& filePath) const = 0;
    
    // Load plugin configuration
    virtual bool loadConfiguration(const std::string& filePath) = 0;
};

} // namespace nlm