#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <optional>
#include <variant>
#include <functional>

namespace nlm {

// Configuration category enumeration
enum class ConfigCategory {
    Neuromodulation,
    Simulation,
    Development,
    Memory,
    Environment,
    Logging,
    Visualization,
    Reward,
    Unknown
};

// Configuration data types for schema validation
enum class ConfigDataType {
    Integer,
    Float,
    Boolean,
    String,
    IntegerVector,
    FloatVector,
    StringVector,
    Any
};

// Validation result enumeration
enum class ValidationResult {
    Valid,
    Invalid,
    PartiallyValid
};

// Validation mode enumeration
enum class ValidationMode {
    Strict,
    Lenient,
    Permissive
};

// Schema validation rule for a configuration value
struct ValidationRule {
    std::string name;
    ConfigDataType dataType;
    std::string description;
    bool required;
    std::optional<double> minValue;
    std::optional<double> maxValue;
    std::optional<std::string> pattern; // For string validation
    std::optional<std::function<bool(const ConfigValue&)>> customValidator;
    ValidationMode defaultMode;
    std::vector<std::string> relatedFields; // For consistency validation
    
    ValidationRule(const std::string& n, ConfigDataType dt, const std::string& desc = "",
                   bool req = false, ValidationMode mode = ValidationMode::Strict)
        : name(n), dataType(dt), description(desc), required(req), 
          defaultMode(mode) {}
};

// Configuration category schema
struct ConfigSchema {
    std::string name;
    ConfigCategory category;
    std::unordered_map<std::string, ValidationRule> rules;
    
    ConfigSchema(const std::string& n, ConfigCategory cat)
        : name(n), category(cat) {}
    
    void addRule(const ValidationRule& rule) {
        rules[rule.name] = rule;
    }
};

// Configuration validation error
struct ValidationError {
    std::string field;
    std::string category;
    std::string message;
    ValidationResult severity;
    std::optional<std::string> fileLocation; // For file-based configs
    int lineNumber; // For file-based configs
    
    ValidationError(const std::string& f, const std::string& cat, const std::string& msg,
                   ValidationResult sev = ValidationResult::Invalid, 
                   const std::optional<std::string>& loc = std::nullopt, int line = 0)
        : field(f), category(cat), message(msg), severity(sev), 
          fileLocation(loc), lineNumber(line) {}
};

// Configuration schema manager
class ConfigSchemaManager {
public:
    static ConfigSchemaManager& getInstance() {
        static ConfigSchemaManager instance;
        return instance;
    }
    
    void registerSchema(const ConfigSchema& schema) {
        schemas[schema.category] = schema;
    }
    
    const ConfigSchema& getSchema(ConfigCategory category) const {
        auto it = schemas.find(category);
        if (it != schemas.end()) {
            return it->second;
        }
        return defaultSchema;
    }
    
    const std::unordered_map<ConfigCategory, ConfigSchema>& getAllSchemas() const {
        return schemas;
    }
    
    void clear() {
        schemas.clear();
    }
    
    bool hasSchema(ConfigCategory category) const {
        return schemas.find(category) != schemas.end();
    }
    
private:
    ConfigSchemaManager() {
        initializeDefaultSchemas();
    }
    
    void initializeDefaultSchemas();
    
    std::unordered_map<ConfigCategory, ConfigSchema> schemas;
    ConfigSchema defaultSchema{"Default", ConfigCategory::Unknown};
};

// Configuration validator
class ConfigValidator {
public:
    ConfigValidator() : currentMode(ValidationMode::Strict) {}
    
    void setMode(ValidationMode mode) {
        currentMode = mode;
    }
    
    ValidationMode getMode() const {
        return currentMode;
    }
    
    std::vector<ValidationError> validate(const ConfigValue& value, const ValidationRule& rule, 
                                        ConfigCategory category, const std::string& field) const;
    
    ValidationResult validateValue(const std::string& fieldName, const ConfigValue& value,
                                 ConfigCategory category, std::vector<ValidationError>& errors) const;
    
    std::vector<ValidationError> validateAll(const std::unordered_map<std::string, ConfigValue>& configData,
                                           ConfigCategory category) const;
    
    static std::string dataTypeToString(ConfigDataType type);
    
private:
    ValidationMode currentMode;
    
    bool validateType(const ConfigValue& value, ConfigDataType expectedType) const;
    bool validateRange(const ConfigValue& value, const ValidationRule& rule) const;
    bool validateStringPattern(const ConfigValue& value, const ValidationRule& rule) const;
    bool validateCustom(const ConfigValue& value, const ValidationRule& rule) const;
    bool validateConsistency(const std::string& fieldName, const ConfigValue& value,
                           const std::unordered_map<std::string, ConfigValue>& allConfig,
                           const std::vector<std::string>& relatedFields) const;
    
    ValidationResult getSeverityForMode(bool isValid, ValidationMode mode) const;
    
    std::string formatValue(const ConfigValue& value) const;
};

// Configuration category helper functions
namespace ConfigCategories {
    ConfigCategory stringToCategory(const std::string& categoryStr);
    std::string categoryToString(ConfigCategory category);
    std::vector<std::string> getCategoryFields(ConfigCategory category);
}

} // namespace nlm