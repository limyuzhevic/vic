#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>
#include <unordered_map>

namespace nlm {

// Forward declarations
class Config;

// Enhanced configuration value types
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

// Configuration source with more detail
enum class ConfigSource {
    Default = 0,
    File = 1,
    CommandLine = 2,
    Runtime = 3,
    Environment = 4
};

// Enhanced configuration validation
struct ConfigValidation {
    std::string key;
    std::string type; // "int", "float", "bool", "string", "list"
    std::string description;
    std::optional<double> min;
    std::optional<double> max;
    std::optional<std::string> pattern; // For string validation
    bool required = false;
    
    // Validation function
    bool validate(const ConfigValue& value) const {
        return validateValue(value, type, min, max, pattern);
    }
    
private:
    bool validateValue(const ConfigValue& value, const std::string& type,
                      const std::optional<double>& min, const std::optional<double>& max,
                      const std::optional<std::string>& pattern) const {
        if (value.index() == static_cast<size_t>(ConfigValueType::None)) return !required;
        
        if (type == "int") {
            if (std::holds_alternative<int>(value)) {
                double val = static_cast<double>(std::get<int>(value));
                return (!min || val >= *min) && (!max || val <= *max);
            }
            return false;
        } else if (type == "float" || type == "double") {
            if (std::holds_alternative<double>(value)) {
                double val = std::get<double>(value);
                return (!min || val >= *min) && (!max || val <= *max);
            } else if (std::holds_alternative<float>(value)) {
                double val = static_cast<double>(std::get<float>(value));
                return (!min || val >= *min) && (!max || val <= *max);
            }
            return false;
        } else if (type == "bool") {
            return std::holds_alternative<bool>(value);
        } else if (type == "string") {
            if (std::holds_alternative<std::string>(value)) {
                const std::string& val = std::get<std::string>(value);
                if (pattern) {
                    // Simple pattern matching (placeholder for regex)
                    return val.find(*pattern) != std::string::npos;
                }
                return true;
            }
            return false;
        }
        return true; // For list types, skip validation
    }
};

// Main configuration class with enhanced features
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON format with validation)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments with validation
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (JSON format)
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with validation and type checking
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Enhanced set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime, bool validate = true);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime, bool validate = true);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime, bool validate = true);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime, bool validate = true);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime, bool validate = true);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime, bool validate = true);
    
    // Check existence and validation
    bool has(const std::string& key) const;
    bool isValid(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary with validation status
    std::string summary() const;
    
    // Enhanced validation setup
    static void registerDefaultValidation(const std::string& key, ConfigValidation validation);
    static void unregisterValidation(const std::string& key);
    static bool validateAll() const;
    
    // Export/import functionality
    std::string toJSON() const;
    bool fromJSON(const std::string& json);
    
    // Configuration presets for different use cases
    static Config createDefaultConfig();
    static Config createQuickStartConfig();
    static Config createResearchConfig();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal validation helpers
    static bool validateValue(const ConfigValue& value, ConfigSource source, bool validate);
    static std::string getErrorMessage(const std::string& key, const ConfigValue& value) const;
    
    // Static validation registry
    static std::unordered_map<std::string, ConfigValidation> validationRegistry;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

// Enum for ConfigValue type checking
enum class ConfigValueType {
    None,
    Int,
    Int64,
    Double,
    Bool,
    String,
    IntList,
    DoubleList,
    StringList
};

} // namespace nlm