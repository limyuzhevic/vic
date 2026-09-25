#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <mutex>
#include <stdexcept>
#include <functional>
#include <map>
#include <variant>
#include <any>
#include <chrono>
#include <algorithm>
#include <sstream>
#include <iomanip>

// Forward declarations
class Config;

namespace nlm {

// Configuration error types for better error reporting
class ConfigError : public std::runtime_error {
public:
    explicit ConfigError(const std::string& message) : std::runtime_error(message) {}
};

class ConfigValidationError : public ConfigError {
public:
    explicit ConfigValidationError(const std::string& message) : ConfigError(message) {}
};

class ConfigFileError : public ConfigError {
public:
    explicit ConfigFileError(const std::string& message) : ConfigError(message) {}
};

class ConfigParseError : public ConfigFileError {
public:
    explicit ConfigParseError(const std::string& message) : ConfigFileError(message) {}
};

// Configuration value types with enhanced support
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
    std::map<std::string, std::any>  // For nested configurations
>;

// Configuration source tracking
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    Override
};

// Configuration entry with enhanced metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::chrono::system_clock::time_point timestamp;
    bool required;
    std::string validator; // Name of validation rule
    std::map<std::string, std::string> metadata;
    
    ConfigEntry()
        : key(), value(), source(ConfigSource::Default), description(), timestamp(), 
          required(false), validator() {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", bool req = false)
        : key(k), value(v), source(s), description(desc), timestamp(std::chrono::system_clock::now()),
          required(req), validator() {}
    
    // Helper to get timestamp as string
    std::string getTimestampString() const {
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};

// Nested configuration support
class NestedConfig {
private:
    std::map<std::string, ConfigValue> values;
    std::string prefix;
    
public:
    explicit NestedConfig(const std::string& prefix = "") : prefix(prefix) {}
    
    void set(const std::string& key, const ConfigValue& value) {
        values[prefix + key] = value;
    }
    
    std::optional<ConfigValue> get(const std::string& key) const {
        auto it = values.find(prefix + key);
        if (it != values.end()) {
            return it->second;
        }
        return std::nullopt;
    }
    
    bool has(const std::string& key) const {
        return values.find(prefix + key) != values.end();
    }
    
    void remove(const std::string& key) {
        values.erase(prefix + key);
    }
    
    std::vector<std::string> getKeys() const {
        std::vector<std::string> keys;
        for (const auto& pair : values) {
            if (pair.first.size() > prefix.size()) {
                keys.push_back(pair.first.substr(prefix.size()));
            }
        }
        return keys;
    }
    
    void clear() {
        values.clear();
    }
    
    size_t size() const {
        return values.size();
    }
    
    // For debugging and serialization
    std::string summary() const {
        std::stringstream ss;
        ss << "NestedConfig(prefix=\"" << prefix << "\") (" << values.size() << " entries):\n";
        for (const auto& pair : values) {
            ss << "  " << pair.first << " = [";
            ss << std::visit([](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    ss << "\"" << val << "\"";
                } else {
                    ss << val;
                }
            }, pair.second);
            ss << "]\n";
        }
        return ss.str();
    }
};

// Configuration validation rules
class ConfigValidator {
public:
    using ValidationFunc = std::function<bool(const ConfigValue&)>;
    using ErrorFunc = std::function<std::string(const ConfigValue&)>;
    
    struct ValidationRule {
        std::string name;
        ValidationFunc validator;
        ErrorFunc errorHandler;
        bool active = true;
    };
    
    // Common validation rules
    static bool isPositive(const ConfigValue& value) {
        return std::visit([](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
                return val > 0;
            } else if constexpr (std::is_same_v<T, std::string>) {
                return !val.empty();
            } else if constexpr (std::is_same_v<T, bool>) {
                return true; // Boolean values are always valid
            } else if constexpr (std::is_same_v<T, std::vector<int>> || 
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>> ||
                                 std::is_same_v<T, std::vector<bool>>) {
                return !val.empty();
            }
            return true;
        }, value);
    }
    
    static std::string getPositiveError(const ConfigValue& value) {
        return "Value must be positive";
    }
    
    static bool isNonNegative(const ConfigValue& value) {
        return std::visit([](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
                return val >= 0;
            }
            return true;
        }, value);
    }
    
    static std::string getNonNegativeError(const ConfigValue& value) {
        return "Value must be non-negative";
    }
    
    static bool isInRange(const ConfigValue& value, double min, double max) {
        return std::visit([&](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
                return val >= min && val <= max;
            }
            return true;
        }, value);
    }
    
    static std::string getRangeError(const ConfigValue& value, double min, double max) {
        std::stringstream ss;
        ss << "Value must be between " << min << " and " << max;
        return ss.str();
    }
    
    static bool isStringLength(const ConfigValue& value, size_t minLen, size_t maxLen) {
        return std::visit([&](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                return val.size() >= minLen && val.size() <= maxLen;
            }
            return true;
        }, value);
    }
    
    static std::string getStringLengthError(const ConfigValue& value, size_t minLen, size_t maxLen) {
        std::stringstream ss;
        ss << "String length must be between " << minLen << " and " << maxLen;
        return ss.str();
    }
    
    static bool matchesPattern(const ConfigValue& value, const std::regex& pattern) {
        return std::visit([&](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                return std::regex_match(val, pattern);
            }
            return true;
        }, value);
    }
    
    static std::string getPatternError(const ConfigValue& value, const std::string& pattern) {
        std::stringstream ss;
        ss << "Value must match pattern: " << pattern;
        return ss.str();
    }
    
    static ValidationFunc createRangeValidator(double min, double max) {
        return [min, max](const ConfigValue& value) {
            return isInRange(value, min, max);
        };
    }
    
    static ErrorFunc createRangeErrorHandler(double min, double max) {
        return [min, max](const ConfigValue& value) {
            return getRangeError(value, min, max);
        };
    }
    
    static ValidationFunc createStringLengthValidator(size_t minLen, size_t maxLen) {
        return [minLen, maxLen](const ConfigValue& value) {
            return isStringLength(value, minLen, maxLen);
        };
    }
    
    static ErrorFunc createStringLengthErrorHandler(size_t minLen, size_t maxLen) {
        return [minLen, maxLen](const ConfigValue& value) {
            return getStringLengthError(value, minLen, maxLen);
        };
    }
    
    static ValidationFunc createPatternValidator(const std::string& pattern) {
        std::regex regex(pattern);
        return [regex](const ConfigValue& value) {
            return matchesPattern(value, regex);
        };
    }
    
    static ErrorFunc createPatternErrorHandler(const std::string& pattern) {
        return [pattern](const ConfigValue& value) {
            return getPatternError(value, pattern);
        };
    }
    
    class ConfigValidatorBuilder {
    private:
        std::vector<ValidationRule> rules;
        
    public:
        ConfigValidatorBuilder& addRule(const std::string& name, 
                                       const ValidationFunc& validator,
                                       const ErrorFunc& errorHandler,
                                       bool active = true) {
            rules.push_back({name, validator, errorHandler, active});
            return *this;
        }
        
        ConfigValidatorBuilder& addPositiveRule() {
            return addRule("positive", isPositive, getPositiveError);
        }
        
        ConfigValidatorBuilder& addNonNegativeRule() {
            return addRule("non_negative", isNonNegative, getNonNegativeError);
        }
        
        ConfigValidatorBuilder& addRangeRule(double min, double max) {
            return addRule("range", createRangeValidator(min, max), 
                          createRangeErrorHandler(min, max));
        }
        
        ConfigValidatorBuilder& addStringLengthRule(size_t minLen, size_t maxLen) {
            return addRule("string_length", createStringLengthValidator(minLen, maxLen),
                          createStringLengthErrorHandler(minLen, maxLen));
        }
        
        std::vector<ValidationRule> build() const {
            return rules;
        }
    };
};

// Enhanced configuration class with modern C++20 features
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept = default;
    Config& operator=(Config&&) noexcept = default;
    
    // Type-safe getters with better error messages
    template<typename T>
    T get(const std::string& key) const;
    
    template<typename T>
    std::optional<T> tryGet(const std::string& key) const noexcept;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    template<typename T>
    std::optional<T> getWithValidation(const std::string& key, 
                                     const ConfigValidator::ValidationFunc& validator) const;
    
    // Set values with enhanced functionality
    void set(const std::string& key, const ConfigValue& value, 
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "",
             bool required = false,
             const std::string& validator = "");
    
    void set(const std::string& key, const std::string& value,
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "",
             bool required = false,
             const std::string& validator = "");
    
    void set(const std::string& key, int value,
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "",
             bool required = false,
             const std::string& validator = "");
    
    void set(const std::string& key, double value,
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "",
             bool required = false,
             const std::string& validator = "");
    
    void set(const std::string& key, bool value,
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "",
             bool required = false,
             const std::string& validator = "");
    
    // Specialized setters for different types
    void setNested(const std::string& prefix, const std::string& key, const ConfigValue& value);
    void setArray(const std::string& key, const std::vector<int>& values);
    void setArray(const std::string& key, const std::vector<double>& values);
    void setArray(const std::string& key, const std::vector<std::string>& values);
    void setArray(const std::string& key, const std::vector<bool>& values);
    
    // Get specialized values
    std::optional<std::vector<int>> getIntArray(const std::string& key) const;
    std::optional<std::vector<double>> getDoubleArray(const std::string& key) const;
    std::optional<std::vector<std::string>> getStringArray(const std::string& key) const;
    std::optional<std::vector<bool>> getBoolArray(const std::string& key) const;
    std::optional<NestedConfig> getNested(const std::string& prefix) const;
    
    // Configuration management
    bool has(const std::string& key) const;
    bool hasNested(const std::string& prefix) const;
    void remove(const std::string& key);
    void removeNested(const std::string& prefix);
    std::vector<std::string> getKeys() const;
    std::vector<std::string> getKeys(const std::string& prefix) const;
    void clear();
    
    // File operations with enhanced error handling
    bool loadFromFile(const std::string& filepath, 
                     bool validate = true,
                     const std::vector<std::string>& allowedExtensions = {"cfg", "json", "yaml", "yml"});
    
    bool saveToFile(const std::string& filepath,
                    bool prettyPrint = false,
                    const std::string& format = "cfg") const;
    
    bool loadFromArgs(int argc, char** argv);
    
    // Validation and error handling
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    void addValidator(const std::string& key, const ConfigValidator::ValidationFunc& validator,
                     const ConfigValidator::ErrorFunc& errorHandler,
                     const std::string& name = "");
    
    // Configuration utilities
    std::string summary() const;
    std::string toJSON(bool prettyPrint = false) const;
    std::string toYAML() const;
    double getMemoryUsage() const; // Returns estimated memory usage in MB
    
    // Thread safety
    void lock() const noexcept;
    void unlock() const noexcept;
    bool tryLock() const noexcept;
    
    // Get configuration statistics
    struct ConfigStats {
        size_t totalEntries;
        size_t fileEntries;
        size_t commandLineEntries;
        size_t runtimeEntries;
        size_t defaultEntries;
        double memoryUsageMB;
        std::chrono::system_clock::time_point lastModified;
    };
    
    ConfigStats getStats() const;
    
    // Comparison operators for testing
    bool operator==(const Config& other) const;
    bool operator!=(const Config& other) const {
        return !(*this == other);
    }
    
    // Configuration merge operations
    void merge(const Config& other, ConfigSource source = ConfigSource::Override);
    void mergeNested(const Config& other, const std::string& prefix, 
                    ConfigSource source = ConfigSource::Override);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toUpper(const std::string& str);
    static std::string escapeJSON(const std::string& str);
    static std::string escapeYAML(const std::string& str);
    
    // JSON/YAML parsing helpers
    static bool parseJSON(const std::string& content, Config& config);
    static bool parseYAML(const std::string& content, Config& config);
    static bool parseCFG(const std::string& content, Config& config);
    
    // Validation helpers
    static bool validateEntry(const ConfigEntry& entry) const;
    static std::vector<std::string> getEntryValidationErrors(const ConfigEntry& entry) const;
    
    // File format detection
    static std::string detectFileFormat(const std::string& filepath);
    static bool isJSONFile(const std::string& content);
    static bool isYAMLFile(const std::string& content);
    static bool isCFGFile(const std::string& content);
    
    friend class ConfigFactory;
}; // namespace nlm

// Configuration factory for creating default configurations
class ConfigFactory {
public:
    static Config createDefaultConfig();
    static Config createMinimalConfig();
    static Config createFromTemplate(const std::string& templateName);
    
    // Template configurations
    static Config createBrainSimulationConfig();
    static Config createSensoryProcessingConfig();
    static Config createMotorControlConfig();
    static Config createDevelopmentConfig();
    static Config createNeuromodulationConfig();
    static Config createMemoryConfig();
    
private:
    static Config createBaseConfig();
    static Config addCoreSettings(Config& config);
    static Config addBrainSettings(Config& config);
    static Config addSensorySettings(Config& config);
    static Config addMotorSettings(Config& config);
    static Config addDevelopmentSettings(Config& config);
    static Config addNeuromodulationSettings(Config& config);
    static Config addMemorySettings(Config& config);
    static Config addExperimentSettings(Config& config);
};

// Utility functions for configuration operations
namespace config_utils {
    // Configuration merging utilities
    void mergeConfigs(Config& target, const Config& source, ConfigSource overrideSource = ConfigSource::Override);
    bool configsEqual(const Config& config1, const Config& config2, bool ignoreSource = true);
    
    // Configuration serialization utilities
    std::string configToString(const Config& config, const std::string& format = "summary");
    bool saveConfigToFile(const Config& config, const std::string& filepath, 
                        const std::string& format = "auto");
    
    // Configuration validation utilities
    bool validateConfig(const Config& config);
    std::vector<std::string> getConfigValidationErrors(const Config& config);
    
    // Configuration comparison utilities
    bool hasSameKeys(const Config& config1, const Config& config2);
    double calculateConfigSimilarity(const Config& config1, const Config& config2);
    
    // Configuration manipulation utilities
    void removeKeysWithPrefix(Config& config, const std::string& prefix);
    void renameKey(Config& config, const std::string& oldKey, const std::string& newKey);
    void replaceValue(Config& config, const std::string& key, const ConfigValue& newValue);
    
    // Configuration statistics and analysis
    Config::ConfigStats calculateConfigStats(const Config& config);
    std::string analyzeConfigComplexity(const Config& config);
    std::map<std::string, size_t> getConfigTypeDistribution(const Config& config);
    
    // Configuration filtering utilities
    Config filterBySource(const Config& config, ConfigSource source);
    Config filterByPrefix(const Config& config, const std::string& prefix);
    Config filterByType(const Config& config, const std::type_info& typeInfo);
    
    // Configuration transformation utilities
    Config transformValues(const Config& config, 
                          std::function<ConfigValue(ConfigValue)> transformer);
    Config extractSubset(const Config& config, const std::vector<std::string>& keys);
    Config expandNestedConfig(const Config& config, const std::string& prefix = "");
} // namespace config_utils

// Explicit template instantiations for common types
template std::optional<int> Config::tryGet<int>(const std::string&) const noexcept;
template std::optional<double> Config::tryGet<double>(const std::string&) const noexcept;
template std::optional<std::string> Config::tryGet<std::string>(const std::string&) const noexcept;
template std::optional<bool> Config::tryGet<bool>(const std::string&) const noexcept;

// Template implementations
inline std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

inline std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

inline std::string Config::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

// Factory function declaration
extern "C" Config createDefaultConfig();