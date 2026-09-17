#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <nlohmann/json.hpp>

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
    std::vector<std::string>,
    std::map<std::string, std::string>  // For nested structures
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
};

// JSON/YAML parser options
struct ParserOptions {
    bool allowComments;
    bool allowTrailingComma;
    bool preserveOrder;
    
    ParserOptions() : allowComments(false), allowTrailingComma(false), preserveOrder(true) {}
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
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // JSON/YAML parsing functions (implementation in Config.cpp)
    bool loadFromJSON(const std::string& jsonString, ConfigSource source, const ParserOptions& options = ParserOptions());
    bool loadFromYAML(const std::string& yamlString, ConfigSource source, const ParserOptions& options = ParserOptions());
    void flattenAndLoadJSON(const nlohmann::json& json, ConfigSource source, const ParserOptions& options);
    std::string flattenKey(const std::string& key, const nlohmann::json& parent) const;
    void setFromJSONValue(const std::string& key, const nlohmann::json& value, ConfigSource source);
    nlohmann::json buildJSONConfig() const;
    bool validateJSON(const nlohmann::json& config, std::vector<std::string>& errors);
    bool jsonToConfigValue(const nlohmann::json& jsonValue, ConfigValue& configValue);
    nlohmann::json configValueToJSON(const ConfigValue& configValue);

    // File format detection
    static bool isJSONFile(const std::string& filepath);
    static bool isYAMLFile(const std::string& filepath);
};

} // namespace nlm
