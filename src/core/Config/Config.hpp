#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <unordered_set>
#include <functional>
#include <limits>

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

// Configuration metadata
struct ConfigMetadata {
    std::string key;
    std::string description;
    std::string type;
    std::any defaultValue;
    std::any minValue;
    std::any maxValue;
    std::string pattern;
    bool required;
    bool validated;
    
    ConfigMetadata() : required(false), validated(false) {}
    ConfigMetadata(const std::string& k, const std::string& d, const std::string& t, const std::any& def,
                  const std::any& min = std::any(), const std::any& max = std::any(),
                  const std::string& pat = "", bool req = false)
        : key(k), description(d), type(t), defaultValue(def), minValue(min), maxValue(max), 
          pattern(pat), required(req), validated(false) {}
};

// Configuration validation rule
struct ConfigValidationRule {
    std::string key;
    std::string ruleType; // "range", "pattern", "list", "required"
    std::any ruleValue1;
    std::any ruleValue2;
    std::string errorMessage;
    
    ConfigValidationRule() {}
    ConfigValidationRule(const std::string& k, const std::string& type, const std::any& v1, 
                        const std::any& v2 = std::any(), const std::string& msg = "")
        : key(k), ruleType(type), ruleValue1(v1), ruleValue2(v2), errorMessage(msg) {}
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    bool isValid;
    std::vector<std::string> validationErrors;
    
    ConfigEntry() : source(ConfigSource::Default), isValid(true) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), isValid(true) {}
};

// Configuration type enum
enum class ConfigType {
    Integer,
    Int64,
    Float,
    Double,
    Boolean,
    String,
    StringList,
    IntList,
    FloatList
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
    
    // Set values with validation
    bool setValidated(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime,
                     const std::string& description = "");
    void setValidatedInt(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime,
                        int min = std::numeric_limits<int>::min(), int max = std::numeric_limits<int>::max(),
                        const std::string& description = "");
    void setValidatedDouble(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime,
                           double min = -std::numeric_limits<double>::max(), double max = std::numeric_limits<double>::max(),
                           const std::string& description = "");
    void setValidatedString(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime,
                           const std::string& pattern = "", const std::string& description = "");
    
    // Get value with validation
    template<typename T>
    std::optional<T> getValidated(const std::string& key, const T& defaultValue = T(),
                                 bool validateRange = true, bool validateType = true) const;
    
    // Configuration validation
    static bool validateInt(int value, int min, int max);
    static bool validateDouble(double value, double min, double max);
    static bool validateString(const std::string& value, const std::string& pattern);
    static bool validateList(const ConfigValue& value, size_t maxSize = 0);
    
    // Get configuration metadata
    std::optional<ConfigMetadata> getMetadata(const std::string& key) const;
    std::vector<ConfigMetadata> getAllMetadata() const;
    
    // Configuration validation rules
    static std::vector<ConfigValidationRule> loadValidationRules();
    static bool validateConfigEntry(const ConfigEntry& entry);
    
    // Configuration presets
    static std::map<std::string, ConfigValue> getDefaultValues();
    static std::map<std::string, ConfigValidationRule> getDefaultValidationRules();
    
    // Batch operations
    bool applyValidatedConfig(const std::map<std::string, ConfigValue>& values);
    bool resetToDefaults();
    
    // Configuration conversion
    static ConfigValue convertToType(const std::string& str, ConfigType targetType);
    static std::string convertToString(const ConfigValue& value);
    
    // Configuration comparison
    bool equals(const Config& other) const;
    bool lessThan(const Config& other) const;
    
    // Configuration statistics
    double getValidationScore() const;
    size_t getEntryCount() const;
    size_t getErrorCount() const;
    
    // Configuration debugging
    std::string debugInfo() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Configuration limits and constraints
    static constexpr size_t MAX_STRING_LENGTH = 1024;
    static constexpr size_t MAX_LIST_SIZE = 1000;
    static constexpr int DEFAULT_INT_MIN = -1000000;
    static constexpr int DEFAULT_INT_MAX = 1000000;
    static constexpr double DEFAULT_DOUBLE_MIN = -1000.0;
    static constexpr double DEFAULT_DOUBLE_MAX = 1000.0;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool matchesPattern(const std::string& str, const std::string& pattern);
    static std::vector<std::string> split(const std::string& str, char delimiter);
    
    // Validation helpers
    static std::string getTypeName(const ConfigValue& value);
    static bool isValidType(const ConfigValue& value, ConfigType expectedType);
    
    // Error handling
    void addValidationError(const std::string& message);
    void clearValidationErrors();
};

// Utility functions
namespace ConfigUtils {
    inline bool stringContains(const std::string& str, const std::string& substr) {
        return str.find(substr) != std::string::npos;
    }
    
    inline std::vector<std::string> splitString(const std::string& str, char delimiter = ',') {
        std::vector<std::string> result;
        std::stringstream ss(str);
        std::string item;
        while (std::getline(ss, item, delimiter)) {
            result.push_back(Config::trim(item));
        }
        return result;
    }
    
    inline bool isNumber(const std::string& str) {
        std::istringstream iss(str);
        double val;
        return (iss >> val) && iss.eof();
    }
    
    inline bool isInteger(const std::string& str) {
        if (str.empty() || (str[0] == '-' && str.size() == 1)) return false;
        std::istringstream iss(str);
        int64_t val;
        char remaining;
        return (iss >> val) && !(iss >> remaining);
    }
}

} // namespace nlm
