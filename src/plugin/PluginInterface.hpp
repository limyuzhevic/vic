#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>

namespace nlm {

// Plugin base class for extensibility
class Plugin {
public:
    virtual ~Plugin() = default;
    
    // Plugin name for identification and debugging
    virtual const char* getName() const = 0;
    
    // Plugin version
    virtual const char* getVersion() const = 0;
    
    // Description of the plugin functionality
    virtual const char* getDescription() const = 0;
    
    // Initialize plugin with configuration
    virtual bool initialize(const std::string& config) = 0;
    
    // Clean up plugin resources
    virtual void shutdown() = 0;
    
    // Check if plugin is running
    virtual bool isRunning() const = 0;
};

// Plugin factory function type
using PluginFactory = std::function<std::unique_ptr<Plugin>()>;

// Plugin registry for managing plugins
class PluginRegistry {
public:
    PluginRegistry() = default;
    ~PluginRegistry() = default;
    
    // Register a plugin factory with a name
    void registerPlugin(const std::string& name, PluginFactory factory);
    
    // Get plugin factory by name
    PluginFactory getPluginFactory(const std::string& name) const;
    
    // Check if plugin is registered
    bool hasPlugin(const std::string& name) const;
    
    // Get all registered plugin names
    std::vector<std::string> getPluginNames() const;
    
    // Load all plugins from directory (platform-specific)
    bool loadPluginsFromDirectory(const std::string& directory);
    
    // Create a plugin instance
    std::unique_ptr<Plugin> createPlugin(const std::string& name);
    
    // Initialize all registered plugins
    bool initializeAllPlugins(const std::string& config);
    
    // Shutdown all plugins
    void shutdownAllPlugins();
    
    // Get global plugin registry instance
    static PluginRegistry& getInstance();
    
private:
    std::map<std::string, PluginFactory> factories_;
    std::map<std::string, std::unique_ptr<Plugin>> instances_;
};

// Plugin interface for component extensions
class ComponentPlugin : public Plugin {
public:
    virtual ~ComponentPlugin() = default;
    
    // Get component type (e.g., "plasticity_rule", "neuromodulator", "memory_system")
    virtual const char* getComponentType() const = 0;
    
    // Get component name
    virtual const char* getComponentName() const = 0;
};

// Plasticity rule plugin interface
class PlasticityRulePlugin : public ComponentPlugin {
public:
    virtual ~PlasticityRulePlugin() = default;
    
    // Get component type - should return "plasticity_rule"
    const char* getComponentType() const override { return "plasticity_rule"; }
    
    // Update synaptic weights based on pre/post synaptic activity
    virtual void update(class Synapse* synapse,
                       const std::vector<Timestamp>& preSpikes,
                       const std::vector<Timestamp>& postSpikes,
                       TimestepDuration dt) = 0;
    
    // Get learning rate for this plasticity rule
    virtual float getLearningRate() const = 0;
    
    // Set learning rate
    virtual void setLearningRate(float rate) = 0;
};

// Neuromodulator plugin interface
class NeuromodulatorPlugin : public ComponentPlugin {
public:
    virtual ~NeuromodulatorPlugin() = default;
    
    // Get component type - should return "neuromodulator"
    const char* getComponentType() const override { return "neuromodulator"; }
    
    // Calculate neuromodulatory effect based on brain state
    virtual float calculateEffect(const class Brain* brain,
                                 TimestepDuration dt) = 0;
    
    // Get baseline level
    virtual float getBaselineLevel() const = 0;
    
    // Set baseline level
    virtual void setBaselineLevel(float level) = 0;
};

// Memory system plugin interface
class MemorySystemPlugin : public ComponentPlugin {
public:
    virtual ~MemorySystemPlugin() = default;
    
    // Get component type - should return "memory_system"
    const char* getComponentType() const override { return "memory_system"; }
    
    // Encode information into memory
    virtual void encode(const std::vector<float>& input,
                       const std::string& context) = 0;
    
    // Retrieve information from memory
    virtual std::vector<float> retrieve(const std::string& context,
                                       float threshold = 0.0f) = 0;
    
    // Get memory capacity
    virtual size_t getCapacity() const = 0;
    
    // Set memory capacity
    virtual void setCapacity(size_t capacity) = 0;
};

// Plugin utilities
class PluginUtils {
public:
    // Create a simple plugin that wraps an existing component
    template<typename T>
    static std::unique_ptr<PlasticityRulePlugin> createPlasticityRulePlugin(
        const std::string& name, const std::string& description);
    
    // Create a neuromodulator plugin
    template<typename T>
    static std::unique_ptr<NeuromodulatorPlugin> createNeuromodulatorPlugin(
        const std::string& name, const std::string& description);
    
    // Create a memory system plugin
    template<typename T>
    static std::unique_ptr<MemorySystemPlugin> createMemorySystemPlugin(
        const std::string& name, const std::string& description);
    
    // Plugin versioning utility
    static std::string getPluginVersion(int major, int minor, int patch);
    
    // Plugin validation
    static bool validatePlugin(const Plugin& plugin);
};

} // namespace nlm