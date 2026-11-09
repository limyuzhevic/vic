#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>
#include <functional>
#include <map>
#include <limits>

namespace nlm {

// Forward declarations
class Config;

// Configuration value types
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    float,
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
};

// Main configuration class with enhanced functionality
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON or simple key=value format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values (defined here so templates instantiate in every TU)
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
    void set(const std::string& key, float value, ConfigSource source = ConfigSource::Runtime);
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
    
    // Get configuration schema
    std::map<std::string, std::string> getKeyTypes() const { return pImpl->keyTypes; }
    
    // Validate a configuration key
    bool validateKey(const std::string& key) const;
    
    // Get default value for a key
    ConfigValue getDefaultValue(const std::string& key) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Enhanced setters for different types
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source);
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source);
    
    // Internal configuration loading
    bool loadFromSimpleFile(std::ifstream& file, ConfigSource source);
    ConfigValue convertStringToConfigValue(const std::string& key, const std::string& valueStr);
    void setFromJson(const std::string& key, const nlohmann::json& value, ConfigSource source);
    
    // Initialize default schema
    void initializeDefaultSchema();
    
    // Friend declaration for JSON access
    friend class nlohmann::json_adapter;
};

} // namespace nlm

// JSON adapter for nlohmann/json
namespace nlohmann {
    template<>
    struct adl_serializer<nlm::ConfigValue> {
        static void to_json(json& j, const nlm::ConfigValue& cv) {
            std::visit([&j](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    j = arg;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    j = arg;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    j = arg;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    j = arg;
                } else {
                    j = arg;
                }
            }, cv);
        }
        
        static void from_json(const json& j, nlm::ConfigValue& cv) {
            // This is a simplified implementation - for complex conversions,
            // we would need to handle all variant types
            if (j.is_string()) {
                cv = j.get<std::string>();
            } else if (j.is_number_integer()) {
                cv = j.get<int64_t>();
            } else if (j.is_number_unsigned()) {
                cv = static_cast<int64_t>(j.get<uint64_t>());
            } else if (j.is_number()) {
                // Try double first, then float
                double d = j.get<double>();
                if (d >= std::numeric_limits<float>::min() && 
                    d <= std::numeric_limits<float>::max()) {
                    cv = static_cast<float>(d);
                } else {
                    cv = d;
                }
            } else if (j.is_boolean()) {
                cv = j.get<bool>();
            } else if (j.is_array()) {
                // For arrays, we need to determine the type
                if (!j.empty()) {
                    if (j[0].is_number_integer()) {
                        std::vector<int> intArray;
                        for (const auto& item : j) {
                            intArray.push_back(item.get<int>());
                        }
                        cv = intArray;
                    } else if (j[0].is_number()) {
                        std::vector<double> doubleArray;
                        for (const auto& item : j) {
                            doubleArray.push_back(item.get<double>());
                        }
                        cv = doubleArray;
                    } else if (j[0].is_string()) {
                        std::vector<std::string> stringArray;
                        for (const auto& item : j) {
                            stringArray.push_back(item.get<std::string>());
                        }
                        cv = stringArray;
                    }
                }
            }
        }
    };
}
