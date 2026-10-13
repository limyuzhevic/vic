#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <map>
#include <filesystem>
#include <string>
#include <vector>
#include <variant>
#include <stdexcept>
#include <algorithm>
#include <sstream>

// JSON and YAML support
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

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

// Configuration format detection
enum class ConfigFormat {
    Unknown,
    KeyValue,
    JSON,
    YAML
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// Configuration exception
class ConfigException : public std::runtime_error {
public:
    explicit ConfigException(const std::string& msg) : std::runtime_error(msg) {}
};

// Configuration validation result
struct ValidationResult {
    bool isValid;
    std::string errorMessage;
    std::vector<std::string> warnings;
    
    ValidationResult() : isValid(true), errorMessage("") {}
    ValidationResult(bool valid, const std::string& error, const std::vector<std::string>& warnings = {})
        : isValid(valid), errorMessage(error), warnings(warnings) {}
    
    static ValidationResult Success() { return ValidationResult(); }
    static ValidationResult Error(const std::string& error, const std::vector<std::string>& warnings = {}) {
        return ValidationResult(false, error, warnings);
    }
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
    
    // Load from file (supports JSON, YAML, and legacy key=value)
    bool loadFromFile(const std::string& filepath);
    
    // Load from string (supports JSON and YAML)
    bool loadFromString(const std::string& content, ConfigFormat format = ConfigFormat::Unknown);
    
    // Load from JSON string
    bool loadFromJSON(const std::string& jsonString);
    
    // Load from YAML string
    bool loadFromYAML(const std::string& yamlString);
    
    // Load from command line arguments (legacy support)
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (auto-detects format from extension)
    bool saveToFile(const std::string& filepath) const;
    
    // Save as JSON
    bool saveToJSON(const std::string& filepath) const;
    
    // Save as YAML
    bool saveToYAML(const std::string& filepath) const;
    
    // Save as key=value (legacy format)
    bool saveToKeyValue(const std::string& filepath) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Validation
    ValidationResult validate() const;
    
    // Format detection
    static ConfigFormat detectFormat(const std::string& filepath);
    static ConfigFormat detectFormatFromContent(const std::string& content);
    
    // Configuration path management
    static std::string getConfigPath(const std::string& filename = "configs/default.cfg");
    
    // Merge configurations
    void merge(const Config& other, bool overwriteExisting = false);
    
    // Type checking
    static bool isTypeCompatible(const ConfigValue& value, const std::type_info& typeInfo);
    
    // String conversion
    static std::string toString(const ConfigValue& value);
    static ConfigValue fromString(const std::string& str, ConfigFormat format = ConfigFormat::Unknown);
    
    // Legacy support (for backward compatibility)
    static bool loadLegacyKeyValueFormat(const std::string& content);
    static std::string saveLegacyKeyValueFormat() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string getFileExtension(const std::string& filepath);
    static ConfigFormat detectFormatFromExtension(const std::string& filepath);
    
    // JSON/YAML conversion helpers
    static void jsonToConfig(const nlohmann::json& json, Config& config);
    static void yamlToConfig(const YAML::Node& yaml, Config& config);
    static nlohmann::json configToJson(const Config& config);
    static YAML::Node configToYaml(const Config& config);
    
    // Legacy parsing
    static bool parseLegacyLine(const std::string& line, std::string& key, std::string& value);
    
    // Type conversion helpers
    static bool tryConvertValue(const std::string& str, ConfigValue& value);
    static std::string getDefaultDescription(const std::string& key);
    
    // Error handling
    static std::string getFormatErrorMessage(ConfigFormat format, const std::string& filepath);
    
    // Configuration validation
    static ValidationResult validateConfigEntry(const ConfigEntry& entry) const;
    static ValidationResult validateTypeCompatibility(const std::string& key, const ConfigValue& value) const;
};

// Explicit template instantiations
namespace nlm {
    template std::optional<int> Config::get<int>(const std::string&) const;
    template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
    template std::optional<double> Config::get<double>(const std::string&) const;
    template std::optional<bool> Config::get<bool>(const std::string&) const;
    template std::optional<std::string> Config::get<std::string>(const std::string&) const;
    
    template int Config::getOr<int>(const std::string&, const int&) const;
    template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
    template double Config::getOr<double>(const std::string&, const double&) const;
    template bool Config::getOr<bool>(const std::string&, const bool&) const;
    template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;
}

} // namespace nlm
