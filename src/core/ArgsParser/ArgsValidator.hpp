#pragma once

#include <string>
#include <vector>
#include <map>
#include <variant>
#include <optional>
#include <functional>
#include <regex>
#include <stdexcept>
#include <sstream>

namespace nlm {

class ArgsValidator {
public:
    ArgsValidator();
    ~ArgsValidator();
    
    // Disable copying, enable moving
    ArgsValidator(const ArgsValidator&) = delete;
    ArgsValidator& operator=(const ArgsValidator&) = delete;
    ArgsValidator(ArgsValidator&&) noexcept;
    ArgsValidator& operator=(ArgsValidator&&) noexcept;
    
    // Add validation rule
    void addValidationRule(const std::string& key, 
                          const std::function<bool(const ArgsValue&)>& validator,
                          const std::string& errorMessage = "Validation failed");
    
    // Add regex validation rule
    void addRegexValidationRule(const std::string& key,
                               const std::string& pattern,
                               const std::string& errorMessage = "Pattern validation failed");
    
    // Add range validation for numeric types
    void addRangeValidation(const std::string& key, double min, double max,
                           const std::string& errorMessage = "Value out of range");
    
    // Add dependency validation (key A requires key B)
    void addDependencyValidation(const std::string& dependentKey,
                                 const std::string& requiredKey,
                                 const std::string& errorMessage = "Dependency not satisfied");
    
    // Validate a single value against all applicable rules
    bool validate(const std::string& key, const ArgsValue& value,
                  std::string& errorMessage) const;
    
    // Validate all entries in a configuration
    bool validateAll(const std::map<std::string, ArgsValue>& config,
                    std::vector<std::string>& errors) const;
    
    // Get validation errors for a specific key
    std::vector<std::string> getErrors(const std::string& key) const;
    
    // Clear all validation rules
    void clear() noexcept;
    
    // Get all validation rules
    const std::map<std::string, std::function<bool(const ArgsValue&)>>& 
        getValidationRules() const { return validationRules; }
    
    // Get all regex validation rules
    const std::map<std::string, std::pair<std::regex, std::string>>& 
        getRegexValidationRules() const { return regexValidationRules; }
    
    // Get all range validation rules
    const std::map<std::string, std::tuple<double, double, std::string>>& 
        getRangeValidationRules() const { return rangeValidationRules; }
    
    // Get all dependency validation rules
    const std::map<std::string, std::pair<std::string, std::string>>& 
        getDependencyValidationRules() const { return dependencyValidationRules; }
    
    // Static utility methods
    static bool validateType(const ArgsValue& value, const std::string& typeName);
    static bool validateRange(const ArgsValue& value, double min, double max,
                             std::string& errorMessage);
    static bool validatePattern(const std::string& value, const std::regex& pattern,
                               std::string& errorMessage);
    static bool validateDependency(const ArgsValue& dependentValue,
                                   const ArgsValue& requiredValue,
                                   std::string& errorMessage);
    
    // Error message formatting
    static std::string formatErrorMessage(const std::string& key,
                                          const std::string& errorType,
                                          const std::string& details = "");
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};