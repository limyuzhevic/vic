#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <chrono>
#include <functional>
#include <sstream>

namespace nlm {

// Forward declaration for JSON library
#ifdef HAVE_NLOHMANN_JSON
#include <nlohmann/json.hpp>
#endif

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
    double timestamp;  // For tracking when entry was set
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), timestamp(0.0) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), timestamp(std::chrono::system_clock::now().time_since_epoch().count() / 1e9) {}
};

// Type trait for config validation
struct ConfigTypeInfo {
    std::string name;
    std::function<bool(const ConfigValue&)> validator;
    std::function<std::string(const ConfigValue&)> toString;
};

// Main configuration class with enhanced features
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON, YAML, or legacy key=value format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (JSON format by default)
    bool saveToFile(const std::string& filepath, bool prettyPrint = true) const;
    
    // Get values with type safety and validation
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
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    
    // Advanced set methods with validation
    bool trySet(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    bool validateKey(const std::string& key) const;
    bool validateValue(const std::string& key, const ConfigValue& value) const;
    
    // Check existence and metadata
    bool has(const std::string& key) const;
    
    // Get entry information
    std::optional<ConfigEntry> getEntry(const std::string& key) const;
    double getEntryTimestamp(const std::string& key) const;
    std::string getEntrySource(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get all entries (for debugging/inspection)
    std::vector<ConfigEntry> getAllEntries() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Configuration validation and schema
    void registerType(const std::string& key, const ConfigTypeInfo& typeInfo);
    bool validateConfig() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Configuration import/export
    bool mergeFromConfig(const Config& other, ConfigSource source = ConfigSource::Runtime);
    bool importFromString(const std::string& jsonString, ConfigSource source = ConfigSource::Runtime);
    
    // Default configuration management
    void setDefault(const std::string& key, const ConfigValue& value);
    void clearDefaults();
    bool hasDefault(const std::string& key) const;
    
    // Configuration comparison
    bool equals(const Config& other) const;
    size_t hash() const;
    
    // Statistics
    size_t getEntryCount() const;
    size_t getEntryCount(ConfigSource source) const;
    std::unordered_map<ConfigSource, size_t> getSourceCounts() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toJSON(const ConfigValue& value);
    static ConfigValue fromJSON(const std::string& jsonString);
    
    // JSON helper functions
#ifdef HAVE_NLOHMANN_JSON
    bool loadFromJSON(const std::string& filepath, bool isJSON);
    bool loadFromJSON(const nlohmann::json& json, ConfigSource source);
#endif
    bool loadFromLegacyFormat(const std::string& filepath);
    void setFromJSONValue(const std::string& key, const nlohmann::json& value, ConfigSource source);
    
    // Legacy compatibility
    void set(const std::string& key, const ConfigValue& value, ConfigSource source, bool skipValidation);
    
    // Schema and validation
    mutable std::unordered_map<std::string, ConfigTypeInfo> typeSchema_;
    mutable std::vector<std::string> validationErrors_;
    
    // Default values
    std::unordered_map<std::string, ConfigValue> defaultValues_;
};

} // namespace nlm

// Explicit template instantiations
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
