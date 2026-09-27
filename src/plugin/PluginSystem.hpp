// Plugin System - Header File
#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <unordered_map>
#include <mutex>

namespace nlm {

// Plugin interface for custom neural components
class NeuralModelPlugin {
public:
    virtual ~NeuralModelPlugin() = default;
    
    // Plugin identification
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getAuthor() const = 0;
    
    // Plugin lifecycle
    virtual bool initialize(const std::map<std::string, std::string>& config) = 0;
    virtual void shutdown() = 0;
    virtual bool isEnabled() const = 0;
    virtual void setEnabled(bool enabled) = 0;
    
    // Neural computation interface
    virtual std::vector<float> computeNeuronState(
        const std::vector<float>& inputs,
        const std::vector<float>& weights,
        const std::vector<float>& biases) = 0;
    
    virtual float computeActivation(float input) = 0;
    virtual float computeDerivative(float input) = 0;
    
    // Configuration
    virtual std::map<std::string, std::string> getDefaultConfig() const = 0;
    virtual bool validateConfig(const std::map<std::string, std::string>& config) = 0;
};

// Plasticity rule plugin
class PlasticityRulePlugin {
public:
    virtual ~PlasticityRulePlugin() = default;
    
    // Plugin identification
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual PlasticityRuleType getRuleType() const = 0;
    
    // Rule configuration
    virtual std::map<std::string, float> getDefaultParameters() const = 0;
    virtual std::vector<std::string> getRequiredParameters() const = 0;
    virtual bool validateParameters(const std::map<std::string, float>& params) = 0;
    
    // Plasticity computation
    virtual float computeWeightChange(
        float preSpikeTime,
        float postSpikeTime,
        float currentWeight,
        const std::map<std::string, float>& params) = 0;
    
    virtual bool shouldApplyPlasticity(
        float preSpikeTime,
        float postSpikeTime,
        float currentWeight,
        const std::map<std::string, float>& params) = 0;
};

// Memory system plugin
class MemorySystemPlugin {
public:
    virtual ~MemorySystemPlugin() = default;
    
    // Plugin identification
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual MemorySystemType getMemoryType() const = 0;
    
    // Memory management
    virtual std::vector<float> store(const std::vector<float>& data) = 0;
    virtual std::vector<float> retrieve(const std::vector<float>& key) = 0;
    virtual bool update(const std::vector<float>& key, const std::vector<float>& data) = 0;
    virtual bool remove(const std::vector<float>& key) = 0;
    virtual void clear() = 0;
    
    // Memory properties
    virtual size_t getCapacity() const = 0;
    virtual size_t getCurrentSize() const = 0;
    virtual float getUsageRatio() const = 0;
    
    // Configuration
    virtual std::map<std::string, std::string> getDefaultConfig() const = 0;
    virtual bool initialize(const std::map<std::string, std::string>& config) = 0;
};

// Neuromodulation plugin
class NeuromodulationPlugin {
public:
    virtual ~NeuromodulationPlugin() = default;
    
    // Plugin identification
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual NeuromodulatorType getModulatorType() const = 0;
    
    // Neuromodulation computation
    virtual float computeModulation(
        float input,
        const std::vector<float>& history,
        const std::map<std::string, float>& params) = 0;
    
    virtual float computeErrorSignal(
        float expected,
        float actual,
        const std::map<std::string, float>& params) = 0;
    
    // Configuration
    virtual std::map<std::string, float> getDefaultParameters() const = 0;
    virtual bool initialize(const std::map<std::string, float>& params) = 0;
};

// Environment interaction plugin
class EnvironmentPlugin {
public:
    virtual ~EnvironmentPlugin() = default;
    
    // Plugin identification
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    
    // Environment simulation
    virtual std::vector<float> step(const std::vector<float>& actions, float timestep) = 0;
    virtual bool reset() = 0;
    virtual bool isTerminal() const = 0;
    virtual float getReward(const std::vector<float>& state) const = 0;
    
    // Environment properties
    virtual std::vector<float> getInitialState() const = 0;
    virtual std::vector<float> getStateSize() const = 0;
    virtual std::vector<float> getActionSize() const = 0;
    virtual std::string getEnvironmentName() const = 0;
    
    // Configuration
    virtual std::map<std::string, std::string> getDefaultConfig() const = 0;
    virtual bool initialize(const std::map<std::string, std::string>& config) = 0;
};

// Plugin manager for loading and managing plugins
class PluginManager {
public:
    static PluginManager& getInstance() {
        static PluginManager instance;
        return instance;
    }
    
    // Plugin loading and unloading
    bool loadPlugin(const std::string& path);
    bool unloadPlugin(const std::string& name);
    bool reloadPlugin(const std::string& name);
    
    // Plugin discovery
    std::vector<std::string> getLoadedPlugins() const;
    std::vector<std::string> getAvailablePlugins() const;
    std::vector<std::string> getPluginCategories() const;
    
    // Plugin access
    template<typename T>
    std::shared_ptr<T> getPlugin(const std::string& name);
    
    std::shared_ptr<NeuralModelPlugin> getNeuralModelPlugin(const std::string& name);
    std::shared_ptr<PlasticityRulePlugin> getPlasticityRulePlugin(const std::string& name);
    std::shared_ptr<MemorySystemPlugin> getMemorySystemPlugin(const std::string& name);
    std::shared_ptr<NeuromodulationPlugin> getNeuromodulationPlugin(const std::string& name);
    std::shared_ptr<EnvironmentPlugin> getEnvironmentPlugin(const std::string& name);
    
    // Plugin management
    bool registerPlugin(std::shared_ptr<NeuralModelPlugin> plugin);
    bool registerPlugin(std::shared_ptr<PlasticityRulePlugin> plugin);
    bool registerPlugin(std::shared_ptr<MemorySystemPlugin> plugin);
    bool registerPlugin(std::shared_ptr<NeuromodulationPlugin> plugin);
    bool registerPlugin(std::shared_ptr<EnvironmentPlugin> plugin);
    
    // Enable/disable plugins
    bool enablePlugin(const std::string& name);
    bool disablePlugin(const std::string& name);
    bool isPluginEnabled(const std::string& name) const;
    
    // Configuration
    bool configurePlugin(const std::string& name, const std::map<std::string, std::string>& config);
    std::map<std::string, std::string> getPluginConfig(const std::string& name) const;
    
    // Plugin discovery and information
    struct PluginInfo {
        std::string name;
        std::string version;
        std::string description;
        std::string author;
        std::string category;
        bool enabled;
        std::map<std::string, std::string> config;
        std::string filePath;
        std::string lastLoaded;
    };
    
    PluginInfo getPluginInfo(const std::string& name) const;
    
    // Plugin lifecycle
    void enableAllPlugins();
    void disableAllPlugins();
    void initializeAllPlugins();
    void shutdownAllPlugins();
    
    // State management
    bool savePluginState(const std::string& filename) const;
    bool loadPluginState(const std::string& filename);
    std::string exportPluginState() const;
    bool importPluginState(const std::string& state);
    
    // Statistics
    size_t getPluginCount() const;
    size_t getEnabledPluginCount() const;
    
    // Error handling
    std::string getLastError() const;
    bool hasError() const;
    
private:
    PluginManager() = default;
    
    // Plugin storage
    struct PluginEntry {
        std::shared_ptr<NeuralModelPlugin> neuralModel;
        std::shared_ptr<PlasticityRulePlugin> plasticityRule;
        std::shared_ptr<MemorySystemPlugin> memorySystem;
        std::shared_ptr<NeuromodulationPlugin> neuromodulation;
        std::shared_ptr<EnvironmentPlugin> environment;
        
        bool enabled;
        std::map<std::string, std::string> config;
        std::string filePath;
        std::string lastLoaded;
        std::string errorMessage;
    };
    
    std::map<std::string, PluginEntry> plugins_;
    mutable std::mutex mutex_;
    std::string lastError_;
    
    // Plugin discovery
    void discoverPlugins();
    std::vector<std::string> scanDirectory(const std::string& directory) const;
    bool isPluginFile(const std::string& filename) const;
    
    // Plugin loading
    bool loadPluginFile(const std::string& path);
    bool loadPluginDynamicLibrary(const std::string& path);
    
    // Validation
    bool validatePlugin(const PluginEntry& entry) const;
    
    // Helper functions
    std::string extractPluginName(const std::string& path) const;
    std::string getPluginCategory(const std::string& path) const;
};

// Factory class for creating neural components using plugins
class PluginNeuralFactory {
public:
    static PluginNeuralFactory& getInstance() {
        static PluginNeuralFactory instance;
        return instance;
    }
    
    // Neural model creation
    std::shared_ptr<NeuralModelPlugin> createNeuralModel(const std::string& pluginName);
    std::shared_ptr<PlasticityRulePlugin> createPlasticityRule(const std::string& pluginName);
    std::shared_ptr<MemorySystemPlugin> createMemorySystem(const std::string& pluginName);
    std::shared_ptr<NeuromodulationPlugin> createNeuromodulation(const std::string& pluginName);
    std::shared_ptr<EnvironmentPlugin> createEnvironment(const std::string& pluginName);
    
    // Component registration
    void registerNeuralModel(const std::string& name, const std::string& pluginName);
    void registerPlasticityRule(const std::string& name, const std::string& pluginName);
    void registerMemorySystem(const std::string& name, const std::string& pluginName);
    void registerNeuromodulation(const std::string& name, const std::string& pluginName);
    void registerEnvironment(const std::string& name, const std::string& pluginName);
    
    // Configuration
    void configureComponent(const std::string& name, const std::map<std::string, std::string>& config);
    std::map<std::string, std::string> getComponentConfig(const std::string& name) const;
    
    // Plugin management
    bool loadPlugin(const std::string& path);
    bool unloadPlugin(const std::string& name);
    std::vector<std::string> getAvailablePlugins() const;
    
private:
    PluginNeuralFactory() = default;
    
    // Component registry
    struct ComponentRegistry {
        std::string pluginName;
        std::map<std::string, std::string> config;
        bool enabled;
    };
    
    std::map<std::string, ComponentRegistry> neuralModels_;
    std::map<std::string, ComponentRegistry> plasticityRules_;
    std::map<std::string, ComponentRegistry> memorySystems_;
    std::map<std::string, ComponentRegistry> neuromodulations_;
    std::map<std::string, ComponentRegistry> environments_;
    
    PluginManager pluginManager_;
};

// Plugin loader utility
class PluginLoader {
public:
    static bool loadPluginLibrary(const std::string& path);
    static bool unloadPluginLibrary(const std::string& path);
    static std::vector<std::string> getPluginLibraries() const;
    
    static std::shared_ptr<void> loadSymbol(const std::string& libraryPath, const std::string& symbolName);
    
private:
    static std::map<std::string, std::shared_ptr<void>> loadedLibraries_;
};

// Plugin utility functions
namespace plugin_utils {
    // Plugin validation
    bool validatePluginName(const std::string& name);
    bool validatePluginVersion(const std::string& version);
    
    // Plugin configuration
    std::map<std::string, std::string> parseConfigString(const std::string& configStr);
    std::string formatConfigString(const std::map<std::string, std::string>& config);
    
    // Plugin information
    std::string getPluginTypeString(const std::string& pluginPath);
    std::string getPluginVersion(const std::string& pluginPath);
    
    // Plugin loading
    bool isPluginCompatible(const std::string& pluginVersion, const std::string& requiredVersion);
    
    // Plugin discovery
    std::vector<std::string> findPlugins(const std::string& directory, const std::string& extension = "");
    bool isValidPluginFile(const std::string& filename);
} // namespace plugin_utils

// Plugin-related exceptions
class PluginException : public std::runtime_error {
public:
    enum class ErrorCode {
        LOAD_FAILED = 1,
        UNLOAD_FAILED = 2,
        ALREADY_LOADED = 3,
        NOT_FOUND = 4,
        INVALID_FORMAT = 5,
        INVALID_VERSION = 6,
        INITIALIZATION_FAILED = 7,
        CONFIG_VALIDATION_FAILED = 8,
        UNKNOWN = 99
    };
    
    PluginException(ErrorCode code, const std::string& message, const std::string& pluginName = "")
        : std::runtime_error(message), code_(code), pluginName_(pluginName) {}
    
    ErrorCode getCode() const { return code_; }
    const std::string& getPluginName() const { return pluginName_; }
    
private:
    ErrorCode code_;
    std::string pluginName_;
};

// Helper macros for plugin declaration
#define DECLARE_NEURAL_MODEL_PLUGIN(name, version, description) \
    extern "C" NeuralModelPlugin* create_plugin(); \
    extern "C" void destroy_plugin(NeuralModelPlugin* plugin); \
    extern "C" const char* get_plugin_name() { return name; } \
    extern "C" const char* get_plugin_version() { return version; } \
    extern "C" const char* get_plugin_description() { return description; }

#define DECLARE_PLASTICITY_RULE_PLUGIN(name, version, description) \
    extern "C" PlasticityRulePlugin* create_plugin(); \
    extern "C" void destroy_plugin(PlasticityRulePlugin* plugin); \
    extern "C" const char* get_plugin_name() { return name; } \
    extern "C" const char* get_plugin_version() { return version; } \
    extern "C" const char* get_plugin_description() { return description; }

#define NLM_REGISTER_PLUGIN(plugin) PluginManager::getInstance().registerPlugin(plugin)

} // namespace nlm