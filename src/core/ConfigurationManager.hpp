#pragma once

/**
 * Configuration Manager
 * 
 * Centralized configuration management system with hierarchical configuration support,
 * schema validation, template management, and dynamic reloading for the NLM brain simulation project.
 * 
 * Features:
 * - Hierarchical configuration with inheritance
 * - Schema-based validation
 * - Template system for common configuration scenarios
 * - Dynamic configuration reloading
 * - Thread-safe configuration operations
 * - Comprehensive error handling
 * - Configuration logging and monitoring
 * - Version management
 * - Cross-platform support
 */

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <functional>
#include <chrono>
#include <algorithm>
#include <unordered_map>
#include <set>
#include <variant>

namespace nlm {
namespace core {

class NestedConfiguration;
class ConfigurationSchema;
class ConfigurationValidator;
class ConfigurationTemplates;
class ConfigurationBackup;
class ConfigurationLogging;
class ConfigurationMonitoring;
class ConfigurationVersioning;

/**
 * Configuration types
 */
enum class ConfigDataType {
    INTEGER,
    DOUBLE,
    BOOLEAN,
    STRING,
    STRING_ARRAY,
    DOUBLE_ARRAY,
    INTEGER_ARRAY,
    OBJECT,
    ARRAY
};

/**
 * Configuration source
 */
enum class ConfigSource {
    DEFAULT,
    FILE,
    COMMAND_LINE,
    ENVIRONMENT,
    RUNTIME,
    INHERITED
};

/**
 * Configuration error
 */
enum class ConfigErrorType {
    INVALID_VALUE,
    MISSING_REQUIRED_FIELD,
    TYPE_MISMATCH,
    VALUE_OUT_OF_RANGE,
    DUPLICATE_KEY,
    INVALID_FORMAT,
    FILE_NOT_FOUND,
    PERMISSION_DENIED,
    SCHEMA_VALIDATION_FAILED,
    TEMPLATE_VALIDATION_FAILED,
    INHERITANCE_ERROR
};

struct ConfigError {
    ConfigErrorType type;
    std::string message;
    std::string path;
    std::string value;
    
    ConfigError(ConfigErrorType t, const std::string& msg, const std::string& path = "")
        : type(t), message(msg), path(path), value("") {}
};

/**
 * Configuration node for hierarchical structure
 */
struct ConfigNode {
    std::string key;
    std::string value;
    ConfigDataType dataType;
    ConfigSource source;
    std::string description;
    std::vector<ConfigNode> children;
    std::string path;
    
    ConfigNode() : dataType(ConfigDataType::STRING), source(ConfigSource::DEFAULT) {}
    
    ConfigNode(const std::string& k, const std::string& v, ConfigDataType type = ConfigDataType::STRING,
               ConfigSource source = ConfigSource::RUNTIME, const std::string& desc = "")
        : key(k), value(v), dataType(type), source(source), description(desc) {}
    
    std::string getFullPath() const {
        return path.empty() ? key : path + "." + key;
    }
    
    bool hasChild(const std::string& childKey) const {
        return std::any_of(children.begin(), children.end(),
            [&childKey](const ConfigNode& child) { return child.key == childKey; });
    }
    
    ConfigNode* getChild(const std::string& childKey);
    const ConfigNode* getChild(const std::string& childKey) const;
    ConfigNode& addChild(const ConfigNode& child);
};

/**
 * Configuration change event
 */
struct ConfigChangeEvent {
    std::string key;
    std::string oldValue;
    std::string newValue;
    ConfigSource source;
    double timestamp;
    std::string sessionId;
    
    ConfigChangeEvent(const std::string& k, const std::string& oldVal, const std::string& newVal,
                     ConfigSource src, double ts, const std::string& session = "")
        : key(k), oldValue(oldVal), newValue(newVal), source(src), timestamp(ts), sessionId(session) {}
};

/**
 * Configuration session for tracking changes
 */
struct ConfigSession {
    std::string sessionId;
    std::string name;
    std::map<std::string, std::string> variables;
    double createdAt;
    double updatedAt;
    std::string description;
    
    ConfigSession(const std::string& id, const std::string& name = "")
        : sessionId(id), name(name), createdAt(0.0), updatedAt(0.0) {}
};

/**
 * Main ConfigurationManager class
 */
class ConfigurationManager {
public:
    // ConfigurationManager singleton
    static ConfigurationManager& getInstance();
    
    // Disable copying, enable moving
    ConfigurationManager(const ConfigurationManager&) = delete;
    ConfigurationManager& operator=(const ConfigurationManager&) = delete;
    ConfigurationManager(ConfigurationManager&&) noexcept = default;
    ConfigurationManager& operator=(ConfigurationManager&&) noexcept = default;
    
    ConfigurationManager();
    ~ConfigurationManager();
    
    // Core configuration operations
    bool initialize(const std::string& configFile = "");
    void shutdown();
    
    // Value retrieval
    std::string get(const std::string& key) const;
    int getInt(const std::string& key) const;
    double getDouble(const std::string& key) const;
    bool getBool(const std::string& key) const;
    std::vector<std::string> getStringList(const std::string& key) const;
    std::vector<double> getDoubleList(const std::string& key) const;
    std::vector<int> getIntList(const std::string& key) const;
    
    std::optional<std::string> getOptional(const std::string& key) const;
    
    // Default values
    std::string getOr(const std::string& key, const std::string& defaultValue) const;
    int getIntOr(const std::string& key, int defaultValue) const;
    double getDoubleOr(const std::string& key, double defaultValue) const;
    bool getBoolOr(const std::string& key, bool defaultValue) const;
    
    // Value setting
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::RUNTIME);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::RUNTIME);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::RUNTIME);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::RUNTIME);
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::RUNTIME);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::RUNTIME);
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::RUNTIME);
    
    // Existence checking
    bool has(const std::string& key) const;
    
    // Node operations
    ConfigNode* getNode(const std::string& key);
    const ConfigNode* getNode(const std::string& key) const;
    ConfigNode& addNode(const ConfigNode& node, ConfigSource source = ConfigSource::RUNTIME);
    bool removeNode(const std::string& key);
    bool moveNode(const std::string& fromKey, const std::string& toKey);
    
    // Configuration validation
    bool validate() const;
    std::vector<ConfigError> getValidationErrors() const;
    bool validateNode(const ConfigNode& node) const;
    
    // Schema operations
    bool setSchema(const ConfigurationSchema& schema);
    bool validateAgainstSchema() const;
    
    // Template operations
    bool applyTemplate(const std::string& templateName);
    bool saveAsTemplate(const std::string& templateName);
    std::vector<std::string> getAvailableTemplates() const;
    
    // Dynamic configuration
    bool reloadFromFile(const std::string& filepath);
    bool reloadFromEnvironment();
    bool reloadFromCommandLine(int argc, char** argv);
    bool shouldReload() const { return needs_reload_; }
    
    // Configuration inheritance
    bool addInheritance(const std::string& parentKey, const std::string& childKey);
    bool removeInheritance(const std::string& childKey);
    std::vector<std::string> getInheritedKeys(const std::string& key) const;
    bool resolveInheritance() const;
    
    // Configuration history
    std::vector<ConfigChangeEvent> getRecentChanges(size_t limit = 100) const;
    void clearChangeHistory();
    
    // Configuration sessions
    std::string createSession(const std::string& name = "", const std::string& description = "");
    bool switchSession(const std::string& sessionId);
    bool removeSession(const std::string& sessionId);
    std::string getCurrentSessionId() const { return current_session_id_; }
    std::vector<ConfigSession> getAllSessions() const { return sessions_; }
    
    // Backup and restore
    bool backup(const std::string& filename) const;
    bool restore(const std::string& filename);
    bool createSnapshot(const std::string& snapshotName);
    bool restoreSnapshot(const std::string& snapshotName);
    std::vector<std::string> getAvailableSnapshots() const { return available_snapshots_; }
    
    // Thread safety
    void lock() const;
    void unlock() const;
    bool tryLock() const;
    
    // Configuration summary
    std::string summary() const;
    size_t getNodeCount() const { return all_nodes_.size(); }
    size_t getKeyCount() const { return all_keys_.size(); }
    double getLastModifiedTime() const { return last_modified_time_; }
    
    // Change monitoring
    using ConfigChangeCallback = std::function<void(const ConfigChangeEvent& event)>;
    void subscribeToChanges(const ConfigChangeCallback& callback);
    void unsubscribeFromChanges(const ConfigChangeCallback& callback);
    void clearChangeCallbacks();
    
    // Performance monitoring
    double getLoadTime() const { return load_time_; }
    double getValidationTime() const { return validation_time_; }
    size_t getCacheSize() const { return cache_.size(); }
    
    // Configuration logging
    bool logChanges() const { return enable_change_logging_; }
    void enableChangeLogging(bool enable) { enable_change_logging_ = enable; }
    void logChange(const ConfigChangeEvent& event);
    
    // Debug and diagnostics
    void enableDebugMode(bool enable) { debug_mode_ = enable; }
    bool isDebugMode() const { return debug_mode_; }
    std::string dump() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    mutable std::mutex mutex_;
    bool needs_reload_ = false;
    bool enable_change_logging_ = true;
    bool debug_mode_ = false;
    double load_time_ = 0.0;
    double validation_time_ = 0.0;
    double last_modified_time_ = 0.0;
    std::string current_session_id_;
    std::vector<ConfigSession> sessions_;
    std::vector<std::string> available_snapshots_;
    std::vector<ConfigChangeCallback> change_callbacks_;
    
    // Internal helpers
    void addToCache(const std::string& key, const ConfigNode& node);
    ConfigNode* getFromCache(const std::string& key);
    void invalidateCache(const std::string& key);
    void clearCache();
    void notifyChange(const ConfigChangeEvent& event);
    bool applyInheritance(const ConfigNode& parent, ConfigNode& child) const;
    void updateNodePath(ConfigNode& node, const std::string& parentPath) const;
    std::vector<ConfigNode> getAllNodes() const;
    std::vector<std::string> getAllKeys() const;
    void rebuildIndices();
    ConfigNode* findNode(const std::string& key);
    const ConfigNode* findNode(const std::string& key) const;
    std::string normalizeKey(const std::string& key) const;
    bool validateNodeValue(const ConfigNode& node) const;
    std::vector<ConfigError> collectValidationErrors() const;
    void markDirty() { needs_reload_ = true; last_modified_time_ = std::chrono::duration<double>::zero(); }
    
    // Friend classes for accessing private members
    friend class NestedConfiguration;
    friend class ConfigurationSchema;
    friend class ConfigurationValidator;
    friend class ConfigurationTemplates;
    friend class ConfigurationBackup;
    friend class ConfigurationLogging;
    friend class ConfigurationMonitoring;
    friend class ConfigurationVersioning;
};

} // namespace core
} // namespace nlm
