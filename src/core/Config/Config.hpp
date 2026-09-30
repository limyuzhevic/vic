#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <filesystem>

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <filesystem>
#include <map>

// JSON library for configuration parsing
namespace nlohmann {
    template<typename T> class json;
}

namespace nlm {
    // Forward declaration for JSON processing
    void to_json(nlohmann::json& j, const ConfigEntry& entry);
    void from_json(const nlohmann::json& j, ConfigEntry& entry);

    // Configuration categories for organization
    enum class ConfigCategory {
        Neural,
        Plasticity,
        Development,
        Neuromodulation,
        Memory,
        Cognition,
        Sensory,
        Prediction,
        Motor,
        Environment,
        Experiment,
        Visualization,
        Logging,
        Random,
        General
    };
    
    // Category name mapping
    inline const std::vector<std::string> ConfigCategoryNames = {
        "Neural", "Plasticity", "Development", "Neuromodulation",
        "Memory", "Cognition", "Sensory", "Prediction", "Motor",
        "Environment", "Experiment", "Visualization", "Logging",
        "Random", "General"
    };
    
    // Configuration type enum for validation
    enum class ConfigType {
        Integer,
        Float,
        Boolean,
        String,
        VectorInt,
        VectorFloat,
        VectorString,
        Unknown
    };

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

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    ConfigCategory category;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), category(ConfigCategory::General) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "", ConfigCategory cat = ConfigCategory::General)
        : key(k), value(v), source(s), description(desc), category(cat) {}
};

// Configuration registry for documentation
struct ConfigRegistry {
    std::map<std::string, ConfigEntry> registry;
    std::map<std::string, std::string> keyToDescription;
    std::map<std::string, ConfigCategory> keyToCategory;
    
    void registerConfig(const std::string& key, const std::string& description, 
                       ConfigCategory category = ConfigCategory::General,
                       const std::string& exampleValue = "");
    
    std::string getDescription(const std::string& key) const;
    ConfigCategory getCategory(const std::string& key) const;
    std::vector<std::string> getKeysByCategory(ConfigCategory category) const;
    std::map<ConfigCategory, std::vector<std::string>> getAllCategories() const;
    void clear();
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
    
    // Configuration registry support
    void registerConfig(const std::string& key, const std::string& description, 
                       ConfigCategory category = ConfigCategory::General,
                       const std::string& exampleValue = "");
    
    // Enhanced getter methods with validation
    template<typename T>
    T getValidated(const std::string& key, const T& defaultValue = T(), 
                   T min = T(), T max = T(), bool required = false) const;
    
    // Convenience methods for common configuration patterns
    std::string getStringOr(const std::string& key, const std::string& defaultValue = "") const;
    int getIntOr(const std::string& key, int defaultValue = 0) const;
    double getDoubleOr(const std::string& key, double defaultValue = 0.0) const;
    bool getBoolOr(const std::string& key, bool defaultValue = false) const;
    
    // Configuration management
    bool has(const std::string& key) const;
    void remove(const std::string& key);
    std::vector<std::string> getKeys() const;
    std::vector<std::string> getKeysByCategory(ConfigCategory category) const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    std::string summary(ConfigCategory category) const;
    
    // Error handling
    std::string getLastError() const { return pImpl->lastError; }
    void clearLastError() { pImpl->lastError.clear(); }
    
    // Configuration validation
    bool validate() const;
    std::string getValidationErrors() const;
    
    // Configuration export/import
    bool exportToJson(const std::string& filepath) const;
    bool importFromJson(const std::string& filepath);
    
    // Configuration comparison
    bool equals(const Config& other) const;
    size_t size() const { return pImpl->entries.size(); }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static ConfigType getConfigType(const ConfigValue& value);
    static bool valueInRange(const ConfigValue& value, const ConfigValue& min, const ConfigValue& max);
    
    // Configuration registry instance
    static ConfigRegistry globalRegistry;
};

// Helper functions for configuration
namespace ConfigHelpers {
    std::string sourceToString(ConfigSource source);
    std::string categoryToString(ConfigCategory category);
    ConfigType valueTypeToString(ConfigType type);
    std::string valueToString(const ConfigValue& value);
    bool stringToValue(const std::string& str, ConfigValue& value);
    
    // Configuration validation helpers
    bool isValidInteger(const std::string& str, int& result, int min = INT_MIN, int max = INT_MAX);
    bool isValidDouble(const std::string& str, double& result, double min = -std::numeric_limits<double>::max(), 
                      double max = std::numeric_limits<double>::max());
    bool isValidBool(const std::string& str, bool& result);
}

} // namespace nlm
