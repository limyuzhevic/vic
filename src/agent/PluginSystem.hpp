#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <functional>
#include <atomic>
#include <thread>
#include <filesystem>
#include <optional>
#include <variant>
#include <typeindex>
#include "../core/Config/Config.hpp"
#include "AgentBrain.hpp"

namespace nlm {

// Plugin system types
enum class PluginState {
    Unloaded,
    Loading,
    Loaded,
    Unloading,
    Failed
};

enum class PluginType {
    System,
    Utility,
    NeuralNetwork,
    Visualization,
    Performance,
    Memory,
    Custom
};

// Plugin interface base class
class IPlugin {
public:
    virtual ~IPlugin() = default;
    
    // Plugin metadata
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
    virtual PluginType getType() const = 0;
    
    // Lifecycle management
    virtual bool initialize(const Config& config) = 0;
    virtual void shutdown() = 0;
    virtual PluginState getState() const = 0;
    
    // Plugin commands
    virtual void onBrainUpdate(std::shared_ptr<Brain> brain) {}
    virtual void onSimulationStep(double timestep) {}
    virtual void onEpisodeStart() {}
    virtual void onEpisodeEnd() {}
    
    // Plugin configuration
    virtual bool validateConfiguration(const Config& config) const = 0;
    virtual void applyConfiguration(const Config& config) = 0;
    
    // Plugin dependency information
    virtual std::vector<std::string> getDependencies() const = 0;
    virtual std::vector<std::string> getProvides() const = 0;
};

// Plugin configuration entry
struct PluginConfigEntry {
    std::string key;
    std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>> value;
    std::string description;
    bool required;
    
    PluginConfigEntry() : key(), value(), description(), required(false) {}
    PluginConfigEntry(const std::string& k, const std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>& v,
                     const std::string& desc = "", bool req = false)
        : key(k), value(v), description(desc), required(req) {}
};

// Plugin metadata
struct PluginMetadata {
    std::string name;
    std::string version;
    std::string description;
    PluginType type;
    std::string author;
    std::string filename;
    std::string path;
    std::vector<std::string> dependencies;
    std::vector<std::string> provides;
    std::vector<PluginConfigEntry> configSchema;
    std::vector<std::string> tags;
    bool enabled;
    PluginState state;
    std::string errorMessage;
    
    PluginMetadata()
        : type(PluginType::Custom), author(""), filename(""), path(""), enabled(true), state(PluginState::Unloaded),
          errorMessage("") {}
};

// Dynamic library handle
#ifdef _WIN32
    #include <windows.h>
    using LibraryHandle = HMODULE;
#else
    #include <dlfcn.h>
    using LibraryHandle = void*;
#endif

// Plugin loading result
struct PluginLoadResult {
    bool success;
    std::string message;
    PluginState state;
    std::shared_ptr<IPlugin> plugin;
    std::unordered_map<std::string, std::string> loadedDependencies;
    
    PluginLoadResult() : success(false), state(PluginState::Failed) {}
    PluginLoadResult(bool s, const std::string& msg, PluginState st)
        : success(s), message(msg), state(st) {}
};

// Plugin system interface
class PluginSystem {
public:
    PluginSystem();
    ~PluginSystem();
    
    // Disable copying
    PluginSystem(const PluginSystem&) = delete;
    PluginSystem& operator=(const PluginSystem&) = delete;
    
    // Singleton access
    static PluginSystem& getInstance() {
        static PluginSystem instance;
        return instance;
    }
    
    // Plugin discovery and loading
    std::vector<PluginMetadata> discoverPlugins(const std::string& directory = ".");
    PluginLoadResult loadPlugin(const std::string& pluginPath, const Config& config);
    PluginLoadResult loadPlugin(const PluginMetadata& metadata, const Config& config);
    
    // Plugin management
    bool unloadPlugin(const std::string& pluginName);
    bool unloadPlugin(const PluginMetadata& metadata);
    bool unloadAllPlugins();
    
    // Plugin information
    bool hasPlugin(const std::string& pluginName) const;
    PluginState getPluginState(const std::string& pluginName) const;
    PluginMetadata* getPluginMetadata(const std::string& pluginName);
    const PluginMetadata* getPluginMetadata(const std::string& pluginName) const;
    
    // Plugin queries
    std::vector<PluginMetadata> getLoadedPlugins() const;
    std::vector<PluginMetadata> getPluginsByType(PluginType type) const;
    std::vector<PluginMetadata> getPluginsByTag(const std::string& tag) const;
    
    // Plugin communication
    void broadcastBrainUpdate(std::shared_ptr<Brain> brain);
    void broadcastSimulationStep(double timestep);
    void broadcastEpisodeStart();
    void broadcastEpisodeEnd();
    
    // Plugin configuration
    bool updatePluginConfiguration(const std::string& pluginName, const Config& config);
    Config getPluginConfiguration(const std::string& pluginName) const;
    
    // System operations
    bool initializeAllPlugins(const Config& config);
    void shutdownAllPlugins();
    void clearAllPlugins();
    
    // Dependency resolution
    bool checkDependencies(const PluginMetadata& metadata) const;
    std::vector<std::string> resolveDependencyChain(const PluginMetadata& metadata) const;
    
    // Thread safety
    void lock() const;
    void unlock() const;
    bool tryLock() const;
    
    // Configuration integration
    void setConfigSchema(const std::string& pluginName, const std::vector<PluginConfigEntry>& schema);
    std::vector<PluginConfigEntry> getConfigSchema(const std::string& pluginName) const;
    
    // Statistics and monitoring
    struct SystemStats {
        size_t totalPluginsLoaded{0};
        size_t activePlugins{0};
        size_t failedPlugins{0};
        uint64_t totalLoadTime{0};
        uint64_t totalUnloadTime{0};
        uint64_t totalMemoryUsage{0};
        std::unordered_map<std::string, uint64_t> pluginLoadTimes;
        std::unordered_map<std::string, uint64_t> pluginUnloadTimes;
        
        void reset() {
            totalPluginsLoaded = activePlugins = failedPlugins = 0;
            totalLoadTime = totalUnloadTime = totalMemoryUsage = 0;
            pluginLoadTimes.clear();
            pluginUnloadTimes.clear();
        }
    };
    
    SystemStats getSystemStats() const;
    
    // Error handling
    std::string getLastError() const { return lastError_; }
    void setLastError(const std::string& error) { lastError_ = error; }
    
    // File operations
    static std::string getPluginFilename(const std::string& pluginName);
    static bool isPluginFile(const std::string& filename);
    static std::string getPluginExtension();
    
private:
    // Internal plugin loading
    PluginLoadResult loadPluginInternal(const PluginMetadata& metadata, const Config& config);
    bool validatePluginMetadata(const PluginMetadata& metadata) const;
    PluginLoadResult loadPluginLibrary(const PluginMetadata& metadata, LibraryHandle& handle);
    std::shared_ptr<IPlugin> createPluginInstance(LibraryHandle handle, const std::string& factoryFunctionName);
    
    // Plugin lifecycle
    bool initializePlugin(std::shared_ptr<IPlugin> plugin, const Config& config);
    void cleanupPlugin(std::shared_ptr<IPlugin> plugin);
    PluginState updatePluginState(const std::string& pluginName, PluginState newState);
    
    // Dependency management
    bool resolveDependencies(const PluginMetadata& metadata, std::unordered_map<std::string, std::shared_ptr<IPlugin>>& loadedPlugins);
    
    // File system operations
    std::vector<std::string> scanDirectoryForPlugins(const std::string& directory) const;
    PluginMetadata parsePluginMetadataFromFile(const std::string& filepath) const;
    bool loadPluginMetadata(const std::string& filepath, PluginMetadata& metadata) const;
    
    // Dynamic library operations
    LibraryHandle loadLibrary(const std::string& filepath);
    bool unloadLibrary(LibraryHandle handle);
    void* getSymbol(LibraryHandle handle, const std::string& symbolName);
    
    // Configuration conversion
    Config convertPluginConfig(const std::vector<PluginConfigEntry>& pluginConfig) const;
    std::vector<PluginConfigEntry> convertConfigToPluginConfig(const Config& config) const;
    
    // Thread synchronization
    mutable std::mutex systemMutex_;
    std::atomic<bool> isLocked_;
    
    // Plugin storage
    std::unordered_map<std::string, PluginMetadata> plugins_;
    std::unordered_map<std::string, std::shared_ptr<IPlugin>> pluginInstances_;
    std::unordered_map<std::string, LibraryHandle> pluginLibraries_;
    
    // Configuration schemas
    std::unordered_map<std::string, std::vector<PluginConfigEntry>> pluginConfigSchemas_;
    
    // Statistics
    SystemStats stats_;
    
    // Error handling
    std::string lastError_;
    
    // Plugin factory
    using PluginFactory = std::function<std::shared_ptr<IPlugin>()>;
    std::unordered_map<std::string, PluginFactory> pluginFactories_;
};

// Plugin factory registration helper
class PluginRegistrar {
public:
    template<typename TPlugin>
    static bool registerPlugin(const std::string& name) {
        auto factory = []() -> std::shared_ptr<IPlugin> {
            return std::make_shared<TPlugin>();
        };
        
        PluginSystem::getInstance().pluginFactories_[name] = factory;
        return true;
    }
};

// Helper macro for plugin registration
#define REGISTER_PLUGIN(pluginClass, pluginName) \
    namespace { \
        bool _plugin_reg_##pluginName = PluginRegistrar::registerPlugin<pluginClass>(pluginName); \
    }

} // namespace nlm