#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>

namespace nlm {

// Forward declarations
class Config;

// Configuration value types
using ConfigValue = std::variant<
    int,
    int64_t,
    uint64_t,
    double,
    float,
    bool,
    std::string,
    std::vector<int>,
    std::vector<int64_t>,
    std::vector<uint64_t>,
    std::vector<double>,
    std::vector<float>,
    std::vector<bool>,
    std::vector<std::string>,
    std::vector<std::vector<double>>
>;

// Configuration type enum for validation
enum class ConfigDataType {
    INTEGER,
    INT64,
    UINT64,
    FLOAT,
    DOUBLE,
    BOOLEAN,
    STRING,
    ARRAY_INT,
    ARRAY_INT64,
    ARRAY_UINT64,
    ARRAY_FLOAT,
    ARRAY_DOUBLE,
    ARRAY_BOOL,
    ARRAY_STRING,
    ARRAY_ARRAY_DOUBLE,
    UNKNOWN
};

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    Environment
};

// Validation result
struct ValidationResult {
    bool valid;
    std::string errorMessage;
    std::string path;
    
    ValidationResult(bool v = true, const std::string& error = "", const std::string& path = "")
        : valid(v), errorMessage(error), path(path) {}
};

// Configuration schema element
struct ConfigSchemaElement {
    std::string key;
    ConfigDataType type;
    bool required;
    ConfigValue defaultValue;
    std::string description;
    std::vector<std::string> allowedValues;
    std::pair<double, double> range;  // For numeric types
    
    ConfigSchemaElement() : required(false), description("") {}
    ConfigSchemaElement(const std::string& k, ConfigDataType t, bool req = false, 
                       const ConfigValue& def = {}, const std::string& desc = "")
        : key(k), type(t), required(req), defaultValue(def), description(desc) {}
};

// Configuration validator
class ConfigValidator {
public:
    virtual ~ConfigValidator() = default;
    virtual ValidationResult validate(const std::string& key, const ConfigValue& value) const = 0;
    virtual bool isValidType(ConfigDataType expected, ConfigDataType actual) const = 0;
};

// Default config validator
class DefaultConfigValidator : public ConfigValidator {
public:
    ValidationResult validate(const std::string& key, const ConfigValue& value) const override;
    bool isValidType(ConfigDataType expected, ConfigDataType actual) const override;
};

// Configuration schema for validation
class ConfigSchema {
public:
    void addElement(const ConfigSchemaElement& element);
    ValidationResult validate(const std::string& key, const ConfigValue& value) const;
    bool hasElement(const std::string& key) const;
    ConfigDataType getType(const std::string& key) const;
    const ConfigSchemaElement& getElement(const std::string& key) const;
    std::vector<std::string> getAllKeys() const;
    void clear();
    
private:
    std::unordered_map<std::string, ConfigSchemaElement> elements_;
};

// Configuration manager with schema support
class ConfigManager {
public:
    ConfigManager();
    ~ConfigManager();
    
    // Set schema for validation
    void setSchema(const ConfigSchema& schema);
    
    // Set custom validator
    void setValidator(std::unique_ptr<ConfigValidator> validator);
    
    // Enhanced set with validation
    void setWithValidation(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    // Get validation errors
    std::vector<ValidationResult> getValidationErrors() const;
    
    // Check if configuration is valid
    bool isValid() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
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
    
    // Get values (defined here so templates instantiate in every TU)
template<typename T>
    std::optional<T> get(const std::string& key) const;

    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;

    // Get configuration value with type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set with validation
    template<typename T>
    void set(const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration validation
    void setSchema(const ConfigSchema& schema);
    ValidationResult validate(const std::string& key, const ConfigValue& value) const;
    bool isValid() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Configuration import/export
    bool loadFromJSON(const std::string& filepath);
    bool loadFromYAML(const std::string& filepath);
    bool saveToJSON(const std::string& filepath) const;
    bool saveToYAML(const std::string& filepath) const;
    
    // Configuration comparison
    bool operator==(const Config& other) const;
    bool operator!=(const Config& other) const;
    
    // Get configuration summary with validation status
    std::string summaryWithValidation() const;
    
    // Get configuration statistics
    struct ConfigStats {
        size_t totalEntries;
        size_t validEntries;
        size_t invalidEntries;
        size_t entriesBySource[static_cast<size_t>(ConfigSource::Environment) + 1];
    };
    
    ConfigStats getStats() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static ConfigDataType getTypeFromValue(const ConfigValue& value);
    static std::string configValueToString(const ConfigValue& value);
};
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm
