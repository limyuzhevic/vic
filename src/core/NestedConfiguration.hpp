#pragma once

/**
 * Nested Configuration
 * 
 * Hierarchical configuration structure supporting nested configurations with inheritance,
 * parent-child relationships, and path-based access for the NLM brain simulation project.
 * 
 * Features:
 * - Nested configuration tree structure
 * - Parent-child relationships
 * - Path-based configuration access
 * - Configuration merging
 * - Sub-configuration management
 * - Hierarchical configuration resolution
 */

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <algorithm>
#include <unordered_map>
#include <set>

namespace nlm {
namespace core {

class ConfigurationManager;

/**
 * Nested configuration flags
 */
enum class NestedConfigFlags {
    NONE = 0,
    READ_ONLY = 1 << 0,
    AUTO_MERGE = 1 << 1,
    DEEP_COPY = 1 << 2,
    PRESERVE_ORDER = 1 << 3
};

/**
 * Configuration path component
 */
struct ConfigPathComponent {
    std::string name;
    size_t index; // For array elements
    bool isWildcard;
    
    ConfigPathComponent(const std::string& n, size_t idx = 0, bool wildcard = false)
        : name(n), index(idx), isWildcard(wildcard) {}
};

/**
 * Configuration path
 */
class ConfigPath {
public:
    ConfigPath() = default;
    explicit ConfigPath(const std::string& path);
    ConfigPath(const std::vector<ConfigPathComponent>& components);
    
    std::string toString() const;
    bool isAbsolute() const { return absolute_; }
    bool isEmpty() const { return components_.empty(); }
    size_t depth() const { return components_.size(); }
    
    const ConfigPathComponent& operator[](size_t index) const;
    ConfigPathComponent& operator[](size_t index);
    
    ConfigPath getParent() const;
    std::string getLastComponent() const;
    
    bool matches(const ConfigPath& other) const;
    bool startsWith(const ConfigPath& other) const;
    ConfigPath join(const std::string& component) const;
    
    bool operator==(const ConfigPath& other) const;
    bool operator!=(const ConfigPath& other) const;
    
private:
    std::vector<ConfigPathComponent> components_;
    bool absolute_ = false;
};

/**
 * Nested configuration entry with metadata
 */
struct NestedConfigEntry {
    std::string key;
    std::string value;
    ConfigDataType dataType;
    ConfigSource source;
    std::string description;
    std::string path;
    std::map<std::string, std::string> metadata;
    double timestamp;
    
    NestedConfigEntry() : dataType(ConfigDataType::STRING), source(ConfigSource::DEFAULT), timestamp(0.0) {}
    
    NestedConfigEntry(const std::string& k, const std::string& v, ConfigDataType type = ConfigDataType::STRING,
                     ConfigSource source = ConfigSource::RUNTIME, const std::string& desc = "")
        : key(k), value(v), dataType(type), source(source), description(desc), timestamp(0.0) {}
};

/**
 * Nested configuration node
 */
class NestedConfigNode {
public:
    NestedConfigNode();
    explicit NestedConfigNode(const std::string& key);
    NestedConfigNode(const std::string& key, const std::string& value, ConfigDataType type = ConfigDataType::STRING);
    
    std::string getKey() const { return entry_.key; }
    std::string getValue() const { return entry_.value; }
    ConfigDataType getDataType() const { return entry_.dataType; }
    ConfigSource getSource() const { return entry_.source; }
    std::string getDescription() const { return entry_.description; }
    std::string getPath() const { return entry_.path; }
    
    void setValue(const std::string& value, ConfigSource source = ConfigSource::RUNTIME);
    void setDescription(const std::string& desc) { entry_.description = desc; }
    void setMetadata(const std::string& key, const std::string& value) { entry_.metadata[key] = value; }
    
    bool hasChild(const std::string& childKey) const;
    NestedConfigNode* getChild(const std::string& childKey);
    const NestedConfigNode* getChild(const std::string& childKey) const;
    NestedConfigNode& addChild(const std::string& key, const std::string& value = "", ConfigDataType type = ConfigDataType::STRING);
    bool removeChild(const std::string& childKey);
    
    size_t getChildCount() const { return children_.size(); }
    std::vector<std::string> getChildKeys() const;
    
    NestedConfigEntry& getEntry() { return entry_; }
    const NestedConfigEntry& getEntry() const { return entry_; }
    
    std::string toString(size_t indent = 0) const;
    
private:
    NestedConfigEntry entry_;
    std::map<std::string, std::unique_ptr<NestedConfigNode>> children_;
    
    friend class NestedConfiguration;
};

/**
 * Nested configuration tree manager
 */
class NestedConfiguration {
public:
    NestedConfiguration();
    ~NestedConfiguration();
    
    // Disable copying, enable moving
    NestedConfiguration(const NestedConfiguration&) = delete;
    NestedConfiguration& operator=(const NestedConfiguration&) = delete;
    NestedConfiguration(NestedConfiguration&&) noexcept = default;
    NestedConfiguration& operator=(NestedConfiguration&&) noexcept = default;
    
    // Configuration tree operations
    NestedConfigNode* getRoot() { return root_.get(); }
    const NestedConfigNode* getRoot() const { return root_.get(); }
    
    NestedConfigNode* getNode(const ConfigPath& path);
    const NestedConfigNode* getNode(const ConfigPath& path) const;
    NestedConfigNode* getNode(const std::string& keyPath);
    const NestedConfigNode* getNode(const std::string& keyPath) const;
    
    NestedConfigNode& addNode(const ConfigPath& path, const std::string& value = "", ConfigDataType type = ConfigDataType::STRING,
                             ConfigSource source = ConfigSource::RUNTIME, const std::string& description = "");
    bool removeNode(const ConfigPath& path);
    bool moveNode(const ConfigPath& fromPath, const ConfigPath& toPath);
    
    // Configuration value access
    std::string getValue(const ConfigPath& path) const;
    int getInt(const ConfigPath& path) const;
    double getDouble(const ConfigPath& path) const;
    bool getBool(const ConfigPath& path) const;
    std::vector<std::string> getStringList(const ConfigPath& path) const;
    std::vector<double> getDoubleList(const ConfigPath& path) const;
    std::vector<int> getIntList(const ConfigPath& path) const;
    
    std::optional<std::string> getOptionalValue(const ConfigPath& path) const;
    
    // Configuration value setting
    void setValue(const ConfigPath& path, const std::string& value, ConfigSource source = ConfigSource::RUNTIME);
    void setValue(const ConfigPath& path, int value, ConfigSource source = ConfigSource::RUNTIME);
    void setValue(const ConfigPath& path, double value, ConfigSource source = ConfigSource::RUNTIME);
    void setValue(const ConfigPath& path, bool value, ConfigSource source = ConfigSource::RUNTIME);
    
    // Configuration tree operations
    bool hasNode(const ConfigPath& path) const;
    std::vector<ConfigPath> getAllNodes() const;
    std::vector<ConfigPath> getChildPaths(const ConfigPath& parentPath) const;
    
    // Configuration tree manipulation
    void clear();
    void merge(const NestedConfiguration& other);
    void exportToFile(const std::string& filepath) const;
    bool importFromFile(const std::string& filepath);
    
    // Configuration validation
    bool validate() const;
    std::vector<ConfigError> getValidationErrors() const;
    
    // Configuration search
    std::vector<ConfigPath> findNodes(const std::string& keyPattern) const;
    std::vector<ConfigPath> findNodesByValue(const std::string& valuePattern) const;
    
    // Configuration statistics
    size_t getNodeCount() const;
    size_t getKeyCount() const;
    double getLastModifiedTime() const { return last_modified_time_; }
    
    // Path operations
    std::string normalizePath(const std::string& path) const;
    ConfigPath resolvePath(const ConfigPath& basePath, const std::string& relativePath) const;
    
    // Configuration monitoring
    using ConfigChangeCallback = std::function<void(const ConfigPath& path, const std::string& oldValue, const std::string& newValue, ConfigSource source)>;
    void subscribeToChanges(const ConfigChangeCallback& callback);
    void unsubscribeFromChanges(const ConfigChangeCallback& callback);
    
    // Thread safety
    void lock() const;
    void unlock() const;
    bool tryLock() const;
    
    // Debug and diagnostics
    std::string dump() const;
    std::string treeView() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    mutable std::mutex mutex_;
    double last_modified_time_ = 0.0;
    
    void notifyChange(const ConfigPath& path, const std::string& oldValue, const std::string& newValue, ConfigSource source);
    NestedConfigNode* findNode(const ConfigPath& path);
    const NestedConfigNode* findNode(const ConfigPath& path) const;
    ConfigPath pathFromComponents(const std::vector<ConfigPathComponent>& components) const;
    
    friend class ConfigurationManager;
};

} // namespace core
} // namespace nlm
