#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <unordered_map>
#include <typeindex>

namespace nlm {

// Plugin system version
constexpr const char* PLUGIN_SYSTEM_VERSION = "1.0.0";

// Plugin type enumeration
enum class PluginType {
    World,
    AgentBrain,
    NeuralArchitecture,
    SensoryProcessor,
    MotorDecoder,
    Neuromodulator,
    MemorySystem,
    PlasticityRule,
    DevelopmentSystem,
    Utility
};

// Plugin load status
enum class PluginStatus {
    Unloaded,
    Loading,
    Loaded,
    Failed,
    Disabled
};

// Plugin information
struct PluginInfo {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    PluginType type;
    std::string category;
    std::vector<std::string> dependencies;
    bool enabled;
    bool required;
    std::map<std::string, std::string> metadata;
    
    PluginInfo() : version("0.1.0"), enabled(true), required(false) {}
    PluginInfo(const std::string& n, const std::string& v, const std::string& d, PluginType t)
        : name(n), version(v), description(d), author("Unknown"), type(t), enabled(true), required(false) {}
};

// Plugin interface - all plugins must implement this
class IPlugin {
public:
    virtual ~IPlugin() = default;
    
    // Return plugin information
    virtual const PluginInfo& getInfo() const = 0;
    
    // Initialize plugin with configuration
    virtual bool initialize(const Config& config) = 0;
    
    // Shutdown plugin
    virtual void shutdown() = 0;
    
    // Check if plugin is enabled
    virtual bool isEnabled() const = 0;
    
    // Set plugin enabled state
    virtual void setEnabled(bool enabled) = 0;
    
    // Plugin-specific initialization
    virtual bool onInitialize() = 0;
    
    // Plugin-specific cleanup
    virtual void onShutdown() = 0;
    
    // Get error message if plugin failed to load
    virtual std::string getError() const = 0;
    
    // Check if plugin has dependencies
    virtual bool hasDependencies(const std::map<std::string, PluginInfo>& loadedPlugins) const = 0;
};

// World plugin interface
class IWorldPlugin : public IPlugin {
public:
    virtual ~IWorldPlugin() = default;
    
    // Create a world instance
    virtual std::unique_ptr<class SimpleWorld> createWorld() = 0;
    
    // Get world configuration options
    virtual std::map<std::string, std::variant<int, double, std::string, bool>> getConfigurationOptions() const = 0;
};

// Agent brain plugin interface
class IAgentBrainPlugin : public IPlugin {
public:
    virtual ~IAgentBrainPlugin() = default;
    
    // Create an agent brain instance
    virtual std::unique_ptr<class AgentBrain> createAgentBrain(std::shared_ptr<class Brain> brain) = 0;
    
    // Get agent brain configuration options
    virtual std::map<std::string, std::variant<int, double, std::string, bool>> getConfigurationOptions() const = 0;
};

// Neural architecture plugin interface
class INeuralArchitecturePlugin : public IPlugin {
public:
    virtual ~INeuralArchitecturePlugin() = default;
    
    // Create a brain with this architecture
    virtual std::shared_ptr<class Brain> createBrain(std::shared_ptr<Config> config) = 0;
    
    // Get neural architecture parameters
    virtual std::map<std::string, std::variant<int, double, std::string, bool>> getArchitectureParameters() const = 0;
};

// Sensory processor plugin interface
class ISensoryProcessorPlugin : public IPlugin {
public:
    virtual ~ISensoryProcessorPlugin() = default;
    
    // Process sensory input
    virtual std::shared_ptr<SensoryInput> processInput(std::shared_ptr<SensoryInput> input, double timestep) = 0;
    
    // Get processor configuration
    virtual std::map<std::string, std::variant<int, double, std::string, bool>> getProcessorConfig() const = 0;
};

// Motor decoder plugin interface
class IMotorDecoderPlugin : public IPlugin {
public:
    virtual ~IMotorDecoderPlugin() = default;
    
    // Decode neural activity to motor command
    virtual MotorCommand decode(std::vector<double> neuralActivity) = 0;
    
    // Get decoder configuration
    virtual std::map<std::string, std::variant<int, double, std::string, bool>> getDecoderConfig() const = 0;
};

// Neuromodulator plugin interface
class INeuromodulatorPlugin : public IPlugin {
public:
    virtual ~INeuromodulatorPlugin() = default;
    
    // Apply neuromodulation
    virtual void applyModulation(std::map<std::string, double> signals, double timestep) = 0;
    
    // Get neuromodulator parameters
    virtual std::map<std::string, std::variant<int, double, std::string, bool>> getNeuromodulatorConfig() const = 0;
};

// Memory system plugin interface
class IMemorySystemPlugin : public IPlugin {
public:
    virtual ~IMemorySystemPlugin() = default;
    
    // Store experience
    virtual void storeExperience(std::shared_ptr<class Experience>) = 0;
    
    // Retrieve relevant experiences
    virtual std::vector<std::shared_ptr<class Experience>> retrieveExperiences(std::shared_ptr<class Query>) = 0;
    
    // Get memory system statistics
    virtual std::map<std::string, double> getMemoryStats() const = 0;
};

// Plasticity rule plugin interface
class IPlasticityRulePlugin : public IPlugin {
public:
    virtual ~IPlasticityRulePlugin() = default;
    
    // Apply plasticity rule
    virtual void applyPlasticity(std::shared_ptr<class Synapse> synapse, double preTime, double postTime, double learningRate) = 0;
    
    // Get plasticity parameters
    virtual std::map<std::string, std::variant<int, double, std::string, bool>> getPlasticityConfig() const = 0;
};

// Development system plugin interface
class IDevelopmentSystemPlugin : public IPlugin {
public:
    virtual ~IDevelopmentSystemPlugin() = default;
    
    // Update development state
    virtual void updateDevelopment(double timestep, std::shared_ptr<Brain> brain) = 0;
    
    // Get developmental parameters
    virtual std::map<std::string, std::variant<int, double, std::string, bool>> getDevelopmentConfig() const = 0;
};

// Plugin factory interface
class IPluginFactory {
public:
    virtual ~IPluginFactory() = default;
    
    // Create plugin instance
    virtual std::unique_ptr<IPlugin> createPlugin() = 0;
    
    // Get plugin info
    virtual PluginInfo getInfo() const = 0;
    
    // Check if factory can create this plugin type
    virtual bool canCreate(PluginType type) const = 0;
};

// Plugin manager for loading and managing plugins
class PluginManager {
public:
    PluginManager();
    ~PluginManager();
    
    // Load plugin from file
    bool loadPlugin(const std::string& pluginPath);
    
    // Load plugin from directory
    bool loadPluginsFromDirectory(const std::string& directoryPath);
    
    // Unload plugin
    bool unloadPlugin(const std::string& pluginName);
    
    // Enable/disable plugin
    bool enablePlugin(const std::string& pluginName);
    bool disablePlugin(const std::string& pluginName);
    
    // Get plugin by name
    std::shared_ptr<IPlugin> getPlugin(const std::string& pluginName) const;
    
    // Get all loaded plugins
    std::vector<std::shared_ptr<IPlugin>> getLoadedPlugins() const;
    
    // Get plugins by type
    std::vector<std::shared_ptr<IPlugin>> getPluginsByType(PluginType type) const;
    
    // Check if plugin is loaded
    bool isPluginLoaded(const std::string& pluginName) const;
    
    // Get plugin information
    std::optional<PluginInfo> getPluginInfo(const std::string& pluginName) const;
    
    // Initialize all enabled plugins
    bool initializeAll(const Config& config);
    
    // Shutdown all plugins
    void shutdownAll();
    
    // Get plugin system version
    static const char* getVersion() { return PLUGIN_SYSTEM_VERSION; }
    
    // Get available plugin directories
    static std::vector<std::string> getDefaultPluginDirectories();
    
    // Create default factories
    static std::vector<std::unique_ptr<IPluginFactory>> createDefaultFactories();
    
    // Validate plugin dependencies
    bool validateDependencies(const PluginInfo& plugin, const std::map<std::string, PluginInfo>& loadedPlugins) const;
    
    // Export plugin list
    std::string exportPluginList() const;
    
    // Import plugin configuration
    bool importPluginConfig(const std::string& configJson);
    
    // Export plugin configuration
    std::string exportPluginConfig(const std::string& pluginName) const;
    
    // Reload plugin
    bool reloadPlugin(const std::string& pluginName, const std::string& pluginPath);
    
    // Get plugin load errors
    std::map<std::string, std::string> getLoadErrors() const;
    
    // Clear load errors
    void clearLoadErrors();
    
    // Check if any errors
    bool hasErrors() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Plugin registry for factory registration
class PluginRegistry {
public:
    PluginRegistry();
    ~PluginRegistry();
    
    // Register plugin factory
    bool registerFactory(std::unique_ptr<IPluginFactory> factory);
    
    // Get factory by plugin name
    std::shared_ptr<IPluginFactory> getFactory(const std::string& pluginName) const;
    
    // Get factory by type
    std::shared_ptr<IPluginFactory> getFactory(PluginType type) const;
    
    // Get all factories
    std::vector<std::shared_ptr<IPluginFactory>> getAllFactories() const;
    
    // Get plugin info by name
    std::optional<PluginInfo> getPluginInfo(const std::string& pluginName) const;
    
    // Get plugins by type
    std::vector<PluginInfo> getPluginsByType(PluginType type) const;
    
    // Check if plugin is supported
    bool isPluginSupported(const std::string& pluginName) const;
    
    // Get supported plugin types
    std::vector<PluginType> getSupportedTypes() const;
    
    // Get default factories (built-in plugins)
    static std::vector<std::unique_ptr<IPluginFactory>> getDefaultFactories();
    
    // Clear all factories
    void clear();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Plugin configuration
class PluginConfig {
public:
    PluginConfig();
    ~PluginConfig();
    
    // Load plugin configuration from file
    bool loadFromFile(const std::string& filepath);
    
    // Save plugin configuration to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get plugin configuration
    std::optional<std::map<std::string, std::variant<int, double, std::string, bool>>> 
        getPluginConfig(const std::string& pluginName) const;
    
    // Set plugin configuration
    bool setPluginConfig(const std::string& pluginName, 
                        const std::map<std::string, std::variant<int, double, std::string, bool>>& config);
    
    // Remove plugin configuration
    bool removePluginConfig(const std::string& pluginName);
    
    // Check if plugin configuration exists
    bool hasPluginConfig(const std::string& pluginName) const;
    
    // Get all plugin configurations
    std::map<std::string, std::map<std::string, std::variant<int, double, std::string, bool>>> 
        getAllPluginConfigs() const;
    
    // Clear plugin configuration
    void clear();
    
    // Get plugin metadata
    std::map<std::string, std::string> getPluginMetadata(const std::string& pluginName) const;
    
    // Set plugin metadata
    bool setPluginMetadata(const std::string& pluginName,
                          const std::map<std::string, std::string>& metadata);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Plugin utilities
namespace PluginUtils {
    // Validate plugin name
    bool isValidPluginName(const std::string& name);
    
    // Sanitize plugin name
    std::string sanitizePluginName(const std::string& name);
    
    // Get plugin version from string
    std::string parseVersion(const std::string& versionString);
    
    // Compare plugin versions
    bool compareVersions(const std::string& version1, const std::string& version2);
    
    // Get current platform
    std::string getCurrentPlatform();
    
    // Get plugin extension for current platform
    std::string getPluginExtension();
    
    // Check if file is a plugin
    bool isPluginFile(const std::string& filepath);
    
    // Load plugin from memory (for embedded systems)
    std::unique_ptr<IPlugin> loadPluginFromMemory(const std::vector<uint8_t>& data);
    
    // Convert plugin configuration to JSON string
    std::string configToJson(const std::map<std::string, std::variant<int, double, std::string, bool>>& config);
    
    // Parse JSON string to plugin configuration
    std::map<std::string, std::variant<int, double, std::string, bool>> 
        jsonToConfig(const std::string& jsonString);
}}

} // namespace nlm