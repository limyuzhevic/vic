#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
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
    std::vector<std::string>
>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    Preset
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
    
    // Apply preset configuration
    void applyPreset(const std::string& presetName);
    
    // Merge configuration from another config object
    void mergeFrom(const Config& other, ConfigSource source = ConfigSource::File);
    
    // Get default configuration template
    static std::string getTemplate(const std::string& format = "json");
    
    // Validate configuration with detailed error reporting
    std::vector<std::string> validateDetailed() const;
    
    // Get configuration quality score (0-100)
    int getQualityScore() const;
    
    // Check if configuration is optimized
    bool isOptimized() const;
    
    // Get memory usage of configuration
    size_t getMemoryUsage() const;

} // namespace nlm

// nlohmann JSON serialization for ConfigValue
template<typename... Ts>
struct nlohmann::json_serializer<std::variant<Ts...>> {
    static void to_json(json& j, const std::variant<Ts...>& value) {
        j = nlm::Config::convertConfigValueToJson(value);
    }
    
    static void from_json(const json& j, std::variant<Ts...>& value) {
        value = nlm::Config::convertJsonToConfigValue(j);
    }
};

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
