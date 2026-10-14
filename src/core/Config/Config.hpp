#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <any>
#include <typeinfo>
#include <sstream>
#include <iomanip>

namespace nlm {

// Forward declarations
class Config;
class ConfigValidator;

// Advanced configuration value types
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>,
    std::vector<std::vector<double>>,  // Matrices
    std::map<std::string, std::any>   // Nested objects
>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Environment,
    Runtime,
    Profile
};

// Configuration validation result
struct ValidationResult {
    bool valid;
    std::string errorMessage;
    std::vector<std::string> warnings;
    
    ValidationResult(bool v = true, const std::string& error = "", const std::vector<std::string>& warnings = {})
        : valid(v), errorMessage(error), warnings(warnings) {}
};

// Configuration entry with enhanced metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string category;
    std::string dataType;
    std::string validationRules;
    bool required;
    double minValue;
    double maxValue;
    std::vector<std::string> allowedValues;
    
    ConfigEntry() 
        : key(), value(), source(ConfigSource::Default), description(), category("general")
        , dataType("auto"), validationRules("")
        , required(false), minValue(std::numeric_limits<double>::lowest())
        , maxValue(std::numeric_limits<double>::max()) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), category("general")
        , dataType("auto"), validationRules("")
        , required(false), minValue(std::numeric_limits<double>::lowest())
        , maxValue(std::numeric_limits<double>::max()) {}
};

// Configuration profile for presets
struct ConfigProfile {
    std::string name;
    std::string description;
    std::vector<std::string> inheritsFrom;
    std::map<std::string, ConfigValue> settings;
    bool isExperimental;
    std::string version;
    
    ConfigProfile(const std::string& n = "", const std::string& desc = "")
        : name(n), description(desc), isExperimental(false), version("1.0") {}
};

// Configuration statistics
struct ConfigStats {
    size_t totalEntries;
    size_t entriesBySource[4]; // Default, File, CommandLine, Runtime
    size_t entriesByCategory;
    std::chrono::system_clock::time_point lastModified;
    size_t memoryUsage;
    
    ConfigStats() : totalEntries(0) {
        std::fill(std::begin(entriesBySource), std::end(entriesBySource), 0);
        entriesByCategory = 0;
        memoryUsage = 0;
    }
};

// Main configuration class with advanced features
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Enhanced loading from file with multiple formats
    bool loadFromFile(const std::string& filepath, const std::string& format = "auto");
    bool loadFromJSON(const std::string& filepath);
    bool loadFromYAML(const std::string& filepath);
    bool loadFromINI(const std::string& filepath);
    bool loadFromTOML(const std::string& filepath);
    
    // Load from environment variables
    void loadFromEnvironment(const std::string& prefix = "NLM_");
    
    // Load from command line arguments with advanced parsing
    bool loadFromArgs(int argc, char** argv);
    bool loadFromArgs(const std::vector<std::string>& args);
    
    // Save to file in various formats
    bool saveToFile(const std::string& filepath, const std::string& format = "json") const;
    bool saveToJSON(const std::string& filepath) const;
    bool saveToYAML(const std::string& filepath) const;
    bool saveToINI(const std::string& filepath) const;
    
    // Configuration profile management
    bool loadProfile(const std::string& profileName, bool merge = true);
    bool saveProfile(const std::string& profileName) const;
    bool deleteProfile(const std::string& profileName);
    std::vector<std::string> getAvailableProfiles() const;
    void registerProfile(const ConfigProfile& profile);
    
    // Advanced configuration operations
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Set with validation and metadata
    template<typename T>
    bool setWithValidation(const std::string& key, const T& value, 
                          const std::string& description = "",
                          const std::string& category = "general",
                          const std::string& dataType = "auto",
                          const std::string& validationRules = "",
                          bool required = false,
                          double minValue = std::numeric_limits<double>::lowest(),
                          double maxValue = std::numeric_limits<double>::max());
    
    // Enhanced get methods with type conversion and validation
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    template<typename T>
    T getOrWithDefault(const std::string& key, const T& defaultValue, ConfigSource minSource = ConfigSource::CommandLine) const;
    
    // Type-safe getters for common types
    int getInt(const std::string& key) const;
    double getDouble(const std::string& key) const;
    bool getBool(const std::string& key) const;
    std::string getString(const std::string& key) const;
    std::vector<double> getVector(const std::string& key) const;
    std::vector<std::vector<double>> getMatrix(const std::string& key) const;
    
    // Nested key access (dot notation)
    template<typename T>
    std::optional<T> getNested(const std::string& key) const;
    
    template<typename T>
    T getNestedOr(const std::string& key, const T& defaultValue) const;
    
    // Configuration validation and analysis
    ValidationResult validate() const;
    bool validateKey(const std::string& key) const;
    ValidationResult validateValue(const std::string& key, const ConfigValue& value) const;
    
    // Configuration statistics and analysis
    ConfigStats getStats() const;
    std::string getConfigurationSummary() const;
    std::vector<std::string> getKeysByCategory(const std::string& category) const;
    std::map<std::string, ConfigValue> getCategory(const std::string& category) const;
    
    // Configuration manipulation
    bool has(const std::string& key) const;
    void remove(const std::string& key);
    void removeByCategory(const std::string& category);
    void clear();
    
    // Configuration comparison and merging
    bool equals(const Config& other) const;
    void merge(const Config& other, ConfigSource source = ConfigSource::Runtime);
    void inheritFrom(const Config& parent, ConfigSource source = ConfigSource::Default);
    
    // Configuration backup and restore
    bool backup(const std::string& filepath) const;
    bool restore(const std::string& filepath);
    
    // Advanced query methods
    std::vector<std::string> getKeys() const;
    std::vector<std::string> getKeysBySource(ConfigSource source) const;
    std::vector<std::string> getKeysByType(const std::string& type) const;
    
    // Configuration transformation
    void transform(const std::function<ConfigValue(ConfigValue)>& transformFunc);
    void filter(const std::function<bool(const ConfigEntry&)>& predicate);
    void sort(const std::function<bool(const ConfigEntry&, const ConfigEntry&)>& comparator);
    
    // Export and import
    std::string exportToJSON() const;
    bool importFromJSON(const std::string& json);
    std::string exportToYAML() const;
    bool importFromYAML(const std::string& yaml);
    
    // Configuration patterns and presets
    void addDefaultProfile(const std::string& name);
    void addScientificProfile(const std::string& name);
    void addPerformanceProfile(const std::string& name);
    void addDebugProfile(const std::string& name);
    
    // Configuration debugging and diagnostics
    void enableDebugMode(bool enabled);
    bool isDebugMode() const;
    std::string getDebugInfo() const;
    void logConfiguration() const;
    
    // Configuration validation rules
    void addValidationRule(const std::string& key, const std::string& rule);
    void removeValidationRule(const std::string& key);
    std::vector<std::string> getValidationRules(const std::string& key) const;
    
    // Configuration value manipulation
    template<typename T>
    bool increment(const std::string& key, T amount = 1);
    
    template<typename T>
    bool decrement(const std::string& key, T amount = 1);
    
    // Configuration comparison
    bool isGreater(const std::string& key, const ConfigValue& value) const;
    bool isLess(const std::string& key, const ConfigValue& value) const;
    bool isEqual(const std::string& key, const ConfigValue& value) const;
    
    // Configuration macros and templates
    void addMacro(const std::string& name, const ConfigValue& value);
    ConfigValue getMacro(const std::string& name) const;
    bool hasMacro(const std::string& name) const;
    
    // Configuration change notification
    using ConfigChangeCallback = std::function<void(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue)>;
    void addChangeCallback(const ConfigChangeCallback& callback);
    void removeChangeCallback(const ConfigChangeCallback& callback);
    void notifyChange(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool stringToBool(const std::string& str);
    static double stringToDouble(const std::string& str);
    static int64_t stringToInt64(const std::string& str);
    
    // Internal parsing and serialization
    bool parseJSON(const std::string& content);
    bool parseYAML(const std::string& content);
    bool parseINI(const std::string& content);
    bool parseTOML(const std::string& content);
    
    std::string serializeToJSON() const;
    std::string serializeToYAML() const;
    std::string serializeToINI() const;
    std::string serializeToTOML() const;
    
    // Key resolution
    std::string resolveKey(const std::string& key) const;
    bool isNestedKey(const std::string& key) const;
    
    // Value validation
    bool validateValueType(const ConfigValue& value, const std::string& expectedType) const;
    bool validateValueRange(const ConfigValue& value, double minValue, double maxValue) const;
    bool validateValueAllowed(const ConfigValue& value, const std::vector<std::string>& allowedValues) const;
    
    // Statistics update
    void updateStats() const;
    
    // Profile management
    void loadBuiltInProfiles();
    std::string getProfilePath(const std::string& profileName) const;
    bool saveProfileToFile(const ConfigProfile& profile) const;
    ConfigProfile loadProfileFromFile(const std::string& profileName) const;
};

// Template implementations
template<typename T>
bool Config::setWithValidation(const std::string& key, const T& value,
                             const std::string& description,
                             const std::string& category,
                             const std::string& dataType,
                             const std::string& validationRules,
                             bool required,
                             double minValue,
                             double maxValue) {
    ValidationResult validation = validateValue(key, ConfigValue(value));
    if (!validation.valid) {
        return false;
    }
    
    set(key, value, ConfigSource::Runtime);
    
    // Update metadata for the entry
    for (auto& entry : pImpl->entries) {
        if (entry.key == key) {
            entry.description = description;
            entry.category = category;
            entry.dataType = dataType;
            entry.validationRules = validationRules;
            entry.required = required;
            entry.minValue = minValue;
            entry.maxValue = maxValue;
            break;
        }
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

template<typename T>
T Config::getOrWithDefault(const std::string& key, const T& defaultValue, ConfigSource minSource) const {
    auto val = get<T>(key);
    if (!val.has_value()) {
        return defaultValue;
    }
    
    // Check if the value comes from a source with higher priority than minSource
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n    
    if (it == pImpl->entries.end()) {
        return defaultValue;
    }
    
    int sourcePriority = static_cast<int>(it->source);
    int minPriority = static_cast<int>(minSource);
    
    if (sourcePriority >= minPriority) {
        return val.value();
    } else {
        return defaultValue;
    }
}

template<typename T>
std::optional<T> Config::getNested(const std::string& key) const {
    if (!isNestedKey(key)) {
        return get<T>(key);
    }
    
    // Parse dot notation key
    size_t dotPos = key.find('.');
    if (dotPos == std::string::npos) {
        return get<T>(key);
    }
    
    std::string parentKey = key.substr(0, dotPos);
    std::string childKey = key.substr(dotPos + 1);
    
    // For now, only support one level of nesting
    auto parentValue = get<std::map<std::string, std::any>>(parentKey);
    if (!parentValue.has_value()) {
        return std::nullopt;
    }
    
    const auto& nestedMap = parentValue.value();
    auto childIt = nestedMap.find(childKey);
    if (childIt == nestedMap.end()) {
        return std::nullopt;
    }
    
    try {
        return std::any_cast<T>(childIt->second);
    } catch (const std::bad_any_cast&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getNestedOr(const std::string& key, const T& defaultValue) const {
    auto val = getNested<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

template<typename T>
bool Config::increment(const std::string& key, T amount) {
    if (!has(key)) {
        return false;
    }
    
    auto val = get<T>(key);
    if (!val.has_value()) {
        return false;
    }
    
    T newValue = val.value() + amount;
    set(key, newValue, ConfigSource::Runtime);
    return true;
}

template<typename T>
bool Config::decrement(const std::string& key, T amount) {
    return increment(key, -amount);
}

// Explicit template instantiations
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;
template std::optional<std::vector<int>> Config::get<std::vector<int>>(const std::string&) const;
template std::optional<std::vector<double>> Config::get<std::vector<double>>(const std::string&) const;
template std::optional<std::vector<std::string>> Config::get<std::vector<std::string>>(const std::string&) const;
template std::optional<std::vector<std::vector<double>>> Config::get<std::vector<std::vector<double>>>(const std::string&) const;
template std::optional<std::map<std::string, std::any>> Config::get<std::map<std::string, std::any>>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm