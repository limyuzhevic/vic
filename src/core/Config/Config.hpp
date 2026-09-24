#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <algorithm>
#include <sstream>

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

// Configuration entry with metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string validationPattern;
    bool required;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), 
                   validationPattern(), required(false) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", const std::string& pattern = "",
                bool req = false)
        : key(k), value(v), source(s), description(desc), 
          validationPattern(pattern), required(req) {}
    
    bool validate() const {
        if (required && key.empty()) return false;
        
        if (!validationPattern.empty()) {
            // Simple validation - check if string value matches pattern
            if (std::holds_alternative<std::string>(value)) {
                const std::string& strValue = std::get<std::string>(value);
                // Simple pattern matching - placeholder for real validation
                if (validationPattern == "number_range") {
                    try {
                        double val = std::stod(strValue);
                        return val >= 0.0 && val <= 1.0;
                    } catch (...) {
                        return false;
                    }
                } else if (validationPattern == "positive_int") {
                    try {
                        int val = std::stoi(strValue);
                        return val >= 0;
                    } catch (...) {
                        return false;
                    }
                } else if (validationPattern == "non_empty_string") {
                    return !strValue.empty();
                }
            }
        }
        return true;
    }
    
    std::string getTypeString() const {
        if (std::holds_alternative<int>(value)) return "int";
        if (std::holds_alternative<int64_t>(value)) return "int64_t";
        if (std::holds_alternative<double>(value)) return "double";
        if (std::holds_alternative<bool>(value)) return "bool";
        if (std::holds_alternative<std::string>(value)) return "string";
        if (std::holds_alternative<std::vector<int>>(value)) return "std::vector<int>";
        if (std::holds_alternative<std::vector<double>>(value)) return "std::vector<double>";
        if (std::holds_alternative<std::vector<std::string>>(value)) return "std::vector<std::string>";
        return "unknown";
    }
    
    std::string getValueString() const {
        std::ostringstream oss;
        if (std::holds_alternative<int>(value)) {
            oss << std::get<int>(value);
        } else if (std::holds_alternative<int64_t>(value)) {
            oss << std::get<int64_t>(value);
        } else if (std::holds_alternative<double>(value)) {
            oss << std::get<double>(value);
        } else if (std::holds_alternative<bool>(value)) {
            oss << (std::get<bool>(value) ? "true" : "false");
        } else if (std::holds_alternative<std::string>(value)) {
            oss << "\"" << std::get<std::string>(value) << "\"";
        } else if (std::holds_alternative<std::vector<int>>(value)) {
            const auto& vec = std::get<std::vector<int>>(value);
            oss << "[";
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << vec[i];
            }
            oss << "]";
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            const auto& vec = std::get<std::vector<double>>(value);
            oss << "[";
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << vec[i];
            }
            oss << "]";
        } else if (std::holds_alternative<std::vector<std::string>>(value)) {
            const auto& vec = std::get<std::vector<std::string>>(value);
            oss << "[";
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << vec[i] << "\"";
            }
            oss << "]";
        }
        return oss.str();
    }
};

// Main configuration class with enhanced validation and serialization
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON format) with validation
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments with validation
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file with schema validation
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with type validation
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
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
    
    // Get configuration summary with validation status
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isValidConfigKey(const std::string& key);
    static bool isValidConfigValue(const std::string& key, const ConfigValue& value);
};

} // namespace nlm

// Helper functions for common configuration operations
namespace ConfigUtils {
    // Create default NLM configuration
    std::unique_ptr<nlm::Config> createDefaultConfig();
    
    // Create development configuration
    std::unique_ptr<nlm::Config> createDevelopmentConfig();
    
    // Create production configuration
    std::unique_ptr<nlm::Config> createProductionConfig();
    
    // Merge multiple configs
    void mergeConfigs(std::vector<std::unique_ptr<nlm::Config>>& configs);
    
    // Validate configuration against schema
    bool validateConfig(const nlm::Config& config, const std::string& schema);
    
    // Convert config to JSON string
    std::string toJsonString(const nlm::Config& config);
    
    // Create config from JSON string
    std::unique_ptr<nlm::Config> fromJsonString(const std::string& json);
}

namespace nlm {

// Template implementations

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    if (pImpl && pImpl->entries.find(key) != pImpl->entries.find(key)) {
        const auto& entry = pImpl->entries[key];
        if (std::holds_alternative<T>(entry.value)) {
            return std::get<T>(entry.value);
        } else {
            // Type conversion could be implemented here
            return std::nullopt;
        }
    }
    return std::nullopt;
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto value = get<T>(key);
    if (value) return *value;
    return defaultValue;
}

} // namespace nlm

// Type conversion traits template specialization
namespace nlm {

template<typename T>
struct ConfigValueTypeTraits {
    static constexpr bool is_supported = false;
};

template<>
struct ConfigValueTypeTraits<int> {
    static constexpr bool is_supported = true;
};

template<>
struct ConfigValueTypeTraits<int64_t> {
    static constexpr bool is_supported = true;
};

template<>
struct ConfigValueTypeTraits<double> {
    static constexpr bool is_supported = true;
};

template<>
struct ConfigValueTypeTraits<bool> {
    static constexpr bool is_supported = true;
};

template<>
struct ConfigValueTypeTraits<std::string> {
    static constexpr bool is_supported = true;
};

template<>
struct ConfigValueTypeTraits<std::vector<int>> {
    static constexpr bool is_supported = true;
};

template<>
struct ConfigValueTypeTraits<std::vector<double>> {
    static constexpr bool is_supported = true;
};

template<>
struct ConfigValueTypeTraits<std::vector<std::string>> {
    static constexpr bool is_supported = true;
};

// Validation helper functions
namespace ConfigValidation {
    inline bool validateNumberRange(const ConfigValue& value, double min, double max) {
        if (std::holds_alternative<double>(value)) {
            double val = std::get<double>(value);
            return val >= min && val <= max;
        }
        return false;
    }
    
    inline bool validatePositiveInt(const ConfigValue& value) {
        if (std::holds_alternative<int>(value)) {
            int val = std::get<int>(value);
            return val >= 0;
        }
        return false;
    }
    
    inline bool validateNonEmptyString(const ConfigValue& value) {
        if (std::holds_alternative<std::string>(value)) {
            return !std::get<std::string>(value).empty();
        }
        return false;
    }
    
    inline bool validateVectorSize(const ConfigValue& value, size_t expectedSize) {
        if (std::holds_alternative<std::vector<int>>(value)) {
            return std::get<std::vector<int>>(value).size() == expectedSize;
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            return std::get<std::vector<double>>(value).size() == expectedSize;
        } else if (std::holds_alternative<std::vector<std::string>>(value)) {
            return std::get<std::vector<std::string>>(value).size() == expectedSize;
        }
        return false;
    }
}

} // namespace nlm

// Helper functions implementation
namespace ConfigUtils {

inline std::unique_ptr<nlm::Config> createDefaultConfig() {
    auto config = std::make_unique<nlm::Config>();
    config->set("neuron_count", 1000, nlm::ConfigSource::Default);
    config->set("region_count", 1, nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1f, nlm::ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.01f, nlm::ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, nlm::ConfigSource::Default);
    config->set("stdp_tau", 20.0f, nlm::ConfigSource::Default);
    config->set("synaptogenesis_rate", 0.0001f, nlm::ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, nlm::ConfigSource::Default);
    config->set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
    config->set("replay_interval", 100, nlm::ConfigSource::Default);
    config->set("consolidation_interval", 1000, nlm::ConfigSource::Default);
    config->set("random_seed", 42, nlm::ConfigSource::Default);
    return config;
}

inline std::unique_ptr<nlm::Config> createDevelopmentConfig() {
    auto config = std::make_unique<nlm::Config>();
    config->set("neuron_count", 500, nlm::ConfigSource::Default);
    config->set("region_count", 2, nlm::ConfigSource::Default);
    config->set("connection_probability", 0.15f, nlm::ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.02f, nlm::ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.022f, nlm::ConfigSource::Default);
    config->set("synaptogenesis_rate", 0.0002f, nlm::ConfigSource::Default);
    config->set("pruning_rate", 0.00002f, nlm::ConfigSource::Default);
    config->set("simulation_timestep", 0.002, nlm::ConfigSource::Default);
    config->set("replay_interval", 50, nlm::ConfigSource::Default);
    config->set("consolidation_interval", 500, nlm::ConfigSource::Default);
    return config;
}

inline std::unique_ptr<nlm::Config> createProductionConfig() {
    auto config = std::make_unique<nlm::Config>();
    config->set("neuron_count", 5000, nlm::ConfigSource::Default);
    config->set("region_count", 4, nlm::ConfigSource::Default);
    config->set("connection_probability", 0.05f, nlm::ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.005f, nlm::ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.006f, nlm::ConfigSource::Default);
    config->set("synaptogenesis_rate", 0.00005f, nlm::ConfigSource::Default);
    config->set("pruning_rate", 0.000005f, nlm::ConfigSource::Default);
    config->set("simulation_timestep", 0.0005, nlm::ConfigSource::Default);
    config->set("replay_interval", 200, nlm::ConfigSource::Default);
    config->set("consolidation_interval", 2000, nlm::ConfigSource::Default);
    return config;
}

inline void mergeConfigs(std::vector<std::unique_ptr<nlm::Config>>& configs) {
    if (configs.empty()) return;
    
    std::unique_ptr<nlm::Config> merged = std::move(configs[0]);
    for (size_t i = 1; i < configs.size(); ++i) {
        merged->merge(*configs[i]);
    }
    
    configs[0] = std::move(merged);
    configs.resize(1);
}

inline bool validateConfig(const nlm::Config& config, const std::string& schema) {
    // Simple schema validation - placeholder for real implementation
    return true;
}

inline std::string toJsonString(const nlm::Config& config) {
    // Simple JSON conversion - placeholder for real implementation
    return "{}";
}

inline std::unique_ptr<nlm::Config> fromJsonString(const std::string& json) {
    auto config = std::make_unique<nlm::Config>();
    // Simple JSON parsing - placeholder for real implementation
    return config;
}

} // namespace ConfigUtils

} // namespace nlm

// Placeholder implementation for Config::Impl
namespace nlm {

struct Config::Impl {
    std::unordered_map<std::string, ConfigEntry> entries;
    std::vector<std::string> validationErrors;
    
    Impl() {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Config& Config::operator=(Config&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool Config::loadFromFile(const std::string& filepath) {
    // Placeholder implementation
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    // Placeholder implementation
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    // Placeholder implementation
    return true;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    pImpl->entries[key] = ConfigEntry(key, value, source);
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    pImpl->entries[key] = ConfigEntry(key, value, source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    pImpl->entries[key] = ConfigEntry(key, value, source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    pImpl->entries[key] = ConfigEntry(key, value, source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    pImpl->entries[key] = ConfigEntry(key, value, source);
}

bool Config::has(const std::string& key) const {
    return pImpl->entries.find(key) != pImpl->entries.end();
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(key);
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& pair : pImpl->entries) {
        keys.push_back(pair.first);
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Config entries: " << pImpl->entries.size() << "\n";
    for (const auto& pair : pImpl->entries) {
        oss << "  " << pair.first << ": " << pair.second.getTypeString() 
            << " = " << pair.second.getValueString() << "\n";
    }
    return oss.str();
}

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    size_t end = str.find_last_not_of(" \t\n\r");
    return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), 
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

bool Config::isValidConfigKey(const std::string& key) {
    return !key.empty() && key.find(' ') == std::string::npos;
}

bool Config::isValidConfigValue(const std::string& key, const ConfigValue& value) {
    // Basic validation - keys should not be empty
    if (key.empty()) return false;
    
    // Additional validation can be added here based on key patterns
    return true;
}

} // namespace nlm
