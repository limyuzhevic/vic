#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "Config.hpp"
#include "ConfigValidator.hpp"

namespace nlm {

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
    bool validateFormatConstraint(const std::string& key, const nlohmann::json& schema) const;

    // Utility functions
    static std::string getSchemaTypeDescription(const nlohmann::json& schema);
    static bool isSchemaValid(const nlohmann::json& schema);
    static nlohmann::json createDefaultSchema();

private:
    ConstraintValidator constraintValidator_;
    DependencyValidator dependencyValidator_;
    ConsistencyValidator consistencyValidator_;
};

} // namespace nlm
