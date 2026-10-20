#pragma once

#include <string>
#include <vector>
#include <map>
#include <variant>
#include <memory>
#include <mutex>
#include <optional>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

// Forward declarations
class ConfigSchema;
class ConfigTemplate;
class ConfigHistory;
class ILogger;

// Configuration value types
using ConfigValue = std::variant<std::string, int, double, bool,
                                std::vector<int>, std::vector<double>, std::vector<std::string>,
                                std::map<std::string, ConfigValue>>;

// Configuration entry structure
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::chrono::system_clock::time_point timestamp;

    ConfigEntry(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime)
        : key(key), value(value), source(source), timestamp(std::chrono::system_clock::now()) {}
};

// ConfigNode for nested configuration
struct ConfigNode {
    std::string name;
    std::vector<ConfigValue> values;
    std::map<std::string, ConfigNode> children;
    ConfigSource source;
};

// Config class
class Config {
public:
    Config();
    ~Config();

    // Disable copying
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    // Move operations
    Config(Config&&) noexcept = default;
    Config& operator=(Config&&) noexcept = default;

    // Loading/saving
    bool loadFromFile(const std::string& filepath);
    bool saveToFile(const std::string& filepath) const;
    bool loadFromJson(const std::string& filepath);
    bool loadFromYaml(const std::string& filepath);
    bool loadFromArgs(int argc, char** argv);
    bool saveToJson(const std::string& filepath) const;
    bool saveToYaml(const std::string& filepath) const;

    // Configuration access
    template<typename T>
    std::optional<T> get(const std::string& key) const;

    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;

    // Configuration modification
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);

    // Query operations
    bool has(const std::string& key) const;
    void remove(const std::string& key);
    std::vector<std::string> getKeys() const;
    void clear();

    // Validation
    bool validate(std::vector<std::string>& errors) const;

    // Utilities
    std::string detectFileFormat(const std::string& filepath) const;
    std::string summary() const;
    std::string trim(const std::string& str);
    std::string toLower(const std::string& str);

    // Template and schema operations
    void setTemplate(std::unique_ptr<ConfigTemplate> template_);
    void setSchema(std::unique_ptr<ConfigSchema> schema);
    void applyTemplate();
    void merge(const Config& other);

    // History and backup
    void backupFile(const std::string& filepath) const;
    void cleanupExpiredLocks();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    // Helper methods
    bool loadFromLegacyFormat(const std::string& filepath);
    bool saveToLegacyFormat(const std::string& filepath) const;
    void validateJsonKeys(const json& j);
    void validateYamlStructure(const YAML::Node& doc);
    ConfigValue jsonValueToConfigValue(const json& val);
    ConfigValue yamlNodeToConfigValue(const YAML::Node& val);
    std::string configValueToJson(const ConfigValue& value) const;
    std::string configValueToYaml(const ConfigValue& value) const;
    bool lockFile(const std::string& filepath);
    void unlockFile(const std::string& filepath);

    // Array conversion helpers
    ConfigValue jsonArrayToConfigValue(const json& arr);
    ConfigValue yamlSequenceToConfigValue(const YAML::Node& seq);
    ConfigValue yamlMappingToConfigValue(const YAML::Node& map);
    ConfigValue jsonObjectToConfigValue(const json& obj);

    // Explicit template instantiations
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
};

} // namespace nlm