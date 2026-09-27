#pragma once

#include <string>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <functional>
#include <stdexcept>
#include <any>
#include <concepts>
#include <map>
#include <vector>

namespace nlm {

class Configuration;

// Configuration validation and error handling
class ConfigValidationError : public std::runtime_error {
public:
    enum class ErrorType {
        SchemaViolation,
        ConsistencyCheckFailed,
        ConstraintViolation,
        DependencyMissing,
        TypeMismatch,
        ValueOutOfRange,
        DuplicateKey,
        InvalidFormat,
        ComplianceViolation
    };

    ConfigValidationError(const std::string& message, ErrorType type = ErrorType::SchemaViolation)
        : std::runtime_error(message), errorType(type) {}

    ErrorType getType() const { return errorType; }

private:
    ErrorType errorType;
};

// Configuration validation results
struct ValidationResult {
    bool isValid;
    std::string message;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    double confidenceScore;

    ValidationResult() : isValid(true), confidenceScore(1.0) {}
};

// Schema definition for configuration validation
class ConfigSchema {
public:
    struct Field {
        std::string name;
        std::string type;
        bool required;
        std::any defaultValue;
        std::function<bool(const std::any&)> validator;
        std::string description;
    };

    std::string name;
    std::string version;
    std::vector<Field> fields;
    std::unordered_map<std::string, std::vector<std::string>> dependencies;
    std::function<bool(const Configuration&)> customValidator;

    ConfigSchema(const std::string& name = "", const std::string& version = "1.0")
        : name(name), version(version) {}

    bool validate(const Configuration& config) const;
    bool validateField(const std::string& fieldName, const std::any& value) const;
};

// Configuration comparator for diff analysis
class ConfigComparator {
public:
    struct ComparisonResult {
        std::string fieldName;
        std::any oldValue;
        std::any newValue;
        std::string changeType; // "added", "removed", "modified"
        bool isBreakingChange;
    };

    std::vector<ComparisonResult> compare(const Configuration& oldConfig, const Configuration& newConfig) const;
    std::string generateDiffReport(const std::vector<ComparisonResult>& results) const;
};

// Main configuration manager class
class ConfigurationManager {
public:
    ConfigurationManager();
    ~ConfigurationManager();

    // Load configuration from file
    bool loadConfiguration(const std::string& filePath);

    // Save configuration to file
    bool saveConfiguration(const std::string& filePath) const;

    // Get configuration value
    template<typename T>
    T getValue(const std::string& key) const;

    // Set configuration value
    template<typename T>
    void setValue(const std::string& key, const T& value);

    // Get configuration as string
    std::string toString() const;

    // Get configuration XML representation
    std::string toXML() const;

    // Get configuration JSON representation
    std::string toJSON() const;

    // Validate configuration
    ValidationResult validate() const;

    // Compare with another configuration
    std::vector<ConfigComparator::ComparisonResult> compare(const ConfigurationManager& other) const;

    // Get configuration size
    size_t getSize() const { return configData.size(); }

    // Check if configuration exists
    bool hasKey(const std::string& key) const { return configData.find(key) != configData.end(); }

    // Get all keys
    std::vector<std::string> getKeys() const;

    // Clear configuration
    void clear() { configData.clear(); }

    // Create configuration from template
    bool createFromTemplate(const std::string& templateName);

    // Apply preset configuration
    bool applyPreset(const std::string& presetName);

private:
    std::unordered_map<std::string, std::any> configData;
    mutable std::mutex configMutex;
    std::string filePath;
    std::string name;
};

} // namespace nlm