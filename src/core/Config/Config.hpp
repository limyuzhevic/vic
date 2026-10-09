#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>

using json = nlohmann::json;

namespace nlm {

// Forward declarations
class Config;

// Configuration value types
using ConfigValue = std::variant<
    int,
    int64_t,
    uint64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<uint64_t>,
    std::vector<std::string>
>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime
};

// Load result enum
enum class LoadResult {
    Success,
    FileNotFound,
    InvalidArgs,
    ParseError,
    ValidationError
};

// Configuration entry with description
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
    
    using Validator = std::function<void(const ConfigEntry& entry)>;
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
    
    // Load from file with JSON/YAML support
    bool loadFromFile(const std::string& filepath);
    
    // Load from file with JSON format (NEW - Phase 2)
    bool loadFromJson(const json& jsonData);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load convenience methods
    LoadResult loadConfigFile(const std::string& filepath);
    LoadResult loadConfigArgs(int argc, char** argv);
    
    // Save to file with JSON format
    bool saveToFile(const std::string& filepath) const;
    
    // Get configuration as JSON string
    std::string toJson() const;
    
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
    void set(const std::string& key, uint64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get all available configuration keys with defaults
    std::vector<std::string> getAvailableKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Generate default configuration as JSON string
    static std::string generateDefaultConfig();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Validation
    void validateConfig() const;
    ConfigEntry::Validator getValidator(const std::string& key) const;
};

} // namespace nlm

// nlohmann/json serializer for ConfigValue (if needed)
// This would typically go in a separate header, but for simplicity we assume
// the user has included nlohmann/json.hpp before using Config

namespace nlohmann {
    // Forward declaration for nested json serializer
    template<typename T>
    struct adl_serializer;
    
    // Custom serializer for ConfigValue
    template<typename T>
    struct adl_serializer<T, nlm::ConfigValue> {
        static void to_json(json& j, const nlm::ConfigValue& v) {
            std::visit([&j](auto&& arg) {
                using U = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<U, std::string>) {
                    j = arg;
                } else if constexpr (std::is_same_v<U, int> || std::is_same_v<U, int64_t>) {
                    j = arg;
                } else if constexpr (std::is_same_v<U, double>) {
                    j = arg;
                } else if constexpr (std::is_same_v<U, bool>) {
                    j = arg;
                } else if constexpr (std::is_same_v<U, std::vector<int>> ||
                                    std::is_same_v<U, std::vector<double>> ||
                                    std::is_same_v<U, std::vector<uint64_t>> ||
                                    std::is_same_v<U, std::vector<std::string>>) {
                    j = arg;
                }
            }, v);
        }
        
        static void from_json(const json& j, nlm::ConfigValue& v) {
            // Attempt to detect type from JSON
            if (j.is_string()) {
                v = j.get<std::string>();
            } else if (j.is_number_integer()) {
                v = j.get<int64_t>();
            } else if (j.is_number_unsigned()) {
                v = j.get<uint64_t>();
            } else if (j.is_number_float()) {
                v = j.get<double>();
            } else if (j.is_boolean()) {
                v = j.get<bool>();
            } else if (j.is_array()) {
                // Try to infer array type from first element
                if (!j.empty()) {
                    if (j[0].is_number_integer()) {
                        v = j.get<std::vector<int64_t>>();
                    } else if (j[0].is_number_float()) {
                        v = j.get<std::vector<double>>();
                    } else if (j[0].is_string()) {
                        v = j.get<std::vector<std::string>>();
                    } else {
                        // Default to int64_t array
                        v = j.get<std::vector<int64_t>>();
                    }
                } else {
                    v = std::vector<std::string>();
                }
            }
        }
    };
}
