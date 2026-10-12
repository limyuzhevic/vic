#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <functional>
#include <map>
#include <chrono>

namespace nlm {

// Forward declarations
class Config;
class ConfigSchema;

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

// Type information for configuration parameters
struct ParameterInfo {
    std::string name;
    std::string description;
    std::string type;
    ConfigValue defaultValue;
    std::string unit; // e.g., "seconds", "Hz", "percentage"
    double min;       // For numeric types
    double max;       // For numeric types
    bool required;    // If required parameter is missing
    std::function<bool(const ConfigValue&)> validator; // Custom validation
    std::string category; // For organization (e.g., "simulation", "learning")
    
    ParameterInfo() 
        : name(), description(), type(), defaultValue(), unit(), min(0.0), max(0.0),
          required(false), validator(nullptr), category() {}
    
    ParameterInfo(const std::string& n, const std::string& d, const ConfigValue& dv,
                  const std::string& t = "auto", const std::string& unit = "",
                  double minVal = 0.0, double maxVal = 0.0)
        : name(n), description(d), type(t), defaultValue(dv), unit(unit), 
          min(minVal), max(maxVal), required(false), validator(nullptr), category() {}
};

// Configuration entry with metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    
    // Timestamp when the entry was set
    std::chrono::system_clock::time_point timestamp;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), 
                   timestamp(std::chrono::system_clock::now()) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc),
          timestamp(std::chrono::system_clock::now()) {}
};

// Configuration statistics
struct ConfigStats {
    size_t totalParameters;
    size_t setByUser;
    size_t setByFile;
    size_t setByCommandLine;
    std::chrono::system_clock::time_point firstSet;
    std::chrono::system_clock::time_point lastModified;
    
    ConfigStats() : totalParameters(0), setByUser(0), setByFile(0), 
                   setByCommandLine(0), firstSet(std::chrono::system_clock::now()),
                   lastModified(std::chrono::system_clock::now()) {}
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
    
    // Get values with type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
    bool set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Advanced set with validation
    template<typename T>
    bool set(const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime);
    
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
    
    // Get configuration statistics
    ConfigStats getStats() const;
    
    // Parameter schema management
    void registerParameter(const ParameterInfo& param);
    bool hasParameter(const std::string& key) const;
    std::optional<ParameterInfo> getParameterInfo(const std::string& key) const;
    std::vector<ParameterInfo> getAllParameterInfo() const;
    std::vector<ParameterInfo> getParametersByCategory(const std::string& category) const;
    
    // Validation and type conversion
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Advanced features
    void setValidator(const std::string& key, std::function<bool(const ConfigValue&)> validator);
    bool isValid(const std::string& key) const;
    
    // Convenience methods for common parameter types
    template<typename T>
    bool setFloat(const std::string& key, T value, const std::string& unit = "", double min = 0.0, double max = 1.0);
    template<typename T>
    bool setInt(const std::string& key, T value, double min = 0.0, double max = std::numeric_limits<T>::max());
    template<typename T>
    bool setBool(const std::string& key, T value);
    template<typename T>
    bool setString(const std::string& key, T value);
    
    // Batch operations
    void setMultiple(const std::map<std::string, ConfigValue>& values, ConfigSource source = ConfigSource::Runtime);
    bool setMultipleWithValidation(const std::map<std::string, ConfigValue>& values, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration presets
    static Config createDefaultConfig();
    static Config createResearchConfig();
    static Config createPerformanceConfig();
    static Config createValidationConfig();
    
    // Help and documentation
    std::string getHelp(const std::string& key) const;
    std::string getParameterList() const;
    std::map<std::string, std::string> getCategoryDescriptions() const;
    
    // Time travel (for simulation purposes)
    void setTimestamp(const std::chrono::system_clock::time_point& timestamp);
    std::chrono::system_clock::time_point getTimestamp() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toString(const ConfigValue& value);
    static bool isTypeValid(const ConfigValue& value, const std::string& type);
    std::optional<ParameterInfo> findParameterInfo(const std::string& key) const;
};

// Helper functions
inline std::ostream& operator<<(std::ostream& os, const ConfigSource& source) {
    switch (source) {
        case ConfigSource::Default: return os << "Default";
        case ConfigSource::File: return os << "File";
        case ConfigSource::CommandLine: return os << "CommandLine";
        case ConfigSource::Runtime: return os << "Runtime";
        default: return os << "Unknown";
    }
}

inline std::ostream& operator<<(std::ostream& os, const ParameterInfo& param) {
    os << param.name << " (" << param.type << ")";
    if (!param.description.empty()) {
        os << ": " << param.description;
    }
    if (!param.unit.empty()) {
        os << " [" << param.unit << "]";
    }
    return os;
}

} // namespace nlm

// Explicit template instantiations for common types
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

template bool Config::set(const std::string&, const int&, ConfigSource);
template bool Config::set(const std::string&, const double&, ConfigSource);
template bool Config::set(const std::string&, const bool&, ConfigSource);
template bool Config::set(const std::string&, const std::string&, ConfigSource);

template bool Config::setFloat(const std::string&, double, const std::string&, double, double);
template bool Config::setInt(const std::string&, int64_t, double, double);
template bool Config::setBool(const std::string&, bool);
template bool Config::setString(const std::string&, const std::string&);
