#pragma once

// Core plugin infrastructure for NLM (Neural Learning Machine)
// This file provides the fundamental building blocks for plugin development

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <unordered_set>
#include <chrono>
#include <mutex>
#include <atomic>
#include <filesystem>
#include <optional>
#include <variant>

namespace nlm {

// Plugin System Version
const std::string PLUGIN_SYSTEM_VERSION = "2.0.0";
const std::string PLUGIN_SYSTEM_NAME = "NLM Plugin System";

// Plugin Loading Status
enum class PluginLoadStatus {
    NotLoaded,
    Loading,
    Loaded,
    Failed,
    Unloading,
    Unloaded,
    Errored
};

// Plugin Security Level
enum class PluginSecurityLevel {
    Low,      // Public plugins, minimal restrictions
    Medium,   // Sandbox with some restrictions
    High,     // Full sandbox, verified code
    Critical  // System-critical plugins, high security
};

// Plugin Capabilities
struct PluginCapabilities {
    std::vector<std::string> abilities;
    std::vector<std::string> integrations;
    std::vector<std::string> dependencies;
    std::vector<std::string> conflicts;
    
    bool canHandle(const std::string& ability) const {
        return std::find(abilities.begin(), abilities.end(), ability) != abilities.end();
    }
    
    bool integratesWith(const std::string& system) const {
        return std::find(integrations.begin(), integrations.end(), system) != integrations.end();
    }
    
    bool dependsOn(const std::string& plugin) const {
        return std::find(dependencies.begin(), dependencies.end(), plugin) != dependencies.end();
    }
    
    bool conflictsWith(const std::string& plugin) const {
        return std::find(conflicts.begin(), conflicts.end(), plugin) != conflicts.end();
    }
};

// Plugin Metadata
struct PluginMetadata {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string license;
    std::string homepage;
    std::string documentation;
    std::string contact;
    
    std::chrono::system_clock::time_point creationDate;
    std::chrono::system_clock::time_point lastUpdated;
    
    PluginSecurityLevel securityLevel;
    PluginCapabilities capabilities;
    
    std::string getDisplayInfo() const {
        return name + " v" + version + " by " + author + " - " + description;
    }
};

// Plugin Configuration Value
using PluginConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>
>;

// Plugin Configuration Entry
struct PluginConfigEntry {
    std::string key;
    PluginConfigValue value;
    std::string description;
    bool required;
    std::string defaultValue;
    
    std::string toString() const {
        if (std::holds_alternative<int>(value)) {
            return std::to_string(std::get<int>(value));
        } else if (std::holds_alternative<double>(value)) {
            return std::to_string(std::get<double>(value));
        } else if (std::holds_alternative<bool>(value)) {
            return std::get<bool>(value) ? "true" : "false";
        } else if (std::holds_alternative<std::string>(value)) {
            return std::get<std::string>(value);
        }
        return "";
    }
};

// Plugin Statistics
struct PluginStatistics {
    std::atomic<size_t> loadCount{0};
    std::atomic<size_t> unloadCount{0};
    std::atomic<size_t> updateCount{0};
    std::atomic<double> totalUpdateTime{0.0};
    std::atomic<size_t> errorCount{0};
    
    double getAverageUpdateTime(size_t count) const {
        return count > 0 ? (totalUpdateTime / count) : 0.0;
    }
};

// Plugin Result
enum class PluginResult {
    Success,
    Failure,
    Warning,
    Error,
    NotImplemented,
    InvalidInput,
    Timeout,
    SecurityViolation
};

// Plugin Event Types
enum class PluginEventType {
    LoadStarted,
    LoadCompleted,
    LoadFailed,
    UnloadStarted,
    UnloadCompleted,
    Update,
    ConfigurationChanged,
    ErrorOccurred,
    WarningOccurred,
    InfoOccurred
};

// Plugin Event
struct PluginEvent {
    PluginEventType type;
    std::string pluginName;
    std::string message;
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, std::string> data;
};

// Plugin Event Callback
using PluginEventCallback = std::function<void(const PluginEvent&)>;

// Base Plugin Interface
class Plugin {
public:
    virtual ~Plugin() = default;
    
    // Plugin identification
    virtual const PluginMetadata& getMetadata() const = 0;
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getAuthor() const = 0;
    
    // Plugin lifecycle
    virtual PluginLoadStatus getLoadStatus() const = 0;
    virtual bool initialize(const std::map<std::string, PluginConfigValue>& config) = 0;
    virtual bool update(double deltaTime) = 0;
    virtual bool shutdown() = 0;
    virtual bool configure(const std::map<std::string, PluginConfigValue>& config) = 0;
    
    // Plugin capabilities
    virtual PluginCapabilities getCapabilities() const = 0;
    virtual bool supportsFeature(const std::string& feature) const = 0;
    
    // Plugin state management
    virtual std::map<std::string, PluginConfigValue> getState() const = 0;
    virtual bool setState(const std::map<std::string, PluginConfigValue>& state) = 0;
    virtual bool saveState(const std::string& filepath) const = 0;
    virtual bool loadState(const std::string& filepath) = 0;
    
    // Plugin information
    virtual std::vector<std::string> getDependencies() const = 0;
    virtual PluginSecurityLevel getSecurityLevel() const = 0;
    virtual std::vector<std::string> getTags() const = 0;
    virtual std::string getCategory() const = 0;
    
    // Plugin statistics
    virtual PluginStatistics getStatistics() const = 0;
    
    // Plugin event handling
    virtual void addEventCallback(PluginEventCallback callback) = 0;
    virtual void removeEventCallback(PluginEventCallback callback) = 0;
    
    // Plugin system integration
    virtual bool integrateWith(std::shared_ptr<Plugin> otherPlugin) = 0;
    virtual bool removeIntegration(std::shared_ptr<Plugin> otherPlugin) = 0;
    
    // Plugin validation
    virtual bool validate() const = 0;
    virtual std::vector<std::string> getValidationErrors() const = 0;
    
    // Plugin commands
    virtual bool executeCommand(const std::string& command, const std::map<std::string, std::string>& args) = 0;
    
    // Plugin help
    virtual std::string getHelp() const = 0;
    
    // Plugin comparison
    virtual bool operator==(const Plugin& other) const = 0;
    virtual bool operator<(const Plugin& other) const = 0;
};

// Plugin Factory Interface
class PluginFactory {
public:
    virtual ~PluginFactory() = default;
    virtual std::unique_ptr<Plugin> createPlugin(const std::string& name) = 0;
    virtual std::string getPluginType() const = 0;
    virtual PluginMetadata getMetadata() const = 0;
    virtual bool canCreate(const std::string& name) const = 0;
};

// Plugin Discovery Interface
class PluginDiscovery {
public:
    virtual ~PluginDiscovery() = default;
    virtual std::vector<std::string> discoverPlugins(const std::filesystem::path& directory) const = 0;
    virtual bool loadPlugin(const std::filesystem::path& filepath) = 0;
    virtual bool unloadPlugin(const std::string& name) = 0;
    virtual std::vector<std::filesystem::path> getPluginPaths() const = 0;
    virtual bool addPluginPath(const std::filesystem::path& path) = 0;
    virtual bool removePluginPath(const std::filesystem::path& path) = 0;
};

// Plugin Security Interface
class PluginSecurity {
public:
    virtual ~PluginSecurity() = default;
    virtual bool validatePlugin(const PluginMetadata& metadata) = 0;
    virtual bool verifyPlugin(const std::filesystem::path& filepath) = 0;
    virtual bool checkPermissions(const PluginMetadata& metadata, const std::vector<std::string>& requiredPermissions) = 0;
    virtual bool isSafe(const Plugin& plugin) const = 0;
    virtual bool quarantinePlugin(const std::string& name) = 0;
    virtual bool restorePlugin(const std::string& name) = 0;
    virtual std::vector<std::string> getSecurityViolations(const std::string& name) const = 0;
};

// Plugin Registry Interface
class PluginRegistry {
public:
    virtual ~PluginRegistry() = default;
    virtual bool registerPlugin(std::unique_ptr<Plugin> plugin) = 0;
    virtual bool unregisterPlugin(const std::string& name) = 0;
    virtual Plugin* getPlugin(const std::string& name) = 0;
    virtual std::vector<Plugin*> getPluginsByCategory(const std::string& category) = 0;
    virtual std::vector<Plugin*> getPluginsByCapability(const std::string& capability) = 0;
    virtual std::vector<Plugin*> getAllPlugins() = 0;
    virtual bool isPluginRegistered(const std::string& name) const = 0;
    virtual size_t getPluginCount() const = 0;
    virtual void clear() = 0;
    virtual std::vector<std::string> getRegisteredPluginNames() const = 0;
};

// Plugin Lifecycle Manager Interface
class PluginLifecycleManager {
public:
    virtual ~PluginLifecycleManager() = default;
    virtual bool startPlugin(const std::string& name) = 0;
    virtual bool stopPlugin(const std::string& name) = 0;
    virtual bool pausePlugin(const std::string& name) = 0;
    virtual bool resumePlugin(const std::string& name) = 0;
    virtual PluginLoadStatus getPluginStatus(const std::string& name) const = 0;
    virtual std::chrono::system_clock::time_point getPluginStartTime(const std::string& name) const = 0;
    virtual std::chrono::system_clock::time_point getPluginStopTime(const std::string& name) const = 0;
    virtual double getPluginUptime(const std::string& name) const = 0;
    virtual std::map<std::string, PluginLoadStatus> getAllPluginStatuses() const = 0;
    virtual bool reloadPlugin(const std::string& name) = 0;
    virtual bool gracefulShutdown() = 0;
    virtual bool forceShutdown() = 0;
};

// Plugin Configuration Manager Interface
class PluginConfigurationManager {
public:
    virtual ~PluginConfigurationManager() = default;
    virtual bool setPluginConfig(const std::string& name, const std::map<std::string, PluginConfigValue>& config) = 0;
    virtual std::map<std::string, PluginConfigValue> getPluginConfig(const std::string& name) const = 0;
    virtual std::map<std::string, std::map<std::string, PluginConfigValue>> getAllPluginConfigs() const = 0;
    virtual bool validatePluginConfig(const std::string& name, const std::map<std::string, PluginConfigValue>& config) const = 0;
    virtual std::vector<std::string> getPluginConfigSchema(const std::string& name) const = 0;
    virtual std::map<std::string, PluginConfigValue> getDefaultPluginConfig(const std::string& name) const = 0;
    virtual bool savePluginConfig(const std::string& name, const std::string& filepath) const = 0;
    virtual bool loadPluginConfig(const std::string& name, const std::string& filepath) = 0;
    virtual void resetPluginConfig(const std::string& name) = 0;
    virtual void resetAllPluginConfigs() = 0;
};

// Plugin System Statistics
struct PluginSystemStatistics {
    size_t totalPlugins{0};
    size_t activePlugins{0};
    size_t loadedPlugins{0};
    size_t failedPlugins{0};
    double systemMemoryUsage{0.0};
    double systemCPUUsage{0.0};
    size_t pluginLoadCount{0};
    size_t pluginUnloadCount{0};
    double averagePluginLoadTime{0.0};
    double averagePluginUnloadTime{0.0};
    
    double getPluginActivationRate() const {
        return totalPlugins > 0 ? (static_cast<double>(activePlugins) / totalPlugins) * 100.0 : 0.0;
    }
};

} // namespace nlm