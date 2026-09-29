#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <mutex>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <algorithm>
#include <stdexcept>
#include <variant>
#include <deque>
#include <map>

namespace nlm {

// Forward declarations for circular dependencies
class Config;
class Logger;

// Enhanced Configuration Value Types with better type safety
enum class ConfigDataType {
    Integer,
    Integer64,
    Double,
    Boolean,
    String,
    StringList,
    DoubleList,
    IntegerList,
    Unknown
};

// Enhanced Configuration Entry with more metadata
struct ConfigEntry {
    std::string key;
    std::variant<int, int64_t, double, bool, std::string,
                 std::vector<int>, std::vector<double>, std::vector<std::string>> value;
    
    // Enhanced metadata
    enum class ConfigSource {
        Default,
        File,
        CommandLine,
        Environment,
        Runtime,
        Override
    } source;
    
    std::string description;
    std::string validationPattern;
    std::string validationErrorMessage;
    ConfigDataType dataType;
    bool isRequired;
    bool isReadOnly;
    
    // Change tracking
    std::chrono::system_clock::time_point lastModified;
    uint64_t modificationCount;
    
    // Comments and metadata
    std::string comment;
    std::map<std::string, std::string> metadata;
    
    ConfigEntry()
        : source(ConfigSource::Default), dataType(ConfigDataType::Unknown), 
          isRequired(false), isReadOnly(false), modificationCount(0) {
        lastModified = std::chrono::system_clock::now();
    }
    
    ConfigEntry(const std::string& k, const std::variant<int, int64_t, double, bool, std::string,
                std::vector<int>, std::vector<double>, std::vector<std::string>>& v,
                ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), dataType(detectDataType(v)),
          isRequired(false), isReadOnly(false), modificationCount(0) {
        lastModified = std::chrono::system_clock::now();
    }
    
    // Helper to detect data type from variant
    static ConfigDataType detectDataType(const std::variant<int, int64_t, double, bool, std::string,
                                          std::vector<int>, std::vector<double>, std::vector<std::string>>& v) {
        return std::visit([](auto&& arg) -> ConfigDataType {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) return ConfigDataType::Integer;
            else if constexpr (std::is_same_v<T, int64_t>) return ConfigDataType::Integer64;
            else if constexpr (std::is_same_v<T, double>) return ConfigDataType::Double;
            else if constexpr (std::is_same_v<T, bool>) return ConfigDataType::Boolean;
            else if constexpr (std::is_same_v<T, std::string>) return ConfigDataType::String;
            else if constexpr (std::is_same_v<T, std::vector<int>>) return ConfigDataType::IntegerList;
            else if constexpr (std::is_same_v<T, std::vector<double>>) return ConfigDataType::DoubleList;
            else if constexpr (std::is_same_v<T, std::vector<std::string>>) return ConfigDataType::StringList;
            else return ConfigDataType::Unknown;
        }, v);
    }
    
    // Validation methods
    bool validate() const {
        if (isReadOnly) return true; // Read-only values are assumed valid
        
        return std::visit([this](auto&& arg) -> bool {
            using T = std::decay_t<decltype(arg)>;
            
            if constexpr (std::is_same_v<T, std::string>) {
                // String validation
                if (!validationPattern.empty()) {
                    // Simple regex validation would go here
                    // For now, just check basic requirements
                    if (arg.empty() && isRequired) return false;
                }
                return true;
            }
            else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                // List validation
                for (const auto& item : arg) {
                    if (item.empty() && isRequired) return false;
                }
                return true;
            }
            else {
                // Numeric/bool validation - basic type checks
                return true;
            }
        }, value);
    }
    
    std::string getValidationError() const {
        return validationErrorMessage.empty() ? "Validation failed" : validationErrorMessage;
    }
    
    // Conversion methods for different types
    template<typename T>
    std::optional<T> getAs() const {
        try {
            if constexpr (std::is_same_v<T, int>) {
                if (dataType == ConfigDataType::Integer) {
                    return std::get<int>(value);
                }
            } else if constexpr (std::is_same_v<T, int64_t>) {
                if (dataType == ConfigDataType::Integer64) {
                    return std::get<int64_t>(value);
                } else if (dataType == ConfigDataType::Integer) {
                    return static_cast<int64_t>(std::get<int>(value));
                }
            } else if constexpr (std::is_same_v<T, double>) {
                if (dataType == ConfigDataType::Double) {
                    return std::get<double>(value);
                } else if (dataType == ConfigDataType::Integer) {
                    return static_cast<double>(std::get<int>(value));
                } else if (dataType == ConfigDataType::Integer64) {
                    return static_cast<double>(std::get<int64_t>(value));
                }
            } else if constexpr (std::is_same_v<T, bool>) {
                if (dataType == ConfigDataType::Boolean) {
                    return std::get<bool>(value);
                }
            } else if constexpr (std::is_same_v<T, std::string>) {
                if (dataType == ConfigDataType::String) {
                    return std::get<std::string>(value);
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    if (dataType == ConfigDataType::IntegerList) {
                        return std::get<std::vector<int>>(value);
                    }
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    if (dataType == ConfigDataType::DoubleList) {
                        return std::get<std::vector<double>>(value);
                    }
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    if (dataType == ConfigDataType::StringList) {
                        return std::get<std::vector<std::string>>(value);
                    }
                }
            }
        } catch (const std::bad_variant_access&) {
            // Type mismatch
        }
        return std::nullopt;
    }
    
    std::string getAsString() const {
        std::ostringstream oss;
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            oss << arg;
        }, value);
        return oss.str();
    }
    
    double getAsDouble() const {
        return std::visit([](auto&& arg) -> double {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) return static_cast<double>(arg);
            else if constexpr (std::is_same_v<T, int64_t>) return static_cast<double>(arg);
            else if constexpr (std::is_same_v<T, double>) return arg;
            else if constexpr (std::is_same_v<T, bool>) return arg ? 1.0 : 0.0;
            else return 0.0; // Default for strings/lists
        }, value);
    }
    
    bool getAsBool() const {
        return std::visit([](auto&& arg) -> bool {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, bool>) return arg;
            else if constexpr (std::is_same_v<T, int>) return arg != 0;
            else if constexpr (std::is_same_v<T, int64_t>) return arg != 0;
            else if constexpr (std::is_same_v<T, double>) return arg != 0.0;
            else return !arg.empty(); // For strings/lists
        }, value);
    }
    
    size_t getSize() const {
        return std::visit([](auto&& arg) -> size_t {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::vector<int>>) return arg.size();
            else if constexpr (std::is_same_v<T, std::vector<double>>) return arg.size();
            else if constexpr (std::is_same_v<T, std::vector<std::string>>) return arg.size();
            else return 0; // Not a container
        }, value);
    }
};

// Enhanced Logger interface for configuration events
class IConfigLogger {
public:
    virtual ~IConfigLogger() = default;
    virtual void logConfigChange(const std::string& key, const ConfigEntry& oldEntry, 
                                const ConfigEntry& newEntry) {}
    virtual void logConfigValidationError(const std::string& key, const std::string& error) {}
    virtual void logConfigLoad(const std::string& source, bool success) {}
    virtual void logConfigError(const std::string& operation, const std::string& details) {}
};

// Main configuration class with enhanced functionality
class Config {
public:
    // Configuration options
    struct ConfigOptions {
        bool validateOnSet = true;
        bool allowUnknownKeys = false;
        bool preserveOrder = true;
        bool caseSensitive = true;
        LogLevel logLevel = LogLevel::Info;
        std::shared_ptr<IConfigLogger> externalLogger = nullptr;
        size_t maxMemoryUsage = 10 * 1024 * 1024; // 10MB
        bool enableHotReload = false;
        std::chrono::milliseconds hotReloadInterval{1000}; // 1 second
        std::string encoding = "utf-8";
        bool enableComments = true;
        bool enableValidation = true;
        std::string jsonIndent = "  "; // For JSON output
    };
    
    Config(const ConfigOptions& options = ConfigOptions{});
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Enhanced loading with multiple format support
    bool loadFromFile(const std::string& filepath);
    bool loadFromJSON(const std::string& jsonContent);
    bool loadFromYAML(const std::string& yamlContent);
    bool loadFromEnvironment(const std::string& prefix = "NLM_");
    bool loadFromArgs(int argc, char** argv);
    
    // Enhanced saving with multiple formats
    bool saveToFile(const std::string& filepath, const std::string& format = "simple") const;
    bool saveToJSON(const std::string& filepath) const;
    std::string saveToJSONString() const;
    bool saveToINI(const std::string& filepath) const;
    bool saveToTOML(const std::string& filepath) const;
    
    // Enhanced value access with type safety and validation
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    template<typename T>
    T getRequired(const std::string& key) const; // Throws if not found
    
    // Enhanced value setting with validation
    void set(const std::string& key, const ConfigValue& value, ConfigEntry::ConfigSource source = ConfigEntry::ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigEntry::ConfigSource source = ConfigEntry::ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigEntry::ConfigSource source = ConfigEntry::ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigEntry::ConfigSource source = ConfigEntry::ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigEntry::ConfigSource source = ConfigEntry::ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigEntry::ConfigSource source = ConfigEntry::ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<int>& value, ConfigEntry::ConfigSource source = ConfigEntry::ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& value, ConfigEntry::ConfigSource source = ConfigEntry::ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::string>& value, ConfigEntry::ConfigSource source = ConfigEntry::ConfigSource::Runtime);
    
    // Configuration validation and verification
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    bool addValidationRule(const std::string& key, const std::string& pattern, const std::string& errorMessage = "");
    
    // Configuration management
    bool has(const std::string& key) const;
    void remove(const std::string& key);
    void clear();
    void reset(); // Reset to default values
    
    // Query and introspection
    std::vector<std::string> getKeys() const;
    std::vector<std::string> getKeysByType(ConfigDataType type) const;
    std::vector<std::string> getKeysBySource(ConfigEntry::ConfigSource source) const;
    ConfigDataType getDataType(const std::string& key) const;
    ConfigEntry::ConfigSource getSource(const std::string& key) const;
    std::string getDescription(const std::string& key) const;
    
    // Enhanced configuration operations
    void merge(const Config& other, bool overrideExisting = true);
    void override(const Config& other);
    Config diff(const Config& other) const;
    bool equals(const Config& other) const;
    
    // Statistics and monitoring
    size_t getEntryCount() const;
    size_t getMemoryUsage() const;
    double getValidationScore() const;
    std::string getStatistics() const;
    
    // Change tracking and history
    void enableChangeTracking(bool enable = true);
    std::vector<ConfigEntry> getChangeHistory() const;
    ConfigEntry getPreviousValue(const std::string& key) const;
    bool hasChanged(const std::string& key) const;
    
    // Hot reload support
    void enableHotReload(const std::string& filepath, const std::chrono::milliseconds& interval = std::chrono::milliseconds{1000});
    void disableHotReload();
    bool isHotReloadEnabled() const;
    std::chrono::system_clock::time_point getLastLoadTime() const;
    
    // Configuration templates and presets
    bool addPreset(const std::string& name, const Config& preset);
    bool removePreset(const std::string& name);
    Config getPreset(const std::string& name) const;
    bool applyPreset(const std::string& name);
    std::vector<std::string> getPresetNames() const;
    
    // Special configuration helpers
    void configureDefaults(); // Set up default configuration
    void configureFromEnvironment(); // Load from environment variables
    bool validateEnvironment(); // Validate environment-specific settings
    
    // Advanced features for experienced users
    void enableAdvancedFeatures(bool enable = true);
    bool areAdvancedFeaturesEnabled() const;
    std::string getAdvancedFeatureStatus() const;
    
    // File format helpers
    static std::vector<std::string> getSupportedFileFormats();
    static bool isFileFormatSupported(const std::string& format);
    static std::string detectFileFormat(const std::string& filepath);
    
    // Performance optimization
    void optimizeMemoryUsage();
    void compressConfiguration(); // Remove unused sections
    void optimizeAccessPatterns(); // Reorganize for faster access
    
    // Serialization and encryption
    std::string encrypt(const std::string& password) const;
    bool decrypt(const std::string& encryptedData, const std::string& password);
    std::string compress() const;
    bool decompress(const std::string& compressedData);
    
    // Error handling and recovery
    std::vector<std::string> getErrorHistory() const;
    void clearErrorHistory();
    bool hasFatalErrors() const;
    bool recoverFromErrors();
    
    // Global configuration management
    static std::shared_ptr<Config> getGlobalConfig();
    static void setGlobalConfig(std::shared_ptr<Config> config);
    static Config& getDefaultConfig();
    
    // Configuration comparison and merging
    static Config merge(const std::vector<Config>& configs);
    static Config merge(const Config& base, const Config& override);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    bool validateKey(const std::string& key) const;
    std::string normalizeKey(const std::string& key) const;
    void logEvent(const std::string& event, const std::string& details = "") const;
    bool checkMemoryUsage() const;
    void updateMemoryUsage() const;
    void cleanupOldEntries();
    void processHotReload();
    
    // File format parsers
    bool parseSimpleFormat(const std::string& content);
    bool parseJSONFormat(const std::string& content);
    bool parseINIFormat(const std::string& content);
    bool parseYAMLFormat(const std::string& content);
    
    // File format formatters
    std::string formatSimple(const ConfigEntry& entry) const;
    std::string formatJSON(const ConfigEntry& entry) const;
    std::string formatINI(const ConfigEntry& entry) const;
    std::string formatYAML(const ConfigEntry& entry) const;
    
    // Validation helpers
    bool validateString(const std::string& value, const std::string& pattern) const;
    bool validateNumberRange(double value, double min, double max) const;
    bool validateListSize(size_t size, size_t min, size_t max) const;
    bool validateEnum(const std::string& value, const std::vector<std::string>& allowed) const;
};

// Convenience functions for common configuration tasks
namespace ConfigHelpers {
    // Predefined configuration sets
    Config createDefaultConfig();
    Config createDevelopmentConfig();
    Config createProductionConfig();
    Config createMinimalConfig();
    Config createTestConfig();
    
    // Configuration modification helpers
    void enableAllFeatures(Config& config);
    void disableAllFeatures(Config& config);
    void setLogLevel(Config& config, LogLevel level);
    void setPerformanceOptions(Config& config, bool enableProfiling, bool enableCaching);
    void configureForEnvironment(Config& config, const std::string& environment);
    
    // Validation helpers
    bool validateConfig(const Config& config, const std::string& environment = "");
    std::vector<std::string> getConfigIssues(const Config& config);
    
    // File format conversion
    std::string convertConfigFormat(const Config& config, const std::string& targetFormat);
    bool saveConfigInMultipleFormats(const Config& config, const std::string& basePath);
    
    // Configuration comparison
    bool configsAreEquivalent(const Config& config1, const Config& config2, 
                           const std::vector<std::string>& ignoreKeys = {});
    double calculateConfigSimilarity(const Config& config1, const Config& config2);
}

// Global configuration instances
extern std::shared_ptr<Config> globalConfig;

// Convenience macros for configuration
#define CONFIG_GET_INT(key) nlm::Config::getGlobalConfig()->get<int>(key)
#define CONFIG_GET_DOUBLE(key) nlm::Config::getGlobalConfig()->get<double>(key)
#define CONFIG_GET_STRING(key) nlm::Config::getGlobalConfig()->get<std::string>(key)
#define CONFIG_GET_BOOL(key) nlm::Config::getGlobalConfig()->get<bool>(key)
#define CONFIG_GET_REQUIRED_INT(key) nlm::Config::getGlobalConfig()->getRequired<int>(key)

#define CONFIG_SET(key, value) nlm::Config::getGlobalConfig()->set(key, value)
#define CONFIG_SET_WITH_SOURCE(key, value, source) nlm::Config::getGlobalConfig()->set(key, value, source)

#define CONFIG_VALIDATE config = nlm::ConfigHelpers::validateConfig(config)

} // namespace nlm

// Explicit template instantiations for common types
namespace nlm {
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;
template std::optional<std::vector<int>> Config::get<std::vector<int>>(const std::string&) const;
template std::optional<std::vector<double>> Config::get<std::vector<double>>(const std::string&) const;
template std::optional<std::vector<std::string>> Config::get<std::vector<std::string>>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;
template std::vector<int> Config::getOr<std::vector<int>>(const std::string&, const std::vector<int>&) const;
template std::vector<double> Config::getOr<std::vector<double>>(const std::string&, const std::vector<double>&) const;
template std::vector<std::string> Config::getOr<std::vector<std::string>>(const std::string&, const std::vector<std::string>&) const;
} // namespace nlm