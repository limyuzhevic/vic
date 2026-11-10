#pragma once

#include "Config.hpp"
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <set>
#include <unordered_map>
#include <memory>
#include <sstream>

namespace nlm {

// Configuration validation error types
enum class ConfigValidationError {
    InvalidType,
    InvalidRange,
    InvalidValue,
    MissingRequired,
    DuplicateKey,
    SchemaViolation,
    FileParseError,
    CommandLineError
};

// Configuration validation rule
struct ConfigValidationRule {
    std::string key;
    std::function<bool(const ConfigValue&)> validator;
    std::string errorMessage;
    bool required = false;
    std::vector<std::string> allowedValues;
    std::pair<double, double> range;
    std::string description;
};

// Configuration schema
class ConfigSchema {
public:
    ConfigSchema();
    
    // Add a validation rule for a specific key
    void addRule(const std::string& key, 
                 const std::function<bool(const ConfigValue&)>& validator,
                 const std::string& errorMessage,
                 bool required = false,
                 const std::vector<std::string>& allowedValues = {},
                 const std::pair<double, double>& range = {-1e6, 1e6},
                 const std::string& description = "");
    
    // Add type-specific validation
    void addIntRule(const std::string& key, const std::pair<int64_t, int64_t>& range, bool required = false);
    void addFloatRule(const std::string& key, const std::pair<double, double>& range, bool required = false);
    void addStringRule(const std::string& key, bool required = false, const std::vector<std::string>& allowedValues = {});
    void addBoolRule(const std::string& key, bool required = false);
    
    // Validate a configuration entry
    std::vector<ConfigValidationError> validate(const ConfigEntry& entry) const;
    
    // Validate entire configuration
    std::vector<std::pair<ConfigValidationError, std::string>> validate(const Config& config) const;
    
    // Get all required keys
    std::vector<std::string> getRequiredKeys() const;
    
    // Get description for a key
    std::string getDescription(const std::string& key) const;
    
    // Get all key descriptions
    std::unordered_map<std::string, std::string> getAllDescriptions() const;
    
    // Check if a key is known (has a schema entry)
    bool isKnownKey(const std::string& key) const;
    
    // Get validation rules for a key
    std::vector<ConfigValidationRule> getRulesForKey(const std::string& key) const;
    
    // Set validation enabled/disabled
    void setValidationEnabled(bool enabled) { validationEnabled_ = enabled; }
    bool isValidationEnabled() const { return validationEnabled_; }
    
private:
    std::vector<ConfigValidationRule> rules_;
    std::unordered_map<std::string, std::string> descriptions_;
    bool validationEnabled_;
};

// Enhanced configuration with validation support
class ValidatedConfig : public Config {
public:
    ValidatedConfig();
    
    // Set the validation schema
    void setSchema(std::shared_ptr<ConfigSchema> schema) { schema_ = schema; }
    
    // Get the validation schema
    std::shared_ptr<ConfigSchema> getSchema() const { return schema_; }
    
    // Validate configuration before setting values
    bool validateBeforeSet(const std::string& key, const ConfigValue& value) const;
    
    // Get validation errors for a key
    std::vector<std::string> getValidationErrors(const std::string& key, const ConfigValue& value) const;
    
    // Clear all configuration and validation state
    void clearAndValidate();
    
    // Load with validation
    bool loadFromFile(const std::string& filepath) override;
    bool loadFromArgs(int argc, char** argv) override;
    
private:
    std::shared_ptr<ConfigSchema> schema_;
    std::vector<std::string> validationMessages_;
};

// Configuration validation utilities
namespace ConfigValidation {
    // Common validation functions
    bool validateInt(const ConfigValue& value);
    bool validateFloat(const ConfigValue& value);
    bool validateString(const ConfigValue& value);
    bool validateBool(const ConfigValue& value);
    bool validateVector(const ConfigValue& value);
    
    // Range validation
    bool validateRange(const ConfigValue& value, const std::pair<double, double>& range);
    bool validateIntRange(const ConfigValue& value, const std::pair<int64_t, int64_t>& range);
    
    // Allowed values validation
    bool validateAllowedValues(const ConfigValue& value, const std::vector<std::string>& allowedValues);
    
    // Type conversion validation
    template<typename T>
    bool convertAndValidate(const ConfigValue& from, T& to);
    
    // Error message generation
    std::string generateValidationErrorMessage(const std::string& key, 
                                               ConfigValidationError error,
                                               const ConfigValue& value);
}

} // namespace nlm