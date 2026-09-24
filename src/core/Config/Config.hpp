#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <nlohmann/json.hpp>

// Forward declarations
class Config;

// Forward declare YAML::Node for the implementation
namespace YAML {
    class Node;
}

namespace nlm {

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

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
    
    // JSON serialization support
    nlohmann::json toJson() const;
    static std::optional<ConfigEntry> fromJson(const nlohmann::json& j);
};
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON or YAML format)
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
    
    // JSON/YAML support
    nlohmann::json toJson() const;
    bool fromJson(const nlohmann::json& j);
    
    // File format detection
    static bool isJsonFile(const std::string& filepath);
    static bool isYamlFile(const std::string& filepath);
    
    // Validation methods
    void setValidationOptions(const ValidationOptions& options);
    ValidationOptions getValidationOptions() const;
    std::vector<ValidationResult> validate() const;
    bool validateParameter(const std::string& key) const;
    bool isValid() const;
    std::string getAnalysisReport() const;
    std::vector<std::string> getWarnings() const;
    std::string getParameterDocumentation(const std::string& key) const;
    std::vector<std::string> getAllParameterNames() const;
    std::vector<std::string> getParametersByCategory(const std::string& category) const;
    bool migrateFromLegacy(const std::unordered_map<std::string, ConfigValue>& legacyConfig);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Validation
    static bool validateKey(const std::string& key);
    static bool validateValue(const ConfigValue& value);
    
    // Type conversion helpers
    static std::optional<ConfigValue> jsonToConfigValue(const nlohmann::json& j);
    static nlohmann::json configValueToJson(const ConfigValue& cv);
    
    // Validation integration
    ValidationResult validateParameterInternal(const std::string& key, const ConfigValue& value) const;
    std::vector<std::string> collectWarnings() const;
    bool validateConsistency() const;
};

} // namespace nlm

// JSON value to ConfigValue conversion (declared outside namespace for template specialization)
template<typename T>
std::optional<T> convertJsonToConfigValue(const nlohmann::json& j);

// ConfigValue to JSON value conversion
template<typename T>
nlohmann::json convertConfigValueToJson(const T& value);
