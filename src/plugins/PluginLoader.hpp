#pragma once

#include "IPlugin.hpp"
#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <functional>

namespace nlm {
namespace plugins {

// Plugin configuration management
class PluginConfig {
public:
    PluginConfig();
    explicit PluginConfig(const std::string& filename);
    ~PluginConfig();
    
    // Configuration loading
    bool loadFromFile(const std::string& filename);
    bool loadFromString(const std::string& configString);
    bool saveToFile(const std::string& filename) const;
    
    // Configuration access
    bool hasKey(const std::string& key) const;
    std::string getString(const std::string& key, const std::string& defaultValue = "") const;
    int getInt(const std::string& key, int defaultValue = 0) const;
    float getFloat(const std::string& key, float defaultValue = 0.0f) const;
    bool getBool(const std::string& key, bool defaultValue = false) const;
    std::vector<std::string> getStringList(const std::string& key) const;
    std::map<std::string, std::string> getSection(const std::string& sectionName) const;
    
    // Configuration setting
    void setString(const std::string& key, const std::string& value);
    void setInt(const std::string& key, int value);
    void setFloat(const std::string& key, float value);
    void setBool(const std::string& key, bool value);
    void setStringList(const std::string& key, const std::vector<std::string>& values);
    void setSection(const std::string& sectionName, const std::map<std::string, std::string>& values);
    
    // Plugin-specific configuration
    std::map<std::string, std::string> getPluginConfig(const std::string& pluginName) const;
    bool setPluginConfig(const std::string& pluginName, const std::map<std::string, std::string>& config);
    bool mergePluginConfig(const std::string& pluginName, const std::map<std::string, std::string>& config);
    
    // Configuration validation
    bool validatePluginConfig(const std::string& pluginName) const;
    std::vector<std::string> getConfigErrors(const std::string& pluginName) const;
    
    // Configuration templates
    static std::map<std::string, std::string> getDefaultPluginConfig(const std::string& pluginName);
    static std::vector<std::string> getRequiredPluginParameters(const std::string& pluginName);
    static std::vector<std::string> getOptionalPluginParameters(const std::string& pluginName);
    
    // Configuration export/import
    std::string exportConfig() const;
    bool importConfig(const std::string& configString);
    
    // Configuration monitoring
    using ConfigChangeCallback = std::function<void(const std::string& key, const std::string& oldValue, const std::string& newValue)>;
    void subscribeToConfigChanges(const ConfigChangeCallback& callback);
    void unsubscribeFromConfigChanges(const ConfigChangeCallback& callback);
    
    // Configuration history
    std::vector<std::pair<std::string, std::string>> getConfigHistory(const std::string& key) const;
    void clearConfigHistory();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Event types
enum class PluginEventType {
    PLUGIN_LOADED,
    PLUGIN_UNLOADED,
    PLUGIN_ENABLED,
    PLUGIN_DISABLED,
    PLUGIN_INITIALIZED,
    PLUGIN_SHUTDOWN,
    PLUGIN_UPDATED,
    PLUGIN_ERROR,
    PLUGIN_CONFIG_CHANGED,
    PLUGIN_DEPENDENCY_RESOLVED,
    PLUGIN_VALIDATION_PASSED,
    PLUGIN_VALIDATION_FAILED,
    SYSTEM_EVENT
};

// Event data
struct PluginEvent {
    PluginEventType type;
    std::string pluginName;
    double timestamp;
    std::map<std::string, std::string> data;
    std::map<std::string, std::string> metadata;
    
    PluginEvent(PluginEventType t, const std::string& name)
        : type(t), pluginName(name), timestamp(0.0) {}
};

// Event system for plugin communication
class PluginEventSystem {
public:
    PluginEventSystem();
    ~PluginEventSystem();
    
    // Event publishing
    void publishEvent(const PluginEvent& event);
    void publishEvent(PluginEventType type, const std::string& pluginName, const std::map<std::string, std::string>& data = {});
    
    // Event subscription
    using EventCallback = std::function<void(const PluginEvent& event)>;
    void subscribeToEvent(PluginEventType eventType, const EventCallback& callback);
    void unsubscribeToEvent(PluginEventType eventType, const EventCallback& callback);
    void subscribeToPluginEvent(const std::string& pluginName, PluginEventType eventType, const EventCallback& callback);
    void unsubscribeFromPluginEvent(const std::string& pluginName, PluginEventType eventType, const EventCallback& callback);
    
    // Event filtering and routing
    void setEventFilter(const std::function<bool(const PluginEvent&)>& filter);
    void addEventRouter(const std::string& eventName, const std::function<void(const PluginEvent&)>& router);
    
    // Event history
    std::vector<PluginEvent> getEventHistory(PluginEventType eventType, size_t limit = 100) const;
    std::vector<PluginEvent> getPluginEventHistory(const std::string& pluginName, PluginEventType eventType, size_t limit = 100) const;
    void clearEventHistory();
    
    // Event statistics
    size_t getEventCount(PluginEventType eventType) const;
    double getAverageEventInterval(PluginEventType eventType) const;
    
    // Event prioritization
    void setEventPriority(const std::string& eventName, int priority);
    int getEventPriority(const std::string& eventName) const;
    
    // Thread safety
    void lock() const;
    void unlock() const;
    bool tryLock() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Plugin loader for dynamic library loading
class PluginLoader {
public:
    PluginLoader();
    ~PluginLoader();
    
    // Plugin loading
    std::unique_ptr<IPlugin> loadPluginFromFile(const std::string& filepath, const std::string& pluginName = "");
    bool unloadPlugin(IPlugin* plugin);
    
    // Plugin discovery
    std::vector<std::string> discoverPlugins(const std::string& directory) const;
    bool isPluginFile(const std::string& filename) const;
    
    // Platform-specific loading
    bool initializePlatformSupport();
    bool cleanupPlatformSupport();
    
    // Error handling
    std::vector<std::string> getLastErrors() const;
    void clearErrors();
    
    // Symbol resolution
    void* resolveSymbol(const std::string& libraryName, const std::string& symbolName);
    bool hasSymbol(const std::string& libraryName, const std::string& symbolName) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace plugins
} // namespace nlm
