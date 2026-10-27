// Enhanced Configuration Validator for NLM Phase 2
// Provides comprehensive configuration validation and schema checking

#pragma once

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <algorithm>
#include <sstream>

// Forward declarations
namespace nlm {
    class Config;
}

// Configuration validation rules
struct ValidationRule {
    std::string key;                    // Configuration key to validate
    std::string type;                   // Expected type (string, int, double, bool, vector<int>, etc.)
    bool required;                      // Whether the key is required
    std::string defaultValue;           // Default value if not provided
    std::vector<std::string> validValues; // Valid values for enum-like types
    std::string minValue;               // Minimum value (numeric types)
    std::string maxValue;               // Maximum value (numeric types)
    std::string description;            // Human-readable description
    bool validated;                     // Whether rule has been validated
    std::string errorMessage;           // Validation error message
    
    ValidationRule(const std::string& k, const std::string& t, bool req = false,
                   const std::string& def = "", const std::string& desc = "")
        : key(k), type(t), required(req), defaultValue(def), description(desc),
          validated(false), errorMessage("") {}
};

// Configuration schema definition
struct ConfigSchema {
    std::string schemaName;                    // Schema name
    std::vector<ValidationRule> validationRules; // Validation rules
    std::map<std::string, std::string> groupDescriptions; // Group descriptions
    bool strictMode;                            // Whether to fail on unknown keys
    std::string version;                        // Schema version
    
    ConfigSchema(const std::string& name = "Default", bool strict = true,
                 const std::string& ver = "1.0")
        : schemaName(name), strictMode(strict), version(ver) {}
    
    void addRule(const ValidationRule& rule) {
        validationRules.push_back(rule);
    }
    
    void addGroup(const std::string& groupName, const std::string& description) {
        groupDescriptions[groupName] = description;
    }
};

class ConfigValidator {
public:
    ConfigValidator();
    ~ConfigValidator() = default;
    
    // Load default configuration schema
    void loadDefaultSchema();
    
    // Load custom schema from file
    bool loadSchemaFromFile(const std::string& filepath);
    
    // Validate configuration against schema
    bool validate(const nlm::Config& config, std::string& errorMessage,
                  const ConfigSchema& schema = getDefaultSchema()) const;
    
    // Validate configuration with specific schema
    bool validateWithSchema(const nlm::Config& config, const ConfigSchema& schema,
                           std::string& errorMessage) const;
    
    // Get default schema
    static ConfigSchema getDefaultSchema();
    
    // Get validation help text
    std::string getValidationHelp(const ConfigSchema& schema) const;
    
    // Create custom schema for specific use cases
    static ConfigSchema createTestSchema();
    static ConfigSchema createProductionSchema();
    static ConfigSchema createMinimalSchema();
    
    // Check if configuration is valid for test execution
    bool isValidForTests(const nlm::Config& config) const;
    
    // Get configuration statistics
    std::string getConfigStats(const nlm::Config& config) const;
    
    // Suggest corrections for validation errors
    std::vector<std::string> suggestCorrections(const std::string& errorMessage) const;
    
private:
    ConfigSchema defaultSchema_;
    std::map<std::string, ConfigSchema> customSchemas_;
    
    // Validation helper functions
    bool validateString(const std::string& key, const nlm::Config& config,
                       const ValidationRule& rule, std::string& errorMessage) const;
    
    bool validateInt(const std::string& key, const nlm::Config& config,
                    const ValidationRule& rule, std::string& errorMessage) const;
    
    bool validateDouble(const std::string& key, const nlm::Config& config,
                       const ValidationRule& rule, std::string& errorMessage) const;
    
    bool validateBool(const std::string& key, const nlm::Config& config,
                     const ValidationRule& rule, std::string& errorMessage) const;
    
    bool validateVector(const std::string& key, const nlm::Config& config,
                       const ValidationRule& rule, std::string& errorMessage) const;
    
    bool validateEnum(const std::string& key, const nlm::Config& config,
                     const ValidationRule& rule, std::string& errorMessage) const;
    
    std::string getTypeDescription(const std::string& type) const;
    std::string formatValidationError(const std::string& key, const std::string& expectedType,
                                     const std::string& actualType) const;
    bool stringToBool(const std::string& str) const;
    int stringToInt(const std::string& str) const;
    double stringToDouble(const std::string& str) const;
    bool isNumericType(const std::string& type) const;
    bool isStringType(const std::string& type) const;
    bool isBoolType(const std::string& type) const;
    bool isVectorType(const std::string& type) const;
    std::string trim(const std::string& str) const;
    bool stringToLower(const std::string& str) const;
    std::string capitalize(const std::string& str) const;
    std::string getDefaultValueForType(const std::string& type) const;
};
