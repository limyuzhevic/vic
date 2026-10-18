#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <regex>
#include <map>
#include <chrono>
#include <filesystem>
#include <algorithm>

namespace nlm {

// Forward declarations
class Config;

// Configuration value types
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>
>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime
};

// Configuration validation rule
enum class ConfigValueType {
    Integer,
    Integer64,
    Double,
    Boolean,
    String,
    StringList,
    DoubleList,
    IntList,
    Object,
    Array,
    Any
};

// Configuration path with dot notation support
class ConfigPath {
public:
    ConfigPath() = default;
    explicit ConfigPath(const std::string& path);
    
    // Get path components
    const std::vector<std::string>& getComponents() const { return components; }
    const std::string& getOriginal() const { return original; }
    
    // Check if path matches a wildcard pattern
    bool matches(const std::string& pattern) const;
    
    // Helper methods
    std::string getParentPath() const;
    std::string getKey() const;
    bool isRoot() const { return components.empty(); }
    
private:
    std::string original;
    std::vector<std::string> components;
};

// Configuration validation rule
struct ValidationRule {
    ConfigValueType type;
    std::optional<std::string> min;
    std::optional<std::string> max;
    std::optional<std::vector<std::string>> enumValues;
    bool required;
    
    ValidationRule() : type(ConfigValueType::Any), required(false) {}
    ValidationRule(ConfigValueType t, bool req = false) : type(t), required(req) {}
};

// Configuration schema
class ConfigSchema {
public:
    ConfigSchema() = default;
    ~ConfigSchema() = default;
    
    // Add validation rule for a key
    void addRule(const std::string& key, const ValidationRule& rule);
    
    // Get validation rule for a key
    const ValidationRule* getRule(const std::string& key) const;
    
    // Check if key matches a rule pattern
    bool hasRule(const std::string& key) const;
    
    // Validate configuration value
    bool validate(const std::string& key, const ConfigValue& value) const;
    
    // Get all keys with rules
    std::vector<std::string> getKeys() const;
    
    // Clear schema
    void clear() { rules.clear(); }
    
    // Load from JSON
    bool loadFromJSON(const std::string& jsonStr);
    
    // Export to JSON
    std::string exportToJSON() const;

private:
    std::unordered_map<std::string, ValidationRule> rules;
};

// Configuration group
class ConfigGroup {
public:
    ConfigGroup() = default;
    ConfigGroup(const std::string& name, const std::string& description = "");
    
    // Get group name
    const std::string& getName() const { return name; }
    
    // Get group description
    const std::string& getDescription() const { return description; }
    
    // Get group configs
    const std::vector<std::string>& getConfigs() const { return configs; }
    
    // Add configuration to group
    void addConfig(const std::string& key);
    
    // Remove configuration from group
    void removeConfig(const std::string& key);
    
    // Check if configuration belongs to group
    bool hasConfig(const std::string& key) const;
    
    // Get parent group
    const std::shared_ptr<ConfigGroup>& getParent() const { return parent; }
    
    // Set parent group (for inheritance)
    void setParent(const std::shared_ptr<ConfigGroup>& parentGroup) { parent = parentGroup; }

private:
    std::string name;
    std::string description;
    std::vector<std::string> configs;
    std::shared_ptr<ConfigGroup> parent;
};

// Configuration entry (enhanced)
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::chrono::system_clock::time_point timestamp;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), timestamp(std::chrono::system_clock::now()) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), timestamp(std::chrono::system_clock::now()) {}
};

// Configuration snapshot
struct ConfigSnapshot {
    std::string id;
    std::string description;
    std::chrono::system_clock::time_point timestamp;
    std::unordered_map<std::string, ConfigValue> values;
    std::string checksum;
    
    ConfigSnapshot() : timestamp(std::chrono::system_clock::now()) {}
    ConfigSnapshot(const std::string& id, const std::string& desc) : id(id), description(desc), timestamp(std::chrono::system_clock::now()) {}
};

// Main configuration class
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Enhanced access methods
    template<typename T>
    std::optional<T> getNested(const std::string& dotNotationKey) const;
    
    template<typename T>
    T getNestedOr(const std::string& dotNotationKey, const T& defaultValue) const;
    
    void setNested(const std::string& dotNotationKey, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    bool hasNested(const std::string& dotNotationKey) const;
    void removeNested(const std::string& dotNotationKey);
    std::vector<std::string> getAllKeys() const;
    
    // Configuration groups
    bool addGroup(const std::string& groupName, const std::string& description = "");
    bool removeGroup(const std::string& groupName);
    const std::shared_ptr<ConfigGroup>& getGroup(const std::string& groupName) const;
    std::vector<std::string> getGroupNames() const;
    bool addConfigToGroup(const std::string& groupName, const std::string& key);
    bool removeConfigFromGroup(const std::string& groupName, const std::string& key);
    std::vector<std::string> getConfigsInGroup(const std::string& groupName) const;
    
    // Schema validation
    void setSchema(const ConfigSchema& schema);
    const ConfigSchema& getSchema() const { return schema; }
    bool validate() const;
    bool validate(const std::string& key) const;
    bool hasSchema() const { return schemaDefined; }
    
    // Inheritance
    bool inheritFrom(const Config& baseConfig);
    bool removeInheritance(const std::string& groupName);
    bool hasInheritance(const std::string& groupName) const;
    
    // Serialization
    std::string exportToJSON() const;
    std::string exportToYAML() const;
    bool importFromJSON(const std::string& jsonStr);
    bool importFromYAML(const std::string& yamlStr);
    std::string exportToCompressed(const std::string& format = "gzip") const;
    bool importFromCompressed(const std::string& data, const std::string& format = "gzip");
    
    // Snapshot functionality
    std::string createSnapshot(const std::string& description = "");
    bool restoreSnapshot(const std::string& snapshotId);
    bool removeSnapshot(const std::string& snapshotId);
    std::vector<std::string> getSnapshotIds() const;
    ConfigSnapshot getSnapshot(const std::string& snapshotId) const;
    
    // Delta operations
    std::string getDelta(const Config& otherConfig) const;
    bool applyDelta(const std::string& deltaJson);
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Static utility methods
    static std::string compress(const std::string& data);
    static std::string decompress(const std::string& data);
    static std::string generateChecksum(const std::string& data);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Schema and groups
    ConfigSchema schema;
    bool schemaDefined;
    std::unordered_map<std::string, std::shared_ptr<ConfigGroup>> groups;
    
    // Inheritance
    std::unordered_map<std::string, std::string> inheritanceMap;
    
    // Snapshots
    std::vector<ConfigSnapshot> snapshots;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static ConfigPath parsePath(const std::string& dotNotation);
    static std::vector<std::string> findMatchingKeys(const std::regex& pattern) const;
    static std::string validateValue(const std::string& key, const ConfigValue& value) const;
    static ConfigValue jsonToConfigValue(const nlohmann::json& j);
    static nlohmann::json configValueToJSON(const ConfigValue& value);
    
    // Backwards compatibility
    std::vector<ConfigEntry> getLegacyEntries() const;
    void applyLegacyEntries(const std::vector<ConfigEntry>& entries);
};

// Hash function for ConfigValue
struct ConfigValueHash {
    size_t operator()(const ConfigValue& v) const {
        return std::visit([](const auto& arg) -> size_t {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                return std::hash<int>()(arg);
            } else if constexpr (std::is_same_v<T, int64_t>) {
                return std::hash<int64_t>()(arg);
            } else if constexpr (std::is_same_v<T, double>) {
                return std::hash<double>()(arg);
            } else if constexpr (std::is_same_v<T, bool>) {
                return std::hash<bool>()(arg);
            } else if constexpr (std::is_same_v<T, std::string>) {
                return std::hash<std::string>()(arg);
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                size_t seed = std::hash<size_t>()(arg.size());
                for (const auto& item : arg) {
                    seed ^= std::hash<int>()(item) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
                }
                return seed;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                size_t seed = std::hash<size_t>()(arg.size());
                for (const auto& item : arg) {
                    seed ^= std::hash<double>()(item) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
                }
                return seed;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                size_t seed = std::hash<size_t>()(arg.size());
                for (const auto& item : arg) {
                    seed ^= std::hash<std::string>()(item) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
                }
                return seed;
            }
            return 0;
        }, v);
    }
};

} // namespace nlm

// Explicit template instantiations
extern template std::optional<int> nlm::Config::get<int>(const std::string&) const;
extern template std::optional<int64_t> nlm::Config::get<int64_t>(const std::string&) const;
extern template std::optional<double> nlm::Config::get<double>(const std::string&) const;
extern template std::optional<bool> nlm::Config::get<bool>(const std::string&) const;
extern template std::optional<std::string> nlm::Config::get<std::string>(const std::string&) const;

extern template int nlm::Config::getOr<int>(const std::string&, const int&) const;
extern template int64_t nlm::Config::getOr<int64_t>(const std::string&, const int64_t&) const;
extern template double nlm::Config::getOr<double>(const std::string&, const double&) const;
extern template bool nlm::Config::getOr<bool>(const std::string&, const bool&) const;
extern template std::string nlm::Config::getOr<std::string>(const std::string&, const std::string&) const;

extern template std::optional<int> nlm::Config::getNested<int>(const std::string&) const;
extern template std::optional<int64_t> nlm::Config::getNested<int64_t>(const std::string&) const;
extern template std::optional<double> nlm::Config::getNested<double>(const std::string&) const;
extern template std::optional<bool> nlm::Config::getNested<bool>(const std::string&) const;
extern template std::optional<std::string> nlm::Config::getNested<std::string>(const std::string&) const;

extern template int nlm::Config::getNestedOr<int>(const std::string&, const int&) const;
extern template int64_t nlm::Config::getNestedOr<int64_t>(const std::string&, const int64_t&) const;
extern template double nlm::Config::getNestedOr<double>(const std::string&, const double&) const;
extern template bool nlm::Config::getNestedOr<bool>(const std::string&, const bool&) const;
extern template std::string nlm::Config::getNestedOr<std::string>(const std::string&, const std::string&) const;
};

} // namespace nlm
