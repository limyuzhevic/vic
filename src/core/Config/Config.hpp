#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <set>
#include <stdexcept>
#include <sstream>
#include <functional>

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

// Configuration validation error
class ConfigError : public std::runtime_error {
public:
    explicit ConfigError(const std::string& message) : std::runtime_error(message) {}
};

// Configuration key validation
class ConfigKeyValidator {
public:
    static void validateKey(const std::string& key);
    static bool isValidKey(const std::string& key);
    
    static void registerKey(const std::string& key, ConfigSource source, const std::string& description = "");
    static std::string getKeyDescription(const std::string& key);
    static ConfigSource getKeySource(const std::string& key);
    static std::set<std::string> getAllRegisteredKeys();
    
    static void validateValue(const std::string& key, const ConfigValue& value);
    static bool isValidValue(const std::string& key, const ConfigValue& value);
    
    static void setValidationCallback(std::function<bool(const std::string&, const ConfigValue&)> callback);
private:
    static std::map<std::string, std::pair<ConfigSource, std::string>> registeredKeys;
    static std::function<bool(const std::string&, const ConfigValue&)> validationCallback;
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    uint64_t timestamp;
    
    ConfigEntry()
        : key(), value(), source(ConfigSource::Default), description(), timestamp(0) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "", uint64_t ts = 0)
        : key(k), value(v), source(s), description(desc), timestamp(ts) {}
};

// Type conversion utilities
class ConfigValueConverter {
public:
    static bool canConvert(const ConfigValue& from, const std::type_info& to) const;
    static std::optional<ConfigValue> convert(const ConfigValue& value, const std::type_info& targetType) const;
    
    static std::string valueToString(const ConfigValue& value);
    static std::optional<ConfigValue> stringToValue(const std::string& str, const std::type_info& targetType);
    
    static std::vector<std::string> getSupportedTypes() const;
private:
    static const std::map<std::type_info, std::function<std::string(const ConfigValue&)>> formatters;
};

// Configuration statistics
struct ConfigStats {
    size_t totalEntries;
    size_t fileEntries;
    size_t commandLineEntries;
    size_t runtimeEntries;
    std::set<std::string> uniqueKeys;
    std::map<std::string, size_t> keyCounts;
    uint64_t oldestEntryAge;
    uint64_t newestEntryAge;
    
    ConfigStats() : totalEntries(0), fileEntries(0), commandLineEntries(0), 
                   runtimeEntries(0), oldestEntryAge(0), newestEntryAge(0) {}
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
    
    // Load from file (JSON/YAML format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from string
    bool loadFromString(const std::string& content, ConfigSource source = ConfigSource::File);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load from JSON string
    bool loadFromJSON(const std::string& json);
    
    // Load from YAML string (if available)
    bool loadFromYAML(const std::string& yaml);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    std::string saveToJSON() const;
    std::string saveToYAML() const;
    
    // Get values with type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Advanced set methods with validation
    void setValidated(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const ConfigValue& value, ConfigSource source, bool validate);
    
    // Check existence and type
    bool has(const std::string& key) const;
    bool hasType(const std::string& key, const std::type_info& type) const;
    
    // Type conversion
    template<typename T>
    std::optional<T> getAs(const std::string& key) const;
    template<typename T>
    T getAsOr(const std::string& key, const T& defaultValue) const;
    
    // Remove and validation
    void remove(const std::string& key);
    bool removeValidated(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    std::vector<std::string> getKeysBySource(ConfigSource source) const;
    std::vector<std::string> getKeysByType(const std::type_info& type) const;
    
    // Clear all
    void clear();
    void clearBySource(ConfigSource source);
    
    // Configuration statistics
    ConfigStats getStats() const;
    size_t getEntryCount() const;
    size_t getEntryCount(ConfigSource source) const;
    
    // Get configuration summary
    std::string summary() const;
    std::string summary(ConfigSource source) const;
    std::string toJSON() const;
    std::string toYAML() const;
    
    // Validation and error handling
    std::vector<std::string> validate() const;
    bool isValid() const;
    std::string getValidationErrors() const;
    
    // Configuration merging
    void merge(const Config& other, ConfigSource source = ConfigSource::File);
    void override(const Config& other, ConfigSource source = ConfigSource::CommandLine);
    
    // Configuration history
    size_t getHistorySize() const;
    void clearHistory();
    std::vector<ConfigEntry> getHistory() const;
    
    // Configuration snapshots
    std::string createSnapshot(const std::string& name = "");
    bool restoreSnapshot(const std::string& name);
    std::vector<std::string> getSnapshotNames() const;
    
    // Configuration comparison
    bool equals(const Config& other) const;
    bool differs(const Config& other) const;
    double similarity(const Config& other) const;
    
    // Configuration search
    std::vector<std::string> findKeys(const std::string& pattern) const;
    std::vector<std::string> findKeysByValue(const ConfigValue& value) const;
    
    // Configuration templates
    void applyTemplate(const std::string& templateName);
    bool hasTemplate(const std::string& templateName) const;
    std::vector<std::string> getAvailableTemplates() const;
    
    // Configuration inheritance
    void setParent(const std::shared_ptr<Config>& parent);
    std::shared_ptr<Config> getParent() const;
    void inheritFromParent();
    
    // Configuration monitoring
    void setChangeCallback(std::function<void(const std::string&, const ConfigValue&)> callback);
    void setValidationErrorCallback(std::function<void(const std::string&, const std::string&)> callback);
    
    // Configuration export/import
    std::string exportConfig() const;
    bool importConfig(const std::string& data);
    
    // Utility methods
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toUpper(const std::string& str);
    static bool stringToBool(const std::string& str);
    static double stringToDouble(const std::string& str);
    static int64_t stringToInt64(const std::string& str);
    static std::string valueToString(const ConfigValue& value);
    
    // Configuration convenience methods
    void setDefaults();
    void resetToDefaults();
    bool hasDefaults() const;
    
    // Configuration debugging
    void enableDebug(bool enable = true);
    bool isDebugEnabled() const;
    std::string debugInfo() const;
    
    // Configuration locking
    void lock();
    void unlock();
    bool isLocked() const;
    
    // Configuration backup
    void createBackup(const std::string& name = "");
    bool restoreBackup(const std::string& name);
    std::vector<std::string> getBackupNames() const;
    
    // Configuration compression
    std::string compress() const;
    bool decompress(const std::string& data);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    void validateKeyValue(const std::string& key, const ConfigValue& value) const;
    void validateKey(const std::string& key) const;
    bool isValidKeyInternal(const std::string& key) const;
    
    void addEntry(const ConfigEntry& entry);
    ConfigEntry findEntry(const std::string& key) const;
    size_t getEntryCount(const std::string& key) const;
    
    void triggerChangeCallback(const std::string& key, const ConfigValue& value) const;
    void triggerValidationErrorCallback(const std::string& key, const std::string& error) const;
    
    void updateStats() const;
    void resetStats() const;
    
    // Friendship declarations
    friend class ConfigKeyValidator;
    friend class ConfigValueConverter;
};

// Global configuration functions
std::shared_ptr<Config> createDefaultConfig();
std::shared_ptr<Config> createMinimalConfig();
std::shared_ptr<Config> createResearchConfig();
std::shared_ptr<Config> loadConfigFromFile(const std::string& filepath);
std::shared_ptr<Config> loadConfigFromArgs(int argc, char** argv);

// Configuration comparison functions
bool configsEqual(const Config& a, const Config& b);
double configSimilarity(const Config& a, const Config& b);

// Configuration statistics functions
ConfigStats getConfigStats(const Config& config);
size_t getConfigSize(const Config& config);
size_t getConfigEntryCount(const Config& config, ConfigSource source);

// Configuration validation functions
std::vector<std::string> validateConfig(const Config& config);
bool isConfigValid(const Config& config);

// Configuration conversion functions
std::string configToJSON(const Config& config);
std::string configToYAML(const Config& config);
bool jsonToConfig(const std::string& json, std::shared_ptr<Config>& config);
bool yamlToConfig(const std::string& yaml, std::shared_ptr<Config>& config);

// Configuration template functions
bool applyConfigTemplate(std::shared_ptr<Config>& config, const std::string& templateName);
std::vector<std::string> getAvailableConfigTemplates();

} // namespace nlm