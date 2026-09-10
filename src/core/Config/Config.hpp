#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <functional>
#include <any>
#include <map>

// Forward declarations for JSON and YAML libraries
namespace nlohmann {
    template<typename T>
    class basic_json;
    using json = basic_json<std::string, std::vector, std::unordered_map, std::string, bool, int64_t, uint64_t, double, std::vector, std::unordered_map>;
}

namespace YAML {
    class Node;
}

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
    std::vector<bool>
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
    
    // Load from file (JSON/YAML format detected by extension)
    bool loadFromFile(const std::string& filepath);
    
    // Load from string (JSON/YAML format)
    bool loadFromString(const std::string& content, const std::string& format = "");
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (JSON format by default, YAML if .yaml extension)
    bool saveToFile(const std::string& filepath, const std::string& format = "json") const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Complex type getters
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    std::optional<std::vector<T>> getVector(const std::string& key) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Complex type setters
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<bool>& value, ConfigSource source = ConfigSource::Runtime);
    
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
    
    // Type conversion helpers
    template<typename T>
    static bool convertFromJSON(const nlohmann::json& j, T& out);
    
    template<typename T>
    static bool convertFromYAML(const YAML::Node& node, T& out);
    
    template<typename T>
    static nlohmann::json convertToJSON(const T& value);
    
    template<typename T>
    static YAML::Node convertToYAML(const T& value);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string getFormat(const std::string& filepath);
    static bool detectFormat(const std::string& content, std::string& format);
    
    // JSON/YAML parsing helpers
    void parseJSON(const nlohmann::json& j, ConfigSource source);
    void parseYAML(const YAML::Node& node, ConfigSource source);
    void parseSimpleFormat(const std::string& content, ConfigSource source);
    
    // File I/O helpers
    std::string readFile(const std::string& filepath) const;
    bool writeFile(const std::string& filepath, const std::string& content) const;
    
    // Serialization helpers
    nlohmann::json toJSON() const;
    YAML::Node toYAML() const;
    std::string toString(const nlohmann::json& j) const;
    std::string toString(const YAML::Node& node) const;
};

} // namespace nlm

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
    
    template bool Config::convertFromJSON<int>(const nlohmann::json&, int&) const;
    template bool Config::convertFromJSON<double>(const nlohmann::json&, double&) const;
    template bool Config::convertFromJSON<std::string>(const nlohmann::json&, std::string&) const;
    template bool Config::convertFromJSON<bool>(const nlohmann::json&, bool&) const;
    
    template nlohmann::json Config::convertToJSON<int>(const int&);
    template nlohmann::json Config::convertToJSON<double>(const double&);
    template nlohmann::json Config::convertToJSON<std::string>(const std::string&);
    template nlohmann::json Config::convertToJSON<bool>(const bool&);
}
