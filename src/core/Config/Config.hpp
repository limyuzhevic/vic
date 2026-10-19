#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

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

// Configuration validation result
struct ValidationResult {
    bool valid;
    std::string errorMessage;
    std::string key;
    
    ValidationResult() : valid(false), errorMessage("Invalid configuration"), key() {}
    ValidationResult(bool v, const std::string& error, const std::string& k = "")
        : valid(v), errorMessage(error), key(k) {}
};

// Configuration entry structure
struct ConfigEntry {
    std::string key;      ///< Configuration key name
    ConfigValue value;     ///< Configuration value (variant type)
    ConfigSource source;   ///< How this value was obtained
    std::string description; ///< Human-readable description of the parameter
    
    ConfigEntry() : source(ConfigSource::Default) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource src = ConfigSource::Default,
               const std::string& desc = "")
        : key(k), value(v), source(src), description(desc) {}
    
    /// Equality comparison based on key
    bool operator==(const ConfigEntry& other) const { return key == other.key; }
    
    /// Less-than comparison for ordered containers
    bool operator<(const ConfigEntry& other) const { return key < other.key; }
};

// Configuration parameter metadata for validation and documentation
struct ConfigParameter {
    std::string key;          ///< Configuration key name
    std::string type;         ///< Data type ("int", "int64_t", "double", "bool", "string", "vector")
    ConfigValue defaultValue;  ///< Default value if not specified
    std::string description;   ///< Human-readable description
    std::string category;      ///< Logical grouping (e.g., "Core", "Plasticity", "Environment")
    bool required;             ///< Whether this parameter is required
    
    // Validation constraints
    std::optional<double> minValue;          ///< Minimum allowed value (for numeric types)
    std::optional<double> maxValue;          ///< Maximum allowed value (for numeric types)
    std::vector<std::string> allowedValues;  ///< List of allowed string values (for enum types)
    
    ConfigParameter() : required(false) {}
    
    ConfigParameter(const std::string& k, const std::string& t, const ConfigValue& defaultVal,
                    const std::string& desc = "", const std::string& cat = "General",
                    bool req = false)
        : key(k), type(t), defaultValue(defaultVal), description(desc), category(cat),
          required(req) {}
    
    /// Check if parameter has validation constraints
    bool hasConstraints() const {
        return minValue.has_value() || maxValue.has_value() || !allowedValues.empty();
    }
    
    /// Convert parameter to string representation
    std::string toString() const {
        std::ostringstream oss;
        oss << "Parameter[key=" << key << ", type=" << type << ", category=" << category;
        if (required) oss << ", required";
        oss << "]";
        return oss.str();
    }
};

// Configuration schema for validation
class ConfigSchema {
public:
    ConfigSchema();
    ~ConfigSchema();
    
    // Add a parameter to the schema
    void addParameter(const ConfigParameter& param);
    
    // Get parameter by key
    const ConfigParameter* getParameter(const std::string& key) const;
    
    // Get all parameters
    std::vector<ConfigParameter> getAllParameters() const;
    
    // Get parameters by category
    std::vector<ConfigParameter> getParametersByCategory(const std::string& category) const;
    
    // Get all categories
    std::vector<std::string> getCategories() const;
    
    // Validate a configuration entry
    ValidationResult validateEntry(const ConfigEntry& entry) const;
    
    // Validate all entries in a config
    std::vector<ValidationResult> validateAll(const std::vector<ConfigEntry>& entries) const;
    
    // Get validation errors for a config
    std::string getValidationErrors(const std::vector<ConfigEntry>& entries) const;
    
    // Get schema documentation
    std::string getSchemaDocumentation() const;
    
    // Check if a key is valid
    bool isValidKey(const std::string& key) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Validation functions
namespace ConfigValidation {
    ValidationResult validateType(const ConfigEntry& entry, const std::string& expectedType);
    ValidationResult validateRange(const ConfigEntry& entry, double min, double max);
    ValidationResult validateEnum(const ConfigEntry& entry, const std::vector<std::string>& allowed);
    ValidationResult validateStringPattern(const ConfigEntry& entry, const std::string& pattern);
}

// Configuration value conversion utilities
namespace ConfigUtils {
    std::string toString(const ConfigValue& value);
    std::string getTypeName(const std::type_info& typeInfo);
    std::string getTypeName(const ConfigValue& value);
}

// Validation integration for configuration system
namespace ConfigValidationIntegration {
    // Initialize validation framework with configuration
    bool initializeValidationFramework(const std::string& configFile = "configs/validation.cfg");
    
    // Validate configuration using schema
    std::vector<ValidationResultDetail> validateWithSchema(const Config& config, const ConfigSchema& schema);
    
    // Validate configuration using framework
    std::vector<ValidationResultDetail> validateWithFramework(const Config& config, const ConfigSchema& schema);
    
    // Get validation results
    const std::vector<ValidationResultDetail>& getValidationResults();
    
    // Save validation results to file
    bool saveValidationResults(const std::string& filepath, const std::string& format = "text");
    
    // Load validation configuration from file
    bool loadValidationConfiguration(const std::string& filepath);
    
    // Get validation framework statistics
    std::string getValidationStatistics();
}

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
    
    // Load from command line arguments with help and validation
    bool loadFromArgs(int argc, char** argv, bool showHelp);
    
    // Load configuration from JSON format
    bool loadFromJSON(const std::string& filepath);
    
    // Load configuration from YAML format
    bool loadFromYAML(const std::string& filepath);
    
    // Get command line help
    static std::string getCommandLineHelp();
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
    
    // Get source string representation
    static std::string getSourceString(ConfigSource source);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm
