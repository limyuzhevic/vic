#pragma once

/**
 * Configuration Schema
 * 
 * Schema definition and validation for configuration values, including type checking,
 * constraints, ranges, and validation rules for the NLM brain simulation project.
 * 
 * Features:
 * - Schema definition for configuration types
 * - Validation constraints and rules
 * - Type validation
 * - Range validation
 * - Pattern validation
 * - Enum validation
 * - Required field validation
 * - Schema inheritance
 * - Schema merging
 */

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <regex>
#include <algorithm>
#include <set>

namespace nlm {
namespace core {

class ConfigurationValidator;

/**
 * Schema field definition
 */
struct SchemaField {
    std::string name;
    ConfigDataType dataType;
    std::string description;
    bool required;
    std::string defaultValue;
    
    // Validation constraints
    std::string validationRegex;
    std::vector<std::string> allowedValues;
    double minValue;
    double maxValue;
    int minItems;
    int maxItems;
    
    SchemaField(const std::string& n, ConfigDataType type)
        : name(n), dataType(type), required(false), minValue(-std::numeric_limits<double>::max()),
          maxValue(std::numeric_limits<double>::max()), minItems(0), maxItems(std::numeric_limits<int>::max()) {}
};

/**
 * Schema rule
 */
enum class SchemaRuleType {
    TYPE_CHECK,
    RANGE_CHECK,
    PATTERN_CHECK,
    ENUM_CHECK,
    REQUIRED_CHECK,
    CUSTOM_CHECK
};

struct SchemaRule {
    SchemaRuleType type;
    std::string fieldName;
    std::string errorMessage;
    std::function<bool(const std::string&, const SchemaField&)> validationFunction;
    
    SchemaRule(SchemaRuleType t, const std::string& field, const std::string& error)
        : type(t), fieldName(field), errorMessage(error) {}
};

/**
 * Configuration schema
 */
class ConfigurationSchema {
public:
    ConfigurationSchema();
    ~ConfigurationSchema();
    
    // Disable copying, enable moving
    ConfigurationSchema(const ConfigurationSchema&) = delete;
    ConfigurationSchema& operator=(const ConfigurationSchema&) = delete;
    ConfigurationSchema(ConfigurationSchema&&) noexcept = default;
    ConfigurationSchema& operator=(ConfigurationSchema&&) noexcept = default;
    
    // Schema definition
    bool addField(const SchemaField& field);
    bool removeField(const std::string& fieldName);
    SchemaField* getField(const std::string& fieldName);
    const SchemaField* getField(const std::string& fieldName) const;
    std::vector<std::string> getFieldNames() const;
    
    // Schema rules
    bool addRule(const SchemaRule& rule);
    bool removeRule(const std::string& fieldName, SchemaRuleType ruleType);
    std::vector<SchemaRule> getRulesForField(const std::string& fieldName) const;
    
    // Schema validation
    bool validate(const std::map<std::string, std::string>& configData) const;
    std::vector<ConfigError> getValidationErrors(const std::map<std::string, std::string>& configData) const;
    
    // Schema merging
    bool merge(const ConfigurationSchema& otherSchema);
    bool inheritsFrom(const ConfigurationSchema& parentSchema) const;
    
    // Schema serialization
    bool saveToFile(const std::string& filepath) const;
    bool loadFromFile(const std::string& filepath);
    std::string toString() const;
    
    // Schema statistics
    size_t getFieldCount() const { return fields_.size(); }
    size_t getRuleCount() const { return rules_.size(); }
    bool isValid() const { return validation_enabled_ && !fields_.empty(); }
    
    // Validation options
    void enableValidation(bool enable) { validation_enabled_ = enable; }
    bool isValidationEnabled() const { return validation_enabled_; }
    
    // Field validation helpers
    static bool validateType(const std::string& value, ConfigDataType type);
    static bool validateRange(const std::string& value, double min, double max);
    static bool validatePattern(const std::string& value, const std::string& pattern);
    static bool validateEnum(const std::string& value, const std::vector<std::string>& allowedValues);
    static bool validateRequired(const std::string& value, bool required);
    static bool validateItems(const std::string& value, int minItems, int maxItems);
    
    // Error formatting
    static std::string formatErrorMessage(const ConfigError& error);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    mutable std::mutex mutex_;
    bool validation_enabled_ = true;
    
    std::vector<SchemaField> fields_;
    std::vector<SchemaRule> rules_;
    
    ConfigDataType stringToDataType(const std::string& typeName) const;
    std::string dataTypeToString(ConfigDataType type) const;
    
    friend class ConfigurationValidator;
};

} // namespace core
} // namespace nlm
