#ifndef NLM_CONFIG_VALIDATION_HPP
#define NLM_CONFIG_VALIDATION_HPP

#include <string>
#include <vector>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <functional>
#include <any>
#include <regex>

namespace nlm {
namespace config {

// Validation result
struct ValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::map<std::string, std::any> metadata;
    
    ValidationResult() : valid(true) {}
    
    ValidationResult& addError(const std::string& error) {
        errors.push_back(error);
        valid = false;
        return *this;
    }
    
    ValidationResult& addWarning(const std::string& warning) {
        warnings.push_back(warning);
        return *this;
    }
    
    ValidationResult& setValid(bool value) {
        valid = value;
        return *this;
    }
};

// Validation rule
class ValidationRule {
public:
    virtual ~ValidationRule() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual ValidationResult validate(const ConfigManager& config) const = 0;
    virtual bool isCritical() const { return true; }
};

// Required field validation rule
class RequiredFieldsRule : public ValidationRule {
public:
    RequiredFieldsRule(const std::vector<std::string>& requiredFields) 
        : requiredFields_(requiredFields) {}
    
    std::string getName() const override { return "RequiredFieldsRule"; }
    std::string getDescription() const override { 
        return "Validates that all required configuration fields are present"; 
    }
    ValidationResult validate(const ConfigManager& config) const override;
    
private:
    std::vector<std::string> requiredFields_;
};

// Field type validation rule
class FieldTypeRule : public ValidationRule {
public:
    struct TypeConstraint {
        std::string fieldName;
        std::string expectedType; // "int", "double", "string", "bool", "vector"
        std::vector<std::string> allowedValues;
    };
    
    FieldTypeRule(const std::vector<TypeConstraint>& constraints) 
        : constraints_(constraints) {}
    
    std::string getName() const override { return "FieldTypeRule"; }
    std::string getDescription() const override { 
        return "Validates field types and allowed values"; 
    }
    ValidationResult validate(const ConfigManager& config) const override;
    
private:
    std::vector<TypeConstraint> constraints_;
};

// Range validation rule
class RangeRule : public ValidationRule {
public:
    struct RangeConstraint {
        std::string fieldName;
        double minValue;
        double maxValue;
        bool inclusiveMin;
        bool inclusiveMax;
    };
    
    RangeRule(const std::vector<RangeConstraint>& constraints) 
        : constraints_(constraints) {}
    
    std::string getName() const override { return "RangeRule"; }
    std::string getDescription() const override { 
        return "Validates that numeric fields are within specified ranges"; 
    }
    ValidationResult validate(const ConfigManager& config) const override;
    
private:
    std::vector<RangeConstraint> constraints_;
};

// Dependency validation rule
class DependencyRule : public ValidationRule {
public:
    struct DependencyConstraint {
        std::string fieldName;
        std::string requiredIfField;
        std::vector<std::string> requiredIfValues;
        std::string errorMessage;
    };
    
    DependencyRule(const std::vector<DependencyConstraint>& constraints) 
        : constraints_(constraints) {}
    
    std::string getName() const override { return "DependencyRule"; }
    std::string getDescription() const override { 
        return "Validates field dependencies and conditional requirements"; 
    }
    ValidationResult validate(const ConfigManager& config) const override;
    
private:
    std::vector<DependencyConstraint> constraints_;
};

// Schema validation rule
class SchemaRule : public ValidationRule {
public:
    SchemaRule(const ConfigSchema& schema) : schema_(schema) {}
    
    std::string getName() const override { return "SchemaRule"; }
    std::string getDescription() const override { 
        return "Validates configuration against a predefined schema"; 
    }
    ValidationResult validate(const ConfigManager& config) const override;
    
private:
    ConfigSchema schema_;
};

// Configuration validator
class ConfigValidator {
public:
    ConfigValidator() = default;
    ~ConfigValidator() = default;
    
    // Validation rule management
    void addRule(std::unique_ptr<ValidationRule> rule);
    void removeRule(const std::string& ruleName);
    bool hasRule(const std::string& ruleName) const;
    
    // Configuration validation
    ValidationResult validate(const ConfigManager& config) const;
    ValidationResult validate(const std::string& configFile) const;
    ValidationResult validate(const std::string& configString, const std::string& format = "auto") const;
    
    // Rule-specific validation
    ValidationResult validateWithRule(const std::string& ruleName, const ConfigManager& config) const;
    
    // Validation statistics
    std::vector<std::string> getRuleNames() const;
    size_t getRuleCount() const { return rules_.size(); }
    
    // Validation report generation
    std::string generateValidationReport(const ValidationResult& result) const;
    std::string generateValidationSummary(const ValidationResult& result) const;
    std::string generateValidationDetailedReport(const ValidationResult& result) const;
    
private:
    std::map<std::string, std::unique_ptr<ValidationRule>> rules_;
};

// Configuration schema validator
class SchemaValidator {
public:
    SchemaValidator() = default;
    ~SchemaValidator() = default;
    
    // Schema management
    void addSchema(const ConfigSchema& schema);
    bool removeSchema(const std::string& schemaName);
    ConfigSchema* getSchema(const std::string& schemaName);
    const ConfigSchema* getSchema(const std::string& schemaName) const;
    
    // Schema validation
    ValidationResult validate(const ConfigManager& config, const std::string& schemaName) const;
    ValidationResult validate(const std::string& configFile, const std::string& schemaName) const;
    
    // Schema registration
    void registerDefaultSchemas();
    bool hasSchema(const std::string& schemaName) const;
    std::vector<std::string> getSchemaNames() const;
    
    // Schema analysis
    std::string analyzeSchemaCompatibility(const ConfigManager& config) const;
    std::map<std::string, double> getSchemaComplianceScore(const ConfigManager& config, const std::string& schemaName) const;
    
private:
    std::map<std::string, ConfigSchema> schemas_;
};

// Configuration consistency validator
class ConfigConsistencyValidator {
public:
    ConfigConsistencyValidator() = default;
    ~ConfigConsistencyValidator() = default;
    
    // Consistency validation
    ValidationResult validate(const ConfigManager& config) const;
    
    // Cross-field validation
    ValidationResult validateCrossFieldDependencies(const ConfigManager& config) const;
    ValidationResult validateValueConsistency(const ConfigManager& config) const;
    ValidationResult validateConfigurationIntegrity(const ConfigManager& config) const;
    
    // Specific consistency checks
    ValidationResult validateNoConflictingSettings(const ConfigManager& config) const;
    ValidationResult validateResourceConstraints(const ConfigManager& config) const;
    ValidationResult validateSimulationParameters(const ConfigManager& config) const;
    
private:
    struct ConsistencyCheck {
        std::string name;
        std::function<ValidationResult(const ConfigManager&)> checkFunction;
        bool isCritical;
        
        ConsistencyCheck(const std::string& name, 
                        std::function<ValidationResult(const ConfigManager&)> func,
                        bool critical = true)
            : name(name), checkFunction(std::move(func)), isCritical(critical) {}
    };
    
    std::vector<ConsistencyCheck> consistencyChecks_;
};

// Configuration audit logger
class ConfigAuditLogger {
public:
    ConfigAuditLogger() = default;
    ~ConfigAuditLogger() = default;
    
    // Audit logging
    void logValidationStart(const std::string& configSource, const std::string& validatorName);
    void logValidationResult(const ValidationResult& result);
    void logValidationError(const std::string& error);
    void logValidationWarning(const std::string& warning);
    
    // Audit log management
    void clearLog();
    size_t getLogSize() const { return logEntries_.size(); }
    std::vector<std::string> getLogEntries() const { return logEntries_; }
    
    // Audit report generation
    std::string generateAuditReport() const;
    std::string generateAuditLog(const std::string& configSource) const;
    
private:
    struct AuditEntry {
        std::chrono::system_clock::time_point timestamp;
        std::string eventType;
        std::string configSource;
        std::string validatorName;
        std::string message;
        std::string level; // "INFO", "WARNING", "ERROR"
    };
    
    std::vector<AuditEntry> logEntries_;
    std::mutex logMutex_;
    
    void addLogEntry(const std::string& eventType, const std::string& configSource,
                    const std::string& validatorName, const std::string& message,
                    const std::string& level = "INFO");
};

// Validation utilities
namespace utils {
    // Configuration file validation
    ValidationResult validateConfigFile(const std::string& filepath);
    ValidationResult validateConfigString(const std::string& content, const std::string& format = "auto");
    
    // Configuration schema utilities
    ConfigSchema loadSchemaFromFile(const std::string& filepath);
    bool saveSchemaToFile(const std::string& filepath, const ConfigSchema& schema);
    std::string schemaToString(const ConfigSchema& schema);
    ConfigSchema stringToSchema(const std::string& schemaString);
    
    // Validation result utilities
    std::string formatValidationResult(const ValidationResult& result);
    std::string formatValidationErrors(const ValidationResult& result);
    std::string formatValidationWarnings(const ValidationResult& result);
    
    // Configuration validation utilities
    bool isValidConfigName(const std::string& name);
    bool isValidConfigValue(const std::string& type, const std::any& value);
    std::string getConfigTypeName(const ConfigValue& value);
    
    // Error message generation
    std::string generateErrorMessage(const std::string& fieldName, const std::string& expected, const std::string& actual);
    std::string generateDependencyError(const std::string& fieldName, const std::string& dependency, const std::string& condition);
    std::string generateRangeError(const std::string& fieldName, double value, double min, double max);
    
    // Configuration comparison
    ValidationResult compareConfigs(const ConfigManager& config1, const ConfigManager& config2);
    std::map<std::string, std::pair<std::any, std::any>> findConfigDifferences(const ConfigManager& config1, const ConfigManager& config2);
    
    // Validation statistics
    std::map<std::string, size_t> countValidationErrors(const ValidationResult& result);
    std::map<std::string, size_t> countValidationWarnings(const ValidationResult& result);
}

} // namespace config
} // namespace nlm

#endif // NLM_CONFIG_VALIDATION_HPP