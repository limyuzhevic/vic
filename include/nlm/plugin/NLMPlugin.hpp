#pragma once

#include <string>
#include <memory>
#include <typeindex>
#include <functional>
#include <vector>
#include <map>
#include <mutex>
#include <unordered_map>

namespace nlm {

// Forward declarations
class NLMPlugin;

/**
 * @enum PluginCategory
 * @brief Categories of plugins supported by the NLM plugin system
 */
enum class PluginCategory : uint8_t {
    Region = 0,
    Neuron = 1,
    Plasticity = 2,
    Sensory = 3,
    Memory = 4,
    Neuromodulation = 5
};

/**
 * @enum PluginStatus
 * @brief Status of a plugin in the system
 */
enum class PluginStatus : uint8_t {
    Unknown,
    Loading,
    Loaded,
    Configured,
    Active,
    Error,
    Unloaded
};

/**
 * @struct PluginMetadata
 * @brief Metadata for a plugin
 */
struct PluginMetadata {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    PluginCategory category;
    std::vector<std::string> dependencies;
    bool enabled;
    std::string configFile;
    
    PluginMetadata() : category(PluginCategory::Region), enabled(true) {}
    PluginMetadata(const std::string& n, const std::string& v, 
                   const std::string& desc, const std::string& auth,
                   PluginCategory cat = PluginCategory::Region)
        : name(n), version(v), description(desc), author(auth), 
          category(cat), enabled(true) {}
};

/**
 * @class NLMPlugin
 * @brief Abstract base class for all NLM plugins
 */
class NLMPlugin {
public:
    virtual ~NLMPlugin() = default;
    
    // Core plugin interface
    virtual const PluginMetadata& getMetadata() const = 0;
    virtual bool initialize() = 0;
    virtual bool configure(const std::string& config) = 0;
    virtual void shutdown() = 0;
    virtual PluginStatus getStatus() const = 0;
    
    // Type-specific factory methods
    virtual bool isRegionPlugin() const { return false; }
    virtual bool isNeuronPlugin() const { return false; }
    virtual bool isPlasticityPlugin() const { return false; }
    virtual bool isSensoryPlugin() const { return false; }
    virtual bool isMemoryPlugin() const { return false; }
    virtual bool isNeuromodulationPlugin() const { return false; }
    
    // Plugin interface for specific component types
    virtual void* getRegionFactory() { return nullptr; }
    virtual void* getNeuronFactory() { return nullptr; }
    virtual void* getPlasticityFactory() { return nullptr; }
    virtual void* getSensoryFactory() { return nullptr; }
    virtual void* getMemoryFactory() { return nullptr; }
    virtual void* getNeuromodulationFactory() { return nullptr; }
    
    // Registry methods (can be called by PluginRegistry)
    virtual bool registerPlugin() { return false; }
    virtual bool unregisterPlugin() { return false; }
};

/**
 * @class PluginFactory
 * @brief Creates plugin instances
 */
class PluginFactory {
public:
    virtual ~PluginFactory() = default;
    
    virtual std::unique_ptr<NLMPlugin> createPlugin() = 0;
    virtual bool canCreatePlugin(const PluginMetadata& metadata) const = 0;
    virtual std::vector<std::string> getSupportedTypes() const = 0;
};

/**
 * @class PluginRegistry
 * @brief Discovers and registers plugins
 */
class PluginRegistry {
public:
    static PluginRegistry& getInstance();
    
    // Plugin discovery
    bool discoverPlugins(const std::string& directory);
    std::vector<std::string> getAvailablePlugins() const;
    
    // Plugin registration
    bool registerPlugin(std::unique_ptr<NLMPlugin> plugin);
    bool unregisterPlugin(const std::string& pluginName);
    
    // Plugin lookup
    std::unique_ptr<NLMPlugin> getPlugin(const std::string& pluginName);
    std::vector<std::unique_ptr<NLMPlugin>> getPluginsByCategory(PluginCategory category) const;
    std::vector<std::unique_ptr<NLMPlugin>> getPlugins() const;
    
    // Dependency resolution
    bool checkDependencies(const PluginMetadata& metadata) const;
    bool resolveDependencies(const PluginMetadata& metadata) const;
    
private:
    PluginRegistry() = default;
    ~PluginRegistry() = default;
    PluginRegistry(const PluginRegistry&) = delete;
    PluginRegistry& operator=(const PluginRegistry&) = delete;
    
    std::unordered_map<std::string, std::unique_ptr<NLMPlugin>> plugins;
    std::mutex mutex;
};

/**
 * @class PluginLoader
 * @brief Loads plugins from shared libraries
 */
class PluginLoader {
public:
    static std::unique_ptr<NLMPlugin> loadPlugin(const std::string& filepath);
    static bool canLoadPlugin(const std::string& filepath);
    static std::vector<std::string> getPluginFiles(const std::string& directory) const;
    
private:
    PluginLoader() = default;
    ~PluginLoader() = default;
};

/**
 * @class PluginConfigurator
 * @brief Configures plugins with settings
 */
class PluginConfigurator {
public:
    static bool configurePlugin(NLMPlugin& plugin, const std::string& configKey, 
                               const std::string& configValue);
    static std::string getPluginConfig(NLMPlugin& plugin, const std::string& configKey);
    static bool savePluginConfig(const std::string& pluginName, 
                                const std::string& configFile = "");
    static bool loadPluginConfig(NLMPlugin& plugin, const std::string& configFile = "");
};

/**
 * @class PluginManager
 * @brief Central registry for all plugins
 */
class PluginManager {
public:
    static PluginManager& getInstance();
    
    // Plugin lifecycle management
    bool loadPlugin(const std::string& filepath);
    bool unloadPlugin(const std::string& pluginName);
    bool enablePlugin(const std::string& pluginName);
    bool disablePlugin(const std::string& pluginName);
    
    // Configuration
    bool configurePlugin(const std::string& pluginName, const std::string& config);
    void configureAllPlugins();
    
    // Plugin query
    std::unique_ptr<NLMPlugin> getPlugin(const std::string& pluginName) const;
    std::vector<std::string> getPluginNames() const;
    PluginStatus getPluginStatus(const std::string& pluginName) const;
    
    // Component factory access
    void* getRegionFactory(const std::string& pluginName) const;
    void* getNeuronFactory(const std::string& pluginName) const;
    void* getPlasticityFactory(const std::string& pluginName) const;
    void* getSensoryFactory(const std::string& pluginName) const;
    void* getMemoryFactory(const std::string& pluginName) const;
    void* getNeuromodulationFactory(const std::string& pluginName) const;
    
    // Statistics and monitoring
    size_t getPluginCount() const;
    size_t getPluginCountByCategory(PluginCategory category) const;
    std::string getSystemSummary() const;
    
    // Initialization and shutdown
    bool initialize();
    void shutdown();
    bool isInitialized() const;
    
    // Configuration integration
    void loadPluginConfigurations();
    void savePluginConfigurations() const;
    
private:
    PluginManager() = default;
    ~PluginManager() = default;
    PluginManager(const PluginManager&) = delete;
    PluginManager& operator=(const PluginManager&) = delete;
    
    std::unordered_map<std::string, std::unique_ptr<NLMPlugin>> plugins;
    std::unordered_map<std::string, PluginStatus> pluginStatuses;
    std::mutex mutex;
    bool initialized;
};

/**
 * @def NLM_PLUGIN
 * @brief Macro for easy plugin registration
 * @details Use this macro to define a plugin class that automatically registers itself
 */
#define NLM_PLUGIN(name, version, description, author, category) \
    namespace { \
        struct PluginRegistrationHelper { \
            PluginRegistrationHelper() { \
                auto plugin = createPluginInstance(); \
                if (plugin) { \
                    PluginRegistry::getInstance().registerPlugin(std::move(plugin)); \
                } \
            } \
        }; \
        static PluginRegistrationHelper pluginRegistrationHelper; \
        std::unique_ptr<NLMPlugin> createPluginInstance(); \
    }

/**
 * @def NLM_PLUGIN_FACTORY
 * @brief Macro for creating plugin factory functions
 * @details Use this macro to define the factory function for a plugin
 */
#define NLM_PLUGIN_FACTORY(pluginClass) \
    std::unique_ptr<NLMPlugin> createPluginInstance() { \
        return std::make_unique<pluginClass>(); \
    }

/**
 * @def NLM_PLUGIN_IMPLEMENTATION
 * @brief Macro for implementing a plugin class
 * @details Use this macro to mark a class as a plugin implementation
 */
#define NLM_PLUGIN_IMPLEMENTATION \
    const PluginMetadata& getMetadata() const override { \
        static PluginMetadata metadata(name, version, description, author, category); \
        return metadata; \
    } \
    bool initialize() override { return true; } \
    bool configure(const std::string& config) override { \
        /* Default configuration - override in derived classes */ \
        return true; \
    } \
    void shutdown() override {} \
    PluginStatus getStatus() const override { \
        return status; \
    } \
    bool registerPlugin() override { \
        return PluginRegistry::getInstance().registerPlugin(shared_from_this()); \
    } \
    bool unregisterPlugin() override { \
        return PluginRegistry::getInstance().unregisterPlugin(getMetadata().name); \
    }

} // namespace nlm