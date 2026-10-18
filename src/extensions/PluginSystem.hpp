#include "PluginSystem.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>

namespace nlm {

// Plugin metadata
struct PluginMetadata {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string license;
    std::string dependencies;
    bool isCore = false;
    bool isEnabled = true;
    auto timestamp;
    std::string apiVersion;
    std::string platform;
    std::map<std::string, std::string> metadata;
};

// Plugin interface
class Plugin {
public:
    virtual ~Plugin() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
    virtual PluginMetadata getMetadata() const = 0;
    
    virtual bool initialize(const nlohmann::json& config) = 0;
    virtual void shutdown() = 0;
    
    virtual bool load() = 0;
    virtual bool unload() = 0;
    
    virtual std::vector<std::string> getCapabilities() const = 0;
    virtual std::vector<std::string> getDependencies() const = 0;
    
    virtual std::vector<nlohmann::json> getApiEndpoints() const = 0;
    virtual nlohmann::json callApi(const std::string& endpoint, const nlohmann::json& params) = 0;
    
    virtual bool validate() const = 0;
    virtual nlohmann::json getStatus() const = 0;
    
    virtual std::vector<std::string> getTags() const = 0;
    virtual bool hasTag(const std::string& tag) const = 0;
    
    virtual std::string getPath() const = 0;
    virtual bool isStatic() const = 0;
    
    virtual void setParent(void* parent) = 0;
    virtual void* getParent() const = 0;
    
    // Plugin lifecycle hooks
    virtual void onPluginLoad() {}
    virtual void onPluginUnload() {}
    virtual void onPluginEnable() {}
    virtual void onPluginDisable() {}
    
    // Event handling
    virtual void onEvent(const std::string& eventType, const nlohmann::json& data) {}
};

// Static plugin base class
class StaticPlugin : public Plugin {
public:
    StaticPlugin(const std::string& name, const std::string& version,
                 const std::string& description = "",
                 const std::string& author = "")
        : name_(name), version_(version), description_(description), author_(author) {}
    
    std::string getName() const override { return name_; }
    std::string getVersion() const override { return version_; }
    std::string getDescription() const override { return description_; }
    
    std::string getPath() const override { return ""; }
    bool isStatic() const override { return true; }
    
protected:
    std::string name_;
    std::string version_;
    std::string description_;
    std::string author_;
};

// Dynamic plugin loader
class PluginLoader {
public:
    PluginLoader();
    ~PluginLoader();
    
    // Plugin loading
    std::unique_ptr<Plugin> loadPlugin(const std::string& path);
    std::unique_ptr<Plugin> loadPlugin(const std::string& name, const std::string& version);
    
    // Plugin management
    bool unloadPlugin(const std::string& name);
    bool unloadPlugin(std::unique_ptr<Plugin>& plugin);
    bool isPluginLoaded(const std::string& name) const;
    std::vector<std::string> getLoadedPlugins() const;
    
    // Plugin discovery
    std::vector<std::string> discoverPlugins(const std::string& directory = "plugins") const;
    std::vector<PluginMetadata> getPluginMetadata(const std::string& directory = "plugins") const;
    
    // Plugin validation
    bool validatePlugin(const std::string& path) const;
    std::vector<std::string> validatePluginDependencies(const PluginMetadata& metadata) const;
    
    // Configuration
    void setPluginDirectory(const std::string& directory);
    std::string getPluginDirectory() const;
    void enableAutoLoad(bool enable);
    bool isAutoLoadEnabled() const;
    
    // Security
    void setSecurityPolicy(const std::string& policy);
    std::string getSecurityPolicy() const;
    bool validateSecurity(const Plugin& plugin) const;
    
    // Statistics
    struct LoaderStats {
        size_t totalPluginsLoaded = 0;
        size_t totalPluginsUnloaded = 0;
        size_t pluginsCurrentlyLoaded = 0;
        size_t failedLoads = 0;
        std::chrono::steady_clock::time_point lastLoad;
        double averageLoadTime = 0.0;
    };
    
    LoaderStats getStats() const;
    void resetStats();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Plugin registry
class PluginRegistry {
public:
    PluginRegistry();
    ~PluginRegistry();
    
    // Plugin registration
    void registerPlugin(std::unique_ptr<Plugin> plugin);
    void unregisterPlugin(const std::string& name);
    bool isPluginRegistered(const std::string& name) const;
    
    // Plugin retrieval
    Plugin* getPlugin(const std::string& name) const;
    std::vector<Plugin*> getPluginsByTag(const std::string& tag) const;
    std::vector<Plugin*> getPluginsByCapability(const std::string& capability) const;
    std::vector<Plugin*> getAllPlugins() const;
    
    // Plugin management
    void enablePlugin(const std::string& name);
    void disablePlugin(const std::string& name);
    void togglePlugin(const std::string& name);
    bool isPluginEnabled(const std::string& name) const;
    
    // Metadata
    PluginMetadata getPluginMetadata(const std::string& name) const;
    void updatePluginMetadata(const std::string& name, const PluginMetadata& metadata);
    
    // Configuration
    void setPluginConfig(const std::string& name, const nlohmann::json& config);
    nlohmann::json getPluginConfig(const std::string& name) const;
    
    // Event system
    void emitEvent(const std::string& eventType, const nlohmann::json& data);
    void addEventListener(const std::string& eventType,
                         std::function<void(const nlohmann::json&)> listener);
    void removeEventListener(const std::string& eventType,
                            std::function<void(const nlohmann::json&)> listener);
    
    // Plugin lifecycle
    void enableAllPlugins();
    void disableAllPlugins();
    void shutdownAllPlugins();
    
    // Statistics
    struct RegistryStats {
        size_t totalPlugins = 0;
        size_t enabledPlugins = 0;
        size_t disabledPlugins = 0;
        size_t failedPlugins = 0;
        std::chrono::steady_clock::time_point lastUpdate;
        std::unordered_map<std::string, PluginMetadata> pluginMetadata;
    };
    
    RegistryStats getStats() const;
    void resetStats();
    
    // Export/Import
    void exportRegistry(const std::string& filename) const;
    void importRegistry(const std::string& filename);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Plugin manager
class PluginManager {
public:
    PluginManager();
    ~PluginManager();
    
    // Initialization
    bool initialize(const nlohmann::json& config = nlohmann::json::object());
    void shutdown();
    bool isInitialized() const;
    
    // Plugin lifecycle
    bool loadPlugin(const std::string& path);
    bool loadPlugin(const std::string& name, const std::string& version,
                   const nlohmann::json& config = nlohmann::json::object());
    
    bool unloadPlugin(const std::string& name);
    bool unloadPlugin(std::unique_ptr<Plugin>& plugin);
    
    // Plugin operations
    void enablePlugin(const std::string& name);
    void disablePlugin(const std::string& name);
    void togglePlugin(const std::string& name);
    bool isPluginEnabled(const std::string& name) const;
    
    // Plugin retrieval
    Plugin* getPlugin(const std::string& name) const;
    std::vector<Plugin*> getPlugins() const;
    std::vector<Plugin*> getPluginsByTag(const std::string& tag) const;
    
    // Plugin metadata
    PluginMetadata getPluginMetadata(const std::string& name) const;
    std::vector<PluginMetadata> getPluginMetadata() const;
    std::vector<std::string> getAvailablePlugins() const;
    
    // Configuration
    void setGlobalConfig(const nlohmann::json& config);
    nlohmann::json getGlobalConfig() const;
    
    // Event system
    void emitEvent(const std::string& eventType, const nlohmann::json& data);
    void addEventListener(const std::string& eventType,
                         std::function<void(const nlohmann::json&)> listener);
    void removeEventListener(const std::string& eventType,
                            std::function<void(const nlohmann::json&)> listener);
    
    // Plugin operations
    void enableAllPlugins();
    void disableAllPlugins();
    void shutdownAllPlugins();
    void reloadPlugin(const std::string& name);
    
    // Statistics
    struct ManagerStats {
        size_t totalPluginsLoaded = 0;
        size_t totalPluginsUnloaded = 0;
        size_t pluginsCurrentlyLoaded = 0;
        size_t failedLoads = 0;
        std::chrono::steady_clock::time_point lastUpdate;
        double averageLoadTime = 0.0;
        double averageUnloadTime = 0.0;
    };
    
    ManagerStats getStats() const;
    void resetStats();
    
    // Monitoring
    void enableMonitoring(bool enable);
    bool isMonitoringEnabled() const;
    nlohmann::json getPerformanceMetrics() const;
    
    // Plugin validation
    bool validatePlugin(const std::string& path) const;
    std::vector<std::string> getValidationErrors(const std::string& path) const;
    
    // API management
    std::vector<nlohmann::json> getAvailableApiEndpoints() const;
    nlohmann::json callPluginApi(const std::string& pluginName,
                               const std::string& endpoint,
                               const nlohmann::json& params = nlohmann::json::object());
    
    // Export/Import
    void exportPluginState(const std::string& filename) const;
    void importPluginState(const std::string& filename);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Plugin utilities
namespace PluginUtils {
    // Plugin discovery
    std::string findPluginFile(const std::string& pluginName,
                              const std::string& pluginDirectory = "plugins");
    std::string findPluginFileByName(const std::string& name,
                                    const std::string& pluginDirectory = "plugins");
    
    // Plugin metadata extraction
    PluginMetadata extractMetadataFromFile(const std::string& filepath);
    PluginMetadata extractMetadataFromSource(const std::string& sourceCode);
    
    // Plugin validation
    bool validatePluginName(const std::string& name);
    bool validatePluginVersion(const std::string& version);
    bool validatePluginPath(const std::string& path);
    bool validatePluginDependencies(const std::string& dependencies);
    
    // File operations
    bool copyPluginFile(const std::string& source, const std::string& destination);
    bool movePluginFile(const std::string& source, const std::string& destination);
    bool deletePluginFiles(const std::string& pluginDirectory);
    
    // Compression
    bool compressPluginFiles(const std::string& sourceDirectory, const std::string& outputFile);
    bool decompressPluginFiles(const std::string& archiveFile, const std::string& outputDirectory);
    
    // Security
    bool validatePluginSourceCode(const std::string& sourceCode);
    bool scanPluginForMalware(const std::string& filepath);
    
    // Logging
    void logPluginEvent(const std::string& pluginName, const std::string& event,
                      const nlohmann::json& data = nlohmann::json::object());
    
    // Plugin updates
    bool checkForPluginUpdates(const std::string& pluginName, const std::string& version);
    bool updatePlugin(const std::string& pluginName, const std::string& version);
    
    // Plugin installation
    bool installPlugin(const std::string& pluginPath, const std::string& installDirectory = "plugins");
    bool uninstallPlugin(const std::string& pluginName, const std::string& pluginDirectory = "plugins");
}

} // namespace nlm
