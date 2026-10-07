#pragma once

/**
 * NLM Plugin System - Extensible Component Architecture
 *
 * This plugin system enables dynamic loading of brain components,
 * neuromodulators, memory systems, and other NLM components at runtime.
 * It provides a clean interface for extending the NLM with custom functionality.
 *
 * Key Features:
 * - Plugin interface with version compatibility checking
 * - Component factory pattern for creating plugin instances
 * - Plugin metadata for discovery and registration
 * - Safe plugin loading with error handling
 * - Thread-safe plugin management
 * - Support for both static and dynamic plugins
 *
 * Usage:
 *   // Create a plugin
 *   class MyCustomComponent : public nlm::BrainComponent {
 *   public:
 *       bool initialize() override { /* ... */ }
 *       void step() override { /* ... */ }
 *       // ... other required methods
 *   };
 *
 *   // Register the plugin
 *   NLM_PLUGIN_EXPORT MyCustomComponentPlugin myPlugin;
 */

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>
#include <mutex>
#include <atomic>
#include <type_traits>

namespace nlm {

// Plugin version for compatibility checking
struct PluginVersion {
    uint32_t major;
    uint32_t minor;
    uint32_t patch;
    
    PluginVersion() : major(1), minor(0), patch(0) {}
    PluginVersion(uint32_t m, uint32_t n, uint32_t p) 
        : major(m), minor(n), patch(p) {}
    
    bool operator==(const PluginVersion& other) const {
        return major == other.major && minor == other.minor && patch == other.patch;
    }
    
    bool compatibleWith(const PluginVersion& other) const {
        return major == other.major;
    }
};

// Plugin metadata for discovery
struct PluginMetadata {
    std::string name;
    std::string description;
    PluginVersion version;
    std::string author;
    std::string category; // "memory", "neuromodulation", "prediction", etc.
    std::vector<std::string> dependencies;
    bool enabledByDefault;
    std::string documentationUrl;
    
    PluginMetadata()
        : version(1, 0, 0), enabledByDefault(true) {}
};

// BrainComponent interface - all plugins must implement this
class BrainComponent {
public:
    virtual ~BrainComponent() = default;
    
    // Component identification
    virtual std::string getName() const = 0;
    virtual PluginVersion getVersion() const = 0;
    virtual std::string getDescription() const = 0;
    
    // Lifecycle management
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual void reset() = 0;
    
    // Main processing step
    virtual void step(TimestepDuration dt) = 0;
    
    // Configuration interface
    virtual bool configure(const std::string& key, const std::string& value) = 0;
    virtual std::string getConfig(const std::string& key) const = 0;
    
    // Statistics and monitoring
    virtual std::string getStats() const = 0;
    virtual double getPerformanceMetric() const = 0;
    
    // Integration with parent brain
    virtual void setBrain(std::shared_ptr<Brain> brain) = 0;
    virtual std::shared_ptr<Brain> getBrain() const = 0;
    
    // Dependency management
    virtual std::vector<std::string> getDependencies() const = 0;
    virtual bool canActivate() const = 0;
};

// Plugin factory interface for creating component instances
class PluginFactory {
public:
    virtual ~PluginFactory() = default;
    virtual std::unique_ptr<BrainComponent> createInstance() = 0;
    virtual PluginMetadata getMetadata() const = 0;
    virtual bool validate() const = 0;
};

// Concrete factory template for specific plugin types
template<typename ComponentType>
class ComponentFactory : public PluginFactory {
public:
    ComponentFactory() = default;
    
    std::unique_ptr<BrainComponent> createInstance() override {
        return std::make_unique<ComponentType>();
    }
    
    PluginMetadata getMetadata() const override {
        return ComponentType::getPluginMetadata();
    }
    
    bool validate() const override {
        return std::is_base_of<BrainComponent, ComponentType>::value;
    }
};

// Plugin manager for loading and managing plugins
class PluginManager {
public:
    PluginManager();
    ~PluginManager();
    
    // Prevent copying
    PluginManager(const PluginManager&) = delete;
    PluginManager& operator=(const PluginManager&) = delete;
    
    // Plugin discovery and loading
    bool loadPlugin(const std::string& pluginPath);
    bool unloadPlugin(const std::string& pluginName);
    bool reloadPlugin(const std::string& pluginName);
    
    // Plugin registration (for statically linked plugins)
    template<typename ComponentType>
    bool registerStaticPlugin() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto factory = std::make_unique<ComponentFactory<ComponentType>>();
        if (!factory->validate()) {
            return false;
        }
        
        PluginMetadata metadata = factory->getMetadata();
        plugins_[metadata.name] = std::move(factory);
        staticPlugins_.push_back(metadata.name);
        
        return true;
    }
    
    // Component creation
    std::unique_ptr<BrainComponent> createComponent(const std::string& pluginName);
    std::vector<std::unique_ptr<BrainComponent>> createComponents(
        const std::vector<std::string>& pluginNames);
    
    // Plugin management
    bool enablePlugin(const std::string& pluginName);
    bool disablePlugin(const std::string& pluginName);
    bool isPluginEnabled(const std::string& pluginName) const;
    
    // Query and discovery
    std::vector<std::string> getAvailablePlugins() const;
    std::vector<std::string> getEnabledPlugins() const;
    PluginMetadata getPluginMetadata(const std::string& pluginName) const;
    std::vector<PluginMetadata> getPluginMetadata() const;
    
    // Brain integration
    void setBrain(std::shared_ptr<Brain> brain);
    std::shared_ptr<Brain> getBrain() const;
    
    // Configuration
    bool configurePlugin(const std::string& pluginName, 
                        const std::string& key, 
                        const std::string& value);
    
    // Statistics
    std::string getManagerStats() const;
    size_t getPluginCount() const;
    size_t getEnabledPluginCount() const;
    
    // Validation and verification
    bool validateAllPlugins() const;
    bool checkDependencies() const;
    
    // Cleanup
    void shutdownAllPlugins();
    void clearAllPlugins();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    mutable std::mutex mutex_;
};

// Plugin export macro for creating plugins
#ifdef _WIN32
    #define NLM_PLUGIN_EXPORT __declspec(dllexport)
#else
    #define NLM_PLUGIN_EXPORT __attribute__((visibility("default")))
#endif

// Plugin base class for easy plugin creation
class PluginBase : public BrainComponent {
protected:
    std::shared_ptr<Brain> brain_;
    std::string name_;
    PluginVersion version_;
    bool initialized_;
    
public:
    PluginBase(const std::string& name, const PluginVersion& version = PluginVersion())
        : name_(name), version_(version), initialized_(false) {}
    
    std::string getName() const override { return name_; }
    PluginVersion getVersion() const override { return version_; }
    bool isInitialized() const { return initialized_; }
    
protected:
    void setInitialized(bool init) { initialized_ = init; }
    
    // Helper for configuration management
    template<typename T>
    bool setTypedConfig(const std::string& key, const T& value) {
        // Implementation would convert T to string
        return configure(key, std::to_string(value));
    }
    
    template<typename T>
    T getTypedConfig(const std::string& key, T defaultValue) const {
        std::string value = getConfig(key);
        // Implementation would convert string to T
        return defaultValue;
    }
};

// Plugin category enumerator
enum class PluginCategory {
    Memory,
    Neuromodulation,
    Prediction,
    Cognition,
    Development,
    Sensory,
    Motor,
    Environment,
    Visualization,
    Utility,
    Custom
};

// Helper functions for plugin management
namespace plugin_utils {
    // Discover available plugins in a directory
    std::vector<std::string> discoverPlugins(const std::string& directory);
    
    // Validate plugin compatibility
    bool validatePluginCompatibility(const PluginMetadata& metadata,
                                     const std::vector<std::string>& systemDeps);
    
    // Generate unique plugin names
    std::string generatePluginName(const std::string& baseName);
    
    // Plugin logging
    void logPluginMessage(const std::string& pluginName,
                         const std::string& message,
                         const std::string& level = "INFO");
    
    // Performance monitoring
    double measurePluginPerformance(std::function<void()> pluginFunction);
    
    // Plugin health check
    bool healthCheckPlugin(const std::string& pluginName);
}

} // namespace nlm
