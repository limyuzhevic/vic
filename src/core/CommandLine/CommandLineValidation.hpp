// Command-line validation functionality for NLM
// Validates command-line arguments against rules and constraints

#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <functional>
#include <memory>
#include <unordered_map>
#include "core/CommandLine/CommandLineParser.hpp"

namespace nlm {

// Validation rule types
enum class ValidationRuleType {
    Range,
    Length,
    Format,
    Dependency,
    MutualExclusion,
    Custom
};

// Validation error with details
struct ValidationError {
    std::string argumentName;
    std::string errorMessage;
    ValidationRuleType ruleType;
    std::string value;
    
    ValidationError(const std::string& arg, const std::string& msg, 
                   ValidationRuleType type = ValidationRuleType::Custom,
                   const std::string& val = "")
        : argumentName(arg), errorMessage(msg), ruleType(type), value(val) {}
};

// Validation rule definition
struct ValidationRule {
    std::string targetArgument;
    ValidationRuleType type;
    std::string condition;
    std::string errorMessage;
    std::function<bool(const std::string&, const std::map<std::string, std::string>&)> customValidator;
    
    ValidationRule(const std::string& target, ValidationRuleType t, const std::string& cond,
                   const std::string& msg, const std::function<bool(const std::string&, const std::map<std::string, std::string>&)> &validator = nullptr)
        : targetArgument(target), type(t), condition(cond), errorMessage(msg), customValidator(validator) {}
};

// Advanced command-line validator
class CommandLineValidation {
public:
    CommandLineValidation() = default;
    
    // Add validation rule
    void addRule(const ValidationRule& rule);
    
    // Add range validation for numeric argument
    void addRangeRule(const std::string& argument, double min, double max,
                      const std::string& errorMessage = "");
    
    // Add length validation for string argument
    void addLengthRule(const std::string& argument, size_t minLength, size_t maxLength,
                      const std::string& errorMessage = "");
    
    // Add format validation using regex
    void addFormatRule(const std::string& argument, const std::string& pattern,
                      const std::string& errorMessage = "");
    
    // Add dependency rule (argument requires another argument)
    void addDependencyRule(const std::string& dependentArgument,
                          const std::string& requiredArgument,
                          const std::string& errorMessage = "");
    
    // Add mutual exclusion rule (arguments cannot both be present)
    void addMutualExclusionRule(const std::string& arg1, const std::string& arg2,
                                const std::string& errorMessage = "");
    
    // Add custom validation function
    void addCustomValidator(const std::string& argument,
                           const std::function<bool(const std::string&, const std::map<std::string, std::string>&)> &validator,
                           const std::string& errorMessage = "");
    
    // Validate all arguments
    bool validate(const std::vector<CommandLineArg>& args) const;
    
    // Validate specific argument
    bool validateArgument(const CommandLineArg& arg) const;
    
    // Get all validation errors
    const std::vector<ValidationError>& getErrors() const { return errors; }
    
    // Clear errors
    void clearErrors() { errors.clear(); }
    
    // Check if validation passed
    bool isValid() const { return errors.empty(); }
    
    // Format validation errors for display
    std::string formatErrors() const;
    
    // Register built-in validators
    void registerBuiltInValidators();
    
private:
    std::vector<ValidationRule> rules;
    std::vector<ValidationError> errors;
    
    // Validate range rule
    bool validateRangeRule(const ValidationRule& rule, const std::string& value) const;
    
    // Validate length rule
    bool validateLengthRule(const ValidationRule& rule, const std::string& value) const;
    
    // Validate format rule
    bool validateFormatRule(const ValidationRule& rule, const std::string& value) const;
    
    // Validate dependency rule
    bool validateDependencyRule(const ValidationRule& rule, const std::string& value,
                               const std::map<std::string, std::string>& argValues) const;
    
    // Validate mutual exclusion rule
    bool validateMutualExclusionRule(const ValidationRule& rule, const std::string& value,
                                    const std::map<std::string, std::string>& argValues) const;
    
    // Validate custom rule
    bool validateCustomRule(const ValidationRule& rule, const std::string& value,
                           const std::map<std::string, std::string>& argValues) const;
    
    // Convert argument vector to value map
    std::map<std::string, std::string> argsToValues(const std::vector<CommandLineArg>& args) const;
    
    // Check if argument is present
    bool isArgumentPresent(const std::string& argumentName, const std::vector<CommandLineArg>& args) const;
    
    // Get argument value
    std::string getArgumentValue(const std::string& argumentName, const std::vector<CommandLineArg>& args) const;
};

} // namespace nlm