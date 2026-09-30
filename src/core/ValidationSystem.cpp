#include "ValidationSystem.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ranges>
#include <chrono>

namespace nlm {

namespace {

// ValidationSchema implementation
struct ValidationSchema::Impl {
    std::unordered_map<std::string, ParameterInfo> parameters;
    std::unordered_map<std::string, ParameterInfo::RangeConstraint> rangeConstraints;
    std::unordered_map<std::string, ParameterInfo::ChoiceConstraint> choiceConstraints;
    std::unordered_map<std::string, ParameterInfo::DependencyConstraint> dependencyConstraints;
    std::unordered_map<std::string, ParameterInfo::CustomConstraint> customConstraints;
    
    // Statistics
    size_t totalValidations = 0;
    size_t successfulValidations = 0;
    size_t failedValidations = 0;
    std::unordered_map<ValidationErrorCode, size_t> errorCounts;
};

void ValidationSchema::addParameter(const std::string& name, const ParameterInfo& info) {
    pImpl->parameters[name] = info;
}

template<typename T>
void ValidationSchema::addParameter(const std::string& name, const std::string& description, 
                                    const T& defaultValue, bool required) {
    ParameterInfo info(name, description, ParameterValue(defaultValue), required);
    addParameter(name, info);
}

void ValidationSchema::addRangeConstraint(const std::string& paramName, double min, double max) {
    ParameterInfo::RangeConstraint constraint{min, max};
    pImpl->rangeConstraints[paramName] = constraint;
    
    auto it = pImpl->parameters.find(paramName);
    if (it != pImpl->parameters.end()) {
        it->second.range = constraint;
    }
}

void ValidationSchema::addChoiceConstraint(const std::string& paramName, 
                                         const std::vector<ParameterValue>& allowedValues) {
    ParameterInfo::ChoiceConstraint constraint{allowedValues};
    pImpl->choiceConstraints[paramName] = constraint;
    
    auto it = pImpl->parameters.find(paramName);
    if (it != pImpl->parameters.end()) {
        it->second.choice = constraint;
    }
}

void ValidationSchema::addDependencyConstraint(const std::string& paramName, 
                                              const std::string& dependentParam,
                                              std::function<bool(const ParameterValue&, const ParameterValue&)> condition) {
    ParameterInfo::DependencyConstraint constraint{dependentParam, condition};
    pImpl->dependencyConstraints[paramName] = constraint;
    
    auto it = pImpl->parameters.find(paramName);
    if (it != pImpl->parameters.end()) {
        it->second.dependency = constraint;
    }
}

void ValidationSchema::addCustomConstraint(const std::string& paramName,
                                          std::function<bool(const ParameterValue&)> validator,
                                          const std::string& errorMessage) {
    ParameterInfo::CustomConstraint constraint{validator, errorMessage};
    pImpl->customConstraints[paramName] = constraint;
    
    auto it = pImpl->parameters.find(paramName);
    if (it != pImpl->parameters.end()) {
        it->second.custom = constraint;
    }
}

ValidationResult ValidationSchema::validateParameter(const std::string& name, const ParameterValue& value) const {
    ValidationResult result;
    
    // Check if parameter exists
    auto paramIt = pImpl->parameters.find(name);
    if (paramIt == pImpl->parameters.end()) {
        result.addError(std::format("Unknown parameter: {}", name));
        return result;
    }
    
    const ParameterInfo& paramInfo = paramIt->second;
    
    // Validate type
    if (!ParameterTypes::isTypeFromValue(value, paramInfo.type)) {
        result.addError(std::format("Invalid type for parameter '{}': expected {}, got {}", 
                                 name, paramInfo.type, ParameterTypes::parameterValueToString(value)));
    }
    
    // Validate range constraint
    auto rangeIt = pImpl->rangeConstraints.find(name);
    if (rangeIt != pImpl->rangeConstraints.end()) {
        if (!ParameterTypes::inRange(value, rangeIt->second.min, rangeIt->second.max)) {
            result.addError(ValidationUtils::generateRangeError(name, rangeIt->second.min, 
                                                             rangeIt->second.max, 
                                                             std::stod(ParameterTypes::parameterValueToString(value))));
        }
    }
    
    // Validate choice constraint
    auto choiceIt = pImpl->choiceConstraints.find(name);
    if (choiceIt != pImpl->choiceConstraints.end()) {
        if (!ParameterTypes::isInChoices(value, choiceIt->second.allowedValues)) {
            result.addError(ValidationUtils::generateChoiceError(name, 
                                                             ParameterTypes::parameterValueToString(value),
                                                             []() {
                                                                 std::stringstream ss;
                                                                 for (size_t i = 0; i < choiceIt->second.allowedValues.size(); ++i) {
                                                                     if (i > 0) ss << ", ";
                                                                     ss << ParameterTypes::parameterValueToString(choiceIt->second.allowedValues[i]);
                                                                 }
                                                                 return ss.str();
                                                             }()));
        }
    }
    
    // Validate dependency constraint
    auto depIt = pImpl->dependencyConstraints.find(name);
    if (depIt != pImpl->dependencyConstraints.end()) {
        // Check if dependent parameter exists in validation set
        // For now, we'll assume it's provided - dependency checking happens at validation level
    }
    
    // Validate custom constraint
    auto customIt = pImpl->customConstraints.find(name);
    if (customIt != pImpl->customConstraints.end()) {
        if (!customIt->second.validator(value)) {
            result.addError(ValidationUtils::generateCustomError(name, customIt->second.errorMessage));
        }
    }
    
    return result;
}

ValidationResult ValidationSchema::validate(const std::unordered_map<std::string, ParameterValue>& parameters) const {
    ValidationResult result;
    
    // Check required parameters
    auto requiredResult = checkRequiredParameters(parameters);
    if (requiredResult.hasErrors()) {
        result.errors.insert(result.errors.end(), requiredResult.errors.begin(), requiredResult.errors.end());
        result.valid = false;
    }
    
    // Validate each provided parameter
    for (const auto& [name, value] : parameters) {
        auto paramResult = validateParameter(name, value);
        if (paramResult.hasErrors()) {
            result.errors.insert(result.errors.end(), paramResult.errors.begin(), paramResult.errors.end());
            result.valid = false;
        }
        if (paramResult.hasWarnings()) {
            result.warnings.insert(result.warnings.end(), paramResult.warnings.begin(), paramResult.warnings.end());
        }
    }
    
    // Validate dependency constraints
    for (const auto& [paramName, depConstraint] : pImpl->dependencyConstraints) {
        auto it = parameters.find(paramName);
        if (it != parameters.end()) {
            auto dependentIt = parameters.find(depConstraint.dependentParameter);
            if (dependentIt != parameters.end()) {
                if (!depConstraint.condition(it->second, dependentIt->second)) {
                    result.addError(ValidationUtils::generateDependencyError(paramName, depConstraint.dependentParameter, 
                                                                           "Dependency constraint violated"));
                }
            }
        }
    }
    
    // Update statistics
    pImpl->totalValidations++;
    if (result.valid) {
        pImpl->successfulValidations++;
    } else {
        pImpl->failedValidations++;
    }
    
    return result;
}

const ParameterInfo* ValidationSchema::getParameterInfo(const std::string& name) const {
    auto it = pImpl->parameters.find(name);
    return it != pImpl->parameters.end() ? &it->second : nullptr;
}

std::vector<std::string> ValidationSchema::getParameterNames() const {
    std::vector<std::string> names;
    names.reserve(pImpl->parameters.size());
    for (const auto& pair : pImpl->parameters) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<std::string> ValidationSchema::getRequiredParameters() const {
    std::vector<std::string> required;
    for (const auto& pair : pImpl->parameters) {
        if (pair.second.required) {
            required.push_back(pair.first);
        }
    }
    return required;
}

ValidationResult ValidationSchema::checkRequiredParameters(const std::unordered_map<std::string, ParameterValue>& parameters) const {
    ValidationResult result;
    
    for (const auto& requiredName : getRequiredParameters()) {
        if (parameters.find(requiredName) == parameters.end()) {
            result.addError(std::format("Missing required parameter: {}", requiredName));
        }
    }
    
    return result;
}

void ValidationSchema::merge(const ValidationSchema& other) {
    for (const auto& [name, info] : other.pImpl->parameters) {
        pImpl->parameters[name] = info;
    }
    
    for (const auto& [name, constraint] : other.pImpl->rangeConstraints) {
        pImpl->rangeConstraints[name] = constraint;
    }
    
    for (const auto& [name, constraint] : other.pImpl->choiceConstraints) {
        pImpl->choiceConstraints[name] = constraint;
    }
    
    for (const auto& [name, constraint] : other.pImpl->dependencyConstraints) {
        pImpl->dependencyConstraints[name] = constraint;
    }
    
    for (const auto& [name, constraint] : other.pImpl->customConstraints) {
        pImpl->customConstraints[name] = constraint;
    }
}

std::string ValidationSchema::getSummary() const {
    std::stringstream ss;
    ss << "Validation Schema Summary:";
    ss << "\n  Parameters: " << pImpl->parameters.size();
    ss << "\n  Required parameters: " << getRequiredParameters().size();
    ss << "\n  Range constraints: " << pImpl->rangeConstraints.size();
    ss << "\n  Choice constraints: " << pImpl->choiceConstraints.size();
    ss << "\n  Dependency constraints: " << pImpl->dependencyConstraints.size();
    ss << "\n  Custom constraints: " << pImpl->customConstraints.size();
    ss << "\n  Total validations: " << pImpl->totalValidations;
    ss << "\n  Successful: " << pImpl->successfulValidations;
    ss << "\n  Failed: " << pImpl->failedValidations;
    
    return ss.str();
}

// ParameterTypes implementation
namespace ParameterTypes {
    template<typename T>
    bool isTypeFromValue(const ParameterValue& value, const std::string& typeName) {
        // Simple type name matching - in production, you'd use more robust type checking
        if (typeName == "int" && std::holds_alternative<int>(value)) return true;
        if (typeName == "int64_t" && std::holds_alternative<int64_t>(value)) return true;
        if (typeName == "double" && std::holds_alternative<double>(value)) return true;
        if (typeName == "float" && std::holds_alternative<float>(value)) return true;
        if (typeName == "bool" && std::holds_alternative<bool>(value)) return true;
        if (typeName == "string" && std::holds_alternative<std::string>(value)) return true;
        if (typeName == "std::vector<int>" && std::holds_alternative<std::vector<int>>(value)) return true;
        if (typeName == "std::vector<double>" && std::holds_alternative<std::vector<double>>(value)) return true;
        if (typeName == "std::vector<string>" && std::holds_alternative<std::vector<std::string>>(value)) return true;
        if (typeName == "std::vector<bool>" && std::holds_alternative<std::vector<bool>>(value)) return true;
        return false;
    }
    
    bool inRange(const ParameterValue& value, double min, double max) {
        try {
            if (std::holds_alternative<double>(value)) {
                return std::get<double>(value) >= min && std::get<double>(value) <= max;
            } else if (std::holds_alternative<float>(value)) {
                return static_cast<double>(std::get<float>(value)) >= min && 
                       static_cast<double>(std::get<float>(value)) <= max;
            } else if (std::holds_alternative<int>(value)) {
                return static_cast<double>(std::get<int>(value)) >= min && 
                       static_cast<double>(std::get<int>(value)) <= max;
            } else if (std::holds_alternative<int64_t>(value)) {
                return static_cast<double>(std::get<int64_t>(value)) >= min && 
                       static_cast<double>(std::get<int64_t>(value)) <= max;
            }
        } catch (...) {
            return false;
        }
        return false;
    }
    
    bool isInChoices(const ParameterValue& value, const std::vector<ParameterValue>& choices) {
        return std::find(choices.begin(), choices.end(), value) != choices.end();
    }
} // namespace ParameterTypes

// DefaultConfigValidator implementation
ValidationResult DefaultConfigValidator::validate(const std::string& configPath) {
    // TODO: Implement file-based configuration parsing
    // For now, just validate against schema
    return ValidationResult(); // Placeholder
}

ValidationResult DefaultConfigValidator::validate(const std::unordered_map<std::string, ParameterValue>& parameters) {
    ValidationResult result = schema.validate(parameters);
    
    // Apply custom validators
    for (const auto& validator : customValidators) {
        auto customResult = validator(parameters);
        if (customResult.hasErrors()) {
            result.errors.insert(result.errors.end(), customResult.errors.begin(), customResult.errors.end());
            result.valid = false;
        }
        if (customResult.hasWarnings()) {
            result.warnings.insert(result.warnings.end(), customResult.warnings.begin(), customResult.warnings.end());
        }
    }
    
    return result;
}

void DefaultConfigValidator::setSchema(const ValidationSchema& schema) {
    this->schema = schema;
}

const ValidationSchema& DefaultConfigValidator::getSchema() const {
    return schema;
}

void DefaultConfigValidator::addCustomValidator(std::function<ValidationResult(const std::unordered_map<std::string, ParameterValue>&)> validator) {
    customValidators.push_back(validator);
}

bool DefaultConfigValidator::loadSchemaFromFile(const std::string& filepath) {
    // TODO: Implement JSON/YAML schema loading
    return false; // Placeholder
}

bool DefaultConfigValidator::saveSchemaToFile(const std::string& filepath) const {
    // TODO: Implement schema saving
    return false; // Placeholder
}

DefaultConfigValidator::ValidationStats DefaultConfigValidator::getValidationStats() const {
    ValidationStats stats;
    stats.totalValidations = schema.pImpl->totalValidations;
    stats.successfulValidations = schema.pImpl->successfulValidations;
    stats.failedValidations = schema.pImpl->failedValidations;
    stats.errorCounts = schema.pImpl->errorCounts;
    return stats;
}

void DefaultConfigValidator::resetStats() {
    schema.pImpl->totalValidations = 0;
    schema.pImpl->successfulValidations = 0;
    schema.pImpl->failedValidations = 0;
    schema.pImpl->errorCounts.clear();
}

// ValidationUtils implementation
namespace ValidationUtils {
    std::string generateRangeError(const std::string& paramName, double expectedMin, double expectedMax, double actual) {
        return std::format("Parameter '{}' value {} is out of range [{}, {}]", 
                          paramName, actual, expectedMin, expectedMax);
    }
    
    std::string generateChoiceError(const std::string& paramName, const std::string& actual, const std::string& expectedChoices) {
        return std::format("Parameter '{}' value '{}' is not in allowed choices: {}", 
                          paramName, actual, expectedChoices);
    }
    
    std::string generateDependencyError(const std::string& paramName, const std::string& dependentParam, const std::string& reason) {
        return std::format("Dependency constraint violated for parameter '{}': depends on '{}': {}", 
                          paramName, dependentParam, reason);
    }
    
    std::string generateCustomError(const std::string& paramName, const std::string& errorMessage) {
        return std::format("Custom validation failed for parameter '{}': {}", paramName, errorMessage);
    }
    
    std::string parameterValueToString(const ParameterValue& value) {
        if (std::holds_alternative<int>(value)) return std::to_string(std::get<int>(value));
        if (std::holds_alternative<int64_t>(value)) return std::to_string(std::get<int64_t>(value));
        if (std::holds_alternative<double>(value)) return std::to_string(std::get<double>(value));
        if (std::holds_alternative<float>(value)) return std::to_string(std::get<float>(value));
        if (std::holds_alternative<bool>(value)) return std::get<bool>(value) ? "true" : "false";
        if (std::holds_alternative<std::string>(value)) return std::get<std::string>(value);
        if (std::holds_alternative<std::vector<int>>(value)) {
            std::stringstream ss;
            const auto& vec = std::get<std::vector<int>>(value);
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << vec[i];
            }
            return ss.str();
        }
        if (std::holds_alternative<std::vector<double>>(value)) {
            std::stringstream ss;
            const auto& vec = std::get<std::vector<double>>(value);
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << vec[i];
            }
            return ss.str();
        }
        if (std::holds_alternative<std::vector<std::string>>(value)) {
            std::stringstream ss;
            const auto& vec = std::get<std::vector<std::string>>(value);
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << "\"" << vec[i] << "\"";
            }
            return ss.str();
        }
        if (std::holds_alternative<std::vector<bool>>(value)) {
            std::stringstream ss;
            const auto& vec = std::get<std::vector<bool>>(value);
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << (vec[i] ? "true" : "false");
            }
            return ss.str();
        }
        return "[unknown]";
    }
    
    bool saveSchemaToJSON(const ValidationSchema& schema, const std::string& filepath) {
        // TODO: Implement JSON schema serialization
        return false; // Placeholder
    }
    
    bool loadSchemaFromJSON(ValidationSchema& schema, const std::string& filepath) {
        // TODO: Implement JSON schema loading
        return false; // Placeholder
    }
    
    void logValidationResult(const ValidationResult& result, const std::string& context) {
        if (result.valid) {
            getGlobalErrorReporter().info(std::format("Validation successful: {}", result.getSummary()));
        } else {
            std::string message = std::format("Validation failed {}: {}", 
                                            !context.empty() ? "for " + context : "",
                                            result.getSummary());
            getGlobalErrorReporter().warn(message);
        }
    }
} // namespace ValidationUtils

// Global validation system implementation
ConfigValidator& getDefaultValidatorInstance() {
    static DefaultConfigValidator validator;
    return validator;
}

ConfigValidator* ValidationSystem::globalValidator = nullptr;

ConfigValidator& ValidationSystem::getValidator() {
    if (!globalValidator) {
        globalValidator = &getDefaultValidatorInstance();
    }
    return *globalValidator;
}

void ValidationSystem::initializeWithDefaults() {
    setGlobalSchema(ValidationSchema{}); // Empty schema for now
}

ValidationResult ValidationSystem::validateConfig(const std::string& configPath) {
    return getValidator().validate(configPath);
}

ValidationResult ValidationSystem::validateParameters(const std::unordered_map<std::string, ParameterValue>& parameters) {
    return getValidator().validate(parameters);
}

void ValidationSystem::setGlobalSchema(const ValidationSchema& schema) {
    getValidator().setSchema(schema);
}

const ValidationSchema& ValidationSystem::getGlobalSchema() {
    return getValidator().getSchema();
}

} // namespace nlm

// Implementation file ends here