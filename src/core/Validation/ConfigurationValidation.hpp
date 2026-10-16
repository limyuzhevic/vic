// NLM Configuration Validation System
// Validates configuration parameters and provides sensible defaults

#ifndef NLM_CONFIGURATION_VALIDATION_HPP
#define NLM_CONFIGURATION_VALIDATION_HPP

#include "core/Config/Config.hpp"
#include <string>
#include <map>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <cctype>

namespace nlm {

// Configuration Validation Exception
class ConfigurationValidationError : public std::runtime_error {
public:
    ConfigurationValidationError(const std::string& message) 
        : std::runtime_error(message) {}
};

// Configuration Parameter Definition
struct ConfigParameter {
    std::string name;
    std::string description;
    std::string type; // "int64_t", "double", "float", "std::string", "bool"
    std::any defaultValue;
    std::any minValue;
    std::any maxValue;
    bool required;
    std::vector<std::string> allowedValues;
    std::string validationRule; // Regex or custom validation function
    
    ConfigParameter() : required(false), minValue(nullptr), maxValue(nullptr) {}
};

// Configuration Schema
class ConfigurationSchema {
public:
    ConfigurationSchema() = default;
    ~ConfigurationSchema() = default;
    
    // Add parameter to schema
    void addParameter(const ConfigParameter& param);
    
    // Add parameter with type inference
    template<typename T>
    void addParameter(const std::string& name, 
                     const std::string& description,
                     const T& defaultValue,
                     bool required = false) {
        ConfigParameter param;
        param.name = name;
        param.description = description;
        param.type = getTypeName<T>();
        param.defaultValue = defaultValue;
        param.required = required;
        parameters[name] = param;
    }
    
    // Add parameter with validation
    template<typename T>
    void addParameter(const std::string& name,
                     const std::string& description,
                     const T& defaultValue,
                     const T& minValue,
                     const T& maxValue,
                     bool required = false) {
        ConfigParameter param;
        param.name = name;
        param.description = description;
        param.type = getTypeName<T>();
        param.defaultValue = defaultValue;
        param.minValue = minValue;
        param.maxValue = maxValue;
        param.required = required;
        parameters[name] = param;
    }
    
    // Validate configuration against schema
    void validate(const Config& config) const;
    
    // Get parameter definition
    const ConfigParameter& getParameter(const std::string& name) const;
    
    // Check if parameter exists in schema
    bool hasParameter(const std::string& name) const;
    
    // Get all parameter names
    std::vector<std::string> getParameterNames() const;
    
    // Get validation errors
    std::vector<std::string> getValidationErrors(const Config& config) const;
    
    // Generate documentation
    std::string generateDocumentation() const;
    
    // Load schema from file
    bool loadFromFile(const std::string& filename);
    
    // Save schema to file
    bool saveToFile(const std::string& filename) const;

private:
    std::map<std::string, ConfigParameter> parameters;
    
    std::string getTypeName(const std::type_info& type);
    template<typename T>
    std::string getTypeName() {
        if (std::is_same<T, int64_t>::value) return "int64_t";
        if (std::is_same<T, double>::value) return "double";
        if (std::is_same<T, float>::value) return "float";
        if (std::is_same<T, std::string>::value) return "std::string";
        if (std::is_same<T, bool>::value) return "bool";
        return "unknown";
    }
    
    template<typename T>
    bool isInRange(const T& value, const std::any& minValue, const std::any& maxValue) const {
        if (!minValue.has_value() || !maxValue.has_value()) return true;
        
        T min = std::any_cast<T>(minValue);
        T max = std::any_cast<T>(maxValue);
        
        return value >= min && value <= max;
    }
    
    bool validateString(const std::string& value, const std::string& rule) const;
};

// Configuration Validator
class ConfigurationValidator {
public:
    ConfigurationValidator() = default;
    ~ConfigurationValidator() = default;
    
    // Validate configuration with schema
    static bool validate(const Config& config, const ConfigurationSchema& schema);
    
    // Validate configuration with default schema
    static bool validate(const Config& config);
    
    // Fix configuration using schema defaults
    static void fixConfiguration(Config& config, const ConfigurationSchema& schema);
    
    // Get validation result with detailed information
    static ValidationResult validateDetailed(const Config& config, 
                                           const ConfigurationSchema& schema);
    
    // Create default configuration schema
    static ConfigurationSchema createDefaultSchema();
    
    // Create command-line configuration schema
    static ConfigurationSchema createCommandLineSchema();
    
    // Create file configuration schema
    static ConfigurationSchema createFileSchema();

private:
    struct ValidationResult {
        bool valid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        std::map<std::string, std::any> fixedValues;
    };
};

// Configuration Fixer
class ConfigurationFixer {
public:
    ConfigurationFixer() = default;
    ~ConfigurationFixer() = default;
    
    // Fix configuration with schema
    static void applyFixes(Config& config, const ConfigurationSchema& schema);
    
    // Fix configuration with default schema
    static void applyFixes(Config& config);
    
    // Get list of fixes that would be applied
    static std::vector<std::string> getFixList(const Config& config, 
                                              const ConfigurationSchema& schema);
    
    // Check if configuration needs fixing
    static bool needsFixing(const Config& config, const ConfigurationSchema& schema);

private:
    static std::vector<std::string> generateFixMessages(const Config& config,
                                                        const ConfigurationSchema& schema);
    
    static bool isValueValid(const std::any& value, const ConfigParameter& param);
};

// Configuration Preset Manager
class ConfigurationPresets {
public:
    ConfigurationPresets() = default;
    ~ConfigurationPresets() = default;
    
    // Get preset configuration
    static Config getPreset(const std::string& presetName);
    
    // List available presets
    static std::vector<std::string> getAvailablePresets();
    
    // Add custom preset
    static void addPreset(const std::string& name, const Config& config);
    
    // Remove preset
    static bool removePreset(const std::string& name);
    
    // Load presets from file
    static bool loadFromFile(const std::string& filename);
    
    // Save presets to file
    static bool saveToFile(const std::string& filename) const;

private:
    static std::map<std::string, Config> presets;
};

// Configuration Logger
class ConfigurationLogger {
public:
    ConfigurationLogger() = default;
    ~ConfigurationLogger() = default;
    
    // Log configuration changes
    static void logChange(const std::string& key, const std::any& oldValue, 
                         const std::any& newValue, const std::string& source);
    
    // Set log level
    static void setLogLevel(const std::string& level);
    
    // Get log level
    static std::string getLogLevel();
    
    // Enable/disable logging
    static void enableLogging(bool enable);
    
    // Check if logging is enabled
    static bool isLoggingEnabled();
    
    // Get log history
    static std::vector<std::string> getLogHistory();
    
    // Clear log history
    static void clearLogHistory();

private:
    static std::vector<std::string> logHistory;
    static std::string logLevel;
    static bool loggingEnabled;
};

// Convenience functions
namespace Configuration {
    // Validate configuration
    bool validate(const Config& config);
    
    // Validate with custom schema
    bool validate(const Config& config, const ConfigurationSchema& schema);
    
    // Fix configuration
    void fix(Config& config);
    
    // Fix with custom schema
    void fix(Config& config, const ConfigurationSchema& schema);
    
    // Get validation errors
    std::vector<std::string> getErrors(const Config& config);
    
    // Get validation warnings
    std::vector<std::string> getWarnings(const Config& config);
    
    // Add preset
    void addPreset(const std::string& name, const Config& config);
    
    // Get preset
    Config getPreset(const std::string& name);
    
    // Log configuration
    void log(const Config& config, const std::string& source);
    
    // Load default schema
    ConfigurationSchema loadDefaultSchema();
    
    // Get default validation rules
    std::map<std::string, ConfigParameter> getDefaultValidationRules();
}

} // namespace nlm

#endif // NLM_CONFIGURATION_VALIDATION_HPP
