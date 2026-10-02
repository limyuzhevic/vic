#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cstdint>

// Forward declarations
namespace rapidjson {
    template<typename> class Document;
    template<typename> class Value;
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

// Configuration source enum
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime
};

// Configuration entry structure
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// JSON utility functions
namespace json_utils {
    // Convert JSON Value to ConfigValue
    ConfigValue jsonToConfigValue(const rapidjson::Value<>& jsonValue);
    
    // Convert ConfigValue to JSON string
    std::string configToJson(const ConfigValue& value);
    
    // Convert nested JSON object to Config map
    std::map<std::string, ConfigValue> jsonObjectToConfigMap(const rapidjson::Value<>& jsonObj);
    
    // Convert Config map to JSON string
    std::string configMapToJson(const std::map<std::string, ConfigValue>& config);
    
    // Parse JSON with error handling
    rapidjson::Document parseJsonFile(const std::string& filepath);
}

class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON/YAML format)
    bool loadFromFile(const std::string& filepath);
    
    // Save to file (JSON format)
    bool saveToFile(const std::string& filepath) const;
    
    // Load from JSON string
    bool loadFromJson(const std::string& jsonString);
    
    // Save to JSON string
    std::string saveToJson() const;
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Get values with type safety
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
    
    // Set with description
    void set(const std::string& key, const ConfigValue& value, const std::string& description, ConfigSource source = ConfigSource::Runtime);
    
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
    
    // Get configuration as nested map for advanced users
    std::map<std::string, ConfigValue> getAsMap() const;
    
    // Set from nested map
    void setFromMap(const std::map<std::string, ConfigValue>& config);
    
    // Validate configuration values
    bool validate() const;
    
    // Get configuration file extension
    static std::string getConfigFileExtension() { return ".json"; }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static void escapeJsonString(std::string& str);
    static std::string unescapeJsonString(const std::string& str);
    
    // Parse simple key=value format (legacy support)
    bool loadLegacyFormat(std::ifstream& file);
    
    // Write JSON with formatting
    static void writeJsonValue(std::ostream& out, const ConfigValue& value, bool pretty = false);
    static void writeJsonEntry(std::ostream& out, const ConfigEntry& entry, bool pretty = false);
    
    // Initialize default values
    void initializeDefaults();
};

// Explicit template instantiations
extern template std::optional<int> Config::get<int>(const std::string&) const;
extern template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
extern template std::optional<double> Config::get<double>(const std::string&) const;
extern template std::optional<bool> Config::get<bool>(const std::string&) const;
extern template std::optional<std::string> Config::get<std::string>(const std::string&) const;

extern template int Config::getOr<int>(const std::string&, const int&) const;
extern template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
extern template double Config::getOr<double>(const std::string&, const double&) const;
extern template bool Config::getOr<bool>(const std::string&, const bool&) const;
extern template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm

#endif // CONFIG_HPP
