#pragma once

#include <string>
#include <vector>
#include <map>
#include <variant>
#include <memory>
#include <optional>
#include <nlohmann/json.hpp>
#include "Config.hpp"

namespace nlm {

/**
 * @enum ValidationErrorCategory
 * @brief Categories of validation errors for better error handling
 */
enum class ValidationErrorCategory {
    Type,          // Type validation errors
    Range,         // Range/value constraint errors
    Dependency,    // Dependency relationship errors
    Consistency,   // Consistency/conflict errors
    Schema,        // Schema validation errors
    Custom        // Custom validation errors
};

/**
 * @class ValidationException
 * @brief Exception thrown during configuration validation failures
 */
class ValidationException : public std::runtime_error {
public:
    ValidationException(const std::string& message, 
                       ValidationErrorCategory category = ValidationErrorCategory::Custom,
                       const std::string& key = "",
                       const std::string& context = "")
        : std::runtime_error(message),
          category_(category),
          key_(key),
          context_(context) {}

    ValidationErrorCategory getCategory() const { return category_; }
    const std::string& getKey() const { return key_; }
    const std::string& getContext() const { return context_; }

private:
    ValidationErrorCategory category_;
    std::string key_;
    std::string context_;
};

/**
 * @struct ValidationResult
 * @brief Structure holding the result of a validation operation
 */
struct ValidationResult {
    bool valid = true;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::vector<std::string> info;

    ValidationResult() = default;
    ValidationResult(bool isValid, const std::vector<std::string>& errs,
                     const std::vector<std::string>& warns = {},
                     const std::vector<std::string>& infos = {})
        : valid(isValid), errors(errs), warnings(warns), info(infos) {}

    bool isValid() const { return valid; }
    size_t errorCount() const { return errors.size(); }
    size_t warningCount() const { return warnings.size(); }
    size_t infoCount() const { return info.size(); }
};

/**
 * @class ConfigValidator
 * @brief Main validator class for configuration validation
 * 
 * Provides comprehensive validation including:
 * - Type validation (int, float, string, bool, arrays)
 * - Range validation (min/max values)
 * - Dependency validation (key dependencies)
 * - Consistency validation (conflicting values)
 */
class ConfigValidator {
public:
    ConfigValidator() = default;
    explicit ConfigValidator(const Config& config) : config_(config) {}

    void setConfig(const Config& config) { config_ = config; }
    const Config& getConfig() const { return config_; }

    // Validation methods
    ValidationResult validateAll() const;
    ValidationResult validateTypes() const;
    ValidationResult validateRanges() const;
    ValidationResult validateDependencies() const;
    ValidationResult validateConsistency() const;

    // Individual validation methods
    ValidationResult validateType(const std::string& key) const;
    ValidationResult validateRange(const std::string& key) const;
    ValidationResult validateDependency(const std::string& key) const;
    ValidationResult validateConsistency(const std::string& key) const;

    // Schema-based validation
    ValidationResult validateAgainstSchema(const nlohmann::json& schema) const;

    // Error collection
    const std::vector<ValidationException>& getErrors() const { return errors_; }
    const std::vector<std::string>& getWarnings() const { return warnings_; }
    const std::vector<std::string>& getInfo() const { return info_; }

    void clearErrors() { errors_.clear(); warnings_.clear(); info_.clear(); }

protected:
    void addError(const ValidationException& error) { errors_.push_back(error); }
    void addWarning(const std::string& warning) { warnings_.push_back(warning); }
    void addInfo(const std::string& info) { info_.push_back(info); }

private:
    Config config_;
    mutable std::vector<ValidationException> errors_;
    mutable std::vector<std::string> warnings_;
    mutable std::vector<std::string> info_;

    // Helper methods for validation logic
    template<typename T>
    bool validateValueType(const std::string& key, T expectedType) const;

    bool isValueWithinRange(const std::string& key, double value) const;
    bool checkDependency(const std::string& key) const;
    bool checkConsistency(const std::string& key) const;
    ValidationResult validateAgainstSchemaRecursive(const nlohmann::json& schema, 
                                                   const std::string& prefix) const;
};

/**
 * @class SchemaValidator
 * @brief JSON schema-like validator for configuration schemas
 * 
 * Supports JSON Schema-like definitions for:
 * - Property definitions with types
 * - Required fields
 * - Nested schemas
 * - Constraints (min, max, enum, patterns)
 */
class SchemaValidator {
public:
    SchemaValidator() = default;

    ValidationResult validate(const Config& config, const nlohmann::json& schema) const;
    ValidationResult validateSchema(const nlohmann::json& schema) const;

    // Schema validation rules
    bool validateRequiredFields(const nlohmann::json& schema, const Config& config) const;
    bool validateTypes(const nlohmann::json& schema, const Config& config) const;
    bool validateConstraints(const nlohmann::json& schema, const Config& config) const;
    bool validateNestedSchemas(const nlohmann::json& schema, const Config& config) const;

    // Constraint validation
    bool validateTypeConstraint(const std::string& key, const nlohmann::json& schema) const;
    bool validateRangeConstraint(const std::string& key, const nlohmann::json& schema) const;
    bool validateEnumConstraint(const std::string& key, const nlohmann::json& schema) const;
    bool validatePatternConstraint(const std::string& key, const nlohmann::json& schema) const;

    // Utility functions
    static std::string getSchemaTypeDescription(const nlohmann::json& schema);
    static bool isSchemaValid(const nlohmann::json& schema);
};

/**
 * @class ConstraintValidator
 * @brief Validator for type and range constraints
 * 
 * Handles validation of:
 * - Type constraints (int, float, string, bool, arrays)
 * - Range constraints (min, max values)
 * - Format constraints (patterns, formats)
 */
class ConstraintValidator {
public:
    ConstraintValidator() = default;

    // Type validation
    bool isValidType(const std::string& key, const ConfigValue& value, 
                    const std::string& expectedType) const;
    bool isValidArrayType(const std::string& key, const ConfigValue& value,
                         const std::string& expectedElementType) const;

    // Range validation
    bool isValidRange(const std::string& key, const ConfigValue& value,
                     double min, double max) const;

    // Format validation
    bool isValidPattern(const std::string& value, const std::string& pattern) const;
    bool isValidFormat(const std::string& key, const std::string& value,
                      const std::string& format) const;

    // Enum validation
    bool isValidEnum(const std::string& key, const ConfigValue& value,
                    const std::vector<std::string>& validValues) const;

    // Custom validation
    bool validateCustom(const std::string& key, const ConfigValue& value,
                       const std::function<bool(const ConfigValue&)>& validator) const;
};

/**
 * @class DependencyValidator
 * @brief Validator for key dependencies and relationships
 * 
 * Handles validation of:
 * - Required key dependencies
 * - Conditional dependencies
 * - Mutually exclusive keys
 * - Optional key requirements
 */
class DependencyValidator {
public:
    DependencyValidator() = default;

    // Required dependencies
    bool hasRequiredKeys(const Config& config, const std::vector<std::string>& requiredKeys) const;

    // Conditional dependencies
    bool checkConditionalDependency(const std::string& key, const Config& config,
                                   const std::string& conditionKey, 
                                   const std::string& conditionValue) const;

    // Mutual exclusivity
    bool isMutuallyExclusive(const std::string& key1, const std::string& key2,
                           const Config& config) const;

    // Conditional requirements
    bool checkConditionalRequirement(const std::string& requiredKey,
                                    const std::string& conditionKey,
                                    const std::string& conditionValue,
                                    const Config& config) const;
};

/**
 * @class ConsistencyValidator
 * @brief Validator for consistency and conflict checking
 * 
 * Handles validation of:
 * - Conflicting value combinations
 * - Logical consistency
 * - Business rule validation
 * - Value relationship validation
 */
class ConsistencyValidator {
public:
    ConsistencyValidator() = default;

    // Conflict validation
    bool checkConflictingValues(const std::string& key1, const std::string& key2,
                               const Config& config) const;

    // Logical consistency
    bool checkLogicalConsistency(const std::string& key, const ConfigValue& value,
                                const Config& config) const;

    // Business rule validation
    bool checkBusinessRule(const std::string& key, const ConfigValue& value,
                          const Config& config,
                          const std::function<bool(const Config&, const ConfigValue&)> rule) const;

    // Relationship validation
    bool checkRelationship(const std::string& key1, const std::string& key2,
                          const std::function<bool(const ConfigValue&, const ConfigValue&)> relationship,
                          const Config& config) const;
};

} // namespace nlm
