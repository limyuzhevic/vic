#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <any>
#include <typeindex>

namespace nlm {

// Plugin system version
const int PLUGIN_VERSION_MAJOR = 1;
const int PLUGIN_VERSION_MINOR = 0;

// Plugin categories
enum class PluginCategory {
    PlasticityRule,
    Neuromodulator,
    MemorySystem,
    CognitiveModule,
    Visualization,
    Environment,
    SensoryModality,
    MotorOutput,
    LearningAlgorithm,
    RewardFunction,
    ObjectiveFunction,
    Custom
};

// Plugin priority levels
enum class PluginPriority {
    Low = 0,
    Normal = 1,
    High = 2,
    Critical = 3
};

// Base interface for all plugins
class IPlugin {
public:
    virtual ~IPlugin() = default;
    
    // Plugin metadata
    virtual const std::string& getName() const = 0;
    virtual const std::string& getDescription() const = 0;
    virtual PluginCategory getCategory() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getAuthor() const = 0;
    virtual PluginPriority getPriority() const = 0;
    
    // Lifecycle management
    virtual bool initialize(const std::any& config) { return true; }
    virtual void shutdown() {}
    virtual bool isInitialized() const { return initialized_; }
    
    // Configuration management
    virtual void setConfig(const std::string& key, const std::any& value) {}
    virtual std::any getConfig(const std::string& key) const { return std::any(); }
    virtual std::vector<std::string> getConfigKeys() const { return {}; }
    
    // Dependency management
    virtual std::vector<std::string> getDependencies() const { return {}; }
    virtual bool checkDependencies() const { return true; }
    
    // Plugin-specific operations
    virtual void configure(const std::any& settings) {}
    virtual void update(double dt) {}
    virtual void reset() {}
    
    // Status and diagnostics
    virtual std::string getStatus() const { return "Initialized"; }
    virtual double getLoadTime() const { return 0.0; }
    
protected:
    bool initialized_ = false;
    std::chrono::steady_clock::time_point creationTime_ = std::chrono::steady_clock::now();
};

// Factory function type for plugin creation
using PluginFactory = std::function<std::unique_ptr<IPlugin>()>;

// Plugin metadata for registration
struct PluginMetadata {
    std::string name;
    std::string description;
    PluginCategory category;
    std::string version;
    std::string author;
    PluginPriority priority;
    std::vector<std::string> dependencies;
    PluginFactory factory;
    std::string filePath;
    bool enabled = true;
    
    // Validation
    bool validate() const {
        return !name.empty() && !description.empty() && 
               factory != nullptr && !version.empty();
    }
};

// Plugin registry for managing all plugins
class PluginRegistry {
public:
    static PluginRegistry& getInstance() {
        static PluginRegistry instance;
        return instance;
    }
    
    PluginRegistry(const PluginRegistry&) = delete;
    PluginRegistry& operator=(const PluginRegistry&) = delete;
    
    // Plugin registration
    bool registerPlugin(const PluginMetadata& metadata);
    bool unregisterPlugin(const std::string& name);
    bool unregisterPluginByPath(const std::string& filePath);
    
    // Plugin management
    std::vector<PluginMetadata> getAllPlugins() const;
    std::vector<PluginMetadata> getPluginsByCategory(PluginCategory category) const;
    std::vector<PluginMetadata> getPluginsByPriority(PluginPriority priority) const;
    std::vector<PluginMetadata> getEnabledPlugins() const;
    std::vector<PluginMetadata> getDisabledPlugins() const;
    
    // Plugin lifecycle management
    bool loadPlugin(const std::string& name);
    bool loadPluginByPath(const std::string& filePath);
    bool unloadPlugin(const std::string& name);
    bool reloadPlugin(const std::string& name);
    bool enablePlugin(const std::string& name);
    bool disablePlugin(const std::string& name);
    
    // Plugin instances management
    template<typename T>
    T* getPluginInstance() {
        auto it = pluginInstances_.find(std::type_index(typeid(T));
        return it != pluginInstances_.end() ? 
            static_cast<T*>(it->second.get()) : nullptr;
    }
    
    template<typename T>
    std::unique_ptr<T> createPluginInstance(const std::string& name) {
        auto metadataIt = findPluginByName(name);
        if (!metadataIt) return nullptr;
        
        auto instance = std::make_unique<T>();
        auto* rawPtr = instance.get();
        pluginInstances_[std::type_index(typeid(T))] = std::move(instance);
        
        if (initializePlugin(rawPtr, metadataIt->second)) {
            return std::unique_ptr<T>(static_cast<T*>(rawPtr));
        }
        
        return nullptr;
    }
    
    // Plugin discovery and scanning
    std::vector<std::string> scanPluginDirectories(const std::vector<std::string>& directories);
    bool loadPluginsFromDirectory(const std::string& directory);
    
    // Configuration management
    bool loadPluginConfigurations(const std::string& configFile);
    bool savePluginConfigurations(const std::string& configFile) const;
    bool updatePluginConfiguration(const std::string& name, const std::any& config);
    
    // Diagnostics and monitoring
    std::string getPluginReport() const;
    size_t getPluginCount() const;
    size_t getEnabledPluginCount() const;
    bool isPluginLoaded(const std::string& name) const;
    bool isPluginEnabled(const std::string& name) const;
    
    // Statistics
    struct PluginStats {
        size_t totalPlugins = 0;
        size_t enabledPlugins = 0;
        size_t disabledPlugins = 0;
        double totalLoadTime = 0.0;
        std::unordered_map<PluginCategory, size_t> pluginsByCategory;
    };
    
    PluginStats getStatistics() const;
    
    // Plugin lifecycle callbacks
    using PluginInitCallback = std::function<void(IPlugin*)>;
    using PluginShutdownCallback = std::function<void(IPlugin*)>;
    using PluginErrorCallback = std::function<void(const std::string&, const std::string&)';
    
    void registerInitCallback(const PluginInitCallback& callback);
    void registerShutdownCallback(const PluginShutdownCallback& callback);
    void registerErrorCallback(const PluginErrorCallback& callback);
    
    // Global plugin system management
    bool initializePluginSystem();
    void shutdownPluginSystem();
    bool isPluginSystemInitialized() const;
    
    // Plugin validation and health checks
    bool validatePluginDependencies(const PluginMetadata& metadata) const;
    bool validatePluginConfiguration(const PluginMetadata& metadata, const std::any& config) const;
    
    // Plugin persistence and recovery
    bool exportPlugins(const std::string& filePath) const;
    bool importPlugins(const std::string& filePath);
    bool backupPlugins(const std::string& backupPath);
    
protected:
    PluginRegistry() : initialized_(false) {}
    ~PluginRegistry() = default;
    
    // Internal helper methods
    PluginMetadata* findPluginByName(const std::string& name);
    const PluginMetadata* findPluginByName(const std::string& name) const;
    PluginMetadata* findPluginByPath(const std::string& filePath);
    const PluginMetadata* findPluginByPath(const std::string& filePath) const;
    
    bool initializePlugin(IPlugin* plugin, const PluginMetadata& metadata);
    void shutdownPlugin(IPlugin* plugin);
    bool validatePluginConfiguration(const std::string& name, const std::any& config);
    
    std::vector<PluginMetadata> discoverPlugins(const std::vector<std::string>& directories);
    bool loadPluginFromFile(const std::string& filePath);
    
    // Plugin instances storage
    std::unordered_map<std::type_index, std::unique_ptr<IPlugin>> pluginInstances_;
    std::unordered_map<std::string, PluginMetadata> registeredPlugins_;
    std::unordered_map<std::string, std::unique_ptr<IPlugin>> activeInstances_;
    
    // Callbacks
    std::vector<PluginInitCallback> initCallbacks_;
    std::vector<PluginShutdownCallback> shutdownCallbacks_;
    std::vector<PluginErrorCallback> errorCallbacks_;
    
    bool initialized_;
    std::chrono::steady_clock::time_point startTime_;
};

// Macro for plugin registration
#define REGISTER_PLUGIN(PluginClass, Name, Description, Category, Priority, Dependencies) 
namespace { 
    struct PluginRegistrar { 
        PluginRegistrar() {
            PluginRegistry& registry = PluginRegistry::getInstance();
            PluginMetadata metadata = {
                Name, Description, Category, "1.0", "Unknown", Priority, Dependencies,
                []() { return std::make_unique<PluginClass>(); }, __FILE__
            };
            registry.registerPlugin(metadata);
        }
    };
    PluginRegistrar registrar;
}

// Global plugin registry access
inline PluginRegistry& PluginRegistry() { return PluginRegistry::getInstance(); }

// Convenience functions for common plugin operations
inline bool loadPlugin(const std::string& name) { return PluginRegistry::getInstance().loadPlugin(name); }
inline bool unloadPlugin(const std::string& name) { return PluginRegistry::getInstance().unloadPlugin(name); }
inline bool enablePlugin(const std::string& name) { return PluginRegistry::getInstance().enablePlugin(name); }
inline bool disablePlugin(const std::string& name) { return PluginRegistry::getInstance().disablePlugin(name); }
inline bool registerPlugin(const PluginMetadata& metadata) { return PluginRegistry::getInstance().registerPlugin(metadata); }

} // namespace nlm
