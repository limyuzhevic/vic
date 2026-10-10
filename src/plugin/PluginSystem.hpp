#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <unordered_map>
#include <type_traits>

namespace nlm {

// Plugin interface version
constexpr uint32_t PLUGIN_INTERFACE_VERSION = 1;

// Plugin type
enum class PluginType {
    NeuralModel,
    PlasticityRule,
    MemorySystem,
    Neuromodulator,
    PredictionAlgorithm,
    CognitiveModule,
    DevelopmentStage,
    Visualization,
    DataSource,
    Custom
};

// Plugin capability
struct PluginCapability {
    std::string name;
    PluginType type;
    std::string version;
    std::string description;
    std::vector<std::string> dependencies;
    bool required;
    
    PluginCapability(const std::string& name, PluginType type, const std::string& version, 
                    const std::string& description, bool required = false)
        : name(name), type(type), version(version), description(description), 
          required(required) {}
};

// Plugin base class
class IPlugin {
public:
    virtual ~IPlugin() = default;
    
    // Get plugin metadata
    virtual PluginCapability getCapability() const = 0;
    
    // Initialize plugin
    virtual bool initialize(const std::map<std::string, std::string>& config) = 0;
    
    // Cleanup plugin
    virtual void cleanup() = 0;
    
    // Check if plugin is initialized
    virtual bool isInitialized() const = 0;
    
    // Get plugin version
    virtual std::string getVersion() const = 0;
    
    // Set plugin manager reference
    virtual void setPluginManager(IPluginManager* manager) {}
};

// Plugin manager interface
class IPluginManager {
public:
    virtual ~IPluginManager() = default;
    
    // Load plugin from file
    virtual bool loadPlugin(const std::string& filepath) = 0;
    
    // Unload plugin
    virtual bool unloadPlugin(const std::string& pluginName) = 0;
    
    // Check if plugin is loaded
    virtual bool isPluginLoaded(const std::string& pluginName) const = 0;
    
    // Get loaded plugins
    virtual std::vector<std::string> getLoadedPlugins() const = 0;
    
    // Get plugin by name
    virtual std::shared_ptr<IPlugin> getPlugin(const std::string& pluginName) = 0;
    
    // Get plugins by type
    virtual std::vector<std::shared_ptr<IPlugin>> getPluginsByType(PluginType type) = 0;
    
    // Configure plugin
    virtual bool configurePlugin(const std::string& pluginName, 
                                 const std::map<std::string, std::string>& config) = 0;
    
    // Get global configuration
    virtual std::map<std::string, std::string> getGlobalConfig() const = 0;
    
    // Set global configuration
    virtual void setGlobalConfig(const std::map<std::string, std::string>& config) = 0;
    
    // Initialize all plugins
    virtual bool initializeAllPlugins() = 0;
    
    // Cleanup all plugins
    virtual void cleanupAllPlugins() = 0;
    
    // Reload all plugins
    virtual bool reloadAllPlugins() = 0;
};

// Plugin loader interface
class IPluginLoader {
public:
    virtual ~IPluginLoader() = default;
    
    // Load plugin from file
    virtual std::shared_ptr<IPlugin> loadPlugin(const std::string& filepath) = 0;
    
    // Check if plugin can be loaded
    virtual bool canLoad(const std::string& filepath) const = 0;
    
    // Get supported file extensions
    virtual std::vector<std::string> getSupportedExtensions() const = 0;
    
    // Get loader name
    virtual std::string getName() const = 0;
};

// Plugin registry
class PluginRegistry {
public:
    static PluginRegistry& getInstance() {
        static PluginRegistry instance;
        return instance;
    }
    
    // Register plugin loader
    void registerLoader(std::unique_ptr<IPluginLoader> loader);
    
    // Get loader by extension
    std::unique_ptr<IPluginLoader> getLoaderByExtension(const std::string& extension);
    
    // Get all loaders
    std::vector<std::unique_ptr<IPluginLoader>> getAllLoaders() const;
    
    // Register plugin factory
    template<typename T>
    void registerPluginType(const std::string& pluginName, 
                           std::function<std::shared_ptr<IPlugin>()> factory) {
        pluginFactories_[pluginName] = factory;
    }
    
    // Create plugin by name
    template<typename T>
    std::shared_ptr<IPlugin> createPlugin(const std::string& pluginName) {
        auto it = pluginFactories_.find(pluginName);
        if (it != pluginFactories_.end()) {
            return it->second();
        }
        return nullptr;
    }
    
    // Get all plugin types
    std::vector<std::string> getAllPluginTypes() const;
    
private:
    PluginRegistry() = default;
    std::vector<std::unique_ptr<IPluginLoader>> loaders_;
    std::map<std::string, std::function<std::shared_ptr<IPlugin>()>> pluginFactories_;
};

// Template plugin base class for specific plugin types
template<typename T>
class PluginBase : public IPlugin {
public:
    virtual ~PluginBase() = default;
    
    // Get capability
    virtual PluginCapability getCapability() const override {
        return capability_;
    }
    
    // Initialize
    virtual bool initialize(const std::map<std::string, std::string>& config) override {
        return initializeImpl(config);
    }
    
    // Cleanup
    virtual void cleanup() override {
        cleanupImpl();
    }
    
    // Check initialized
    virtual bool isInitialized() const override {
        return isInitialized_;
    }
    
    // Get version
    virtual std::string getVersion() const override {
        return version_;
    }
    
    // Set manager
    virtual void setPluginManager(IPluginManager* manager) override {
        manager_ = manager;
        setPluginManagerImpl(manager);
    }
    
    // Pure virtual methods for implementation
    virtual bool initializeImpl(const std::map<std::string, std::string>& config) = 0;
    virtual void cleanupImpl() = 0;
    virtual void setPluginManagerImpl(IPluginManager* manager) {}
    
protected:
    PluginCapability capability_;
    std::string version_;
    bool isInitialized_ = false;
    IPluginManager* manager_ = nullptr;
    
    PluginBase(const PluginCapability& capability, const std::string& version) 
        : capability_(capability), version_(version) {}
};

// Plugin configuration
struct PluginConfig {
    std::string name;
    std::string filepath;
    PluginType type;
    std::map<std::string, std::string> config;
    bool autoLoad;
    bool enabled;
    std::vector<std::string> dependencies;
    
    PluginConfig(const std::string& name, const std::string& filepath, PluginType type)
        : name(name), filepath(filepath), type(type), autoLoad(true), enabled(true) {}
};

// Plugin statistics
struct PluginStats {
    std::string name;
    PluginType type;
    std::string version;
    std::string description;
    bool isInitialized;
    uint64_t loadCount;
    uint64_t callCount;
    double totalExecutionTime;
    std::string lastError;
    
    PluginStats(const std::string& name, PluginType type, const std::string& version, 
               const std::string& description)
        : name(name), type(type), version(version), description(description), 
          isInitialized(false), loadCount(0), callCount(0), totalExecutionTime(0.0) {}
};

} // namespace nlm