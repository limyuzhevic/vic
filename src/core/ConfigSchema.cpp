#include "ConfigSchema.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <limits>
#include <sstream>

namespace nlm {

// ConfigSchema implementation
void ConfigSchema::addConstraint(const TypeConstraint& constraint) {
    constraints[constraint.key] = constraint;
}

void ConfigSchema::addConstraint(const std::string& key, const std::string& typeName,
                                 const std::vector<ValidationRule>& rules,
                                 const std::any& defaultValue,
                                 bool required, const std::string& description,
                                 const std::string& unit, const std::vector<std::string>& tags) {
    TypeConstraint constraint;
    constraint.key = key;
    constraint.typeName = typeName;
    constraint.validationRules = rules;
    constraint.defaultValue = defaultValue;
    constraint.required = required;
    constraint.description = description;
    constraint.unit = unit;
    constraint.tags = tags;
    constraints[key] = constraint;
}

void ConfigSchema::addGroup(const GroupConstraint& group) {
    groups[group.groupName] = group;
}

void ConfigSchema::addInheritanceRule(const InheritanceRule& rule) {
    inheritanceRules.push_back(rule);
}

void ConfigSchema::setOptions(const SchemaOptions& opts) {
    options = opts;
}

bool ConfigSchema::validate(const std::string& key, const std::any& value) const {
    auto it = constraints.find(key);
    if (it == constraints.end()) {
        return options.allowUnknownKeys;
    }
    
    const auto& constraint = it->second;
    
    // Check type compatibility
    if (options.validateTypes && !isTypeCompatible(constraint.typeName, value)) {
        return false;
    }
    
    // Check required field
    if (constraint.required && value.type() == typeid(std::string)) {
        std::string strValue = std::any_cast<std::string>(value);
        if (strValue.empty()) {
            return false;
        }
    }
    
    // Run validation rules
    for (const auto& rule : constraint.validationRules) {
        if (options.validateRanges && (rule.type == ValidationRule::RuleType::Range || 
             rule.type == ValidationRule::RuleType::Length)) {
            if (!rule.validate(value)) {
                return false;
            }
        } else if (options.validatePatterns && (rule.type == ValidationRule::RuleType::Pattern ||
             rule.type == ValidationRule::RuleType::RegularExpression)) {
            if (!rule.validate(value)) {
                return false;
            }
        } else if (rule.type == ValidationRule::RuleType::Callback) {
            if (!rule.validate(value)) {
                return false;
            }
        } else if (rule.type == ValidationRule::RuleType::InList) {
            if (!rule.validate(value)) {
                return false;
            }
        }
    }
    
    return true;
}

bool ConfigSchema::validate(const std::string& key, const ConfigValue& value) const {
    return validate(key, std::any(value));
}

bool ConfigSchema::validate(const std::string& key, const std::string& valueStr) const {
    return validate(key, std::any(valueStr));
}

bool ConfigSchema::validateAll(const std::vector<ConfigEntry>& entries) const {
    for (const auto& entry : entries) {
        if (!validate(entry.key, entry.value)) {
            return false;
        }
    }
    return true;
}

std::vector<std::string> ConfigSchema::getRequiredKeys() const {
    std::vector<std::string> requiredKeys;
    for (const auto& [key, constraint] : constraints) {
        if (constraint.required) {
            requiredKeys.push_back(key);
        }
    }
    return requiredKeys;
}

std::vector<std::string> ConfigSchema::getOptionalKeys() const {
    std::vector<std::string> optionalKeys;
    for (const auto& [key, constraint] : constraints) {
        if (!constraint.required) {
            optionalKeys.push_back(key);
        }
    }
    return optionalKeys;
}

std::vector<std::string> ConfigSchema::getKeysOfType(const std::string& typeName) const {
    std::vector<std::string> keys;
    for (const auto& [key, constraint] : constraints) {
        if (constraint.typeName == typeName) {
            keys.push_back(key);
        }
    }
    return keys;
}

std::vector<std::string> ConfigSchema::getKeysByTag(const std::string& tag) const {
    std::vector<std::string> keys;
    for (const auto& [key, constraint] : constraints) {
        if (std::find(constraint.tags.begin(), constraint.tags.end(), tag) != constraint.tags.end()) {
            keys.push_back(key);
        }
    }
    return keys;
}

std::string ConfigSchema::getDescription(const std::string& key) const {
    auto it = constraints.find(key);
    if (it != constraints.end()) {
        return it->second.description;
    }
    return "";
}

std::string ConfigSchema::getUnit(const std::string& key) const {
    auto it = constraints.find(key);
    if (it != constraints.end()) {
        return it->second.unit;
    }
    return "";
}

std::vector<std::string> ConfigSchema::getTags(const std::string& key) const {
    auto it = constraints.find(key);
    if (it != constraints.end()) {
        return it->second.tags;
    }
    return {};
}

std::any ConfigSchema::getDefaultValue(const std::string& key) const {
    auto it = constraints.find(key);
    if (it != constraints.end()) {
        return it->second.defaultValue;
    }
    return std::any();
}

bool ConfigSchema::isRequired(const std::string& key) const {
    auto it = constraints.find(key);
    if (it != constraints.end()) {
        return it->second.required;
    }
    return false;
}

std::string ConfigSchema::generateTemplate() const {
    std::ostringstream oss;
    oss << "{\n";
    
    // Required fields
    oss << "  \"required\": [\n";
    for (size_t i = 0; i < getRequiredKeys().size(); ++i) {
        oss << "    \"" << getRequiredKeys()[i] << "\"";
        if (i < getRequiredKeys().size() - 1) oss << ",";
        oss << "\n";
    }
    oss << "  ],\n";
    
    // Optional fields
    oss << "  \"optional\": [\n";
    for (size_t i = 0; i < getOptionalKeys().size(); ++i) {
        oss << "    \"" << getOptionalKeys()[i] << "\"";
        if (i < getOptionalKeys().size() - 1) oss << ",";
        oss << "\n";
    }
    oss << "  ],\n";
    
    // Schema definitions
    oss << "  \"schema\": {\n";
    for (size_t i = 0; i < getRequiredKeys().size() + getOptionalKeys().size(); ++i) {
        std::string key = (i < getRequiredKeys().size()) ? getRequiredKeys()[i] : getOptionalKeys()[i - getRequiredKeys().size()];
        oss << "    \"" << key << "\": {\n";
        
        const auto& constraint = constraints.find(key)->second;
        oss << "      \"type\": \"" << constraint.typeName << "\"\n";
        if (!constraint.description.empty()) {
            oss << "      \"description\": \"" << constraint.description << "\"\n";
        }
        if (!constraint.unit.empty()) {
            oss << "      \"unit\": \"" << constraint.unit << "\"\n";
        }
        if (!constraint.tags.empty()) {
            oss << "      \"tags\": [\n";
            for (size_t j = 0; j < constraint.tags.size(); ++j) {
                oss << "        \"" << constraint.tags[j] << "\"";
                if (j < constraint.tags.size() - 1) oss << ",";
                oss << "\n";
            }
            oss << "      ]\n";
        }
        
        oss << "    }";
        if (i < getRequiredKeys().size() + getOptionalKeys().size() - 1) oss << ",";
        oss << "\n";
    }
    oss << "  }\n";
    oss << "}\n";
    
    return oss.str();
}

bool ConfigSchema::isValidTemplate(const std::string& templateContent) const {
    // Simple validation - could be enhanced with JSON schema validation
    return templateContent.find("type") != std::string::npos &&
           templateContent.find("schema") != std::string::npos;
}

TypeConstraint* ConfigSchema::getConstraint(const std::string& key) {
    auto it = constraints.find(key);
    if (it != constraints.end()) {
        return &it->second;
    }
    return nullptr;
}

const TypeConstraint* ConfigSchema::getConstraint(const std::string& key) const {
    auto it = constraints.find(key);
    if (it != constraints.end()) {
        return &it->second;
    }
    return nullptr;
}

void ConfigSchema::removeConstraint(const std::string& key) {
    constraints.erase(key);
}

bool ConfigSchema::hasConstraint(const std::string& key) const {
    return constraints.find(key) != constraints.end();
}

GroupConstraint* ConfigSchema::getGroup(const std::string& groupName) {
    auto it = groups.find(groupName);
    if (it != groups.end()) {
        return &it->second;
    }
    return nullptr;
}

const GroupConstraint* ConfigSchema::getGroup(const std::string& groupName) const {
    auto it = groups.find(groupName);
    if (it != groups.end()) {
        return &it->second;
    }
    return nullptr;
}

void ConfigSchema::removeGroup(const std::string& groupName) {
    groups.erase(groupName);
}

bool ConfigSchema::hasGroup(const std::string& groupName) const {
    return groups.find(groupName) != groups.end();
}

void ConfigSchema::clearInheritanceRules() {
    inheritanceRules.clear();
}

 size_t ConfigSchema::getInheritanceRuleCount() const {
    return inheritanceRules.size();
}

bool ConfigSchema::equals(const ConfigSchema& other) const {
    return constraints == other.constraints && groups == other.groups &&
           inheritanceRules == other.inheritanceRules;
}

std::string ConfigSchema::toJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    
    // Constraints
    oss << "  \"constraints\": [\n";
    for (size_t i = 0; i < constraints.size(); ++i) {
        const auto& [key, constraint] = *std::next(constraints.begin(), i);
        oss << "    {\n";
        oss << "      \"key\": \"" << key << "\"\n";
        oss << "      \"typeName\": \"" << constraint.typeName << "\"\n";
        oss << "      \"required\": " << (constraint.required ? "true" : "false") << "\n";
        if (!constraint.description.empty()) {
            oss << "      \"description\": \"" << constraint.description << "\"\n";
        }
        if (!constraint.validationRules.empty()) {
            oss << "      \"validationRules\": [\n";
            for (size_t j = 0; j < constraint.validationRules.size(); ++j) {
                const auto& rule = constraint.validationRules[j];
                oss << "        {\n";
                oss << "          \"type\": \"" << static_cast<int>(rule.type) << "\"\n";
                oss << "          \"parameter1\": \"" << rule.parameter1 << "\"\n";
                if (!rule.parameter2.empty()) {
                    oss << "          \"parameter2\": \"" << rule.parameter2 << "\"\n";
                }
                oss << "        }";
                if (j < constraint.validationRules.size() - 1) oss << ",";
                oss << "\n";
            }
            oss << "      ]\n";
        }
        oss << "    }";
        if (i < constraints.size() - 1) oss << ",";
        oss << "\n";
    }
    oss << "  ],\n";
    
    // Groups
    if (!groups.empty()) {
        oss << "  \"groups\": [\n";
        for (size_t i = 0; i < groups.size(); ++i) {
            const auto& [groupName, group] = *std::next(groups.begin(), i);
            oss << "    {\n";
            oss << "      \"name\": \"" << group.groupName << "\"\n";
            oss << "      \"description\": \"" << group.description << "\"\n";
            oss << "      \"priority\": " << group.priority << "\n";
            oss << "      \"isGlobal\": " << (group.isGlobal ? "true" : "false") << "\n";
            oss << "    }";
            if (i < groups.size() - 1) oss << ",";
            oss << "\n";
        }
        oss << "  ]\n";
    }
    
    oss << "}\n";
    return oss.str();
}

std::string ConfigSchema::toYAML() const {
    std::ostringstream oss;
    oss << "schema:\n";
    
    // Constraints
    oss << "  constraints:\n";
    for (size_t i = 0; i < constraints.size(); ++i) {
        const auto& [key, constraint] = *std::next(constraints.begin(), i);
        oss << "    " << key << ":\n";
        oss << "      type: " << constraint.typeName << "\n";
        oss << "      required: " << (constraint.required ? "true" : "false") << "\n";
        if (!constraint.description.empty()) {
            oss << "      description: " << constraint.description << "\n";
        }
    }
    
    // Groups
    if (!groups.empty()) {
        oss << "  groups:\n";
        for (const auto& [groupName, group] : groups) {
            oss << "    " << groupName << ":\n";
            oss << "      description: " << group.description << "\n";
            oss << "      priority: " << group.priority << "\n";
            oss << "      isGlobal: " << (group.isGlobal ? "true" : "false") << "\n";
        }
    }
    
    return oss.str();
}

// Static helper functions
std::string ConfigSchema::getTypeName(const std::any& value) {
    if (value.type() == typeid(int)) return "int";
    if (value.type() == typeid(int64_t)) return "int64_t";
    if (value.type() == typeid(double)) return "double";
    if (value.type() == typeid(bool)) return "bool";
    if (value.type() == typeid(std::string)) return "string";
    if (value.type() == typeid(std::vector<int>)) return "vector<int>";
    if (value.type() == typeid(std::vector<int64_t>)) return "vector<int64_t>";
    if (value.type() == typeid(std::vector<double>)) return "vector<double>";
    if (value.type() == typeid(std::vector<bool>)) return "vector<bool>";
    if (value.type() == typeid(std::vector<std::string>)) return "vector<string>";
    if (value.type() == typeid(std::vector<std::any>)) return "vector<any>";
    return "unknown";
}

bool ConfigSchema::isTypeCompatible(const std::string& expectedType, const std::any& value) {
    std::string actualType = getTypeName(value);
    return actualType == expectedType || expectedType == "any" || expectedType == "auto";
}

std::regex ConfigSchema::compilePattern(const std::string& pattern) {
    try {
        return std::regex(pattern);
    } catch (const std::regex_error&) {
        return std::regex(".*"); // Match anything if invalid
    }
}

// Validation rule factory methods
ValidationRule ConfigSchema::rangeRule(double min, double max) {
    std::ostringstream oss;
    oss << min << "," << max;
    return ValidationRule(ValidationRule::RuleType::Range, oss.str());
}

ValidationRule ConfigSchema::patternRule(const std::string& regexPattern) {
    return ValidationRule(ValidationRule::RuleType::Pattern, regexPattern);
}

ValidationRule ConfigSchema::lengthRule(size_t length) {
    std::ostringstream oss;
    oss << length;
    return ValidationRule(ValidationRule::RuleType::Length, oss.str());
}

ValidationRule ConfigSchema::inListRule(const std::vector<std::string>& values) {
    return ValidationRule(ValidationRule::RuleType::InList, "", "", values);
}

ValidationRule ConfigSchema::callbackRule(const std::function<bool(const std::any&)>& validator) {
    return ValidationRule(ValidationRule::RuleType::Callback, "", "", {}, validator);
}

// Type constraint factory methods
void ConfigSchema::addIntConstraints(const std::string& key, int min,
                                     int max, bool required,
                                     const std::string& description) {
    std::ostringstream oss;
    oss << min << "," << max;
    ValidationRule rangeRule = ValidationRule(ValidationRule::RuleType::Range, oss.str());
    addConstraint(key, "int", {rangeRule}, std::any(min), required, description);
}

void ConfigSchema::addDoubleConstraints(const std::string& key, double min,
                                        double max, bool required,
                                        const std::string& description) {
    std::ostringstream oss;
    oss << min << "," << max;
    ValidationRule rangeRule = ValidationRule(ValidationRule::RuleType::Range, oss.str());
    addConstraint(key, "double", {rangeRule}, std::any(min), required, description);
}

void ConfigSchema::addStringConstraints(const std::string& key, size_t minLength,
                                        size_t maxLength, const std::string& pattern,
                                        bool required, const std::string& description) {
    std::vector<ValidationRule> rules;
    if (minLength > 0 || maxLength < SIZE_MAX) {
        std::ostringstream oss;
        oss << minLength << "," << maxLength;
        rules.push_back(ValidationRule(ValidationRule::RuleType::Length, oss.str()));
    }
    if (!pattern.empty()) {
        rules.push_back(patternRule(pattern));
    }
    addConstraint(key, "string", rules, std::any(std::string()), required, description);
}

void ConfigSchema::addBoolConstraints(const std::string& key, bool required,
                                      const std::string& description) {
    addConstraint(key, "bool", {}, std::any(false), required, description);
}

// Initialize builtin types
void ConfigSchema::initializeBuiltinTypes() {
    // Initialize common type constraints
    addIntConstraints("int", INT32_MIN, INT32_MAX, false);
    addStringConstraints("string", 0, SIZE_MAX, "", false);
    addBoolConstraints("bool", false);
}

} // namespace nlm