#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <any>
#include <mutex>
#include <atomic>
#include <fstream>
#include <sstream>
#include <functional>
#include "Config.hpp"
#include "ConfigSchema.hpp"
#include "ConfigValidator.hpp"
#include "ConfigSource.hpp"
#include "ConfigCallbacks.hpp"
#include "ConfigGroup.hpp"
#include "ConfigTemplate.hpp"
#include "ConfigProfile.hpp"

namespace nlm {

// Configuration manager with advanced features
class ConfigManager {
public:
    struct ManagerConfig {
        bool enableValidation;
        bool enableAuditing;
        bool enableChangeTracking;
        bool enableCaching;
        bool enableLazyLoading;
        int maxHistorySize;
        std::string auditLogFile;
        std::chrono::milliseconds cacheTimeout;
        
        ManagerConfig()
            : enableValidation(true), enableAuditing(true), enableChangeTracking(true),
              enableCaching(true), enableLazyLoading(false), maxHistorySize(1000),
              auditLogFile("config_audit.log"), cacheTimeout(std::chrono::milliseconds(100)) {}
    };

    ConfigManager(const ManagerConfig& config = ManagerConfig());
    ~ConfigManager();

    // Load configuration from all sources
    bool loadConfiguration();

    // Save configuration to all writable sources
    bool saveConfiguration() const;

    // Get configuration value
    template<typename T>
    std::optional<T> get(const std::string& key) const;

    // Get configuration value or default
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;

    // Set configuration value
    template<typename T>
    bool set(const std::string& key, const T& value,
             ConfigSource::Type source = ConfigSource::Type::Runtime,
             const std::string& description = "");

    // Check if key exists
    bool has(const std::string& key) const;

    // Remove configuration key
    bool remove(const std::string& key);

    // Get all keys
    std::vector<std::string> getKeys() const;

    // Get configuration summary
    std::string summary() const;

    // Add configuration source
    bool addSource(std::shared_ptr<ConfigSource> source);

    // Remove configuration source
    bool removeSource(const std::string& typeName);

    // Get configuration sources
    std::vector<std::shared_ptr<ConfigSource>> getSources() const { return sources; }

    // Set schema for validation
    void setSchema(std::shared_ptr<ConfigSchema> schema) { this->schema = schema; }

    // Get schema
    std::shared_ptr<ConfigSchema> getSchema() const { return schema; }

    // Add change listener
    void addChangeListener(std::shared_ptr<ConfigChangeListener> listener);

    // Remove change listener
    void removeChangeListener(const std::string& listenerName);

    // Add validation callback
    void addValidationCallback(std::shared_ptr<ConfigValidationCallback> callback);

    // Remove validation callback
    void removeValidationCallback(const std::string& callbackName);

    // Add update callback
    void addUpdateCallback(std::shared_ptr<ConfigUpdateCallback> callback);

    // Remove update callback
    void removeUpdateCallback(const std::string& callbackName);

    // Add error callback
    void addErrorCallback(std::shared_ptr<ConfigErrorCallback> callback);

    // Remove error callback
    void removeErrorCallback(const std::string& callbackName);

    // Validate configuration
    bool validateConfiguration() const;

    // Get configuration history
    std::vector<ConfigChangeEvent> getHistory() const { return history; }

    // Clear history
    void clearHistory() { history.clear(); }

    // Export configuration to file
    bool exportToFile(const std::string& filepath, const std::string& format = "json") const;

    // Import configuration from file
    bool importFromFile(const std::string& filepath, const std::string& format = "json");

    // Get configuration groups
    std::shared_ptr<ConfigGroup> getGroups() const { return groups; }

    // Get configuration templates
    std::vector<std::unique_ptr<ConfigTemplate>> getTemplates() const { return templates; }

    // Add configuration template
    bool addTemplate(std::unique_ptr<ConfigTemplate> template);

    // Get configuration profiles
    std::vector<std::unique_ptr<ConfigProfile>> getProfiles() const { return profiles; }

    // Add configuration profile
    bool addProfile(std::unique_ptr<ConfigProfile> profile);

    // Apply profile
    bool applyProfile(const std::string& profileName);

    // Apply template
    bool applyTemplate(const std::string& templateName);

    // Lock configuration
    void lockConfiguration() { configurationLocked = true; }

    // Unlock configuration
    void unlockConfiguration() { configurationLocked = false; }

    // Check if configuration is locked
    bool isLocked() const { return configurationLocked; }

    // Get configuration manager version
    static const char* getVersion() { return "1.0.0"; }

private:
    // Internal helpers
    bool loadFromSource(std::shared_ptr<ConfigSource> source);
    bool saveToSource(std::shared_ptr<ConfigSource> source) const;
    void notifyChange(const ConfigChangeEvent& event);
    bool validateAgainstSchema(const Config& config) const;
    void logAudit(const ConfigChangeEvent& event) const;
    void addToHistory(const ConfigChangeEvent& event);

    // Member variables
    std::vector<std::shared_ptr<ConfigSource>> sources;
    std::shared_ptr<ConfigSchema> schema;
    std::shared_ptr<ConfigGroup> groups;
    std::vector<std::unique_ptr<ConfigTemplate>> templates;
    std::vector<std::unique_ptr<ConfigProfile>> profiles;
    std::vector<std::shared_ptr<ConfigChangeListener>> changeListeners;
    std::vector<std::shared_ptr<ConfigValidationCallback>> validationCallbacks;
    std::vector<std::shared_ptr<ConfigUpdateCallback>> updateCallbacks;
    std::vector<std::shared_ptr<ConfigErrorCallback>> errorCallbacks;
    std::vector<ConfigChangeEvent> history;
    mutable std::mutex mutex;
    bool configurationLocked;
    ManagerConfig managerConfig;
    
    // Cache for frequently accessed values
    mutable std::unordered_map<std::string, std::any> cache;
    mutable std::chrono::steady_clock::time_point lastCacheClear;
};

// Configuration watcher for monitoring changes
class ConfigWatcher {
public:
    ConfigWatcher(std::shared_ptr<ConfigManager> manager);
    ~ConfigWatcher();

    // Start watching
    bool start();

    // Stop watching
    void stop();

    // Check if watcher is running
    bool isRunning() const { return running; }

    // Set watch interval
    void setWatchInterval(const std::chrono::milliseconds& interval) { watchInterval = interval; }

    // Get watch interval
    std::chrono::milliseconds getWatchInterval() const { return watchInterval; }

private:
    std::shared_ptr<ConfigManager> manager;
    std::atomic<bool> running;
    std::chrono::milliseconds watchInterval;
    std::thread watchThread;
    mutable std::mutex mutex;

    void watchLoop();
};

// Configuration history tracker
class ConfigHistory {
public:
    ConfigHistory(size_t maxSize = 1000);
    ~ConfigHistory() = default;

    // Add event to history
    void addEvent(const ConfigChangeEvent& event);

    // Get recent events
    std::vector<ConfigChangeEvent> getRecentEvents(size_t count = 10) const;

    // Get events by type
    std::vector<ConfigChangeEvent> getEventsByType(ConfigChangeEvent::Type type) const;

    // Get events by key
    std::vector<ConfigChangeEvent> getEventsByKey(const std::string& key) const;

    // Clear history
    void clear() { events.clear(); }

    // Get history size
    size_t size() const { return events.size(); }

    // Check if history is empty
    bool empty() const { return events.empty(); }

private:
    std::vector<ConfigChangeEvent> events;
    size_t maxSize;
    mutable std::mutex mutex;
};

// Configuration optimizer for performance improvements
class ConfigOptimizer {
public:
    ConfigOptimizer() = default;
    ~ConfigOptimizer() = default;

    // Optimize configuration loading
    static void optimizeLoading(ConfigManager& manager);

    // Optimize configuration access
    static void optimizeAccess(ConfigManager& manager);

    // Compress configuration
    static std::string compress(const Config& config);

    // Decompress configuration
    static bool decompress(Config& config, const std::string& compressed);

    // Suggest configuration optimizations
    static std::vector<std::string> suggestOptimizations(const ConfigManager& manager);

private:
    static std::string compressImpl(const Config& config);
    static bool decompressImpl(Config& config, const std::string& compressed);
};

} // namespace nlm