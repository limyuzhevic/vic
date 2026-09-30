#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>
#include <typeindex>
#include <unordered_map>
#include <set>

namespace nlm {

// Forward declarations
class Config;

// Enhanced configuration value types
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>,
    std::vector<bool>,
    std::map<std::string, std::variant<int, double, bool, std::string>>  // JSON-like object
>;

// Enhanced configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    Environment,
    Database,
    Network
};

// Enhanced configuration validation
enum class ValidationLevel {
    Strict,
    Lenient,
    Minimal
};

// Configuration validation rule
struct ValidationRule {
    std::string keyPattern;
    std::function<bool(const ConfigValue&)> validator;
    std::string errorMessage;
    bool optional = false;
    std::vector<std::string> allowedValues;
    std::pair<double, double> numericRange;
};

// Configuration preset
struct ConfigPreset {
    std::string name;
    std::string description;
    std::map<std::string, ConfigValue> settings;
    bool isSystem = false;  // System presets shouldn't be saved
};

// Main enhanced configuration class
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // ===== ENHANCED LOADING/SAVING =====
    
    // Load from file (JSON/YAML/TOML formats)
    bool loadFromFile(const std::string& filepath);
    bool loadFromJSON(const std::string& jsonString);
    bool loadFromYAML(const std::string& yamlString);
    bool loadFromTOML(const std::string& tomlString);
    
    // Load from various sources
    bool loadFromEnvironment(const std::string& prefix = "NLM_");
    bool loadFromDatabase(const std::string& connectionString, const std::string& query);
    bool loadFromNetwork(const std::string& url, const std::string& authToken = "");
    
    // Load from command line arguments with enhanced parsing
    bool loadFromArgs(int argc, char** argv, const std::string& prefix = "");
    
    // Load from existing config
    bool merge(const Config& other);
    
    // Save to file
    bool saveToFile(const std::string& filepath, bool pretty = true) const;
    bool saveToJSON(const std::string& filepath, bool pretty = true) const;
    std::string toJSON(bool pretty = true) const;
    
    // ===== ENHANCED GET/SET WITH VALIDATION =====
    
    // Get values with validation
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "");
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "");
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "");
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "");
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "");
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "");
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "");
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "");
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "");
    void set(const std::string& key, const std::vector<bool>& value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "");
    void set(const std::string& key, const std::map<std::string, ConfigValue>& value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "");
    
    // ===== ENHANCED CONFIGURATION MANAGEMENT =====
    
    // Configuration presets
    bool addPreset(const ConfigPreset& preset);
    bool removePreset(const std::string& name);
    bool applyPreset(const std::string& name, ConfigSource source = ConfigSource::Runtime);
    std::vector<std::string> getPresetNames() const;
    ConfigPreset getPreset(const std::string& name) const;
    
    // Configuration validation
    void addValidationRule(const ValidationRule& rule);
    void removeValidationRule(const std::string& keyPattern);
    bool validate(ValidationLevel level = ValidationLevel::Strict) const;
    std::vector<std::string> getValidationErrors(ValidationLevel level = ValidationLevel::Strict) const;
    
    // Configuration inheritance
    void addInheritanceSource(const std::string& sourceName, const Config& config);
    void clearInheritanceSources();
    
    // Configuration comparison
    bool equals(const Config& other) const;
    
    // ===== ADVANCED QUERYING AND FILTERING =====
    
    // Query configuration
    std::vector<std::string> queryKeys(const std::string& pattern = "*") const;
    std::vector<ConfigEntry> queryEntries(const std::string& pattern = "*") const;
    std::map<std::string, ConfigValue> query(const std::string& pattern = "*") const;
    
    // Filter configuration
    Config filter(const std::function<bool(const ConfigEntry&)>& predicate) const;
    Config filterBySource(ConfigSource source) const;
    Config filterByKeyPrefix(const std::string& prefix) const;
    
    // ===== ADVANCED CONFIGURATION OPERATIONS =====
    
    // Configuration statistics
    size_t getEntryCount() const;
    std::map<ConfigSource, size_t> getSourceCounts() const;
    std::set<std::string> getKeySet() const;
    std::vector<std::pair<std::string, ConfigSource>> getSortedEntries() const;
    
    // Configuration lifecycle
    void clear();
    void resetToDefaults();
    void snapshot(const std::string& name);
    void restoreSnapshot(const std::string& name);
    std::vector<std::string> getSnapshotNames() const;
    
    // ===== ADVANCED SERIALIZATION =====
    
    // Serialization/deserialization
    std::string toString() const;
    bool fromString(const std::string& str);
    
    // Diff operations
    std::vector<ConfigEntry> diff(const Config& other) const;
    bool hasChanges(const Config& other) const;
    
    // ===== ADVANCED UTILITIES =====
    
    // Configuration completion (auto-fill missing values)
    void complete(const Config& templateConfig);
    
    // Configuration optimization
    void optimizeForPerformance();
    void optimizeForMemory();
    void optimizeForNetwork();
    
    // Configuration monitoring
    void enableMonitoring(bool enable);
    bool isMonitoringEnabled() const;
    std::map<std::string, double> getPerformanceMetrics() const;
    
    // ===== EXISTING API (MAINTAINED FOR COMPATIBILITY) =====
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get configuration summary
    std::string summary() const;
    
    // ===== NEW ADVANCED API =====
    
    // Performance optimization
    void enableCaching(bool enable);
    bool isCachingEnabled() const;
    
    // Validation presets
    static ValidationRule createRangeValidator(const std::string& keyPattern, 
                                              double min, double max,
                                              const std::string& errorMsg = "");
    
    static ValidationRule createEnumValidator(const std::string& keyPattern,
                                             const std::vector<std::string>& allowedValues,
                                             const std::string& errorMsg = "");
    
    static ValidationRule createTypeValidator(const std::string& keyPattern,
                                             const std::type_index& type,
                                             const std::string& errorMsg = "");
    
    // Preset definitions
    static ConfigPreset createDevelopmentPreset();
    static ConfigPreset createProductionPreset();
    static ConfigPreset createTestingPreset();
    static ConfigPreset createBenchmarkPreset();
    
    // Constants
    static constexpr const char* DEFAULT_CONFIG_FILE = "configs/default.cfg";
    static constexpr const char* DEVELOPMENT_CONFIG_FILE = "configs/development.cfg";
    static constexpr const char* PRODUCTION_CONFIG_FILE = "configs/production.cfg";
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool matchesPattern(const std::string& key, const std::string& pattern);
    static std::string toJSON(const ConfigValue& value, bool pretty = true);
    static std::string toString(const ConfigValue& value);
    
    // Validation helpers
    bool validateValue(const ConfigEntry& entry, ValidationLevel level) const;
    std::vector<std::string> validateAll(ValidationLevel level) const;
    
    // Performance helpers
    void updatePerformanceMetrics() const;
    
    // Serialization helpers
    bool parseJSON(const std::string& jsonString);
    bool parseYAML(const std::string& yamlString);
    bool parseTOML(const std::string& tomlString);
    
    // Inheritance helpers
    Config mergeInheritance() const;
};

// Explicit template instantiations
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;
template std::optional<std::vector<int>> Config::get<std::vector<int>>(const std::string&) const;
template std::optional<std::vector<double>> Config::get<std::vector<double>>(const std::string&) const;
template std::optional<std::vector<std::string>> Config::get<std::vector<std::string>>(const std::string&) const;
template std::optional<std::vector<bool>> Config::get<std::vector<bool>>(const std::string&) const;
template std::optional<std::map<std::string, ConfigValue>> Config::get<std::map<std::string, ConfigValue>>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;
template std::vector<int> Config::getOr<std::vector<int>>(const std::string&, const std::vector<int>&) const;
template std::vector<double> Config::getOr<std::vector<double>>(const std::string&, const std::vector<double>&) const;
template std::vector<std::string> Config::getOr<std::vector<std::string>>(const std::string&, const std::vector<std::string>&) const;
template std::vector<bool> Config::getOr<std::vector<bool>>(const std::string&, const std::vector<bool>&) const;
template std::map<std::string, ConfigValue> Config::getOr<std::map<std::string, ConfigValue>>(const std::string&, const std::map<std::string, ConfigValue>&) const;

} // namespace nlm